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

#include "api/parms.h"
#include "uber_ispc.h"

namespace embree
{
  struct Uber
  {
    static void* create(const Parms& parms)
    {
		const Color diffColor = parms.getColor("diffColor",one);
		const Color reflColor = parms.getColor("reflColor",one);
		ISPCRef Kd = parms.getTexture("diffTexture");
		const Vec2f s0 = parms.getVec2f("s0",Vec2f(0.0f,0.0f));
		const Vec2f ds = parms.getVec2f("ds",Vec2f(1.0f,1.0f));
		const float surfEta = parms.getFloat("surfEta",1.4f);
		const float roughness = parms.getFloat("roughness",0.01f);
		const float etaOutside = parms.getFloat("etaOutside",1.0f);
		const float etaInside  = parms.getFloat("etaInside",1.4f);
		const Color transmissionOutside = parms.getColor("transColorOutside",one);
		const Color transmissionInside  = parms.getColor("transColor",one);

      return ispc::Uber__new((ispc::vec3f&)diffColor,
								(ispc::vec3f&)reflColor,
								Kd.ptr,
								(ispc::vec2f&)s0,
								(ispc::vec2f&)ds,
								surfEta,
								roughness,
								etaOutside,
								(ispc::vec3f&)transmissionOutside,
                                etaInside,
								(ispc::vec3f&)transmissionInside);
    }
  };
}

