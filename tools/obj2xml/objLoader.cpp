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

#include <string.h>
#include <strings.h>
#include "objLoader.h"
#include <stdio.h>

static inline bool operator < (const Vec3i &a, const Vec3i &b) {

    if (a.i != b.i) return(a.i < b.i);
    if (a.j != b.j) return(a.j < b.j);
    if (a.k != b.k) return(a.k < b.k);
    return(false);

}

OBJLoader::OBJLoader(FILE *objFile) {

    /*! bookkeeping buffer */
    std::vector<std::vector<Vec3i> > faceGroup;

    /*! current mesh material name */
    char materialName[1024];  sprintf(materialName, "default");

    /*! iterate over lines of the file, flush the face group on EOF */
    for (char line[1024] ; fgets(line, 1024, objFile) ? true : (flushFaceGroup(faceGroup, materialName), false); ) {

        /*! acquire the first token on this line */
        char token[1024];  if (!sscanf(line, "%s", token)) continue;

        /*! face definition */
        if (!strcasecmp(token, "f")) { std::vector<Vec3i> face;  loadFace(line, face);  faceGroup.push_back(face); }

        /*! load material library */
        if (!strcasecmp(token, "mtllib")) { char libraryName[1024];  sscanf(line, "%*s %s", libraryName);  loadMTL(libraryName); }

        /*! use material */
        if (!strcasecmp(token, "usemtl")) { flushFaceGroup(faceGroup, materialName);  sscanf(line, "%*s %s", materialName); }

        /*! vertex coordinates */
        if (!strcasecmp(token, "v"))  { Vec3f value;  sscanf(line, "%*s %f %f %f", &value.x, &value.y, &value.z);  v.push_back(value); }

        /*! vertex normal */
        if (!strcasecmp(token, "vn")) { Vec3f value;  sscanf(line, "%*s %f %f %f", &value.x, &value.y, &value.z);  vn.push_back(value); }

        /*! texture coordinates */
        if (!strcasecmp(token, "vt")) { Vec2f value;  sscanf(line, "%*s %f %f", &value.x, &value.y);  vt.push_back(value); }

    }

}

uint32_t OBJLoader::appendVertex(const Vec3i &vertex, Mesh &mesh, std::map<Vec3i, uint32_t> &vertexMap) {

    /*! determine if we've seen this vertex before */
    const std::map<Vec3i, uint32_t>::iterator &entry = vertexMap.find(vertex);

    /*! two vertices match only if positions, normals, and texture coordinates match */
    if (entry != vertexMap.end()) return(entry->second);

    /*! this is a new vertex, store the indices */
    if (vertex.i >= 0) mesh.positions.push_back(v[vertex.i]);
    if (vertex.j >= 0) mesh.normals.push_back(vn[vertex.j]);
    if (vertex.k >= 0) mesh.texcoords.push_back(vt[vertex.k]);

    /*! map this vertex to a unique id */
    return(vertexMap[vertex] = int(mesh.positions.size()) - 1);

}

void OBJLoader::flushFaceGroup(std::vector<std::vector<Vec3i> > &faceGroup, const std::string materialName) {

    /*! temporary storage */
    std::map<Vec3i, uint32_t> vertexMap;

    /*! mesh that will be constructed from this face group */
    Mesh mesh;  mesh.material = materials[materialName];

    /*! construct a mesh for this face group */
    for (size_t face=0 ; face < faceGroup.size() ; face++) {

        /*! triangulate the face with a triangle fan */
        for (size_t i=0, j=1, k=2 ; k < faceGroup[face].size() ; j++, k++) {

            Vec3i triangle;
            triangle.i = appendVertex(faceGroup[face][i], mesh, vertexMap);
            triangle.j = appendVertex(faceGroup[face][j], mesh, vertexMap);
            triangle.k = appendVertex(faceGroup[face][k], mesh, vertexMap);
            mesh.triangles.push_back(triangle);

        }
    } 

    /* fix some materials */
    if (//mesh.material.name == "BeltStrap1" ||
        //mesh.material.name == "BeltBuckle1" || 
        //mesh.material.name == "Belt1" || 
        mesh.material.name == "JeansStraps1" ||
        //mesh.material.name == "JeansButton1" || 
        mesh.material.name == "Jeans1")// ||
        //mesh.material.name == "2_SkinHip") 
    {
      mesh.material.Ks.r = 0; mesh.material.Ks.g = 0; mesh.material.Ks.b = 0;
    }
    
    /* fix some texture coordinate issues */
    for (size_t i=0; i<mesh.texcoords.size(); i++)
      mesh.texcoords[i].y = 1.0f-mesh.texcoords[i].y;
    
    /*! append the mesh to the model */
    if (faceGroup.size()) model.push_back(mesh);  
    faceGroup.clear();
}

void OBJLoader::flushMaterial(Material &material, const std::string materialName) {

  if (strstr(material.name.c_str(),"Skin")) {
    material.Ks.r = 1.0f; material.Ks.g = 1.0f; material.Ks.b = 1.0f; 
  }

    /*! store the material */
    materials[materialName] = material;

    /*! clear the material */
    material = Material();

}

