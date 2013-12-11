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

#ifndef __EMBREE_TRIANGLE_H__
#define __EMBREE_TRIANGLE_H__

#include "../shapes/shape.h"

namespace embree
{
  /*! Implements a single triangle. */
  class Triangle : public Shape
  {
  public:

    /*! Construction from acceleration structure type. */
    Triangle (const AccelType& ty)
      : Shape(ty) {}

    /*! Construction from parameter container. */
    Triangle (const Parms& parms) : Shape(parms)
    {
      v0 = parms.getVector3f("v0");
      v1 = parms.getVector3f("v1");
      v2 = parms.getVector3f("v2");
    }

    /*! Triangle construction from its vertices. */
    Triangle (const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const AccelType& ty)
      : Shape(ty), v0(v0), v1(v1), v2(v2), Ng(normalize(cross(v2-v0,v1-v0))) {
    }

    /*! Transforms the triangle. */
    Ref<Shape> transform(const AffineSpace3f& xfm) const {
      return new Triangle(xfmPoint(xfm,v0),xfmPoint(xfm,v1),xfmPoint(xfm,v2),ty);
    }

    size_t numTriangles() const { return 1; }
    size_t numVertices () const { return 3; }

    BBox3f extract(RTCScene scene, size_t id) const
    {
      unsigned mesh = rtcNewTriangleMesh (scene, RTC_GEOMETRY_STATIC, 1, 3);
      if (mesh != id) throw std::runtime_error("ID does not match");
      Vec3fa* vertices = (Vec3fa*) rtcMapBuffer(scene,mesh,RTC_VERTEX_BUFFER); 
      RTCTriangle* triangles = (RTCTriangle*) rtcMapBuffer(scene,mesh,RTC_INDEX_BUFFER);
      BBox3f bounds = empty;
      triangles[0].v0 = 0;
      triangles[0].v1 = 1;
      triangles[0].v2 = 2;
      vertices[0] = Vec3fa(v0.x,v0.y,v0.z);
      vertices[1] = Vec3fa(v1.x,v1.y,v1.z);
      vertices[2] = Vec3fa(v2.x,v2.y,v2.z);
      bounds.grow(v0);
      bounds.grow(v1);
      bounds.grow(v2);
      rtcUnmapBuffer(scene,mesh,RTC_VERTEX_BUFFER); 
      rtcUnmapBuffer(scene,mesh,RTC_INDEX_BUFFER);
      return bounds;
    }

    /*! Post intersection to compute shading data. */
    void postIntersect(const Ray& ray, DifferentialGeometry& dg) const {
      dg.P = ray.org+ray.tfar*ray.dir;
      dg.Ng = this->Ng;
      dg.Ns = this->Ng;
      dg.st = Vec2f(ray.u,ray.v);
      dg.error = max(abs(ray.tfar),reduce_max(abs(dg.P)));
    }

  public:
    Vector3f v0;   //!< 1st vertex of triangle.
    Vector3f v1;   //!< 2nd vertex of triangle.
    Vector3f v2;   //!< 3rd vertex of triangle.
    Vector3f Ng;   //!< Geometry normal of triangle.
  };
}

#endif
