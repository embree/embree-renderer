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

#include "ispc_device.h"
#include "image/image.h"
#include "sys/taskscheduler.h"
#include "api/swapchain.h"
#include "sys/sync/barrier.h"

/* include general stuff */
#include "api/handle.h"
#include "api/data.h"
#include "api/parms.h"
#include "scene_ispc.h"
#include "instance_ispc.h"
#include "swapchain_ispc.h"

/* include all cameras */
#include "cameras/pinholecamera.h"
#include "cameras/depthoffieldcamera.h"

/* include all lights */
#include "light_ispc.h"
#include "lights/ambientlight.h"
#include "lights/pointlight.h"
#include "lights/spotlight.h"
#include "lights/directionallight.h"
#include "lights/distantlight.h"
#include "lights/hdrilight.h"
#include "lights/trianglelight.h"

/* include all materials */
#include "materials/matte.h"
#include "materials/plastic.h"
#include "materials/dielectric.h"
#include "materials/thindielectric.h"
#include "materials/mirror.h"
#include "materials/metal.h"
#include "materials/metallicpaint.h"
#include "materials/matte_textured.h"
#include "materials/obj.h"
#include "materials/velvet.h"

/* include all shapes */
#include "shape_ispc.h"
#include "shapes/trianglemesh.h"
#include "shapes/sphere.h"
#include "shapes/triangle.h"

/* include all textures */
#include "image3c_ispc.h"
#include "image3ca_ispc.h"
#include "image3f_ispc.h"
#include "image3fa_ispc.h"
#include "textures/nearestneighbor.h"

/* include all tonemappers */
#include "tonemappers/defaulttonemapper.h"

/* include all renderers */
#include "renderer_ispc.h"
#include "renderers/debugrenderer.h"
#include "renderers/pathtracer.h"

/* include ray tracing core interface */
#include <embree2/rtcore.h>

#include <stack>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define strcasecmp lstrcmpiA
#pragma warning(disable:4297) // function assumed not to throw an exception but does
#endif

double __get_seconds() {
  return embree::getSeconds();
}

extern "C" {
  int g_serverCount = 1;
  int g_serverID = 0;
}

namespace embree
{
  /*******************************************************************
                  creation of device
  *******************************************************************/
  
  __dllexport Device* create(const char* parms, size_t numThreads, const char* rtcore_cfg)
  {
    return new ISPCDevice(numThreads, rtcore_cfg);
  }

  /*******************************************************************
                             construction
  *******************************************************************/
  
  ISPCDevice::ISPCDevice(size_t numThreads, const char* cfg)
  {
    rtcInit(cfg);
  }

  ISPCDevice::~ISPCDevice() {
    rtcExit();
  }

  Device::RTCamera ISPCDevice::rtNewCamera(const char* type) 
  {
    if      (!strcasecmp(type,"pinhole"     )) return (Device::RTCamera) new ISPCCreateHandle<PinHoleCamera>;
    else if (!strcasecmp(type,"depthoffield")) return (Device::RTCamera) new ISPCCreateHandle<DepthOfFieldCamera>;
    else throw std::runtime_error("unknown camera type: "+std::string(type));
  }

  Device::RTData ISPCDevice::rtNewData(const char* type, size_t bytes, const void* data)
  {
    if      (!strcasecmp(type,"immutable"        )) return (Device::RTData) new ConstHandle<Data>(new Data(bytes,data,true));
    else if (!strcasecmp(type,"immutable_managed")) return (Device::RTData) new ConstHandle<Data>(new Data(bytes,data,false));
    else throw std::runtime_error("unknown data buffer type: "+std::string(type));
  }

  Device::RTData ISPCDevice::rtNewDataFromFile(const char* type, const char* fileName, size_t offset, size_t bytes)
  {
    /*! we always load locally */
    if (!strncmp(fileName,"server:",7)) 
      fileName += 7;

    if (!strcasecmp(type,"immutable")) 
    {
      FILE* file = fopen(fileName,"rb");
      if (!file) throw std::runtime_error("cannot open file "+(std::string)fileName);
      
      Data* data = new Data(bytes);
      fseek(file,(long)offset,SEEK_SET);
      if (bytes != fread(data->map(),1,bytes,file))
        throw std::runtime_error("error filling data buffer from file");
      fclose(file);

      return (Device::RTData) new ConstHandle<Data>(data);
    }
    else
      throw std::runtime_error("unknown data buffer type: "+std::string(type));
  }

