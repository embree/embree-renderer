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
#include "math/vec3.h"
#include "math/affinespace.h"
#include "../obj2xml/objLoader.h"
#include "../../devices/device/loaders/xml_parser.h"

namespace embree
{
  class VRMLLoader {
  public:

    typedef AffineSpaceT<LinearSpace3<Vec3f> > AffineSpace3f;
    
    /*! loaded geometry */
    Ref<Scene> scene;

    /*! mapping from name to material */
    std::map<std::string,Ref<Material> > name2material;

    /*! default material */
    Ref<Material> defaultMaterial;
    
    /*! constructor */
    VRMLLoader(const char* fileName);

  private:
    std::vector<Vec3f> parsePointArray(Ref<Stream<Token> >& cin, const AffineSpace3f& space);
    std::vector<Vec3f> parseNormalArray(Ref<Stream<Token> >& cin, const AffineSpace3f& space);
    std::vector<Vec2f> parseTexCoordArray(Ref<Stream<Token> >& cin);
    std::vector<Vec3i> parseTriangleArray(Ref<Stream<Token> >& cin);
    Ref<Mesh> parseIndexedFaceSet(Ref<Stream<Token> >& cin, const AffineSpace3f& space);
    Ref<Mesh> parseGeometry(Ref<Stream<Token> >& cin, const AffineSpace3f& space);
    Ref<Material> parseMaterial(Ref<Stream<Token> >& cin, const std::string& name);
    Ref<Material> parseAppearance(Ref<Stream<Token> >& cin);
    void parseShape(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible);
    void parseChildrenList(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible);
    void parseChildren(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible);
    void parseTransform(Ref<Stream<Token> >& cin, const AffineSpace3f& space_in, bool visible);
    void parseGroup(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible);
    void parseNode(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible);
  };
}

#endif
