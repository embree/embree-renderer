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

#ifndef __EMBREE_BACKEND_SCENE_FLAT_H__
#define __EMBREE_BACKEND_SCENE_FLAT_H__

#include "scene.h"
#include <embree2/rtcore.h>

namespace embree
{
  /*! Flat scene, no support for instancing, best render performance. */
  class BackendSceneFlat : public BackendScene
  {
  public:

    struct Primitive : public RefCount
    {
    public:
      Primitive (const Ref<Shape>& shape,
                 const Ref<Light>& light,
                 const Ref<Material>& material,
                 const light_mask_t illumMask,
                 const light_mask_t shadowMask)
        : shape(shape), light(light), material(material),
          illumMask(illumMask), shadowMask(shadowMask) {}

      __forceinline void postIntersect(const Ray& ray, DifferentialGeometry& dg) const 
      {
        dg.material   = material.ptr;
        dg.light      = (AreaLight*) light.ptr;
        dg.illumMask  = illumMask;
        dg.shadowMask = shadowMask;
        shape->postIntersect(ray,dg);
      }
      
    public:
      Ref<Shape> shape;
      Ref<Light> light;
      Ref<Material> material;
      light_mask_t illumMask;  /*! which light masks we receive illum from */
      light_mask_t shadowMask; /*! which light masks we cast shadows to */
    };  // Primitive

    /*! API handle that manages user actions. */
    class Handle : public BackendScene::Handle {
      ALIGNED_CLASS;
    public:
      
      void setPrimitive(size_t slot, Ref<PrimitiveHandle> prim) 
      {
		// BUG BUG BUG - this mostly works, although the visual artifacts suggest something is
		// still wrong
        if (slot >= prims.size()) {
			prims.resize(slot+1);
			modified.resize(slot+1);
		}
        Ref<Shape> shape = prim->getShapeInstance();
        Ref<Light> light = prim->getLightInstance();
        if (light) shape = light->shape();
        if (shape) shape = shape->transform(prim->transform);
        if (light) light = light->transform(prim->transform,prim->illumMask,prim->shadowMask);
		if (prims[slot]) {
			// check if existing primitive is a shape, if so, changes will require
			// a rebuild (may be assigned to NULL later, but still need to rebuild).
			if (!(prims[slot]->light)) {
			  rebuild = true;
			}
        }
        prims[slot] = new Primitive(shape,light,prim->getMaterialInstance(),prim->illumMask,prim->shadowMask);
		// If new primitive is a shape we need to rebuild
		if (!light) rebuild=true;
		modified[slot] = true;
      }
      
      void create() 
      {
		if (rebuild) {
			RTCScene scene = rtcNewScene(RTC_SCENE_STATIC,RTC_INTERSECT1);
			for (size_t i=0; i<prims.size(); i++) {
			  if (prims[i] && prims[i]->shape)
				prims[i]->shape->extract(scene,i);
			}
			rtcCommit(scene);
        
			/* create new scene */
			instance = new BackendSceneFlat(prims,scene);
			rebuild = false;
		}
		else
		{
		// BUG BUG BUG - this mostly works, although the visual artifacts suggest something is
		// still wrong
			// Just replace/add existing lights - geometry unchanged
			size_t numoldlights = instance->allLights.size();  // lights already in the geometry list
			Ref<BackendSceneFlat>& besf = (Ref<BackendSceneFlat>&)instance;
			size_t numoldgeom = besf->geometry.size(); // size of old geometry list
			size_t numnewgeom = prims.size();  // size of new geometry list

			// Zap old light list and rebuild
			instance->allLights.clear();
			instance->envLights.clear();
			for (size_t i=0; i<numnewgeom; i++) 
			{
				// This mirrors what new BackendSceneFlat(prims,scene) does
				const Ref<Primitive>& prim = prims[i];
				if (prim && prim->light) 
					besf->add(prim->light);
			}

			size_t currnewlight = 0;
			// Now go through old geometry list and replace old lights with new ones
			for (size_t i=0; i<numoldgeom; i++) {
				size_t j;
				const Ref<Primitive>& prim = besf->geometry[i];
				if (prim && prim->light) {
					// Replace old light with new one
					for  (j = currnewlight; j<numnewgeom; j++) {
						// Find next light in new list
						const Ref<Primitive>& newprim = prims[j];
						if (newprim && newprim->light)
							break;  // Found the next light
					}
					// And replace the old light with the new one
					besf->geometry[i] = prims[j];

					currnewlight = j+1;  // Record where to start looking for the next light
				} // found light
			}

			// Finally, add any new stuff (lights) to the end
			for (size_t i=currnewlight; i<numnewgeom; i++)
				besf->geometry.push_back(prims[i]);

		}
      }
      
    public:
      std::vector<Ref<Primitive> > prims;
    };  // Handle
        
    /*! Construction of scene. */
    BackendSceneFlat (const std::vector<Ref<Primitive> >& geometry, RTCScene scene)
      : BackendScene(scene), geometry(geometry)
    {
      for (size_t i=0; i<geometry.size(); i++) {
        const Ref<Primitive>& prim = geometry[i];
        if (prim && prim->light) add(prim->light);
      }
    }

    /*! Helper to call the post intersector of the shape instance,
     *  which will call the post intersector of the shape. */
    void postIntersect(const Ray& ray, DifferentialGeometry& dg) const {
      if (ray) geometry[ray.id0]->postIntersect(ray,dg);
    }

	void updateObjectMaterial(size_t slot, Ref<InstanceHandle<Material> > material)
	{
		Ref<Primitive>& prim = geometry[slot];
		prim->material = material->getInstance();
	}

  private:
    std::vector<Ref<Primitive> > geometry;  //!< Geometry of the scene
  };
}

#endif