  Device::RTImage ISPCDevice::rtNewImage(const char* type, size_t width, size_t height, const void* data, const bool copy)
  {
    if (!strcasecmp(type,"RGB8")) 
      return (Device::RTImage) new ISPCConstHandle(ispc::Image3c__new(width,height,(ispc::vec3uc*)data,copy));
    else if (!strcasecmp(type,"RGBA8"))
      return (Device::RTImage) new ISPCConstHandle(ispc::Image3ca__new(width,height,(unsigned*)data,copy));
    else if (!strcasecmp(type,"RGB_FLOAT32"))
      return (Device::RTImage) new ISPCConstHandle(ispc::Image3f__new(width,height,(ispc::vec3f*)data,copy));
    else if (!strcasecmp(type,"RGBA_FLOAT32"))
      return (Device::RTImage) new ISPCConstHandle(ispc::Image3fa__new(width,height,(ispc::vec3fa*)data,copy));
    else
      throw std::runtime_error("unknown image type: "+std::string(type));
  }

  Device::RTImage ISPCDevice::rtNewImageFromFile(const char* file)
  {
#if defined(__MIC__)      
      throw std::runtime_error("rtNewImageFromFile not supported on MIC");
#else
    if (!strncmp(file,"server:",7)) file += 7;
    Ref<Image> image = loadImage(file);
    if (Ref<Image4f> img = image.dynamicCast<Image4f>())
      return rtNewImage("RGBA_FLOAT32",img->width,img->height,img->ptr(),true);
    else if (Ref<Image3f> img = image.dynamicCast<Image3f>())
      return rtNewImage("RGB_FLOAT32",img->width,img->height,img->ptr(),true);
    else if (Ref<Image4c> img = image.dynamicCast<Image4c>())
      return rtNewImage("RGBA8",img->width,img->height,img->ptr(),true);
    else if (Ref<Image3c> img = image.dynamicCast<Image3c>())
      return rtNewImage("RGB8",img->width,img->height,img->ptr(),true);
    else {
      int c = 0xFFFFFFFF;
      return rtNewImage("RGB8",1,1,&c,true);
    }
#endif
  }

  Device::RTTexture ISPCDevice::rtNewTexture(const char* type) 
  {
    if      (!strcasecmp(type,"nearest")) return (Device::RTTexture) new ISPCCreateHandle<NearestNeighborTexture>;
    else if (!strcasecmp(type,"image"  )) return (Device::RTTexture) new ISPCCreateHandle<NearestNeighborTexture>;
    else throw std::runtime_error("unknown texture type: "+std::string(type));
  }

