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

#include "../brdfs/conductor.h"
#include "../brdfs/dielectric.h"
#include "../brdfs/lambertian.h"
#include "../brdfs/microfacet.h"
#include "../brdfs/specular.h"
#include "../brdfs/translucent.h"
#include "../materials/material.h"
#include "../textures/texture.h"
#include <iostream>
#include "stdio.h"

namespace embree {

    //! This class can be used to model a wide range of physical material
    //! types including Lambertian, metals, fully transparent media, and
    //! translucency with simple scattering.
    //!
    class Uber : public Material {
    public:

        Uber(const Parms &options) {

            //! Absorption coefficient.
            absorption = options.getColor("absorption", Color(0.0f));

            //! Bump map.
            bumpMap = options.getTexture("bumpMap", NULL);

            //! Diffuse reflectance coefficient and map.
            diffuse = options.getColor("diffuse", Color(0.0f));  
            diffuseMap = options.getTexture("diffuseMap", NULL);

            //! Multilayer diffusion coefficients.
            getFloat4Array(options.getData("diffusionCoefficients"), diffusionCoefficients, 
                    Vec4f(1.0f));

            //! Diffusion scattering exponent.
            diffusionExponent = options.getFloat("diffusionExponent", 100.0f);

            //! Index of refraction can be specified as a scalar or vector float.
            refraction = (options.getFloat("refraction", -1.0f) < 0.0f) ? 
                options.getColor("refraction", Color(1.0f)) : 
                Color(options.getFloat("refraction"));

            //! Surface roughness.
            roughness = options.getFloat("roughness", 0.0f);

            //! Specular reflectance coefficient and map.
            specular = options.getColor("specular", Color(0.0f));  
            specularMap = options.getTexture("specularMap", NULL);

            //! Transmitted radiance coefficient.
            transmission = options.getColor("transmission", Color(0.0f));

            //! Transmitted radiance penetration depth.
            transmissionDepth = options.getFloat("transmissionDepth", float(9999999.0f));

            //! These settings are only used to detect when the medium changes.
            if (transmission != Color(0.0f)) { mediumInside.transmission = Color(0.9995f);  isMediaInterface = true; }
#if 0            
std::cout << "diffuse " << diffuse << " roughness = " << roughness << " specular = " 
   << specular << "\n   transmission = " << transmission << "\n";
#endif
        }

        inline void bendSurfaceNormal(const DifferentialGeometry &hit) const {

            //! Tangent space normal from the texture map.
            const Color texel = bumpMap->get(hit.st);

            //! Expand the normal range from [0, 1] to [-1, 1].
            const Vector3f bend(2.0f * texel.r - 1.0f, 2.0f * texel.g - 1.0f, 2.0f * texel.b - 1.0f);

            //! Bend the surface normal at the hit point.
            hit.Ns = normalize(hit.Ns + normalize(bend.x * hit.Tx + bend.y * hit.Ty + bend.z * hit.Ns));

        }

        inline Color sampleTexture(const DifferentialGeometry &hit, const Ref<Texture> texture) const {

            //! Convenience routine which always returns a valid Color.
            return((texture != NULL) ? texture->get(hit.st) : Color(1.0f));

        }

        void shade(const Ray &ray, const Medium &currentMedium, const DifferentialGeometry &hit, CompositedBRDF &brdfs) const {

            //! Bend the surface normal according to the bump map.
            if (bumpMap != NULL) 
                bendSurfaceNormal(hit);

            //! Material is fully opaque and is thus assumed to be a Fresnel conductor.
            if (transmission == Color(0.0f)) 
                shadeConductor(hit, brdfs);

            //! Material is partially or fully transparent and is thus assumed to be a dielectric.
            if (transmission != Color(0.0f)) 
                shadeDielectric(currentMedium, hit, brdfs);

        }

        void shadeConductor(const DifferentialGeometry &hit, CompositedBRDF &brdfs) const {

            //! Diffuse reflected color.
            Color diffuseColor = diffuse * sampleTexture(hit, diffuseMap);

            //! Specular reflected color.
            Color specularColor = specular * sampleTexture(hit, specularMap);

            //! Surface is perfectly specular.
            if (roughness == 0.0f) 
                brdfs.add(NEW_BRDF(Conductor)(specularColor, refraction, absorption));

            //! Surface is partially or fully diffuse.
            if (roughness > 0.0f) 
                brdfs.add(NEW_BRDF(Lambertian)(diffuseColor));

            //! Surface is partially diffuse and specular.
            if (roughness < 1.0f && roughness > 0.0f) 
                brdfs.add(NEW_BRDF(MicrofacetConductor)(specularColor, 
                        FresnelConductor(refraction, absorption), 
                        PowerCosineDistribution(rcp(roughness), hit.Ns)));

        }

