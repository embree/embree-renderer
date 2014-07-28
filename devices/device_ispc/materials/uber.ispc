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

#include "material.isph"
#include "brdfs/dielectriclayer_lambertian.isph"
#include "brdfs/dielectric_reflection.isph"
#include "brdfs/dielectric_transmission.isph"
#include "brdfs/microfacet_plastic.isph"
#include "brdfs/lambertian.isph"
#include "textures/texture.isph"

struct Uber
{
  Material base;
  float roughness;
  int transparent;
  Lambertian lambertian;
  DielectricLayerLambertian substrate;
  DielectricReflection reflection;
  MicrofacetPlastic microfacet;
  vec2f s0;                     //!< Offset for texture coordinates.
  vec2f ds;                     //!< Scaling for texture coordinates.
  uniform Texture* uniform Kd;  //!< Texture mapped to the surface.
  Medium mediumOutside;
  Medium mediumInside;
  DielectricReflection reflection_io;
  DielectricReflection reflection_oi;
  DielectricTransmission transmission_io;
  DielectricTransmission transmission_oi;
};

///////////////////////////////////////////////////////////////////////////////
// Implementation

void Uber__shade(const uniform Material *uniform _this,
                  const Ray&                  ray,             
                  const Medium&               currentMedium,   
                  const DifferentialGeometry& dg,              
                  uniform CompositedBRDF&     brdfs)           
{
  const uniform Uber* uniform this = (const uniform Uber* uniform)_this;
//  if (0 == this->transparent) {
  if (this->Kd) {
	  COMPOSITED_BRDF_ADD(brdfs,Lambertian,
                      this->Kd->get(this->Kd,add(mul(this->ds,dg.st),this->s0)));
  } else {
//	  CompositedBRDF__add(&brdfs,&this->substrate.base);
	  CompositedBRDF__add(&brdfs,&this->lambertian.base);
  }
//  }
#if 1
  if (1 == this->transparent) {
	// Transparency
	/*! the ray transitions from outside to inside */
	if (eq(currentMedium,this->mediumOutside)) {
		CompositedBRDF__add(&brdfs,&this->reflection_oi.base);
		CompositedBRDF__add(&brdfs,&this->transmission_oi.base);
	}
  
	/*! the ray transitions from inside to outside */
	else {
		CompositedBRDF__add(&brdfs,&this->reflection_io.base);
		CompositedBRDF__add(&brdfs,&this->transmission_io.base);
	}
  } // transparency
#endif
#if 0
  if (this->roughness == 0.0f) 
	  CompositedBRDF__add(&brdfs,&this->reflection.base);
  else
	  CompositedBRDF__add(&brdfs,&this->microfacet.base);
#endif
}

void Uber__selectNextMedium(const uniform Material *uniform _this,
                                  varying Medium &lastMedium) 
{
  const uniform Uber *uniform this = (const uniform Uber *uniform)_this;
  
  if (eq(lastMedium,this->mediumOutside)) lastMedium = this->mediumInside;
  else                                    lastMedium = this->mediumOutside;
}

void Uber__Destructor(uniform RefCount* uniform _this)
{
  uniform Uber* uniform this = (uniform Uber* uniform) _this;
  if (this->Kd)
	RefCount__DecRef(&this->Kd->base);
  Material__Destructor(_this);
}

void Uber__Constructor(uniform Uber *uniform this,
                        const uniform vec3f& diffColor,
						const uniform vec3f& reflColor,
						uniform Texture* uniform Kd,
						const uniform vec2f& s0,
						const uniform vec2f& ds,
						const uniform float surfEta,
						const uniform float roughness,
						const uniform float etaOutside,
						const uniform vec3f& transmissionOutside,
                        const uniform float etaInside,
						const uniform vec3f& transmissionInside)
{
  Material__Constructor(&this->base,Uber__Destructor,Uber__shade,Uber__selectNextMedium);
  if (Kd)
	RefCount__IncRef(&Kd->base);
  this->Kd = Kd;
  this->s0 = s0;
  this->ds = ds;
  this->roughness = roughness;
  this->mediumOutside.eta          = etaOutside;
  this->mediumOutside.transmission = transmissionOutside;
  this->mediumInside.eta           = etaInside;
  this->mediumInside.transmission  = transmissionInside;
  DielectricReflection__Constructor  (&this->reflection_io  ,etaInside ,etaOutside);
  DielectricReflection__Constructor  (&this->reflection_oi  ,etaOutside,etaInside);
  DielectricTransmission__Constructor(&this->transmission_io,etaInside ,etaOutside);
  DielectricTransmission__Constructor(&this->transmission_oi,etaOutside,etaInside);
/*
  DielectricLayerLambertian__Constructor(&this->substrate,
                                         make_vec3f(1.0f), 1.0f, surfEta, 
                                         make_Lambertian(diffColor));
*/
  this->lambertian = make_Lambertian(diffColor);

  DielectricReflection__Constructor(&this->reflection, 1.0f, surfEta);

  MicrofacetPlastic__Constructor(&this->microfacet,reflColor,
                                 make_FresnelDielectric(1.0f,surfEta), 
                                 make_PowerCosineDistribution(rcp(roughness)));
  if ((this->mediumInside.transmission.x == 0.0f) &&
	  (this->mediumInside.transmission.y == 0.0f) &&
	  (this->mediumInside.transmission.z == 0.0f))
	  this->transparent = 0;
  else
	  this->transparent = 1;
}

///////////////////////////////////////////////////////////////////////////////
// External API

export void* uniform Uber__new(const uniform vec3f& diffColor,
								const uniform vec3f& reflColor,
								void* uniform Kd,
								const uniform vec2f& s0,
								const uniform vec2f& ds,
								const uniform float surfEta,
								const uniform float roughness,
								const uniform float etaOutside,
								const uniform vec3f& transmissionOutside,
                                const uniform float etaInside,
								const uniform vec3f& transmissionInside)
{
  uniform Uber *uniform this = uniform new uniform Uber;
  Uber__Constructor(this, diffColor, reflColor, (uniform Texture* uniform)Kd, s0, ds, 
	  surfEta, roughness, etaOutside, transmissionOutside, etaInside, transmissionInside);
  return this;
}