  Device::RTMaterial ISPCDevice::rtNewMaterial(const char* type) 
  {
    if (!strcasecmp(type,"Matte")) return (Device::RTMaterial) new ISPCCreateHandle<Matte>;
    else if (!strcasecmp(type,"Plastic")       ) return (Device::RTMaterial) new ISPCCreateHandle<Plastic>;
    else if (!strcasecmp(type,"Dielectric")    ) return (Device::RTMaterial) new ISPCCreateHandle<Dielectric>;
    else if (!strcasecmp(type,"Glass")         ) return (Device::RTMaterial) new ISPCCreateHandle<Dielectric>;
    else if (!strcasecmp(type,"ThinDielectric")) return (Device::RTMaterial) new ISPCCreateHandle<ThinDielectric>;
    else if (!strcasecmp(type,"ThinGlass")     ) return (Device::RTMaterial) new ISPCCreateHandle<ThinDielectric>;
    else if (!strcasecmp(type,"Mirror")        ) return (Device::RTMaterial) new ISPCCreateHandle<Mirror>;
    else if (!strcasecmp(type,"Metal")         ) return (Device::RTMaterial) new ISPCCreateHandle<Metal>;
    //else if (!strcasecmp(type,"BrushedMetal")  ) return (Device::RTMaterial) new ISPCCreateHandle<BrushedMetal>;
    else if (!strcasecmp(type,"MetallicPaint") ) return (Device::RTMaterial) new ISPCCreateHandle<MetallicPaint>;
    else if (!strcasecmp(type,"MatteTextured") ) return (Device::RTMaterial) new ISPCCreateHandle<MatteTextured>;
    else if (!strcasecmp(type,"Obj")           ) return (Device::RTMaterial) new ISPCCreateHandle<Obj>;
    else if (!strcasecmp(type,"Velvet")        ) return (Device::RTMaterial) new ISPCCreateHandle<Velvet>;
    //else if (!strcasecmp(type,"Velvet2")       ) return (Device::RTMaterial) new ISPCCreateHandle<Velvet2>;
    //else if (!strcasecmp(type,"Satin")         ) return (Device::RTMaterial) new ISPCCreateHandle<Satin>;
    //else if (!strcasecmp(type,"Skin")          ) return (Device::RTMaterial) new ISPCCreateHandle<Skin>;
    //else if (!strcasecmp(type,"Cotton")        ) return (Device::RTMaterial) new ISPCCreateHandle<Cotton>;
    //else if (!strcasecmp(type,"Woven")         ) return (Device::RTMaterial) new ISPCCreateHandle<Woven>;
    //else if (!strcasecmp(type,"WovenIsotropic")) return (Device::RTMaterial) new ISPCCreateHandle<WovenIsotropic>;
    //else if (!strcasecmp(type,"Eye"           )) return (Device::RTMaterial) new ISPCCreateHandle<Eye>;
    //else if (!strcasecmp(type,"EyeLash"       )) return (Device::RTMaterial) new ISPCCreateHandle<EyeLash>;
    else { 
      //throw std::runtime_error("unknown material type: "+std::string(type));
      printf("WARNING: unknown material \"%s\", using Matte as default\n",type);
      return (Device::RTMaterial) new ISPCCreateHandle<Matte>;
    }
  }

  Device::RTShape ISPCDevice::rtNewShape(const char* type) 
  {
    _RTHandle* shape = NULL;
    if      (!strcasecmp(type,"trianglemesh")) shape = new ISPCCreateHandle<ISPCTriangleMesh>;
    else if (!strcasecmp(type,"triangle")    ) shape = new ISPCCreateHandle<Triangle>;
    else if (!strcasecmp(type,"sphere")      ) shape = new ISPCCreateHandle<Sphere>;
    //else if (!strcasecmp(type,"disk")        ) shape = new ISPCCreateHandle<Disk,Shape>;
    else throw std::runtime_error("unknown shape type: "+std::string(type));
    return (Device::RTShape) shape;
  }

  Device::RTLight ISPCDevice::rtNewLight(const char* type) 
  {
    if      (!strcasecmp(type,"ambientlight"    )) return (Device::RTLight) new ISPCCreateHandle<AmbientLight>;
    else if (!strcasecmp(type,"pointlight"      )) return (Device::RTLight) new ISPCCreateHandle<PointLight>;
    else if (!strcasecmp(type,"spotlight"       )) return (Device::RTLight) new ISPCCreateHandle<SpotLight>;
    else if (!strcasecmp(type,"directionallight")) return (Device::RTLight) new ISPCCreateHandle<DirectionalLight>;
    else if (!strcasecmp(type,"distantlight"    )) return (Device::RTLight) new ISPCCreateHandle<DistantLight>;
    else if (!strcasecmp(type,"hdrilight"       )) return (Device::RTLight) new ISPCCreateHandle<HDRILight>;
    else if (!strcasecmp(type,"trianglelight"   )) return (Device::RTLight) new ISPCCreateHandle<TriangleLight>;
    else throw std::runtime_error("unknown light type: "+std::string(type));
  }

  /*! Primitive Handle */
  class PrimitiveHandle : public _RTHandle {   
    ALIGNED_CLASS;
  public:

    /*! Constructs new primitive. */
    PrimitiveHandle (const ISPCRef& shape, const ISPCRef& light, const ISPCRef& material, const AffineSpace3f& transform,
                     light_mask_t illumMask = -1, light_mask_t shadowMask = -1)
      : shape(shape), light(light), material(material), transform(transform),
        illumMask(illumMask), shadowMask(shadowMask), light_instance(NULL) {}

