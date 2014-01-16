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

#include "../../devices/device/loaders/xml_parser.h"
#include "image/image.h"
#include "math/affinespace.h"
#include "math/color.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include <stack>

#include <stdio.h>
#include <stdlib.h>

namespace embree
{
  class XML2OBJ
  {
  public:

    XML2OBJ(const FileName& fileName, const FileName& objFileName, const FileName& mtlFileName);
   ~XML2OBJ();

  public:
    void loadPointLight(const Ref<XML>& xml);
    void loadSpotLight(const Ref<XML>& xml);
    void loadDirectionalLight(const Ref<XML>& xml);
    void loadDistantLight(const Ref<XML>& xml);
    void loadAmbientLight(const Ref<XML>& xml);
    void loadTriangleLight(const Ref<XML>& xml);
    void loadHDRILight(const Ref<XML>& xml);
    float loadMaterialParmFloat(const Ref<XML>& parms, std::string tag, float def);
    Vec3f loadMaterialParmVec3f(const Ref<XML>& parms, std::string tag, Vec3f def);
    int loadMaterial(const Ref<XML>& xml);
    void loadTriangleMesh(const Ref<XML>& xml);
    void loadSphere(const Ref<XML>& xml);
    void loadDisk(const Ref<XML>& xml);
    void loadQuadLight(const Ref<XML>& xml);
    void loadScene(const Ref<XML>& xml);
    void loadTransformNode(const Ref<XML>& xml);
    void loadGroupNode(const Ref<XML>& xml);

  private:
    template<typename T> T load(const Ref<XML>& xml) { return T(zero); }
    template<typename T> T load(const Ref<XML>& xml, T opt) { return T(zero); }
    template<typename T> std::vector<T> loadBinary(const Ref<XML>& xml);
    std::vector<Vec2f> loadVec2fArray(const Ref<XML>& xml);
    std::vector<Vec3f> loadVec3fArray(const Ref<XML>& xml);
    std::vector<Vec3i> loadVec3iArray(const Ref<XML>& xml);

  private:
    FileName path;         //!< path to XML file
    FILE* binFile;         //!< .bin file for reading binary data
    FileName binFileName;  //!< name of the .bin file

    FILE* objFile;
    FILE* mtlFile;
    int nextVertexID;
    int nextTexCoordID;
    int nextNormalID;
    int nextMaterialID;

  private:
    std::stack<AffineSpace3f> transforms;                              //!< stack of transformations
    std::map<std::string,int> materialMap;              //!< named materials
    std::map<Ref<XML>,int> materialCache;              //!< map for detecting repeated materials
    //std::map<std::string,std::vector<Handle<Device::RTPrimitive> > > sceneMap;  //!< named parts of the scene

  public:
    //std::vector<Handle<Device::RTPrimitive> > model;   //!< stores the output scene
  };

  //////////////////////////////////////////////////////////////////////////////
  //// Loading standard types from an XML node
  //////////////////////////////////////////////////////////////////////////////

  template<> std::string XML2OBJ::load<std::string>(const Ref<XML>& xml) {
    if (xml->body.size() < 1) throw std::runtime_error(xml->loc.str()+": wrong string body");
    return xml->body[0].String();
  }

  template<> bool XML2OBJ::load<bool>(const Ref<XML>& xml, bool opt) {
    if (xml == null) return opt;
    if (xml->body.size() != 1) throw std::runtime_error(xml->loc.str()+": wrong bool body");
    return xml->body[0].Int() != 0;
  }

  template<> int XML2OBJ::load<int>(const Ref<XML>& xml) {
    if (xml->body.size() != 1) throw std::runtime_error(xml->loc.str()+": wrong int body");
    return xml->body[0].Int();
  }

  template<> Vec2i XML2OBJ::load<Vec2i>(const Ref<XML>& xml) {
    if (xml->body.size() != 2) throw std::runtime_error(xml->loc.str()+": wrong int2 body");
    return Vec2i(xml->body[0].Int(),xml->body[1].Int());
  }

