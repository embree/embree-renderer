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

#ifndef __EMBREE_TRANSLUCENT_BRDF_H__
#define __EMBREE_TRANSLUCENT_BRDF_H__

#include "../brdfs/brdf.h"

namespace embree {

    //! A BRDF for initializing a transmission ray into a translucent material
    //! from a hit point on the surface.  The hemisphere beneath the point is
    //! randomly sampled according to a power cosine distribution.
    //!
    class TranslucentEntry : public BRDF {
    public:

        __forceinline TranslucentEntry(float diffusionExponent)
            : BRDF(SPECULAR_TRANSMISSION), diffusionExponent(diffusionExponent) {}

        __forceinline Color eval(const Vector3f &wo, const DifferentialGeometry &hit, 
                                const Vector3f &wi) const
            { return(zero); }

        __forceinline Color sample(const Vector3f &wo, const DifferentialGeometry &hit, 
                                Sample3f &wi, const Vec2f &s) const
            { 
              //wi = Sample3f(-wo, 1.0f);
              wi = powerCosineSampleHemisphere(s.x, s.y, -wo, diffusionExponent);  
              wi.pdf = 1.0f;  
              return(one); 
            }

        __forceinline float pdf(const Vector3f &wo, const DifferentialGeometry &hit, const Vector3f &wi) const
            { return(zero); }

    private:

        //! Diffusion scattering exponent.
        float diffusionExponent;

    };

    //! A BRDF for collecting radiance on the surface of a translucent material
    //! at the exit point of a transmitted ray.  The radiance is attenuated by
    //! the distance the ray traveled through the material using a multilayer
    //! diffusion profile specified by the user.
    //!
    class TranslucentExit : public BRDF {
    public:

        __forceinline TranslucentExit(const Color &transmission, float transmissionDepth, 
                const vector_t<Vec4f> &diffusionCoefficients) :
                BRDF(SPECULAR_TRANSMISSION), diffusionCoefficients(diffusionCoefficients), 
                transmission(transmission), transmissionDepth(transmissionDepth) {}

        __forceinline Color eval(const Vector3f &wo, const DifferentialGeometry &hit, 
                const Vector3f &wi) const
            { 
                return(transmission * diffusionFunction(hit.t / transmissionDepth)); 
            }

        __forceinline Color sample(const Vector3f &wo, const DifferentialGeometry &hit, 
                Sample3f &wi, const Vec2f &s) const
            { 
                wi = -wo;  
                return(eval(wo, hit, wi)); 
            }

        __forceinline float pdf(const Vector3f &wo, const DifferentialGeometry &hit, 
                const Vector3f &wi) const
            { return(zero); }

    private:

        //! Multilayer diffusion coefficients.
        const vector_t<Vec4f> &diffusionCoefficients;

        //! Transmitted radiance coefficient.
        Color transmission;

        //! Transmitted radiance penetration depth.
        float transmissionDepth;

        //! Diffusion function.
        Color diffusionFunction(float distance) const {

            //! Color defaults to black.
            Color diffused = zero;  
            for (size_t i=0 ; i < diffusionCoefficients.size() ; i++) {

                //! Gaussian blur weights (x, y, z) and variance (w).
                const Vec4f &coefficient = diffusionCoefficients[i];

                //! Compute a sum-of-Gaussians.
                diffused += Color(coefficient.x, coefficient.y, coefficient.z) * Color(exp(-distance * distance / coefficient.w));

            } return(diffused);

        }

    };

}

#endif  // __EMBREE_TRANSLUCENT_BRDF_H__