    /*! Creation of new primitive. */
    void create() 
    { 
      if (light) {
        light_instance = ispc::Light__transform(light.ptr,
                                                (ispc::vec3f&)transform.l.vx,
                                                (ispc::vec3f&)transform.l.vy,
                                                (ispc::vec3f&)transform.l.vz,
                                                (ispc::vec3f&)transform.p);
      }
    }

    /*! Setting parameters. */
    void set(const std::string& property, const Variant& data) 
    { 
      if      (property == "illumMask" ) illumMask  = data.getInt();
      else if (property == "shadowMask") shadowMask = data.getInt();
    }
    
    ISPCRef getLightInstance() 
    {
      if (!light_instance) create();
      return light_instance;
    }


  public:
    ISPCRef shape;              //!< Shape in case of a shape primitive
    ISPCRef light;              //!< Light in case of a light primitive
    ISPCRef material;           //!< Material of shape primitive
    AffineSpace3f transform;    //!< Transformation of primitive
    light_mask_t illumMask; 
    light_mask_t shadowMask;
    ISPCRef light_instance;
  };

  Device::RTPrimitive ISPCDevice::rtNewShapePrimitive(Device::RTShape shape_i, Device::RTMaterial material_i, const float* transform)
  {
    ISPCNormalHandle* shape = castHandle<ISPCNormalHandle>(shape_i,"shape");
    ISPCNormalHandle* material = castHandle<ISPCNormalHandle>(material_i,"material");
    AffineSpace3f space = transform ? copyFromArray(transform) : AffineSpace3f(one);
    return (Device::RTPrimitive) new PrimitiveHandle(shape->instance,NULL,material->instance,space);
  }

  Device::RTPrimitive ISPCDevice::rtNewLightPrimitive(Device::RTLight light_i, Device::RTMaterial material_i, const float* transform)
  {
    ISPCNormalHandle* light = castHandle<ISPCNormalHandle>(light_i,"light");
    ISPCRef material = NULL;
    if (material_i) material = castHandle<ISPCNormalHandle>(material_i,"material")->instance;
    AffineSpace3f space = transform ? copyFromArray(transform) : AffineSpace3f(one);
    return (Device::RTPrimitive) new PrimitiveHandle(NULL,light->instance,material,space);
  }

  Device::RTPrimitive ISPCDevice::rtTransformPrimitive(Device::RTPrimitive primitive, const float* transform) 
  {
    PrimitiveHandle* prim = dynamic_cast<PrimitiveHandle*>((_RTHandle*)primitive);
    AffineSpace3f space = transform ? copyFromArray(transform) : AffineSpace3f(one);
    return (Device::RTPrimitive) new PrimitiveHandle(prim->shape,prim->light,prim->material,space * prim->transform);
  }

  class SceneHandle : public _RTHandle 
  {
    ALIGNED_CLASS;
  public:

    SceneHandle () 
      : instance(ispc::Scene__new()) {}

    void set(const std::string& property, const Variant& data) {
    }

    void set(size_t slot, PrimitiveHandle* prim)
    {
      if (prim == NULL)
        ispc::Scene__set(instance.ptr,slot,NULL);

      if (prim->shape)
      {
        ISPCRef shape = ispc::Shape__transform(prim->shape.ptr,
                                               (ispc::vec3f&)prim->transform.l.vx,
                                               (ispc::vec3f&)prim->transform.l.vy,
                                               (ispc::vec3f&)prim->transform.l.vz,
                                               (ispc::vec3f&)prim->transform.p);

        ISPCRef inst = ispc::Instance__new(shape.ptr,prim->material.ptr,NULL);
        ispc::Scene__set(instance.ptr,slot,inst.ptr);
      }
      
      else if (prim->light)
      {
        ISPCRef light = ispc::Light__transform(prim->light.ptr,
                                               (ispc::vec3f&)prim->transform.l.vx,
                                               (ispc::vec3f&)prim->transform.l.vy,
                                               (ispc::vec3f&)prim->transform.l.vz,
                                               (ispc::vec3f&)prim->transform.p);
        
        ISPCRef shape = ispc::Light__shape(light.ptr);
        ISPCRef inst = ispc::Instance__new(shape.ptr,prim->material.ptr,light.ptr);
        ispc::Scene__set(instance.ptr,slot,inst.ptr);
      }
      else throw std::runtime_error("invalid primitive");
    }