  template<> Vec3i XML2OBJ::load<Vec3i>(const Ref<XML>& xml) {
    if (xml->body.size() != 3) throw std::runtime_error(xml->loc.str()+": wrong int3 body");
    return Vec3i(xml->body[0].Int(),xml->body[1].Int(),xml->body[2].Int());
  }

  template<> Vec4i XML2OBJ::load<Vec4i>(const Ref<XML>& xml) {
    if (xml->body.size() != 4) throw std::runtime_error(xml->loc.str()+": wrong int4 body");
    return Vec4i(xml->body[0].Int(),xml->body[1].Int(),xml->body[2].Int(),xml->body[3].Int());
  }

  template<> float XML2OBJ::load<float>(const Ref<XML>& xml) {
    if (xml->body.size() != 1) throw std::runtime_error(xml->loc.str()+": wrong float body");
    return xml->body[0].Float();
  }

  template<> float XML2OBJ::load<float>(const Ref<XML>& xml, float opt) {
    if (xml == null) return opt;
    if (xml->body.size() != 1) throw std::runtime_error(xml->loc.str()+": wrong float body");
    return xml->body[0].Float();
  }

  template<> Vec2f XML2OBJ::load<Vec2f>(const Ref<XML>& xml) {
    if (xml->body.size() != 2) throw std::runtime_error(xml->loc.str()+": wrong float2 body");
    return Vec2f(xml->body[0].Float(),xml->body[1].Float());
  }

  template<> Vec3f XML2OBJ::load<Vec3f>(const Ref<XML>& xml) {
    if (xml->body.size() != 3) throw std::runtime_error(xml->loc.str()+": wrong float3 body");
    return Vec3f(xml->body[0].Float(),xml->body[1].Float(),xml->body[2].Float());
  }

  template<> Vec4f XML2OBJ::load<Vec4f>(const Ref<XML>& xml) {
    if (xml->body.size() != 4) throw std::runtime_error(xml->loc.str()+": wrong float4 body");
    return Vec4f(xml->body[0].Float(),xml->body[1].Float(),xml->body[2].Float(),xml->body[3].Float());
  }

  template<> Color XML2OBJ::load<Color>(const Ref<XML>& xml) {
    if (xml->body.size() != 3) throw std::runtime_error(xml->loc.str()+": wrong color body");
    return Color(xml->body[0].Float(),xml->body[1].Float(),xml->body[2].Float());
  }

  template<> AffineSpace3f XML2OBJ::load<AffineSpace3f>(const Ref<XML>& xml) 
  {
    if (xml->parm("translate") != "") {
      float x,y,z; sscanf(xml->parm("translate").c_str(),"%f %f %f",&x,&y,&z);
      return AffineSpace3f::translate(Vector3f(x,y,z));
    } else if (xml->parm("scale") != "") {
      float x,y,z; sscanf(xml->parm("scale").c_str(),"%f %f %f",&x,&y,&z);
      return AffineSpace3f::scale(Vector3f(x,y,z));
    } else if (xml->parm("rotate_x") != "") {
      float degrees; sscanf(xml->parm("rotate_x").c_str(),"%f",&degrees);
      return AffineSpace3f::rotate(Vector3f(1,0,0),deg2rad(degrees));
    } else if (xml->parm("rotate_y") != "") {
      float degrees; sscanf(xml->parm("rotate_y").c_str(),"%f",&degrees);
      return AffineSpace3f::rotate(Vector3f(0,1,0),deg2rad(degrees));
    } else if (xml->parm("rotate_z") != "") {
      float degrees; sscanf(xml->parm("rotate_z").c_str(),"%f",&degrees);
      return AffineSpace3f::rotate(Vector3f(0,0,1),deg2rad(degrees));
    } else if (xml->parm("rotate") != "" && xml->parm("axis") != "") {
      float degrees; sscanf(xml->parm("rotate").c_str(),"%f",&degrees);
      float x,y,z; sscanf(xml->parm("axis").c_str(),"%f %f %f",&x,&y,&z);
      return AffineSpace3f::rotate(Vector3f(x,y,z),deg2rad(degrees));
    } else {
      if (xml->body.size() != 12) throw std::runtime_error(xml->loc.str()+": wrong AffineSpace body");
      return AffineSpace3f(LinearSpace3f(xml->body[0].Float(),xml->body[1].Float(),xml->body[ 2].Float(),
					 xml->body[4].Float(),xml->body[5].Float(),xml->body[ 6].Float(),
					 xml->body[8].Float(),xml->body[9].Float(),xml->body[10].Float()),
			   Vector3f(xml->body[3].Float(),xml->body[7].Float(),xml->body[11].Float()));
    }
  }

