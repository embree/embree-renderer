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
#include "brdfs/conductor.isph"
#include "brdfs/dielectric_reflection.isph"
#include "brdfs/dielectric_transmission.isph"
#include "brdfs/microfacet_metal.isph"
#include "brdfs/translucent.isph"
#include "brdfs/specular.isph"
#include "brdfs/lambertian.isph"
#include "textures/texture.isph"


struct Uber
{
    Material base;
    
    //! Absorption coefficient.
    vec3f absorption;

    //! Bump map.
    uniform Texture* uniform bumpMap;

    //! Diffuse reflectance coefficient and map.
    vec3f diffuse;  
    uniform Texture* uniform diffuseMap;

    //! Multilayer diffusion coefficients.
    uniform struct vcc4f* uniform diffusionCoefficients;
    int numCoefficients;

    //! Diffusion scattering exponent.
    float diffusionExponent;

    //! Index of refraction.
    vec3f refraction;

    //! Surface roughness.
    float roughness;

    //! Specular reflectance coefficient and map.
    vec3f specular;  
    uniform Texture* uniform specularMap;

    //! Transmitted radiance coefficient.
    vec3f transmission;

    //! Transmitted radiance penetration depth.
    float transmissionDepth;
    
    Medium mediumOutside;
    Medium mediumInside;
    DielectricReflection reflection_io;

    DielectricTransmission transmission_io;
    DielectricTransmission transmission_oi;

    TranslucentEntry translucentEntry;
    TranslucentExit translucentExit;
    
    FresnelConductor fresnel;
    PowerCosineDistribution powerdist;
};

///////////////////////////////////////////////////////////////////////////////
// Implementation

// Do bump-mapping
void Uber__bendSurfaceNormal(const uniform Uber* uniform this,
                    const Ray&                 ray,
                    DifferentialGeometry &dg) 
{

    //! Tangent space normal from the texture map.
    vec3f texel;
    if (this->bumpMap)
        texel = this->bumpMap->get(this->bumpMap, dg.st);
    else
        return;

    //! Expand the normal range from [0, 1] to [-1, 1].
    vec3f bend = make_vec3f(2.0 * texel.x - 1.0, 2.0 * texel.y - 1.0, 2.0 * texel.z - 1.0);

    //! Bend the surface normal at the hit point.
    dg.Ns = normalize(add(add(mul(bend.x, dg.Tx), mul(bend.y, dg.Ty)), mul(bend.z, dg.Ns)));
}

//! Convenience routine which always returns a valid Color.
vec3f Uber__sampleTexture(DifferentialGeometry &dg, uniform Texture* uniform  texture)
{
    if (NULL != texture)
        return texture->get(texture, dg.st);
    else
        return make_vec3f(1.0);
}

void Uber__shadeConductor(const uniform Uber *uniform this,  
        DifferentialGeometry& dg,              
        uniform CompositedBRDF&     brdfs)           
{    
    //! Diffuse reflected color.
    const vec3f diffuseColor = mul(this->diffuse, Uber__sampleTexture(dg, this->diffuseMap));

    //! Specular reflected color.
    const vec3f specularColor = mul(this->specular, Uber__sampleTexture(dg, this->specularMap));
    
    //! Surface is perfectly specular.
    if (this->roughness == 0.0f)
        COMPOSITED_BRDF_ADD(brdfs, Conductor, specularColor, this->refraction, this->absorption);

    //! Surface is partially or fully diffuse.
    if (this->roughness > 0.0f) 
        COMPOSITED_BRDF_ADD(brdfs, Lambertian, diffuseColor);

    //! Surface is partially diffuse and specular.
    if (this->roughness < 1.0f && this->roughness > 0.0f) 
    {
        COMPOSITED_BRDF_ADD(brdfs, MicrofacetMetal, specularColor, this->fresnel, this->powerdist);               
    }
}

