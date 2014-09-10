// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "integrators/pathtraceintegrator.h"

namespace embree
{
  PathTraceIntegrator::PathTraceIntegrator(const Parms& parms)
    : lightSampleID(-1), firstScatterSampleID(-1), firstScatterTypeSampleID(-1)
  {
    maxDepth        = parms.getInt  ("maxDepth"       ,10    );
    minContribution = parms.getFloat("minContribution",0.01f );
    epsilon         = parms.getFloat("epsilon"        ,32.0f)*float(ulp);
    filterCaustics  = parms.getBool("filterCaustics"  , false);
    disableCausticReflection = parms.getBool("disableCausticReflection", false);
    disableCausticTransmission = parms.getBool("disableCausticTransmission", false);
    backplate       = parms.getImage("backplate");
  }
  
  void PathTraceIntegrator::requestSamples(Ref<SamplerFactory>& samplerFactory, const Ref<BackendScene>& scene)
  {
    precomputedLightSampleID.resize(scene->allLights.size());

    lightSampleID = samplerFactory->request2D();
    for (size_t i=0; i<scene->allLights.size(); i++) {
      precomputedLightSampleID[i] = -1;
      if (scene->allLights[i]->precompute())
        precomputedLightSampleID[i] = samplerFactory->requestLightSample(lightSampleID, scene->allLights[i]);
    }
    firstScatterSampleID = samplerFactory->request2D((int)maxDepth);
    firstScatterTypeSampleID = samplerFactory->request1D((int)maxDepth);
  }