  template<typename Ty>
  std::vector<Ty> XML2OBJ::loadBinary(const Ref<XML>& xml)
  {
    if (!binFile) 
      throw std::runtime_error("cannot open file "+binFileName.str()+" for reading");

    size_t ofs = atol(xml->parm("ofs").c_str());
    fseek(binFile,long(ofs),SEEK_SET);

    size_t size = atol(xml->parm("size").c_str());
    std::vector<Ty> data;
    data.resize(size);
    if (size != fread(&data[0], sizeof(Ty), size, binFile)) 
      throw std::runtime_error("error reading from binary file: "+binFileName.str());

    return data;
  }

  std::vector<Vec2f> XML2OBJ::loadVec2fArray(const Ref<XML>& xml)
  {
    /*! do not fail if array does not exist */
    if (!xml) return std::vector<Vec2f>();

    if (xml->parm("ofs") != "") {
      return loadBinary<Vec2f>(xml);
    }

    size_t elts = xml->body.size();
    if (elts % 2 != 0) throw std::runtime_error(xml->loc.str()+": wrong vector<float3> body");
    std::vector<Vec2f> data; data.resize(elts/3);
    for (size_t i=0; i<data.size(); i++) 
      data[i] = Vec2f(xml->body[2*i+0].Float(),xml->body[2*i+1].Float());
    return data;
  }

  std::vector<Vec3f> XML2OBJ::loadVec3fArray(const Ref<XML>& xml)
  {
    /*! do not fail if array does not exist */
    if (!xml) return std::vector<Vec3f>();

    if (xml->parm("ofs") != "") {
      return loadBinary<Vec3f>(xml);
    }

    size_t elts = xml->body.size();
    if (elts % 3 != 0) throw std::runtime_error(xml->loc.str()+": wrong vector<float3> body");
    std::vector<Vec3f> data; data.resize(elts/3);
    for (size_t i=0; i<data.size(); i++) 
      data[i] = Vec3f(xml->body[3*i+0].Float(),xml->body[3*i+1].Float(),xml->body[3*i+2].Float());
    return data;
  }

  std::vector<Vec3i> XML2OBJ::loadVec3iArray(const Ref<XML>& xml)
  {
    /*! do not fail if array does not exist */
    if (!xml) return std::vector<Vec3i>();

    if (xml->parm("ofs") != "") {
      return loadBinary<Vec3i>(xml);
    }

    size_t elts = xml->body.size();
    if (elts % 3 != 0) throw std::runtime_error(xml->loc.str()+": wrong vector<int3> body");
    std::vector<Vec3i> data; data.resize(elts/3);
    for (size_t i=0; i<data.size(); i++) 
      data[i] = Vec3i(xml->body[3*i+0].Int(),xml->body[3*i+1].Int(),xml->body[3*i+2].Int());
    return data;
  }

  //////////////////////////////////////////////////////////////////////////////
  //// Loading of objects from XML file
  //////////////////////////////////////////////////////////////////////////////

