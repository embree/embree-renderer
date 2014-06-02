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

#include "shape.isph"

void Shape__Destructor(uniform RefCount* uniform this)
{
  LOG(print("Shape__Destructor\n"));
  RefCount__Destructor(this);
}

void Shape__Constructor(uniform Shape* uniform this,
                        uniform DestructorFunc destructor,
                        uniform PostIntersectFunc postIntersect,
                        uniform ShapeTransformFunc transform,
                        uniform AddFunc add_)
{
  LOG(print("Shape__Constructor\n"));
  RefCount__Constructor(&this->base,destructor);
  this->postIntersect = postIntersect;
  this->transform = transform;
  this->add = add_;
}

export void* uniform Shape__transform(void* uniform _this,
                                      const uniform vec3f& vx,
                                      const uniform vec3f& vy,
                                      const uniform vec3f& vz,
                                      const uniform vec3f& p) 
{
  const uniform Shape* uniform this = (const uniform Shape* uniform) _this;
  uniform AffineSpace3f xfm = make_AffineSpace3f(vx,vy,vz,p);
  return this->transform(this,xfm);
}

export uniform int Shape__add(void* uniform scene,
                              void* uniform _shape)
{
  const uniform Shape* uniform shape = (const uniform Shape* uniform) _shape;
  return shape->add((RTCScene)scene,shape);
}