  Color PathTraceIntegrator::Li(LightPath& lightPath, const Ref<BackendScene>& scene, IntegratorState& state)
  {
    /*! Terminate path if too long or contribution too low. */
    if (lightPath.depth >= maxDepth || reduce_max(lightPath.throughput) < minContribution)
      return zero;

    /*! Traverse ray. */
    DifferentialGeometry dg;
    //scene->intersector->intersect(lightPath.lastRay);
    rtcIntersect(scene->scene,(RTCRay&)lightPath.lastRay);
    scene->postIntersect(lightPath.lastRay,dg);
    state.numRays++;
    
    Color L = zero;
    const Vector3f wo = -lightPath.lastRay.dir;
#if 0
    BRDFType directLightingBRDFTypes = (BRDFType)(DIFFUSE|GLOSSY); 
    BRDFType giBRDFTypes = (BRDFType)(SPECULAR);
#else
    BRDFType directLightingBRDFTypes = (BRDFType)(DIFFUSE); 
    BRDFType giBRDFTypes = (BRDFType)(ALL);
#endif

    /*! Environment shading when nothing hit. */
    if (!lightPath.lastRay)
    {
      if (backplate && lightPath.unbend) {
        const int x = clamp(int(state.pixel.x * backplate->width ), 0, int(backplate->width )-1);
        const int y = clamp(int(state.pixel.y * backplate->height), 0, int(backplate->height)-1);
        L = backplate->get(x, y);
      }
      else {
        if (!lightPath.ignoreVisibleLights)
          for (size_t i=0; i<scene->envLights.size(); i++)
            L += scene->envLights[i]->Le(wo);
      }
      return L;
    }

    /*! face forward normals */
    bool backfacing = false;
    if (dot(dg.Ng, lightPath.lastRay.dir) > 0) {
      backfacing = true; dg.Ng = -dg.Ng; dg.Ns = -dg.Ns;
    }

    /*! Shade surface. */
    CompositedBRDF brdfs;
    if (dg.material) dg.material->shade(lightPath.lastRay, lightPath.lastMedium, dg, brdfs);

    /*! Add light emitted by hit area light source. We may not include the contribution if it leads to caustic effects that are disabled.
        We also track the light hit as part of the path history. */
    bool lightHit = false;

    if (!lightPath.ignoreVisibleLights && dg.light && !backfacing &&
        !(disableCausticReflection && (lightPath.history.hitHistory & DIFFUSE_SPECULAR_REFLECTION_HIT)) &&
        !(disableCausticTransmission && (lightPath.history.hitHistory & DIFFUSE_SPECULAR_TRANSMISSION_HIT)))
    {
      L += dg.light->Le(dg,wo);
      lightHit = true;
    }

    /*! Global illumination. Pick one BRDF component and sample it. */
    if (lightPath.depth < maxDepth)
    {
      /*! sample brdf */
      Sample3f wi; BRDFType type;
      Vec2f s  = state.sample->getVec2f(firstScatterSampleID     + lightPath.depth);
      float ss = state.sample->getFloat(firstScatterTypeSampleID + lightPath.depth);
      Color c  = brdfs.sample(wo, dg, wi, type, s, ss, giBRDFTypes);

      /*! add to light path history if caustics are being modified */
      bool firstDiffuseHit = false;

      if(filterCaustics || disableCausticReflection || disableCausticTransmission)
      {
        if((lightPath.history.hitHistory & DIFFUSE_SPECULAR_REFLECTION_HIT || lightPath.history.hitHistory & DIFFUSE_SPECULAR_TRANSMISSION_HIT)
            && lightHit)
          lightPath.history.hitHistory = LightPathHitHistory(lightPath.history.hitHistory | DIFFUSE_SPECULAR_LIGHT_HIT);

        if((lightPath.history.hitHistory & DIFFUSE_HIT) && (type & SPECULAR_REFLECTION || type & GLOSSY_REFLECTION))
          lightPath.history.hitHistory = LightPathHitHistory(lightPath.history.hitHistory | DIFFUSE_SPECULAR_REFLECTION_HIT);

        if((lightPath.history.hitHistory & DIFFUSE_HIT) && (type & SPECULAR_TRANSMISSION || type & GLOSSY_TRANSMISSION))
          lightPath.history.hitHistory = LightPathHitHistory(lightPath.history.hitHistory | DIFFUSE_SPECULAR_TRANSMISSION_HIT);

        /*! we consider the hit a diffuse hit if the material has a diffuse BRDF (whether or not it was chosen in this path) */
        if(!(lightPath.history.hitHistory & DIFFUSE_HIT) && brdfs.has(DIFFUSE))
        {
          lightPath.history.hitHistory = LightPathHitHistory(lightPath.history.hitHistory | DIFFUSE_HIT);
          firstDiffuseHit = true;
        }
      }

      /*! Continue only if we hit something valid. */
      if (c != Color(zero) && wi.pdf > 0.0f) {

        /*! Compute a simple distance attenuated volumetric effect. */
//      const Color &transmissionColor   = lightPath.lastMedium.transmission;
//      const float  transmissionDepth   = lightPath.lastMedium.transmissionDepth;
//      const float  transmissionFalloff = lightPath.lastMedium.transmissionFalloff;
//      if (transmissionColor != Color(one)) c = c * exp(-lightPath.lastRay.tfar / transmissionDepth * transmissionFalloff) * transmissionColor;

        /*! Tracking medium if we hit a medium interface. */
        Medium nextMedium = lightPath.lastMedium;
        if (type & TRANSMISSION) nextMedium = dg.material->nextMedium(lightPath.lastMedium);

        /*! Continue the path. */
        LightPath scatteredPath = lightPath.extended(Ray(dg.P, wi, dg.error*epsilon, inf, lightPath.lastRay.time), lightPath.history,
                                                     nextMedium, c, (type & directLightingBRDFTypes) != NONE);

        /*! We maintain the contribution from the first diffuse hit onward in a separate buffer to filter caustics. */
        if(filterCaustics && firstDiffuseHit)
        {
          lightPath.history.firstDiffuseGeomID = lightPath.lastRay.id0; // id0 == geomID
          lightPath.history.postDiffuseContribution = c * Li(scatteredPath, scene, state) * rcp(wi.pdf);
          L += lightPath.history.postDiffuseContribution;
        }
        else
          L += c * Li(scatteredPath, scene, state) * rcp(wi.pdf);
      }
    }

    /*! Check if any BRDF component uses direct lighting. */
    bool useDirectLighting = false;
    for (size_t i=0; i<brdfs.size(); i++)
      useDirectLighting |= (brdfs[i]->type & directLightingBRDFTypes) != NONE;

    /*! Direct lighting. Shoot shadow rays to all light sources. */
    if (useDirectLighting)
    {
      for (size_t i=0; i<scene->allLights.size(); i++)
      {
        if ((scene->allLights[i]->illumMask & dg.illumMask) == 0)
          continue;

        /*! Either use precomputed samples for the light or sample light now. */
        LightSample ls;
        if (scene->allLights[i]->precompute()) ls = state.sample->getLightSample(precomputedLightSampleID[i]);
        else ls.L = scene->allLights[i]->sample(dg, ls.wi, ls.tMax, state.sample->getVec2f(lightSampleID));

        /*! Ignore zero radiance or illumination from the back. */
        //if (ls.L == Color(zero) || ls.wi.pdf == 0.0f || dot(dg.Ns,Vector3f(ls.wi)) <= 0.0f) continue; 
        if (ls.L == Color(zero) || ls.wi.pdf == 0.0f) continue;

        /*! Evaluate BRDF */
        Color brdf = brdfs.eval(wo, dg, ls.wi, directLightingBRDFTypes);
        if (brdf == Color(zero)) continue;

        /*! Test for shadows. */
        Ray shadowRay(dg.P, ls.wi, dg.error*epsilon, ls.tMax-dg.error*epsilon, lightPath.lastRay.time,dg.shadowMask);
        //bool inShadow = scene->intersector->occluded(shadowRay);
        rtcOccluded(scene->scene,(RTCRay&)shadowRay);
        state.numRays++;
        if (shadowRay) continue;

        /*! Evaluate BRDF. */
        L += ls.L * brdf * rcp(ls.wi.pdf);
      }
    }
    
    return L;
  }

  Color PathTraceIntegrator::Li(Ray& ray, const Ref<BackendScene>& scene, IntegratorState& state, LightPathHistory& history) {
    LightPath path(ray, history);
    return Li(path, scene, state);
  }
}