  void XML2OBJ::loadPointLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadSpotLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadDirectionalLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadDistantLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadAmbientLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadTriangleLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadQuadLight(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadHDRILight(const Ref<XML>& xml) 
  {
  }

  float XML2OBJ::loadMaterialParmFloat(const Ref<XML>& parms, const std::string tag, float def)
  {
    for (size_t i=0; i<parms->children.size(); i++) 
    {
      Ref<XML> entry = parms->children[i];
      if (entry->parm("name") == tag && entry->name == "float1" ) { 
        return load<float>(entry);
      }
    }
    return def;
  }

  Vec3f XML2OBJ::loadMaterialParmVec3f(const Ref<XML>& parms, const std::string tag, Vec3f def)
  {
    for (size_t i=0; i<parms->children.size(); i++) 
    {
      Ref<XML> entry = parms->children[i];
      if (entry->parm("name") == tag && entry->name == "float3" ) { 
        return load<Vec3f>(entry);
      }
    }
    return def;
  }

  int XML2OBJ::loadMaterial(const Ref<XML>& xml) 
  {
    if (xml->parm("id") != "")
      return materialMap[xml->parm("id")];

    Ref<XML> parms = xml->child("parameters");
    if (materialCache.find(parms) != materialCache.end()) {
      return materialCache[parms];
    }

    std::string code = load<std::string>(xml->child("code"));
    int material = nextMaterialID++;
    if (code == "Velvet") {
      Vec3f diffuse = loadMaterialParmVec3f(parms,"reflectance",1.0f);
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd %f %f %f\n",diffuse.x,diffuse.y,diffuse.z);
    } else if (code == "Dielectric") {
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd 1 1 1\n");
    } else if (code == "MetallicPaint") {
      Vec3f diffuse = loadMaterialParmVec3f(parms,"shadeColor",1.0f);
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd %f %f %f\n",diffuse.x,diffuse.y,diffuse.z);
    } else if (code == "Metal") {
      Vec3f reflectance = loadMaterialParmVec3f(parms,"reflectance",one);
      Vec3f eta = loadMaterialParmVec3f(parms,"eta",1.4f);
      Vec3f k = loadMaterialParmVec3f(parms,"k",0.0f);
      float roughness = loadMaterialParmFloat(parms,"roughness",0.01f);
      float kmax = max(k.x,k.y,k.z);
      if (kmax != 0.0f) k /= kmax;
      reflectance = reflectance * k;
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd %f %f %f\n",reflectance.x,reflectance.y,reflectance.z);
    } else if (code == "Plastic") {
      Vec3f pigmentColor = loadMaterialParmVec3f(parms,"pigmentColor",1.0f);
      float eta = loadMaterialParmFloat(parms,"eta",1.4f);
      float roughness = loadMaterialParmFloat(parms,"roughness",0.01f);
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd %f %f %f\n",pigmentColor.x,pigmentColor.y,pigmentColor.z);
    } else if (code == "Matte") {
      Vec3f reflectance = loadMaterialParmVec3f(parms,"reflectance",1.0f);
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd %f %f %f\n",reflectance.x,reflectance.y,reflectance.z);
    } else if (code == "ThinGlass") {
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"Kd 0 0 0\n");
    } else if (code == "OBJ") {
      float d = loadMaterialParmFloat(parms,"d",1.0f);
      Vec3f Kd = loadMaterialParmVec3f(parms,"Kd",one);
      Vec3f Ks = loadMaterialParmVec3f(parms,"Ks",zero);
      float Ns = loadMaterialParmFloat(parms,"Ns",10.0f);
      fprintf(mtlFile,"newmtl material%i\n",material);
      fprintf(mtlFile,"d %f\n",d);
      fprintf(mtlFile,"Kd %f %f %f\n",Kd.x,Kd.y,Kd.z);
      fprintf(mtlFile,"Ks %f %f %f\n",Ks.x,Ks.y,Ks.z);
      fprintf(mtlFile,"Ns %f\n",Ns);
    } else {
      std::cout << "unknown material \"" << code << "\""<< std::endl;
      fprintf(mtlFile,"newmtl material%i\n",material);
    }
    materialCache[parms] = material;
    return material;
  }

