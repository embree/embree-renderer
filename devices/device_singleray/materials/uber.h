// ======================================================================== //
// Copyright 2009-2014 Intel Corporation                                    //
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

#ifndef __EMBREE_UBER_H__
#define __EMBREE_UBER_H__

#include "../materials/material.h"
#include "../brdfs/lambertian.h"
#include "../brdfs/dielectric.h"
#include "../brdfs/dielectriclayer.h"
#include "../brdfs/microfacet.h"


namespace embree
{
  /*! Implements a diffuse material. */
  class Uber : public Material
  {
    typedef Microfacet<FresnelDielectric,PowerCosineDistribution > MicrofacetPlastic;
//    typedef Microfacet<FresnelConductor,PowerCosineDistribution > MicrofacetMetal;

  public:

    /*! Construction from parameters. */
    Uber (const Parms& parms) {
      diffColor = parms.getColor("diffColor",one);
	  reflColor = parms.getColor("reflColor",one);
      Kd = parms.getTexture("diffTexture");
      s0 = parms.getVec2f("s0",Vec2f(0.0f,0.0f));
      ds = parms.getVec2f("ds",Vec2f(1.0f,1.0f));
      surfEta      = parms.getFloat("surfEta",1.4f);
      roughness    = parms.getFloat("roughness",0.01f);
//	  realRefract  = parms.getColor("realRefract",Color(1.4f));
//    imagRefract  = parms.getColor("imagRefract",Color(0.0f));
      mediumOutside.eta          = parms.getFloat("etaOutside",1.0f);
      mediumInside.eta           = parms.getFloat("etaInside",1.4f);
      mediumOutside.transmission = parms.getColor("transColorOutside",one);
      mediumInside.transmission  = parms.getColor("transColor",one);
      isMediaInterface           = true;
	  rcpRoughness = rcp(roughness);
	  if ((mediumInside.transmission.r == 0.0f) &&
		  (mediumInside.transmission.g == 0.0f) &&
		  (mediumInside.transmission.b == 0.0f))
		  transparent = 0;
	  else
		  transparent = 1;

      /*! precompute BRDF components for more efficient shading */
      reflection_io   = new DielectricReflection  (mediumInside.eta, mediumOutside.eta);
      reflection_oi   = new DielectricReflection  (mediumOutside.eta,mediumInside.eta);
      transmission_io = new DielectricTransmission(mediumInside.eta, mediumOutside.eta);
      transmission_oi = new DielectricTransmission(mediumOutside.eta,mediumInside.eta);
    }

    /*! Destruction */
    ~Uber()
    {
      if (reflection_io  ) delete reflection_io;   reflection_io = NULL;
      if (reflection_oi  ) delete reflection_oi;   reflection_oi = NULL;
      if (transmission_io) delete transmission_io; transmission_io = NULL;
      if (transmission_oi) delete transmission_oi; transmission_oi = NULL;
    }

    void shade(const Ray& ray, const Medium& currentMedium, const DifferentialGeometry& dg, CompositedBRDF& brdfs) const {

#if 1
	// Diffuse color
      if (Kd) {
			brdfs.add(NEW_BRDF(Lambertian)(Kd->get(ds*dg.st+s0)));
	  }
	  else {
		  // diffuse plastic
//		  brdfs.add(NEW_BRDF(DielectricLayer<Lambertian >)(one, 1.0f, surfEta, Lambertian (diffColor)));
		  brdfs.add(NEW_BRDF(Lambertian)(diffColor));
	  }
#endif
#if 1
	  if (1 == transparent) {
		  // Transparency
		  /*! the ray transitions from outside to inside */
		  if (currentMedium == mediumOutside) {
			brdfs.add(reflection_oi);
			brdfs.add(transmission_oi);
		  }

		  /*! the ray transitions from inside to outside */
		  else {
			brdfs.add(reflection_io);
			brdfs.add(transmission_io);
		  }
	  }  // transparency
#endif
#if 1
	  // Specular reflection
      /*! use dielectric reflection in case of a specular surface */
      if (roughness == 0.0f)
        brdfs.add(NEW_BRDF(DielectricReflection)(1.0f, surfEta));

      /*! otherwise use the microfacet BRDF to model the rough surface */
      else
        brdfs.add(NEW_BRDF(MicrofacetPlastic)(reflColor, FresnelDielectric(1.0f, surfEta), PowerCosineDistribution(rcpRoughness,dg.Ns)));
#endif
#if 0
	  /*! handle the special case of a specular metal through a special BRDF */
      if (roughness == 0.0f)
        brdfs.add(NEW_BRDF(Conductor)(reflColor, realRefract, imagRefract));

      /*! otherwise use the microfacet BRDF model */
      else
        brdfs.add(NEW_BRDF(MicrofacetMetal)(reflColor, FresnelConductor(realRefract,imagRefract), PowerCosineDistribution(rcpRoughness,dg.Ns)));

#endif
    }

  protected:

    /*! Diffuse reflectance of the surface. The range is from 0
     *  (black) to 1 (white). */
    Color diffColor;
	Color reflColor;
//	Color realRefract;  //!< Real part of refraction index
//	Color imagRefract;  //!< Imaginary part of refraction index
	Vec2f s0;         //!< Offset for texture coordinates.
    Vec2f ds;         //!< Scaling for texture coordinates.
    Ref<Texture> Kd;  //!< Texture mapped to the surface.
    float surfEta;    //!< Refraction index of the dielectric layer.
    float roughness;    //!< Roughness parameter. The range goes from 0 (specular) to 1 (diffuse).
    float rcpRoughness; //!< Reciprocal roughness parameter.
	int transparent;

  private:
    DielectricReflection*   reflection_io;    //!< Reflection component for inside to outside transition.
    DielectricReflection*   reflection_oi;    //!< Reflection component for outside to inside transition.
    DielectricTransmission* transmission_io;  //!< Transmission component for inside to outside transition.
    DielectricTransmission* transmission_oi;  //!< Transmission component for outside to inside transition.
  };
}

#endif
