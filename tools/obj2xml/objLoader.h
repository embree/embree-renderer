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

#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <map>
#include <string>
#include <vector>

struct Col3f { float r, g, b; };
struct Vec2f { float x, y;    };
struct Vec3f { float x, y, z; };
struct Vec3i { int   i, j, k; };

struct Material {

  std::string name;
  std::string map_d;   float d;     /*! opacity value        */
  std::string map_Ka;  Col3f Ka;    /*! ambient color        */
  std::string map_Kd;  Col3f Kd;    /*! diffuse color        */
  std::string map_Ks;  Col3f Ks;    /*! specular color       */
  std::string map_Ns;  float Ns;    /*! specular coefficient */
  std::string map_Bump;             /*! bump map */
  
  Material() {
    
    Ka.r = 0.5f, Ka.g = 0.5f, Ka.b = 0.5f;  d  = 1.0f;
    Kd.r = 0.5f, Kd.g = 0.5f, Kd.b = 0.5f;  Ns = 0.0f;
    Ks.r = 0.0f, Ks.g = 0.0f, Ks.b = 0.0f;
    
  }
  
};

struct Mesh {

    std::vector<Vec3f> positions;
    std::vector<Vec3f> normals;
    std::vector<Vec2f> texcoords;
    std::vector<Vec3i> triangles;
    Material           material;

};

class OBJLoader {
public:

    /*! loaded geometry */
    std::vector<Mesh> model;

    /*! constructor */
    OBJLoader(FILE *objFile);

private:

    /*! bookkeeping buffers */
    std::vector<Vec3f> v;
    std::vector<Vec3f> vn;
    std::vector<Vec2f> vt;

    /*! materials library */
    std::map<std::string, Material> materials;

    /*! parses a string */
    char* parseString(const char* in, char* out);

    /*! append unique vertices to the mesh */
    uint32_t appendVertex(const Vec3i &vertex, Mesh &mesh, std::map<Vec3i, uint32_t> &vertexMap);

    /*! write out the face list defining the current mesh */
    void flushFaceGroup(std::vector<std::vector<Vec3i> > &faceGroup, const std::string materialName);

    /*! store and reset the current material */
    void flushMaterial(Material &material, const std::string materialName);

    /*! load attribute buffer indices for each vertex in the current face */
    void loadFace(char *line, std::vector<Vec3i> &face);

    /*! load an OBJ material library */
    void loadMTL(const std::string libraryName);

};

inline std::vector<Mesh> loadOBJ(const char *fileName) {

    /*! open the OBJ file */
    FILE *objFile = fopen(fileName, "r");  if (!objFile) printf("  ERROR:  unable to open %s\n", fileName), exit(1);

    /*! the main loader routine */
    OBJLoader loader(objFile);  fclose(objFile);  return(loader.model);

}

#endif