  void XML2OBJ::loadTriangleMesh(const Ref<XML>& xml) 
  {
    int material = loadMaterial(xml->child("material"));
    std::vector<Vec3f> positions = loadVec3fArray(xml->childOpt("positions"));
    std::vector<Vec3f> normals   = loadVec3fArray(xml->childOpt("normals"  ));
    std::vector<Vec2f> texcoords = loadVec2fArray(xml->childOpt("texcoords"));
    std::vector<Vec3i> triangles = loadVec3iArray(xml->childOpt("triangles"));

    for (size_t i=0; i<positions.size(); i++) 
      positions[i] = xfmPoint(transforms.top(),Vector3f(positions[i].x,positions[i].y,positions[i].z));

    for (size_t i=0; i<normals.size(); i++) 
      normals[i] = xfmVector(transforms.top(),Vector3f(normals[i].x,normals[i].y,normals[i].z));

    fprintf(objFile,"usemtl material%i\n",material);
    for (size_t i=0; i<positions.size(); i++)
      fprintf(objFile,"v %f %f %f\n",positions[i].x,positions[i].y,positions[i].z);
    for (size_t i=0; i<normals.size(); i++)
      fprintf(objFile,"vn %f %f %f\n",normals[i].x,normals[i].y,normals[i].z);
    for (size_t i=0; i<texcoords.size(); i++)
      fprintf(objFile,"vt %f %f\n",texcoords[i].x,texcoords[i].y);
    
    for (size_t i=0; i<triangles.size(); i++)
    {
      if (texcoords.size() == positions.size()) 
      {
        if (normals.size() == positions.size()) 
          fprintf(objFile,"f %i/%i/%i %i/%i/%i %i/%i/%i\n",
                  triangles[i].x+nextVertexID,triangles[i].x+nextTexCoordID,triangles[i].x+nextNormalID,
                  triangles[i].y+nextVertexID,triangles[i].y+nextTexCoordID,triangles[i].y+nextNormalID,
                  triangles[i].z+nextVertexID,triangles[i].z+nextTexCoordID,triangles[i].z+nextNormalID);
        else 
          fprintf(objFile,"f %i/%i %i/%i %i/%i\n",
                  triangles[i].x+nextVertexID,triangles[i].x+nextTexCoordID,
                  triangles[i].y+nextVertexID,triangles[i].y+nextTexCoordID,
                  triangles[i].z+nextVertexID,triangles[i].z+nextTexCoordID);
      }
      else if (normals.size() == positions.size()) 
        fprintf(objFile,"f %i//%i %i//%i %i//%i\n",
                triangles[i].x+nextVertexID,triangles[i].x+nextNormalID,
                triangles[i].y+nextVertexID,triangles[i].y+nextNormalID,
                triangles[i].z+nextVertexID,triangles[i].z+nextNormalID);
      else 
        fprintf(objFile,"f %i %i %i\n",
                triangles[i].x+nextVertexID,
                triangles[i].y+nextVertexID,
                triangles[i].z+nextVertexID);
    }

    nextVertexID+=positions.size();
    nextTexCoordID+=texcoords.size();
    nextNormalID+=normals.size();
  }