    void create() {
      ispc::Scene__commit(instance.ptr);
    }

  public:
    ISPCRef instance;
  };

  Device::RTScene ISPCDevice::rtNewScene(const char* type) {
    if (!strcmp(type,"default" )) return (Device::RTScene) new SceneHandle;
    else throw std::runtime_error("unknown scene type: "+std::string(type));
  }
     
  void ISPCDevice::rtSetPrimitive(RTScene hscene, size_t slot, RTPrimitive hprim) 
  {
    SceneHandle* scene = castHandle<SceneHandle>(hscene,"scene");
    if (hprim == NULL) { scene->set(slot,NULL); return; }
    PrimitiveHandle* prim = dynamic_cast<PrimitiveHandle*>((_RTHandle*)hprim);
    scene->set(slot,prim);
  }

  Device::RTToneMapper ISPCDevice::rtNewToneMapper(const char* type)
  {
    if (!strcasecmp(type,"default")) return (Device::RTToneMapper) new ISPCCreateHandle<DefaultToneMapper>;
    else throw std::runtime_error("unknown tonemapper type: "+std::string(type));
  }

  Device::RTRenderer ISPCDevice::rtNewRenderer(const char* type) 
  {
    if      (!strcasecmp(type,"debug"     )) return (Device::RTRenderer) new ISPCCreateHandle<DebugRenderer>;
    else if (!strcasecmp(type,"pathtracer")) return (Device::RTRenderer) new ISPCCreateHandle<PathTracer>;
    else throw std::runtime_error("unknown renderer type: " + std::string(type));
  }

  Device::RTFrameBuffer ISPCDevice::rtNewFrameBuffer(const char* type, size_t width, size_t height, size_t buffers, void** ptrs) 
  {
    if      (!strcasecmp(type,"RGB_FLOAT32")) return (Device::RTFrameBuffer) new ISPCConstHandle(ispc::SwapChainRGBFloat32__new(width,height,buffers,(void**)ptrs));
    else if (!strcasecmp(type,"RGBA8"      )) return (Device::RTFrameBuffer) new ISPCConstHandle(ispc::SwapChainRGBA8__new(width,height,buffers,(void**)ptrs));
#if !defined(__MIC__)
    else if (!strcasecmp(type,"RGB8"       )) return (Device::RTFrameBuffer) new ISPCConstHandle(ispc::SwapChainRGB8__new(width,height,buffers,(void**)ptrs));
#endif
    else throw std::runtime_error("unknown framebuffer type: "+std::string(type));
  }

  void* ISPCDevice::rtMapFrameBuffer(Device::RTFrameBuffer swapchain_i, int bufID) 
  {
    ISPCConstHandle* swapchain = castHandle<ISPCConstHandle>(swapchain_i,"framebuffer");
    return ispc::SwapChain__map(swapchain->instance.ptr,bufID);
  }

  void ISPCDevice::rtUnmapFrameBuffer(Device::RTFrameBuffer swapchain_i, int bufID) 
  {
    ISPCConstHandle* swapchain = castHandle<ISPCConstHandle>(swapchain_i,"framebuffer");
    ispc::SwapChain__unmap(swapchain->instance.ptr);
  }

  void ISPCDevice::rtSwapBuffers(Device::RTFrameBuffer swapchain_i) 
  {
    ISPCConstHandle* swapchain = castHandle<ISPCConstHandle>(swapchain_i,"framebuffer");
    ispc::SwapChain__swap(swapchain->instance.ptr);
  }

  void ISPCDevice::rtIncRef(Device::RTHandle handle) {
    ((_RTHandle*)handle)->incRef();
  }

  void ISPCDevice::rtDecRef(Device::RTHandle handle) {
    ((_RTHandle*)handle)->decRef();
  }

  /*******************************************************************
                  setting of parameters
  *******************************************************************/