void Uber__shadeDielectricEntry(const uniform Uber *uniform this,
        DifferentialGeometry& dg,              
        uniform CompositedBRDF&     brdfs,
        const vec3f &diffuseColor,
        const vec3f &specularColor)
{

    //! Surface is perfectly transmissive.
    if (this->transmissionDepth >= 999999.0f)
        CompositedBRDF__add(&brdfs, &this->transmission_oi.base);

    //! Surface is translucent.
    if (this->transmissionDepth < 999999.0f) 
        CompositedBRDF__add(&brdfs, &this->translucentEntry.base);
        //COMPOSITED_BRDF_ADD(brdfs, TranslucentEntry, this->diffusionExponent);

    //! Surface is perfectly specular.
    if (this->roughness == 0.0f) 
        COMPOSITED_BRDF_ADD(brdfs, DielectricReflection, 1.0f, this->refraction.x, specularColor);

    //! Surface is partially or fully diffuse.
    if (this->roughness > 0.0f)
        COMPOSITED_BRDF_ADD(brdfs, Lambertian, diffuseColor);

    //! Specular is partially diffuse and specular.
    if (this->roughness < 1.0f && this->roughness > 0.0f) 
        COMPOSITED_BRDF_ADD(brdfs, Specular, specularColor, (1.0f - this->roughness) * 99.0f);

} 

void Uber__shadeDielectricExit(const uniform Uber *uniform this,
        DifferentialGeometry& dg,              
        uniform CompositedBRDF&     brdfs)
{

    //! Surface is perfectly transmissive.
    if (this->transmissionDepth >= 999999.0f) 
        CompositedBRDF__add(&brdfs,&this->transmission_io.base);

    //! Internal reflection in perfectly transmissive materials.
    if (this->transmissionDepth >= 999999.0f && this->roughness == 0.0f) 
        CompositedBRDF__add(&brdfs, &this->reflection_io.base);

    //! Surface is translucent.
    if (this->transmissionDepth < 999999.0f)
        CompositedBRDF__add(&brdfs, &this->translucentExit.base);
        //COMPOSITED_BRDF_ADD(brdfs, TranslucentExit, this->transmission, this->transmissionDepth,
        //                    this->numCoefficients, this->diffusionCoefficients);

} 

void Uber__shadeDielectric(const uniform Uber *uniform this,
        const Medium&  currentMedium, 
        DifferentialGeometry& dg,              
        uniform CompositedBRDF&     brdfs)
{
    //! Diffuse reflected color.
    const vec3f diffuseColor = mul(this->diffuse, Uber__sampleTexture(dg, this->diffuseMap));

    //! Specular reflected color.
    const vec3f specularColor = mul(this->specular, Uber__sampleTexture(dg, this->specularMap));

    //! Transmission into the material.
    if (eq(currentMedium, this->mediumOutside))
        Uber__shadeDielectricEntry(this, dg, brdfs, diffuseColor, specularColor);

    //! Transmission through the material and back out again.
    if (eq(currentMedium, this->mediumInside)) 
        Uber__shadeDielectricExit(this, dg, brdfs);

}

void Uber__shade(const uniform Material *uniform _this,
                  const Ray&                  ray,             
                  const Medium&               currentMedium,   
                  const DifferentialGeometry& dg,              
                  uniform CompositedBRDF&     brdfs)           
{
    const uniform Uber* uniform this = (const uniform Uber* uniform)_this;
  
    // Make a local copy of the differential geometry so that we can
    // change the normal
    DifferentialGeometry localdg = dg;

    //! Bend the surface normal according to the bump map.
    if (this->bumpMap)
        Uber__bendSurfaceNormal(this, ray, localdg);

    //! Material is fully opaque and is thus assumed to be a Fresnel conductor.
    if (eq(this->transmission, make_vec3f(0.0f))) 
        Uber__shadeConductor(this, localdg, brdfs);
        
    //! Material is partially or fully transparent and is thus assumed to be a dielectric.
    if (ne(this->transmission, make_vec3f(0.0f))) 
        Uber__shadeDielectric(this, currentMedium, localdg, brdfs);
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
  
  if (this->bumpMap)
	RefCount__IncRef(&this->bumpMap->base);

  if (this->diffuseMap)
	RefCount__IncRef(&this->diffuseMap->base);

  if (this->specularMap)
	RefCount__IncRef(&this->specularMap->base);
  
  Material__Destructor(_this);
}

