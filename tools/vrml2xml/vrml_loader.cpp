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

#include "vrml_loader.h"

#define IGNORE_VARIANTS 1
#define SWAP_YZ 1

namespace embree
{
  std::vector<Vec3f> VRMLLoader::parsePointArray(Ref<Stream<Token> >& cin, const AffineSpace3f& space)
  {
    std::vector<Vec3f> points;

    if (cin->get() != Token::Sym("[")) 
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");
      
    while (cin->peek() != Token::Sym("]")) 
    {
      Vec3f p;
      p.x = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      p.y = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      p.z = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      p = xfmPoint(space,p);
#if SWAP_YZ
      std::swap(p.y,p.z);
#endif
      points.push_back(p);
    }
    cin->get();
    return points;
  }

  std::vector<Vec3f> VRMLLoader::parseNormalArray(Ref<Stream<Token> >& cin, const AffineSpace3f& space)
  {
    std::vector<Vec3f> normals;

    if (cin->get() != Token::Sym("[")) 
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");
      
    while (cin->peek() != Token::Sym("]")) 
    {
      Vec3f n;
      n.x = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      n.y = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      n.z = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      n = xfmNormal(space,n);
#if SWAP_YZ
      std::swap(n.y,n.z);
#endif
      normals.push_back(n);
    }
    cin->get();
    return normals;
  }

  std::vector<Vec2f> VRMLLoader::parseTexCoordArray(Ref<Stream<Token> >& cin)
  {
    std::vector<Vec2f> texcoords;

    if (cin->get() != Token::Sym("[")) 
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");
      
    while (cin->peek() != Token::Sym("]")) 
    {
      Vec2f t;
      t.x = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      t.y = cin->get().Float(); 
      if (cin->peek() == Token::Sym(",")) cin->get();

      texcoords.push_back(t);
    }
    cin->get();
    return texcoords;
  }

