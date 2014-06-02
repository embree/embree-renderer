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

#include "scene.isph"
#include "shapes/shape.isph"

void Scene__Destructor(uniform RefCount* uniform _this)
{
  LOG(print("Scene__Destructor\n"));
  uniform Scene* uniform this = (uniform Scene* uniform) _this;

  for (uniform int i=0; i<this->numInstances; i++)
    RefCount__DecRef(&this->instances[i]->base);

  delete[] this->allLights;
  delete[] this->envLights;
  delete[] this->instances;
  delete[] this->handle2id;
  delete[] this->id2handle;

  rtcDeleteScene(this->handle);

  RefCount__Destructor(_this);
}

typedef uniform Light* UniformLightPtr;
typedef uniform EnvironmentLight* UniformEnvLightPtr;
typedef uniform Instance* UniformInstancePtr;

void Scene__Constructor(uniform Scene* uniform this)
{
  LOG(print("Scene__Constructor\n"));
  RefCount__Constructor(&this->base,Scene__Destructor);

  this->numAllLights = 0;
  this->allLights = uniform new UniformLightPtr[16*1024]; // FIMXE: hardcoded constant
  for (uniform int i=0; i<16*1024; i++) this->allLights[i] = NULL;

  this->numEnvLights = 0;
  this->envLights = uniform new UniformEnvLightPtr[16*1024]; // FIXME: hardcoded constant
  for (uniform int i=0; i<16*1024; i++) this->envLights[i] = NULL;

  this->numInstances = 0;
  this->instances = uniform new UniformInstancePtr [1024*1024]; // FIXME: hardcoded constant
  this->handle2id = uniform new uint[1024*1024];
  this->id2handle = uniform new uint[1024*1024];
  for (uniform int i=0; i<1024*1024; i++) {
    this->instances[i] = NULL;
    this->handle2id[i] = -1;
    this->id2handle[i] = -1;
  }

  this->handle = rtcNewScene(RTC_SCENE_STATIC,RTC_INTERSECT_VARYING);
}

export void* uniform Scene__new()
{
  uniform Scene *uniform this = uniform new uniform Scene;
  Scene__Constructor(this);
  return this;
}

export void Scene__set(void* uniform scene_in, uniform int slot, void* uniform instance_in)
{
  uniform Scene*    uniform scene    = (uniform Scene*    uniform) scene_in;
  uniform Instance* uniform instance = (uniform Instance* uniform) instance_in;
  
  if (scene->instances[slot]) {
    rtcDeleteGeometry(scene->handle,scene->id2handle[slot]);
    RefCount__DecRef(&scene->instances[slot]->base);
  }
  scene->instances[slot] = instance;
  if (instance) {
    RefCount__IncRef(&instance->base);
    if (instance->shape) {
      uniform uint handle = instance->shape->add(scene->handle,instance->shape);
      scene->handle2id[handle] = slot;
      scene->id2handle[slot] = handle;
    }
  }
  scene->numInstances = max(scene->numInstances,(uniform uint)(slot+1));
}

export void Scene__commit(void* uniform scene_in)
{
  uniform Scene* uniform scene = (uniform Scene* uniform) scene_in;
  scene->numAllLights = 0;
  scene->numEnvLights = 0;
  for (uniform size_t i=0; i<scene->numInstances; i++) {
    uniform Instance* uniform instance = scene->instances[i];
    if (instance == NULL) continue;
    if (instance->light) {
      scene->allLights[scene->numAllLights++] = instance->light;
      if (instance->light->type & ENV_LIGHT) {
        scene->envLights[scene->numEnvLights++] = (uniform EnvironmentLight* uniform) instance->light;
      }
    }
  }
  rtcCommit(scene->handle);
}
