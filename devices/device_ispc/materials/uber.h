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

#pragma once
#include <iostream>
#include "api/parms.h"
#include "uber_ispc.h"

//! This class can be used to model a wide range of physical material
//! types including Lambertian, metals, fully transparent media, and
//! translucency with simple scattering.
//!
namespace embree
{
  struct Uber
  {
    struct vc4f 
    {
      float x;
      float y;
      float z;
      float w;
    };
    
        //! Load an array of Vec4f values.
    static void getFloat4Array(Variant stream, struct vc4f * &array, 
        int &numCoefficients, const Vec4f &defaultValue) 
    {
        int numElements;
        
        //! Set a default if no values were specified.
        if (!stream.data) 
        { 
            numCoefficients = 1; 
            array = (struct vc4f *)malloc(sizeof(struct vc4f));
            array[0].x = defaultValue.x;
            array[0].y = defaultValue.y;
            array[0].z = defaultValue.z;
            array[0].w = defaultValue.w;  
            return; 
        }
        
        //! Allocate storage for the array.
        numElements = stream.data->size();

        array = (struct vc4f *)malloc(sizeof(struct vc4f) * numElements);

        //! Transcribe the values.
        for (size_t i=0 ; i < numElements ; i++) 
        {
            Vec4f layer = stream.data->getVector4f(i);
            array[i].x = layer.x;
            array[i].y = layer.y;
            array[i].z = layer.z;
            array[i].w = layer.w;
        }
        
        numCoefficients = numElements;
    }  // getFloat4Array
    
    static void* create(const Parms& parms)
    {
        //! Absorption coefficient.
        Color absorption = parms.getColor("absorption", Color(0.0f));

        //! Bump map.
        ISPCRef bumpMap = parms.getTexture("bumpMap", NULL);

        //! Diffuse reflectance coefficient and map.
        Color diffuse = parms.getColor("diffuse", Color(0.0f));  
        ISPCRef diffuseMap = parms.getTexture("diffuseMap", NULL);
        
        //! Multilayer diffusion coefficients.
        struct vc4f* diffusionCoefficients;
        int numCoefficients;
        // BUG BUG BUG - Nice memory leak - these are not deallocated
        getFloat4Array(parms.getData("diffusionCoefficients"), diffusionCoefficients, 
                numCoefficients, Vec4f(1.0f));
        
        //! Diffusion scattering exponent.
        float diffusionExponent = parms.getFloat("diffusionExponent", 100.0f);

        //! Index of refraction can be specified as a scalar or vector float.
        //! It's up to the plugin to send fown the one that is wanted.
        Vec3f refraction = (parms.getFloat("refraction", -1.0f) < 0.0f) ? 
                    parms.getVector3f("refraction", Vector3f(1.0f)) : 
                    Vec3f(parms.getFloat("refraction"));

        //! Surface roughness.
        float roughness = parms.getFloat("roughness", 0.0f);

        //! Specular reflectance coefficient and map.
        Color specular = parms.getColor("specular", Color(0.0f));  
        ISPCRef specularMap = parms.getTexture("specularMap", NULL);

        //! Transmitted radiance coefficient.
        Color transmission = parms.getColor("transmission", Color(0.0f));

        //! Transmitted radiance penetration depth.
        float transmissionDepth = parms.getFloat("transmissionDepth", float(9999999.0f));

        return ispc::Uber__new((ispc::vec3f&)absorption,
                                bumpMap.ptr,
                                (ispc::vec3f&)diffuse,
                                diffuseMap.ptr,
                                diffusionExponent,
                                (ispc::vec3f&)refraction,
                                roughness,
                                (ispc::vec3f&)specular,
                                specularMap.ptr,
                                (ispc::vec3f&)transmission,
                                transmissionDepth,
                                numCoefficients,
                                diffusionCoefficients);
    }  // create
    
/*    
    ~Uber()
    {
        // Would that this worked...
        if ((numCoefficients > 0) && NULL != (diffusionCoefficients))
        {
            free(diffusionCoefficients);
            diffusionCoefficients = NULL;
            numCoefficients = 0;
        }
    }
 */
  };  // Uber  
}