        void shadeDielectric(const Medium &currentMedium, const DifferentialGeometry &hit, 
            CompositedBRDF &brdfs) const 
        {

            //! Diffuse reflected color.
            Color diffuseColor = diffuse * sampleTexture(hit, diffuseMap);

            //! Specular reflected color.
            Color specularColor = specular * sampleTexture(hit, specularMap);

            //! Transmission into the material.
            if (currentMedium == mediumOutside) 
                shadeDielectricEntry(hit, brdfs, diffuseColor, specularColor);

            //! Transmission through the material.
            if (currentMedium == mediumInside) 
                shadeDielectricExit(hit, brdfs, specularColor);

        }

        void shadeDielectricEntry(const DifferentialGeometry &hit, CompositedBRDF &brdfs, 
            const Color &diffuseColor, const Color &specularColor) const 
        {

            //! Surface is perfectly transmissive.
            if (transmissionDepth >= 999999.0f) 
                brdfs.add(NEW_BRDF(DielectricTransmission)(1.0f, refraction.r));

            //! Surface is translucent.
            if (transmissionDepth < 999999.0f) 
                brdfs.add(NEW_BRDF(TranslucentEntry)(diffusionExponent));

            //! Surface is perfectly specular.
            if (roughness == 0.0f) 
                brdfs.add(NEW_BRDF(DielectricReflection)(1.0f, refraction.r, specularColor));

            //! Surface is partially or fully diffuse.
            if (roughness > 0.0f) 
                brdfs.add(NEW_BRDF(Lambertian)(diffuseColor));
            
            //! Specular is partially diffuse and specular.
            if (roughness < 1.0f && roughness > 0.0f) 
                brdfs.add(NEW_BRDF(Specular)(specularColor, (1.0f - roughness) * 99.0f));

        }

        void shadeDielectricExit(const DifferentialGeometry &hit, CompositedBRDF &brdfs, 
            const Color &specularColor) const 
        {

            //! Surface is perfectly transmissive.
            if (transmissionDepth >= 999999.0f) 
                brdfs.add(NEW_BRDF(DielectricTransmission)(refraction.r, 1.0f, transmission));

            //! Internal reflection in perfectly transmissive materials.
           if (transmissionDepth == 999999.0f && roughness == 0.0f) 
                brdfs.add(NEW_BRDF(DielectricReflection)(refraction.r, 1.0f));

            //! Surface is translucent.
            if (transmissionDepth < 999999.0f) 
                brdfs.add(NEW_BRDF(TranslucentExit)(transmission, transmissionDepth, 
                        diffusionCoefficients));

        }

    protected:

        //! Microfacet specialization for Fresnel conductors.
        typedef Microfacet<FresnelConductor, PowerCosineDistribution> MicrofacetConductor;

        //! Absorption coefficient.
        Color absorption;

        //! Bump map.
        Ref<Texture> bumpMap;

        //! Diffuse reflectance coefficient and map.
        Color diffuse;  
        Ref<Texture> diffuseMap;

        //! Multilayer diffusion coefficients.
        vector_t<Vec4f> diffusionCoefficients;

        //! Diffusion scattering exponent.
        float diffusionExponent;

        //! Index of refraction.
        Color refraction;

        //! Surface roughness.
        float roughness;

        //! Specular reflectance coefficient and map.
        Color specular;  
        Ref<Texture> specularMap;

        //! Transmitted radiance coefficient.
        Color transmission;

        //! Transmitted radiance penetration depth.
        float transmissionDepth;

        //! Load an array of Vec4f values.
        static void getFloat4Array(Variant stream, vector_t<Vec4f> &array, const Vec4f &defaultValue) {

            //! Set a default if no values were specified.
            if (!stream.data) { array.resize(1);  array[0] = defaultValue;  return; }

            //! Allocate storage for the array.
            array.resize(stream.data->size());

            //! Transcribe the values.
            for (size_t i=0 ; i < stream.data->size() ; i++) array[i] = stream.data->getVector4f(i);

        }

    };

}

#endif // __EMBREE_UBER_H__