  std::vector<Vec3i> VRMLLoader::parseTriangleArray(Ref<Stream<Token> >& cin)
  {
    std::vector<Vec3i> triangles;

    if (cin->get() != Token::Sym("[")) 
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");
      
    while (cin->peek() != Token::Sym("]")) 
    {
      Vec3i tri;
      tri.x = cin->get().Int(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      tri.y = cin->get().Int(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      tri.z = cin->get().Int(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      cin->get().Int(); 
      if (cin->peek() == Token::Sym(",")) cin->get();

      triangles.push_back(tri);
    }
    cin->get();
    return triangles;
  }

  Ref<Mesh> VRMLLoader::parseIndexedFaceSet(Ref<Stream<Token> >& cin, const AffineSpace3f& space)
  {
    Ref<Mesh> mesh = new Mesh(defaultMaterial);

    if (cin->get() != Token::Sym("{")) 
      throw std::runtime_error(cin->unget().Location().str()+": { expected");

    while (cin->peek() != Token::Sym("}"))
    {
      std::string tag = cin->get().Identifier();
      if (tag == "solid") {
        std::string value = cin->get().Identifier();
      } 
      else if (tag == "coord") {
        std::string name = cin->get().Identifier();
        if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
        std::string type = cin->get().Identifier();
        mesh->positions = parsePointArray(cin,space);
        if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
      }
      else if (tag == "normal")
      {
        std::string name = cin->get().Identifier();
        if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
        std::string type = cin->get().Identifier();
        mesh->normals = parseNormalArray(cin,space);
        if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
      }    
      else if (tag == "texCoord")
      {
        std::string name = cin->get().Identifier();
        if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
        std::string type = cin->get().Identifier();
        mesh->texcoords = parseTexCoordArray(cin);
        if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
      }
      else if (tag == "coordIndex")
      {
        mesh->triangles = parseTriangleArray(cin);
      }
      else if (tag == "normalIndex")
      {
        std::vector<Vec3i> normalIndex = parseTriangleArray(cin);
        if (normalIndex != mesh->triangles)
          throw std::runtime_error("normal indices not supported");
      }
      else if (tag == "texCoordIndex")
      {
        std::vector<Vec3i> texcoordIndex = parseTriangleArray(cin);
        if (texcoordIndex != mesh->triangles)
          throw std::runtime_error("texcoord indices not supported");
      }
      else
        std::cout << "unknown indexed face set array: " << tag << std::endl;
    }
    cin->get();
    return mesh;
  }

  Ref<Mesh> VRMLLoader::parseGeometry(Ref<Stream<Token> >& cin, const AffineSpace3f& space)
  {
    std::string tag = cin->get().Identifier();
    if (tag == "DEF") {
      std::string name = cin->get().Identifier();
      tag = cin->get().Identifier();
    }
    
    if (tag == "IndexedFaceSet"  ) return parseIndexedFaceSet(cin,space);
    else {
      std::cout << "unknown geometry: " << tag << std::endl;
      return new Mesh(defaultMaterial);
    }
  }

  Ref<Material> VRMLLoader::parseMaterial(Ref<Stream<Token> >& cin, const std::string& name)
  {
    Ref<Material> material = new Material(name);
    
    if (cin->get() != Token::Sym("{")) 
      throw std::runtime_error(cin->unget().Location().str()+": { expected");
      
    while (cin->peek() != Token::Sym("}")) 
    {
      std::string tag = cin->get().Identifier();
      if (tag == "ambientIntensity") {
        cin->get().Float();
      }
      else if (tag == "diffuseColor") {
        material->Kd.r = cin->get().Float();
        material->Kd.g = cin->get().Float();
        material->Kd.b = cin->get().Float();
      }
      else if (tag == "emissiveColor") {
        material->Ka.r = cin->get().Float();
        material->Ka.g = cin->get().Float();
        material->Ka.b = cin->get().Float();
      }
      else if (tag == "shininess") {
        material->Ns = cin->get().Float();
      }
      else if (tag == "specularColor") {
        material->Ks.r = cin->get().Float();
        material->Ks.g = cin->get().Float();
        material->Ks.b = cin->get().Float();
      }
      else if (tag == "transparency") {
        material->d = 1.0f-cin->get().Float();
      }
      else 
        throw std::runtime_error(cin->unget().Location().str()+": invalid material");
    }
    cin->get();
    return material;
  }

  Ref<Material> VRMLLoader::parseAppearance(Ref<Stream<Token> >& cin)
  {
    std::string name = "";
    std::string tag = cin->get().Identifier();
    if (tag == "DEF") {
      name = cin->get().Identifier();
      tag = cin->get().Identifier();
    }
    else if (tag == "USE") {
      name = cin->get().Identifier();
      if (name2material.find(name) == name2material.end())
        throw std::runtime_error("material with name "+name+" not known");
      return name2material[name];
    }
    if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
    tag = cin->get().Identifier();
    if (tag != "material") throw std::runtime_error(cin->unget().Location().str()+": material expected");
    std::string type2 = cin->get().Identifier();
    Ref<Material> material = parseMaterial(cin,name);
    name2material[name] = material;
    scene->materials.push_back(material);
    if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
    return material;
  }

  void VRMLLoader::parseShape(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible)
  {
    static size_t numTriangles = 0;

    if (cin->get() != Token::Sym("{"))
      throw std::runtime_error(cin->unget().Location().str()+": { expected");

    std::string tag = cin->get().Identifier();
    if (tag != "geometry"  ) throw std::runtime_error(cin->unget().Location().str()+": geometry expected");
    Ref<Mesh> mesh = parseGeometry(cin,space);

    if (cin->peek() == Token::Id("appearance")) {
      cin->get();
      mesh->material = parseAppearance(cin);
    }

    if (visible) {
      std::cout << "+" << std::flush;
      numTriangles += mesh->triangles.size();
      scene->meshes.push_back(mesh);
    }
    else
      std::cout << "-" << std::flush;

    if (cin->get() != Token::Sym("}"))
      throw std::runtime_error(cin->unget().Location().str()+": } expected");
  }

  void VRMLLoader::parseChildrenList(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible)
  {
    if (cin->get() != Token::Sym("["))
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");

    while (cin->peek() != Token::Sym("]")) 
      parseNode(cin,space,visible);

    cin->get();
  }

  void VRMLLoader::parseChildren(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible)
  {
    if (cin->get() != Token::Sym("{"))
      throw std::runtime_error(cin->unget().Location().str()+": { expected");

    if (cin->get().Identifier() != "children")
      throw std::runtime_error(cin->unget().Location().str()+": expected children");

    parseChildrenList(cin,space,visible);

    if (cin->get() != Token::Sym("}"))
      throw std::runtime_error(cin->unget().Location().str()+": } expected");
  }

  void VRMLLoader::parseTransform(Ref<Stream<Token> >& cin, const AffineSpace3f& space_in, bool visible)
  {
    Vec3f center = zero;
    Vec3f rotationAxis = Vec3f(1,0,0);
    float rotationAngle = 0.0f;
    Vec3f scaleMagnitude = Vec3f(1,1,1);
    Vec3f scaleOrientation = Vec3f(1,0,0);
    float scaleOrientationAngle = 1.0f;
    Vec3f translation = zero;

    if (cin->get() != Token::Sym("{"))
      throw std::runtime_error(cin->unget().Location().str()+": { expected");

    while (cin->peek() != Token::Id("children")) 
    {
      std::string type = cin->get().Identifier();
      if (type == "center") {
        center.x = cin->get().Float();
        center.y = cin->get().Float();
        center.z = cin->get().Float();
      } 
      else if (type == "rotation") {
        rotationAxis.x = cin->get().Float();
        rotationAxis.y = cin->get().Float();
        rotationAxis.z = cin->get().Float();
        rotationAngle  = cin->get().Float();
      }
      else if (type == "scale") {
        scaleMagnitude.x = cin->get().Float();
        scaleMagnitude.y = cin->get().Float();
        scaleMagnitude.z = cin->get().Float();
      }
      else if (type == "scaleOrientation") {
        scaleOrientation.x = cin->get().Float();
        scaleOrientation.y = cin->get().Float();
        scaleOrientation.z = cin->get().Float();
        scaleOrientationAngle = cin->get().Float();
      }
      else if (type == "translation") {
        translation.x = cin->get().Float();
        translation.y = cin->get().Float();
        translation.z = cin->get().Float();
      }
      else 
        throw std::runtime_error(cin->unget().Location().str()+": invalid transformation");
    }
    cin->get();

    AffineSpace3f space = space_in *
	AffineSpace3f::translate(translation) *
	AffineSpace3f::translate(center) *
	AffineSpace3f::rotate(rotationAxis,rotationAngle) *
	AffineSpace3f::rotate(scaleOrientation,scaleOrientationAngle) *
	AffineSpace3f::scale(scaleMagnitude) *
	rcp(AffineSpace3f::rotate(scaleOrientation,scaleOrientationAngle)) *
	AffineSpace3f::translate(-center);

    parseChildrenList(cin,space,visible);

    if (cin->get() != Token::Sym("}"))
      throw std::runtime_error(cin->unget().Location().str()+": } expected");
  }

  void VRMLLoader::parseGroup(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible)
  {
    if (cin->get() != Token::Sym("{"))
      throw std::runtime_error(cin->unget().Location().str()+": { expected");

    if (cin->get().Identifier() != "children")
      throw std::runtime_error(cin->unget().Location().str()+": children expected");
    
    parseChildrenList(cin,space,visible);

    if (cin->get() != Token::Sym("}"))
      throw std::runtime_error(cin->unget().Location().str()+": } expected");
  }

  void VRMLLoader::parseNode(Ref<Stream<Token> >& cin, const AffineSpace3f& space, bool visible)
  {
    std::string tag = cin->get().Identifier();
    if (tag == "DEF") {
      std::string name = cin->get().Identifier();
#if IGNORE_VARIANTS
      if (name.find("ALTERNATIVE") != std::string::npos) { // && name.find("ALTERNATIVE_1_") == std::string::npos) {
	visible = false;
	//PRINT(name);
      }
#endif
      tag = cin->get().Identifier();
    }
    
    if      (tag == "Shape"    ) parseShape(cin,space,visible);
    else if (tag == "Group"    ) parseGroup(cin,space,visible);
    else if (tag == "Transform") parseTransform(cin,space,visible);
    else throw std::runtime_error(cin->unget().Location().str()+": unknown node type");
  }

  VRMLLoader::VRMLLoader(const char* fileName) 
    : scene(new Scene), defaultMaterial(new Material("default"))
  {
    Ref<Stream<int> > chars  = new FileStream(fileName);

    /* create lexer for XML file */
    std::vector<std::string> symbols;
    symbols.push_back("#");
    symbols.push_back("{");
    symbols.push_back("}");
    symbols.push_back("[");
    symbols.push_back("]");
    symbols.push_back(",");
    Ref<Stream<Token> > cin = new TokenStream(chars,TokenStream::alpha + TokenStream::ALPHA + "_", TokenStream::separators, symbols);

    // FIXME: hack to skip header
    cin->get();
    cin->get();
    cin->get();
    cin->get();
    cin->get();

    AffineSpace3f space(one);
    scene->materials.push_back(defaultMaterial);
    parseNode(cin,space,true);
  }
}
  