void Uber__Constructor(uniform Uber *uniform this,
                        const uniform vec3f& absorption,
                        uniform Texture* uniform bumpMap,
                        const uniform vec3f& diffuse,
                        uniform Texture* uniform diffuseMap,
                        const uniform float diffusionExponent,
                        const uniform vec3f& refraction,
                        const uniform float roughness,
                        const uniform vec3f& specular,
                        uniform Texture* uniform specularMap,
                        const uniform vec3f& transmission,
                        const uniform float transmissionDepth,
                        const uniform int numCoefficients,
                        uniform struct vcc4f* uniform diffusionCoefficients)
{
  Material__Constructor(&this->base,Uber__Destructor,Uber__shade,Uber__selectNextMedium);
  if (bumpMap)
	RefCount__IncRef(&bumpMap->base);
  this->bumpMap = bumpMap;
  if (diffuseMap)
	RefCount__IncRef(&diffuseMap->base);
  this->diffuseMap = diffuseMap;
  if (specularMap)
	RefCount__IncRef(&specularMap->base);
  this->specularMap = specularMap;
  
  this->absorption = absorption;
  this->diffuse = diffuse;
  this->diffusionExponent = diffusionExponent;
  this->refraction = refraction;
  this->roughness = roughness;
  this->specular = specular;
  this->transmission = transmission;
  this->transmissionDepth = transmissionDepth;
  this->numCoefficients = numCoefficients;
  this->diffusionCoefficients = diffusionCoefficients;
  
  DielectricReflection__Constructor  (&this->reflection_io  ,refraction.x ,1.0f, make_vec3f(1.0f));

  DielectricTransmission__Constructor(&this->transmission_io, refraction.x, 1.0f, transmission);
  DielectricTransmission__Constructor(&this->transmission_oi, 1.0f, refraction.x, make_vec3f(1.0f));

  this->translucentEntry = make_TranslucentEntry(diffusionExponent);
  this->translucentExit = make_TranslucentExit(transmission, transmissionDepth,
                            numCoefficients, diffusionCoefficients);
  this->fresnel = make_FresnelConductor(refraction, absorption);
  this->powerdist = make_PowerCosineDistribution(rcp(roughness));
  
  // Set up purely for interface detection
  this->mediumOutside.eta          = 1.0f;
  this->mediumOutside.transmission = make_vec3f(1.0f);
  this->mediumInside.eta           = refraction.x;
  this->mediumInside.transmission  = make_vec3f(0.98765f);

}

///////////////////////////////////////////////////////////////////////////////
// External API

export void* uniform Uber__new(const uniform vec3f& absorption,
                                void* uniform bumpMap,
                                const uniform vec3f& diffuse,
                                void* uniform diffuseMap,
                                const uniform float diffusionExponent,
                                const uniform vec3f& refraction,
                                const uniform float roughness,
                                const uniform vec3f& specular,
                                void* uniform specularMap,
                                const uniform vec3f& transmission,
                                const uniform float transmissionDepth,
                                const uniform int numCoefficients,
                                void* uniform diffusionCoefficients)
{
  uniform Uber *uniform this = uniform new uniform Uber;
  
Uber__Constructor(this, absorption, (uniform Texture* uniform)bumpMap, diffuse,
    (uniform Texture* uniform)diffuseMap, diffusionExponent, refraction,
    roughness, specular, (uniform Texture* uniform)specularMap, transmission,
    transmissionDepth, numCoefficients, 
    (uniform struct vcc4f* uniform)diffusionCoefficients);

  return this;
}
