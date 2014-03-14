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

#ifndef __VRML_LOADER_H__
#define __VRML_LOADER_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include "../obj2xml/objLoader.h"
#include "../../devices/device/loaders/xml_parser.h"

namespace embree
{
  class VRMLLoader {
  public:
    
    /*! loaded geometry */
    std::vector<Mesh> model;
    
    /*! constructor */
    VRMLLoader(const char* fileName);

  private:
    
    /*! materials library */
    std::map<std::string, Material> materials;
  };
  
  std::vector<Mesh> loadVRML(const char *fileName);
}

#endif
