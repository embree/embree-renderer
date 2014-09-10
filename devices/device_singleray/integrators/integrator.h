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

#ifndef __EMBREE_INTEGRATOR_H__
#define __EMBREE_INTEGRATOR_H__

#include "renderers/ray.h"
#include "../api/scene.h"
#include "../samplers/sampler.h"

namespace embree
{
    /*! We track certain types of hits to disable or filter caustics. */
    enum LightPathHitHistory {
      NO_HIT                            = 0x00000000,   /*! no tracked hits in history                                     */
      DIFFUSE_HIT                       = 0x00000001,   /*! a diffuse surface has been hit in the path                     */
      DIFFUSE_SPECULAR_REFLECTION_HIT   = 0x00000010,   /*! a diffuse surface and then specular surface hit (reflection)   */
      DIFFUSE_SPECULAR_TRANSMISSION_HIT = 0x00000100,   /*! a diffuse surface and then specular surface hit (transmission) */
      DIFFUSE_SPECULAR_LIGHT_HIT        = 0x00001000    /*! a diffuse, specular, light hit sequence                        */
    };

    /*! Geometry IDs >= 0 are valid. We also track edges between different geometry IDs. */
    enum GeometryIdType {
      GEOMETRY_ID_INVALID = -1,
      GEOMETRY_ID_EDGE = -2
    };

    /*! Tracked components of the light path history */
    struct LightPathHistory {
      LightPathHistory() : hitHistory(NO_HIT), firstDiffuseGeomID(GEOMETRY_ID_INVALID), postDiffuseContribution(0.f) {}

      LightPathHitHistory hitHistory;
      int firstDiffuseGeomID;
      Color postDiffuseContribution;
    };

  /*! Integrator State */
  struct IntegratorState
  {
    IntegratorState () : sample(NULL), pixel(0.0f,0.0f), numRays(0) {}
  public:
    const PrecomputedSample* sample;  /*!< Sampler used to generate (pseudo) random numbers. */
    Vec2f                    pixel;   /*!< normalized pixel location on screen */
    size_t                   numRays; /*!< Used to count the number of rays shot.            */
  };
  
  /*! Interface to different integrators. The task of the integrator
   *  is to compute the incoming radiance along some ray. */
  class Integrator : public RefCount {
    ALIGNED_CLASS
  public:

    /*! Integrators need a virtual destructor. */
    virtual ~Integrator() {}

    /*! Allows the integrator to register required samples at the sampler. */
    virtual void requestSamples(Ref<SamplerFactory>& samplerFactory, const Ref<BackendScene>& scene) {}

    /*! Computes the radiance arriving at the origin of the ray from
     *  the ray direction. */
    virtual Color Li(      Ray&               ray,     /*!< Ray to compute the radiance along.                */
                     const Ref<BackendScene>& scene,   /*!< Scene geometry and lights.                        */
                     IntegratorState&   state,
                     LightPathHistory& history) = 0;

  };
}

#endif