  void XML2OBJ::loadSphere(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadDisk(const Ref<XML>& xml) 
  {
  }

  void XML2OBJ::loadTransformNode(const Ref<XML>& xml) 
  {
    /*! Pre-multiply the incoming transform with the top of the transform stack. */
    transforms.push(transforms.top()*load<AffineSpace3f>(xml->children[0]));

    /*! The transform at the top of the stack will be applied to new lights and shapes. */
    for (size_t i=1; i<xml->children.size(); i++) {
      loadScene(xml->children[i]);
    }

    /*! Remove the transform associated with this node from the transform stack. */
    transforms.pop();  
  }

  void XML2OBJ::loadGroupNode(const Ref<XML>& xml) 
  {
    for (size_t i=0; i<xml->children.size(); i++) {
      loadScene(xml->children[i]);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //// Loading of scene graph node from XML file
  //////////////////////////////////////////////////////////////////////////////
  
  void XML2OBJ::loadScene(const Ref<XML>& xml)
  {
    if (xml->name == "assign") 
    {
      if (xml->parm("type") == "material")
        materialMap[xml->parm("id")] = loadMaterial(xml->child(0));
      //else if (xml->parm("type") == "scene")
      //  sceneMap[xml->parm("id")] = loadScene(xml->child(0));
      else 
        throw std::runtime_error(xml->loc.str()+": unknown type: "+xml->parm("type"));
    }
    else 
    {
      /*if (xml->name == "xml") {
        prims = loadXML(path + xml->parm("src"));
        for (size_t i=0; i<prims.size(); i++)
          prims[i] = g_device->rtTransformPrimitive(prims[i],copyToArray(transforms.top()));
      }
      else if (xml->name == "obj") {
        prims = loadOBJ(path + xml->parm("src"));
        for (size_t i=0; i<prims.size(); i++)
          prims[i] = g_device->rtTransformPrimitive(prims[i],copyToArray(transforms.top()));
      }
      else if (xml->name == "extern") {
        prims = rtLoadScene(path + xml->parm("src"));
        for (size_t i=0; i<prims.size(); i++)
          prims[i] = g_device->rtTransformPrimitive(prims[i],copyToArray(transforms.top()));
      }
      else if (xml->name == "ref") {
        prims = sceneMap[xml->parm("id")];
        for (size_t i=0; i<prims.size(); i++)
          prims[i] = g_device->rtTransformPrimitive(prims[i],copyToArray(transforms.top()));
          }*/
      
      if (xml->name == "PointLight"      ) loadPointLight      (xml);
      else if (xml->name == "SpotLight"       ) loadSpotLight       (xml);
      else if (xml->name == "DirectionalLight") loadDirectionalLight(xml);
      else if (xml->name == "DistantLight"    ) loadDistantLight    (xml);
      else if (xml->name == "AmbientLight"    ) loadAmbientLight    (xml);
      else if (xml->name == "TriangleLight"   ) loadTriangleLight   (xml);
      else if (xml->name == "QuadLight"       ) loadQuadLight       (xml);
      else if (xml->name == "HDRILight"       ) loadHDRILight       (xml);

      else if (xml->name == "TriangleMesh"    ) loadTriangleMesh    (xml);
      else if (xml->name == "Sphere"          ) loadSphere          (xml);
      else if (xml->name == "Disk"            ) loadDisk            (xml);
      else if (xml->name == "Group"           ) loadGroupNode       (xml);
      else if (xml->name == "Transform"       ) loadTransformNode   (xml);
      
      else throw std::runtime_error(xml->loc.str()+": unknown tag: "+xml->name);
    }
  }

  XML2OBJ::XML2OBJ(const FileName& fileName, const FileName& objFileName, const FileName& mtlFileName) 
    : binFile(NULL), objFile(NULL), mtlFile(NULL), nextVertexID(1), nextTexCoordID(1), nextNormalID(1), nextMaterialID(0)
  {
    /* create obj and mtl files */
    objFile = fopen(objFileName.c_str(),"w");
    mtlFile = fopen(mtlFileName.c_str(),"w");
    fprintf(objFile,"mtllib %s\n",mtlFileName.c_str());

    /* open bin file */
    path = fileName.path();
    binFileName = fileName.setExt(".bin");
    binFile = fopen(binFileName.c_str(),"rb");

    /* start conversion */
    transforms.push(AffineSpace3f(one));
    Ref<XML> xml = parseXML(fileName);
    if (xml->name != "scene") throw std::runtime_error(xml->loc.str()+": invalid scene tag");
    for (size_t i=0; i<xml->children.size(); i++) {
      //std::vector<Handle<Device::RTPrimitive> > prims = 
      loadScene(xml->children[i]);
      //model.insert(model.end(), prims.begin(), prims.end());
    }

  }

  XML2OBJ::~XML2OBJ() 
  {
    if (transforms.size()) transforms.pop();
    if (binFile) fclose(binFile);
    if (objFile) fclose(objFile);
    if (mtlFile) fclose(mtlFile);
  }
}

int main(int argc, char **argv) 
{
  /*! all file names must be specified on the command line */
  if (argc != 4) printf("  USAGE:  xml2obj <infile.xml> <outfile.obj> <outfile.mtl>\n"), exit(1);
  
  std::string inxml = argv[1];
  std::string outobj = argv[2];
  std::string outmtl = argv[3];
  embree::XML2OBJ(inxml,outobj,outmtl);
}