  void ISPCDevice::rtSetBool1(Device::RTHandle handle, const char* property, bool x) {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x));
  }

  void ISPCDevice::rtSetBool2(Device::RTHandle handle, const char* property, bool x, bool y)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y));
  }

  void ISPCDevice::rtSetBool3(Device::RTHandle handle, const char* property, bool x, bool y, bool z) {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y,z));
  }

  void ISPCDevice::rtSetBool4(Device::RTHandle handle, const char* property, bool x, bool y, bool z, bool w)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y,z,w));
  }

  void ISPCDevice::rtSetInt1(Device::RTHandle handle, const char* property, int x)  {
    Lock<MutexSys> lock(mutex);
    if (!property) throw std::runtime_error("invalid property");
    if (!handle  ) {
      if      (!strcmp(property,"serverID"   )) g_serverID = x;
      else if (!strcmp(property,"serverCount")) g_serverCount = x;
      return;
    }
    ((_RTHandle*)handle)->set(property,Variant(x));
  }

  void ISPCDevice::rtSetInt2(Device::RTHandle handle, const char* property, int x, int y)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y));
  }

  void ISPCDevice::rtSetInt3(Device::RTHandle handle, const char* property, int x, int y, int z)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y,z));
  }

  void ISPCDevice::rtSetInt4(Device::RTHandle handle, const char* property, int x, int y, int z, int w)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y,z,w));
  }

  void ISPCDevice::rtSetFloat1(Device::RTHandle handle, const char* property, float x)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x));
  }

  void ISPCDevice::rtSetFloat2(Device::RTHandle handle, const char* property, float x, float y)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y));
  }

  void ISPCDevice::rtSetFloat3(Device::RTHandle handle, const char* property, float x, float y, float z)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y,z));
  }

  void ISPCDevice::rtSetFloat4(Device::RTHandle handle, const char* property, float x, float y, float z, float w)  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(x,y,z,w));
  }

  void ISPCDevice::rtSetArray(Device::RTHandle handle_i, const char* property, const char* type, Device::RTData data_i, size_t size, size_t stride, size_t ofs)
  {
    ConstHandle<Data>* data    = castHandle<ConstHandle<Data> >(data_i,"data");
    _RTHandle* handle = (_RTHandle*)handle_i;
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    if      (!strcasecmp(type,"bool1" )) handle->set(property,Variant(data->instance,Variant::BOOL1 ,size,stride == size_t(-1) ? 1*sizeof(bool ) : stride, ofs));
    else if (!strcasecmp(type,"bool2" )) handle->set(property,Variant(data->instance,Variant::BOOL2 ,size,stride == size_t(-1) ? 2*sizeof(bool ) : stride, ofs));
    else if (!strcasecmp(type,"bool3" )) handle->set(property,Variant(data->instance,Variant::BOOL3 ,size,stride == size_t(-1) ? 3*sizeof(bool ) : stride, ofs));
    else if (!strcasecmp(type,"bool4" )) handle->set(property,Variant(data->instance,Variant::BOOL4 ,size,stride == size_t(-1) ? 4*sizeof(bool ) : stride, ofs)); 
    else if (!strcasecmp(type,"int1"  )) handle->set(property,Variant(data->instance,Variant::INT1  ,size,stride == size_t(-1) ? 1*sizeof(int  ) : stride, ofs));
    else if (!strcasecmp(type,"int2"  )) handle->set(property,Variant(data->instance,Variant::INT2  ,size,stride == size_t(-1) ? 2*sizeof(int  ) : stride, ofs));
    else if (!strcasecmp(type,"int3"  )) handle->set(property,Variant(data->instance,Variant::INT3  ,size,stride == size_t(-1) ? 3*sizeof(int  ) : stride, ofs));
    else if (!strcasecmp(type,"int4"  )) handle->set(property,Variant(data->instance,Variant::INT4  ,size,stride == size_t(-1) ? 4*sizeof(int  ) : stride, ofs));
    else if (!strcasecmp(type,"float1")) handle->set(property,Variant(data->instance,Variant::FLOAT1,size,stride == size_t(-1) ? 1*sizeof(float) : stride, ofs));
    else if (!strcasecmp(type,"float2")) handle->set(property,Variant(data->instance,Variant::FLOAT2,size,stride == size_t(-1) ? 2*sizeof(float) : stride, ofs));
    else if (!strcasecmp(type,"float3")) handle->set(property,Variant(data->instance,Variant::FLOAT3,size,stride == size_t(-1) ? 3*sizeof(float) : stride, ofs)); 
    else if (!strcasecmp(type,"float4")) handle->set(property,Variant(data->instance,Variant::FLOAT4,size,stride == size_t(-1) ? 4*sizeof(float) : stride, ofs));
    else throw std::runtime_error("unknown array type: "+std::string(type));
  }

  void ISPCDevice::rtSetString(Device::RTHandle handle, const char* property, const char* str) 
  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(str));
  }

  void ISPCDevice::rtSetImage(Device::RTHandle handle, const char* property, Device::RTImage img) 
  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    if (ISPCConstHandle* image = dynamic_cast<ISPCConstHandle*>((_RTHandle*)img)) {
      if (!image->instance) throw std::runtime_error("invalid image value");
      ((_RTHandle*)handle)->set(property,Variant(Variant::IMAGE,image->instance));
    } 
    else throw std::runtime_error("invalid image handle");
  }

  void ISPCDevice::rtSetTexture(Device::RTHandle handle, const char* property, Device::RTTexture tex) 
  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ISPCNormalHandle* texture = castHandle<ISPCNormalHandle>(tex,"texture");
    ((_RTHandle*)handle)->set(property,Variant(Variant::TEXTURE,texture->instance));
  }

  void ISPCDevice::rtSetTransform(Device::RTHandle handle, const char* property, const float* transform)
  {
    Lock<MutexSys> lock(mutex);
    if (!handle  ) throw std::runtime_error("invalid handle"  );
    if (!property) throw std::runtime_error("invalid property");
    ((_RTHandle*)handle)->set(property,Variant(copyFromArray(transform)));
  }
  
  void ISPCDevice::rtClear(Device::RTHandle handle) {
    Lock<MutexSys> lock(mutex);
    if (!handle) throw std::runtime_error("invalid handle");
    ((_RTHandle*)handle)->clear();
  }

  void ISPCDevice::rtCommit(Device::RTHandle handle) {
    Lock<MutexSys> lock(mutex);
    if (!handle) throw std::runtime_error("invalid handle");
    ((_RTHandle*)handle)->create();
  }

  /*******************************************************************
                            render call
  *******************************************************************/

  void ISPCDevice::rtRenderFrame(Device::RTRenderer renderer_i, Device::RTCamera camera_i,
                                   Device::RTScene scene_i, Device::RTToneMapper toneMapper_i, 
                                   Device::RTFrameBuffer swapchain_i, int accumulate)
  {
    Lock<MutexSys> lock(mutex);
    ISPCNormalHandle* renderer   = castHandle<ISPCNormalHandle>(renderer_i   ,"renderer"  );
    ISPCNormalHandle* camera     = castHandle<ISPCNormalHandle>(camera_i     ,"camera"    );
    SceneHandle* scene       = castHandle<SceneHandle> (scene_i      ,"scene"     );
    ISPCNormalHandle* toneMapper = castHandle<ISPCNormalHandle>(toneMapper_i ,"tonemapper");
    ISPCConstHandle* swapchain    = castHandle<ISPCConstHandle>  (swapchain_i,"framebuffer");

    ispc::Renderer__renderFrameInit(renderer->instance.ptr,scene->instance.ptr);
    double t0 = getSeconds();
    int numRays = ispc::Renderer__renderFrame(renderer->instance.ptr,camera->instance.ptr,scene->instance.ptr,toneMapper->instance.ptr,swapchain->instance.ptr,accumulate);
    double dt = getSeconds() - t0;
    printf("render %3.2f fps, %.2f ms,  %3.3f mrps\n",1.0f/dt,dt*1000.0f,numRays/dt*1E-6); flush(std::cout);
  }

  bool ISPCDevice::rtPick(Device::RTCamera camera_i, float x, float y, Device::RTScene scene_i, float& px, float& py, float& pz)
  {
    Lock<MutexSys> lock(mutex);

    /* extract objects from handles */
    ISPCNormalHandle* camera = castHandle<ISPCNormalHandle>(camera_i,"camera");
    SceneHandle* scene   = castHandle<SceneHandle> (scene_i ,"scene" );

    /* trace ray */
    Vector3f p;
    bool hit = ispc::Renderer__pick(camera->instance.ptr,x,y,scene->instance.ptr,(ispc::vec3f&)p);
    px = p.x; py = p.y; pz = p.z;
    return hit;
  }
}