void OBJLoader::loadFace(char *line, std::vector<Vec3i> &face) {

    for (char *token = strtok(line, " f\t\r\n") ; token ; token = strtok(NULL, " \t\r\n")) {

        /*! vertex is defined as indices into position, normal, texture coordinate buffers */
        Vec3i vertex;  vertex.i = -1, vertex.j = -1, vertex.k = -1;

        /*! vertex has texture coordinates and a normal */
        if (sscanf(token, "%d/%d/%d", &vertex.i, &vertex.k, &vertex.j) == 3) {

            vertex.i = (vertex.i > 0) ? vertex.i - 1 : (vertex.i == 0 ? 0 :  v.size() + vertex.i);
            vertex.j = (vertex.j > 0) ? vertex.j - 1 : (vertex.j == 0 ? 0 : vn.size() + vertex.j);
            vertex.k = (vertex.k > 0) ? vertex.k - 1 : (vertex.k == 0 ? 0 : vt.size() + vertex.k);
            face.push_back(vertex);

        /*! vertex has a normal */
        } else if (sscanf(token, "%d//%d", &vertex.i, &vertex.j) == 2) {

            vertex.i = (vertex.i > 0) ? vertex.i - 1 : (vertex.i == 0 ? 0 :  v.size() + vertex.i);
            vertex.j = (vertex.j > 0) ? vertex.j - 1 : (vertex.j == 0 ? 0 : vn.size() + vertex.j);
            face.push_back(vertex);

        /*! vertex has texture coordinates */
        } else if (sscanf(token, "%d/%d", &vertex.i, &vertex.k) == 2) {

            vertex.i = (vertex.i > 0) ? vertex.i - 1 : (vertex.i == 0 ? 0 :  v.size() + vertex.i);
            vertex.k = (vertex.k > 0) ? vertex.k - 1 : (vertex.k == 0 ? 0 : vt.size() + vertex.k);
            face.push_back(vertex);

        /*! vertex has no texture coordinates or normal */
        } else if (sscanf(token, "%d", &vertex.i) == 1) {

            vertex.i = (vertex.i > 0) ? vertex.i - 1 : (vertex.i == 0 ? 0 : v.size() + vertex.i);
            face.push_back(vertex);

        }

    }

}

char* OBJLoader::parseString(const char* in, char* out)
{
  in+=strspn(in, " \t");
  if (in[0] == '\"') in++;
  strcpy(out,in);
  while (true) {
    size_t len = strlen(out);
    if (len == 0) return NULL;
    if (out[len-1] != '\"' && out[len-1] != ' ' && out[len-1] != '\r' && out[len-1] != '\n') break;
    out[len-1] = 0;
  }

  if (out[0] == '/') out++;
  const char* apath = "C:/Users/swoop/Documents/DAZ 3D/Studio/My Library/Runtime/";
  if (strcasestr(out,apath) == out) {
    out+=strlen(apath);
    if (out[0] == 'T') out[0] = 't';
  }
  return out;
}

void OBJLoader::loadMTL(const std::string libraryName) {

    /*! open the MTL file */
    FILE *mtlFile = fopen(libraryName.c_str(), "r");  if (!mtlFile) { printf("  ERROR:  unable to open %s\n", libraryName.c_str());  return; }

    /*! current material and name */
    Material material;  char materialName[1024];  sprintf(materialName, "default");

    /*! iterate over lines of the file, store the current material on EOF */
    for (char line[1024] ; fgets(line, 1024, mtlFile) ? true : (flushMaterial(material, materialName), false); ) {

        /*! acquire the first token on this line */
        char token[1024];  if (!sscanf(line, "%s", token)) continue;

        /*! ignore comments */
        if (!strcmp(token, "#")) continue;

        /*! opacity value */
        if (!strcasecmp(token, "d")) { sscanf(line, "%*s %f", &material.d); }

        /*! ambient color */
        if (!strcasecmp(token, "Ka")) { sscanf(line, "%*s %f %f %f", &material.Ka.r, &material.Ka.g, &material.Ka.b); }

        /*! diffuse color */
        if (!strcasecmp(token, "Kd")) { sscanf(line, "%*s %f %f %f", &material.Kd.r, &material.Kd.g, &material.Kd.b); }

        /*! specular color */
        if (!strcasecmp(token, "Ks")) { sscanf(line, "%*s %f %f %f", &material.Ks.r, &material.Ks.g, &material.Ks.b); }

        /*! opacity texture */
        if (!strcasecmp(token, "map_d")) { char textureName[1024];  material.map_d = parseString(line+5,textureName); }

        /*! ambient color texture */
        if (!strcasecmp(token, "map_Ka")) { char textureName[1024];  material.map_Ka = parseString(line+6,textureName); }

        /*! diffuse color texture */
        if (!strcasecmp(token, "map_Kd")) { char textureName[1024];  material.map_Kd = parseString(line+6,textureName); }

        /*! specular color texture */
        if (!strcasecmp(token, "map_Ks")) { char textureName[1024];  material.map_Ks = parseString(line+6,textureName); }

        /*! specular coefficient texture */
        if (!strcasecmp(token, "map_Ns")) { char textureName[1024];  material.map_Ns = parseString(line+6,textureName); }

        /*! bump map */
        if (!strcasecmp(token, "map_Bump")) { char textureName[1024];  material.map_Bump = parseString(line+8,textureName); }

        /*! new material delimiter */
        if (!strcasecmp(token, "newmtl")) { flushMaterial(material, materialName);  sscanf(line, "%*s %s", materialName); material.name = materialName; }

        /*! specular coefficient */
        if (!strcasecmp(token, "Ns")) { sscanf(line, "%*s %f", &material.Ns); }

    } fclose(mtlFile);

}

