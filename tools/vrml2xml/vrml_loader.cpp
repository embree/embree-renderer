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

namespace embree
{
  std::vector<Vec3f> VRMLLoader::parsePointArray(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    std::vector<Vec3f> points;

    if (cin->get() != Token::Sym("[")) 
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");
      
    while (cin->peek() != Token::Sym("]")) 
    {
      Vec3f p;
      p.x = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      p.y = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      p.z = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();

      points.push_back(xfmPoint(space,p));
    }
    cin->get();
    return points;
  }

  std::vector<Vec3f> VRMLLoader::parseNormalArray(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    std::vector<Vec3f> normals;

    if (cin->get() != Token::Sym("[")) 
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");
      
    while (cin->peek() != Token::Sym("]")) 
    {
      Vec3f n;
      n.x = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      n.y = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      n.z = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();

      normals.push_back(xfmNormal(space,n));
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
      t.x = cin->getFloat(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      t.y = cin->getFloat(); 
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
      tri.x = cin->getInt(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      tri.y = cin->getInt(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      tri.z = cin->getInt(); 
      if (cin->peek() == Token::Sym(",")) cin->get();
      cin->getInt(); 
      if (cin->peek() == Token::Sym(",")) cin->get();

      triangles.push_back(tri);
    }
    cin->get();
    return triangles;
  }

  Mesh VRMLLoader::parseIndexedFaceSet(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    Mesh mesh;
    std::string tag = cin->getSymbol();
    if (tag == "solid") {
      std::string value = cin->getSymbol();
    }
    if (tag == "coord") {
      std::string name = cin->getSymbol();
      if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
      std::string type = cin->getSymbol();
      mesh.positions = parsePointArray(cin,space);
      if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
    }
    else if (tag == "normal")
    {
      std::string name = cin->getSymbol();
      if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
      std::string type = cin->getSymbol();
      mesh.normals = parseNormalArray(cin,space);
      if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
    }    
    else if (tag == "texCoord")
    {
      std::string name = cin->getSymbol();
      if (cin->get() != Token::Sym("{")) throw std::runtime_error(cin->unget().Location().str()+": { expected");
      std::string type = cin->getSymbol();
      mesh.texcoords = parseTexCoordArray(cin);
      if (cin->get() != Token::Sym("}")) throw std::runtime_error(cin->unget().Location().str()+": } expected");
    }
    else if (tag == "coordIndex")
    {
      std::string name = cin->getSymbol();
      mesh.triangles = parseTriangleArray(cin);
    }
    else
      std::cout << "inknown indexed face set array: " << tag << std::endl;

    return mesh;
  }

  void VRMLLoader::parseGeometry(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    std::string tag = cin->getSymbol();
    if (tag == "DEF") {
      std::string name = cin->getSymbol();
      tag = cin->getSymbol();
    }
    
    if (tag == "IndexedFaceSet"  ) parseIndexedFaceSet(cin,space);
    else std::cout << "unknown geometry: " << tag << std::endl;
  }

  void VRMLLoader::parseShape(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    std::string tag = cin->getSymbol();
    if      (tag == "geometry"  ) parseGeometry(cin,space);
    else if (tag == "appearance") parseAppearance(cin,space);
    else throw std::runtime_error(cin->unget().Location().str()+": invalid shape");
  }

  void VRMLLoader::parseChildrenList(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    if (cin->get() != Token::Sym("["))
      throw std::runtime_error(cin->unget().Location().str()+": [ expected");

    while (cin->peek() != Token::Sym("]")) 
      parseNode(cin,space);

    cin->get();
  }

  void VRMLLoader::parseChildren(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    if (cin->get() != Token::Sym("{"))
      throw std::runtime_error(cin->unget().Location().str()+": { expected");

    if (cin->getSymbol() != "children")
      throw std::runtime_error(cin->unget().Location().str()+": expected children");

    parseChildrenList(cin,space);

    if (cin->get() != Token::Sym("}"))
      throw std::runtime_error(cin->unget().Location().str()+": } expected");
  }

  void VRMLLoader::parseTransform(Ref<Stream<Token> >& cin, const AffineSpace3fa& space_in)
  {
    Vec3fa center = zero;
    Vec3fa rotationAxis = Vec3fa(1,0,0);
    Vec3fa rotationAngle = 0.0f;
    Vec3fa scaleMagnitude = Vec3fa(1,1,1);
    Vec3fa scaleOrientation = Vec3fa(1,0,0);
    Vec3fa translation = zero;

    std::string type = cin->getSymbol();
    if (type == "center") {
      center.x = cin->getFloat();
      center.y = cin->getFloat();
      center.z = cin->getFloat();
    } 
    else if (type == "rotation") {
      rotationAxis.x = cin->getFloat();
      rotationAxis.y = cin->getFloat();
      rotationAxis.z = cin->getFloat();
      rotationAngle = cin->getFloat();
    }
    else if (type == "scale") {
      rotationMagnitude.x = cin->getFloat();
      rotationMagnitude.y = cin->getFloat();
      rotationMagnitude.z = cin->getFloat();
    }
    else if (type == "scaleOrientation") {
      rotationOrientation.x = cin->getFloat();
      rotationOrientation.y = cin->getFloat();
      rotationOrientation.z = cin->getFloat();
      cin->getFloat();
    }
    else if (type == "translation") {
      translation.x = cin->getFloat();
      translation.y = cin->getFloat();
      translation.z = cin->getFloat();
    }
    else 
      throw std::runtime_error(cin->unget().Location().str()+": invalid transformation");

    if (cin->getSymbol() != "children")
      throw std::runtime_error(cin->unget().Location().str()+": expected children");

    parseChildrenList(cin,space);
  }

  void VRMLLoader::parseNode(Ref<Stream<Token> >& cin, const AffineSpace3fa& space)
  {
    std::string tag = cin->getSymbol();
    if (tag == "DEF") {
      std::string name = cin->getSymbol();
      tag = cin->getSymbol();
    }
    
    if      (tag == "Shape"    ) parseShape(cin,space);
    else if (tag == "Group"    ) parseGroup(cin,space);
    else if (tag == "Transform") parseTransform(cin,space);
    else throw std::runtime_error(cin->unget().Location().str()+": unknown node type");
  }

  VRMLLoader::VRMLLoader(const char* fileName) 
  {
    Ref<Stream<int> > chars  = new FileStream(fileName);

    /* create lexer for XML file */
    std::vector<std::string> symbols;
    symbols.push_back("{");
    symbols.push_back("}");
    symbols.push_back("[");
    symbols.push_back("]");
    symbols.push_back(",");
    Ref<Stream<Token> > cin = new TokenStream(chars,TokenStream::alpha + TokenStream::ALPHA + "_", TokenStream::separators, symbols);

    parseNode(cin);

  }
}
  
