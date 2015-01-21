// ======================================================================== //
// Copyright 2009-2015 Intel Corporation                                    //
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
//
// This software contains copyrighted code owned by Autodesk, Inc. but has 
// been modified and is not endorsed by Autodesk, Inc.
//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

#include <stdio.h>

#include <EmbreeViewportRenderer.h>


SceneData::SceneData()   // Constructor
{
   numPrimitives = -1;
   numIndices = -1;
   numPositions = -1;
   boundBox.clear();
   transform.setToIdentity();
   color.r = 0; color.g = 0; color.b = 0; color.a = 1;
   intensity = 0;
   lightDirection = MFloatVector::zero;
   coneAngle = 0;
   penumbraAngle = 0;
   decayRate = 0;
   diffuseTextureFile.clear();
   specularTextureFile.clear();
   bumpTextureFile.clear();
   haveTexture = 0;
   diffcolor[0] = 1.0; diffcolor[1] = 1.0; diffcolor[2] = 1.0; 
   transparentColor[0] = 0.0; transparentColor[1] = 0.0; transparentColor[2] = 0.0;
   specularColor[0] = 0.0; specularColor[1] = 0.0; specularColor[2] = 0.0;
   absorption[0] = 0.0; absorption[1] = 0.0; absorption[2] = 0.0;
   roughness = 0.0f;
   refractIndex = 1.5f;
   diffusionExponent = 100.0f;
   transmissionDepth = 9999999.9f;
    
   eindex = -1;
   lindex = -1;
}

SceneData::SceneData(const SceneData &copyin)   // Copy constructor to handle pass by value.
{                             
   numPrimitives = copyin.numPrimitives;
   numIndices = copyin.numIndices;
   numPositions = copyin.numPositions;
   boundBox = copyin.boundBox;
   transform = copyin.transform;
   color = copyin.color;
   intensity = copyin.intensity;
   lightDirection = copyin.lightDirection;
   coneAngle = copyin.coneAngle;
   penumbraAngle = copyin.penumbraAngle;
   decayRate = copyin.decayRate;
   diffuseTextureFile = copyin.diffuseTextureFile;
   specularTextureFile = copyin.specularTextureFile;
   bumpTextureFile = copyin.bumpTextureFile;
   haveTexture = copyin.haveTexture;
   diffcolor[0] = copyin.diffcolor[0]; diffcolor[1] = copyin.diffcolor[1]; diffcolor[2] = copyin.diffcolor[2];
   transparentColor[0] = copyin.transparentColor[0]; transparentColor[1] = copyin.transparentColor[1]; transparentColor[2] = copyin.transparentColor[2];
   specularColor[0] = copyin.specularColor[0]; specularColor[1] = copyin.specularColor[1]; specularColor[2] = copyin.specularColor[2];
   absorption[0] = copyin.absorption[0]; absorption[1] = copyin.absorption[1]; absorption[2] = copyin.absorption[2];
   roughness = copyin.roughness;
   refractIndex = copyin.refractIndex;
   diffusionExponent = copyin.diffusionExponent;
   transmissionDepth = copyin.transmissionDepth;

   eindex = copyin.eindex;
   lindex = copyin.lindex;
}

ostream &operator<<(ostream &output, const SceneData &aaa)
{
   output << "Scene Data element:" << endl;
   output << aaa.numPrimitives << ' ' << aaa.numIndices << ' ' << aaa.numPositions << endl;
   output << aaa.boundBox.center() << ' ' << aaa.boundBox.width() << ' ' << aaa.boundBox.height() << ' ' << aaa.boundBox.depth() << endl;
   output << aaa.transform << endl;
   output << aaa.color << ' ' << aaa.intensity << ' ' << aaa.lightDirection << endl;
   output << aaa.coneAngle << ' ' << aaa.penumbraAngle << ' ' << aaa.decayRate << endl;
   output << "Embree Index " << aaa.eindex << "  List Index " << aaa.lindex << endl;
   if (aaa.haveTexture) {
	   output << "Diffuse Texture name " << aaa.diffuseTextureFile << endl;
       output << "Specular Texture name " << aaa.specularTextureFile << endl;
       output << "Bump Texture name " << aaa.bumpTextureFile << endl;
   }
   output << "diffcolor = " << aaa.diffcolor[0] << "," << aaa.diffcolor[1] << "," << aaa.diffcolor[2] << endl; 
   output << "transparentColor = " << aaa.transparentColor[0] << "," << aaa.transparentColor[1] << "," << aaa.transparentColor[2] << endl; 
   output << "specularColor = " << aaa.specularColor[0] << "," << aaa.specularColor[1] << "," << aaa.specularColor[2] << endl; 
   output << "absorption = " << aaa.absorption[0] << "," << aaa.absorption[1] << "," << aaa.absorption[2] << endl; 
   output << "roughness=" << aaa.roughness << " refractindex= " << aaa.refractIndex << endl;
   output << "diffusionExponent=" << aaa.diffusionExponent << " transmissionDepth= " << aaa.transmissionDepth << endl;
   return output;
}

SceneData& SceneData::operator=(const SceneData &rhs)
{
   this->numPrimitives = rhs.numPrimitives;
   this->numIndices = rhs.numIndices;
   this->numPositions = rhs.numPositions;
   this->boundBox = rhs.boundBox;
   this->transform = rhs.transform;
   this->color = rhs.color;
   this->intensity = rhs.intensity;
   this->lightDirection = rhs.lightDirection;
   this->coneAngle = rhs.coneAngle;
   this->penumbraAngle = rhs.penumbraAngle;
   this->decayRate = rhs.decayRate;
   this->haveTexture = rhs.haveTexture;
   this->diffuseTextureFile = rhs.diffuseTextureFile;
   this->specularTextureFile = rhs.specularTextureFile;
   this->bumpTextureFile = rhs.bumpTextureFile;
   this->diffcolor[0] = rhs.diffcolor[0]; this->diffcolor[1] = rhs.diffcolor[1]; this->diffcolor[2] = rhs.diffcolor[2]; 
   this->transparentColor[0] = rhs.transparentColor[0]; this->transparentColor[1] = rhs.transparentColor[1]; this->transparentColor[2] = rhs.transparentColor[2]; 
   this->specularColor[0] = rhs.specularColor[0]; this->specularColor[1] = rhs.specularColor[1]; this->specularColor[2] = rhs.specularColor[2]; 
   this->absorption[0] = rhs.absorption[0]; this->absorption[1] = rhs.absorption[1]; this->absorption[2] = rhs.absorption[2]; 
   this->roughness = rhs.roughness;
   this->refractIndex = rhs.refractIndex;
   this->diffusionExponent = rhs.diffusionExponent;
   this->transmissionDepth = rhs.transmissionDepth;
   
   this->eindex = rhs.eindex;
   this->lindex = rhs.lindex;
   return *this;
}

int SceneData::operator==(const SceneData &rhs) const
{
   if( this->numPrimitives != rhs.numPrimitives) return 0;
   if( this->numIndices != rhs.numIndices) return 0;
   if( this->numPositions != rhs.numPositions) return 0;
   if( this->transform != rhs.transform) return 0; 
   if( this->intensity != rhs.intensity) return 0;
   if( this->boundBox.center() != rhs.boundBox.center()) return 0;
   if( this->boundBox.min() != rhs.boundBox.min()) return 0;
   if( this->boundBox.max() != rhs.boundBox.max()) return 0;
   if( this->color != rhs.color) return 0;
   if( this->lightDirection != rhs.lightDirection) return 0;
   if( this->coneAngle != rhs.coneAngle) return 0;
   if( this->penumbraAngle != rhs.penumbraAngle) return 0;
   if( this->decayRate != rhs.decayRate) return 0;
   if( this->haveTexture != rhs.haveTexture) return 0;
   if( this->diffuseTextureFile != rhs.diffuseTextureFile) return 0;
   if( this->specularTextureFile != rhs.specularTextureFile) return 0;
   if( this->bumpTextureFile != rhs.bumpTextureFile) return 0;
   if( this->diffcolor[0] != rhs.diffcolor[0]) return 0; if( this->diffcolor[1] != rhs.diffcolor[1]) return 0; if( this->diffcolor[2] != rhs.diffcolor[2]) return 0;
   if( this->transparentColor[0] != rhs.transparentColor[0]) return 0; if( this->transparentColor[1] != rhs.transparentColor[1]) return 0; if( this->transparentColor[2] != rhs.transparentColor[2]) return 0;
   if( this->specularColor[0] != rhs.specularColor[0]) return 0; if( this->specularColor[1] != rhs.specularColor[1]) return 0; if( this->specularColor[2] != rhs.specularColor[2]) return 0;
   if( this->absorption[0] != rhs.absorption[0]) return 0; if( this->absorption[1] != rhs.absorption[1]) return 0; if( this->absorption[2] != rhs.absorption[2]) return 0;
   if( this->roughness != rhs.roughness) return 0;
   if( this->refractIndex != rhs.refractIndex) return 0;
   if( this->diffusionExponent != rhs.diffusionExponent) return 0;
   if( this->transmissionDepth != rhs.transmissionDepth) return 0;

   if( this->eindex != rhs.eindex) return 0;
   if( this->lindex != rhs.lindex) return 0;

   return 1;
}

// There isn't a really good way to do a "smaller than" comparison here
int SceneData::operator<(const SceneData &rhs) const
{
   if( this->numPrimitives < rhs.numPrimitives ) return 1;
   if( this->numIndices < rhs.numIndices ) return 1;
   if( this->numPositions < rhs.numPositions ) return 1;
   if( this->intensity < rhs.intensity ) return 1;
   if( ((this->color.r + this->color.g + this->color.b) * this->color.a) <
	        ((rhs.color.r + rhs.color.g + rhs.color.b) * rhs.color.a)) return 1;
   if( this->boundBox.min().distanceTo(this->boundBox.max()) <
	        rhs.boundBox.min().distanceTo(rhs.boundBox.max())) return 1;
   if( this->coneAngle < rhs.coneAngle) return 1;
   if( this->penumbraAngle < rhs.penumbraAngle) return 1;
   if( this->diffcolor[0] < rhs.diffcolor[0]) return 1;   if( this->diffcolor[1] < rhs.diffcolor[1]) return 1;   if( this->diffcolor[2] < rhs.diffcolor[2]) return 1;
   if( this->transparentColor[0] < rhs.transparentColor[0]) return 1;   if( this->transparentColor[1] < rhs.transparentColor[1]) return 1;   if( this->transparentColor[2] < rhs.transparentColor[2]) return 1;
   if( this->specularColor[0] < rhs.specularColor[0]) return 1;   if( this->specularColor[1] < rhs.specularColor[1]) return 1;   if( this->specularColor[2] < rhs.specularColor[2]) return 1;
   if( this->absorption[0] < rhs.absorption[0]) return 1;   if( this->absorption[1] < rhs.absorption[1]) return 1;   if( this->absorption[2] < rhs.absorption[2]) return 1;
   if( this->roughness < rhs.roughness) return 1;
   if( this->refractIndex < rhs.refractIndex) return 1;
   if( this->diffusionExponent < rhs.diffusionExponent) return 1;
   if( this->transmissionDepth < rhs.transmissionDepth) return 1;

  return 0;
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonSingle::EmbreeViewportRendererXeonSingle( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonSingle")
{
	// Set the ui name
	fUIName.set( "Single-Ray Embree Renderer for Intel(R) Xeon(R) host");
}

void EmbreeViewportRendererXeonSingle::createRenderDevice()
{
	if (m_device == NULL) 
	{
      m_device = embree::Device::rtCreateDevice("default",   // single ray device
	          m_numThreads,m_rtcore_cfg.c_str());
	}
    m_plugintype = 1;
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonPhiSingle::EmbreeViewportRendererXeonPhiSingle( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonPhiSingle")
{
	// Set the ui name
	fUIName.set( "Single-Ray Embree Renderer for Intel(R) Xeon Phi(TM) coprocessor");
}

void EmbreeViewportRendererXeonPhiSingle::createRenderDevice()
{
	if (m_device == NULL) 
	{
      m_device = embree::Device::rtCreateDevice("singleray_knc",   // single-ray over COI
	          m_numThreads,m_rtcore_cfg.c_str());
	}
    m_plugintype = 2;
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonISPC::EmbreeViewportRendererXeonISPC( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonISPC")
{
	// Set the ui name
	fUIName.set( "ISPC Embree Renderer for Intel(R) Xeon(R) host");
}

void EmbreeViewportRendererXeonISPC::createRenderDevice()
{
	if (m_device == NULL) 
	{
      m_device = embree::Device::rtCreateDevice("ispc",   // ispc_device
	          m_numThreads,m_rtcore_cfg.c_str());
	}
    m_plugintype = 4;
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonPhiISPC::EmbreeViewportRendererXeonPhiISPC( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonPhiISPC")
{
	// Set the ui name
	fUIName.set( "ISPC Embree Renderer for Intel(R) Xeon Phi(TM) coprocessor");
}

void EmbreeViewportRendererXeonPhiISPC::createRenderDevice()
{
	if (m_device == NULL) 
	{

      m_device = embree::Device::rtCreateDevice("ispc_knc",   // ispc_device over COI
             	    m_numThreads,m_rtcore_cfg.c_str());
//                  m_numThreads,"builder=morton64");
      // Note - the 64-bit Morton builder is slower than the 32-bit one
      // for dynamic scenes, but in return it supports more complex scenes
      // My understanding is that Xeon automatically switches to a 64-bit
      // Morton builder on need
	}
    m_plugintype = 5;
}

// ------------------------------------------------------------------
EmbreeViewportRendererHybridISPC::EmbreeViewportRendererHybridISPC( )
	: EmbreeViewportRenderer("EmbreeViewportRendererHybridISPC")
{
	// Set the ui name
	fUIName.set( "Hybrid ISPC Embree Renderer");
}

void EmbreeViewportRendererHybridISPC::createRenderDevice()
{
	if (m_device == NULL) 
	{
      m_device = embree::Device::rtCreateDevice("hybrid_ispc",   
	          m_numThreads,m_rtcore_cfg.c_str());
	}
    m_plugintype = 6;
}

// ------------------------------------------------------------------

// Gets called when the plugin is *loaded* in the plugin manager
EmbreeViewportRenderer::EmbreeViewportRenderer( const MString & name )
:	MViewportRenderer( name )
{
	// This renderer overrides some or all drawing
	fRenderingOverride = MViewportRenderer::kOverrideThenUI;
//	fRenderingOverride = MViewportRenderer::kOverrideAllDrawing;

	// We only support the software API
	m_API = MViewportRenderer::kSoftware;

    // Misc
    m_plugintype = 0;
    
	/* rendering device and handles */
	m_device = NULL;
	embree::g_device = NULL;
	m_renderer = NULL;
	m_tonemapper = NULL;
	m_backplate = NULL;
	m_render_scene = NULL;

	/* rendering settings */
	m_scene = "default";
	m_accel = "default";
	m_builder = "default";
	m_traverser = "default";
	m_depth = -1;                       //!< recursion depth - -1 sets default
    
    //!< samples per pixel for ordinary rendering
#if 0
	m_spp = 1;            // default, but starts out too sparse and requires too
                          // many refreshes given how infrequently Maya does 
                          // viewport refreshes
#else
	m_spp = 4;            // This produces better initial results and faster
                          // convergence to a usable image
#endif

	/* output settings */
	m_numBuffers = 2;                   //!< number of buffers in the framebuffer
	m_refine = 1;                       //!< refinement mode - 1 = refine with each render, 0 = reset
	m_gamma = 1.0f;
	m_vignetting = false;
	m_fullscreen = false;
	m_width = 512;
	m_height = 512;
	m_format = "RGBA8";
	m_rtcore_cfg = "";   // used in rtcInit for the device - many possible tweaks
 
        int numThreads = 1;
#if defined(_WIN32)
        SYSTEM_INFO sysinfo;
        GetSystemInfo( &sysinfo );

        numThreads = sysinfo.dwNumberOfProcessors; 
#else
        numThreads = sysconf( _SC_NPROCESSORS_ONLN );
#endif
	m_numThreads = numThreads; 

	// Change initialize camera detection structures
	for (int i = 0; i < VIEWPORTARRAYSIZE; i++)
	{
		m_viewportArray[i].oldwidth=-1;
		m_viewportArray[i].oldheight=0;
		m_viewportArray[i].oldeye = MPoint(0,0,0);
		m_viewportArray[i].oldlookAt = MVector(0,0,0);
		m_viewportArray[i].oldeyeUp = MVector(0,0,0);
		m_viewportArray[i].frameBuffer = NULL;
		m_viewportArray[i].camera = NULL;
		m_viewportArray[i].fullCameraPathName.clear();
		/* camera settings */
		m_viewportArray[i].camPos = embree::Vector3f(0.0f,0.0f,0.0f);
		m_viewportArray[i].camLookAt = embree::Vector3f(1.0f,0.0f,0.0f);
		m_viewportArray[i].camUp     = embree::Vector3f(0,1,0);
		m_viewportArray[i].camFieldOfView = 64.0f;
		m_viewportArray[i].camRadius = 0.0f;
		m_viewportArray[i].accumulate = m_refine;
	}

	m_oldnumObjects = 0;
	m_oldnumLights = 0;

	m_objectDatabase.clear();
	m_lightDatabase.clear();
}

/* virtual */
EmbreeViewportRenderer::~EmbreeViewportRenderer()
{
   // uninitialize() gets called automatically when the plugin is *unloaded*
   // by the plugin manager window, so no need to call it here
}

/* virtual */
// Gets called when the plugin is first used after being loaded in the plugin manager
MStatus
EmbreeViewportRenderer::initialize()
{
    if (NULL == m_device)
        return MStatus::kFailure;  // Device not created successfully
    else
        embree::g_device = m_device;  // WARNING WARNING - yes, the renderer needs this global set

    /* create global objects */
    m_renderer = m_device->rtNewRenderer("pathtracer");

    if (m_depth >= 0) m_device->rtSetInt1(m_renderer, "maxDepth", m_depth);
    m_device->rtSetInt1(m_renderer, "sampler.spp", m_spp);
    
    // Turning this off for now until I am sure it is working properly
//    m_device->rtSetBool1(m_renderer, "filterCaustics", true);
//    m_device->rtSetBool1(m_renderer, "disableCausticReflection", true);
//    m_device->rtSetBool1(m_renderer, "disableCausticTransmission", true);

    m_device->rtCommit(m_renderer);
    
    m_tonemapper = m_device->rtNewToneMapper("default");
    m_device->rtSetFloat1(m_tonemapper, "gamma", m_gamma);
    m_device->rtSetBool1(m_tonemapper, "vignetting", m_vignetting);
    m_device->rtCommit(m_tonemapper);
    
    return MStatus::kSuccess;
}

/* virtual */
// Gets called when the plugin is *unloaded* in the plugin manager
MStatus
EmbreeViewportRenderer::uninitialize()
{
	m_backplate = NULL;
	m_tonemapper = NULL;
	m_renderer = NULL;
	m_prims.clear();
	m_lights.clear();
	m_render_scene = NULL;

	embree::rtClearTextureCache();
	embree::rtClearImageCache();
	// Necessary to make sure that the Embree core shuts down fully
	if (m_device != NULL) {
		delete m_device;  // This will shut down the Embree thread pool
                          // Which can be problematic if two host plug-ins are loaded
		m_device = NULL;
		embree::g_device = NULL;  // WARNING WARNING - anyone still using this is in trouble
                                  // Such as if you have two host plug-ins loaded
	}

	for (int i = 0; i < VIEWPORTARRAYSIZE; i++)
	{
		m_viewportArray[i].oldwidth=-1;
		m_viewportArray[i].oldheight=0;
		m_viewportArray[i].oldeye = MPoint(0,0,0);
		m_viewportArray[i].oldlookAt = MVector(0,0,0);
		m_viewportArray[i].oldeyeUp = MVector(0,0,0);
		m_viewportArray[i].fullCameraPathName.clear();
		/* camera settings */
		m_viewportArray[i].camPos = embree::Vector3f(0.0f,0.0f,0.0f);
		m_viewportArray[i].camLookAt = embree::Vector3f(1.0f,0.0f,0.0f);
		m_viewportArray[i].camUp     = embree::Vector3f(0,1,0);
		m_viewportArray[i].camFieldOfView = 64.0f;
		m_viewportArray[i].camRadius = 0.0f;
		m_viewportArray[i].accumulate = m_refine;
	}

	return MStatus::kSuccess;
}

#define RUNTIMEAVG 5
/* virtual */
MStatus
EmbreeViewportRenderer::render(const MRenderingInfo &renderInfo)
{
    static int first_time = 1;
    static int count=0;
    static double accum = 0.0;
    double t0, dt;

    // Print header for benchmarking information    
    if (1 == first_time)
    {
        first_time = 0;
        if (m_plugintype == 1) {
            printf("\n*** single-ray host calls/sec ***\n");  fflush(0);
        }
        if (m_plugintype == 2) {
            printf("\n*** single-ray coprocessor calls/sec ***\n");  fflush(0);
        }
        if (m_plugintype == 4) {
            printf("\n*** ispc host calls/sec *** \n");  fflush(0);
        }
        if (m_plugintype == 5) {
            printf("\n*** ispc coprocessor calls/sec ***\n");  fflush(0);
        }
        if (m_plugintype == 6) {
            printf("\n*** hybrid ispc calls/sec ***\n");  fflush(0);
        }
    }
    
    t0 = embree::getSeconds();
    
    if (NULL == m_device)
        return MStatus::kFailure;  // Device not created successfully
    else
        embree::g_device = m_device;  // Try to minimize problems from this evil
                                      // global.  Here's hoping Maya doesn't update
                                      // multiple windows at once, but rather in
                                      // sequence...

    // We don't care what renderInfo.renderingAPI() is used
    // Update the frame
	renderInEmbreeSampleRenderer( renderInfo );
    
    dt = embree::getSeconds() - t0;
    accum += dt;
    count++;
    
    // Print "performance" information, including set up we do before
    // calling the Embree-based renderer.  We have disabled the fps/mrps
    // measurements in ISPCDevice::rtRenderFrame and 
    // IntegratorRenderer::RenderJob::finish as a result - these latter
    // functions would report just runtime used by rendering, not
    // setup.
    if (count >= RUNTIMEAVG)
    {
        printf("%g, ", (double)RUNTIMEAVG / accum);  fflush(0);
        count = 0;
        accum = 0.0;
    }
    
	return MStatus::kSuccess;
}

/* virtual */
bool
EmbreeViewportRenderer::nativelySupports( MViewportRenderer::RenderingAPI api,
									   float version )
{
	// Do API check
	return (api == m_API);
}

/* virtual */ bool
EmbreeViewportRenderer::override( MViewportRenderer::RenderingOverride override )
{
	// Check override
	return (override == fRenderingOverride);
}


////////////////////////////
// Utility Functions
////////////////////////////

embree::Handle<embree::Device::RTData> 
	EmbreeViewportRenderer::loadVec2fArray(float *inData, unsigned int numElem)
{
	embree::Vec2f *data = NULL;

	if (inData == NULL) return NULL;

	data = (embree::Vec2f*) embree::alignedMalloc(numElem*sizeof(embree::Vec2f));
	for (unsigned int i=0; i < numElem; i++) 
	{
		data[i] = embree::Vec2f(inData[2*i+0], inData[2*i+1]);
//		cout << data[i] << "\n";
	}

	return m_device->rtNewData("immutable_managed", numElem*sizeof(embree::Vec2f), data);
}

embree::Handle<embree::Device::RTData> 
	EmbreeViewportRenderer::loadVec3fArray(float *inData, unsigned int numElem)
{
	embree::Vec3f *data = NULL;

	if (inData == NULL) return NULL;

	data = (embree::Vec3f*) embree::alignedMalloc(numElem*sizeof(embree::Vec3f));
	for (unsigned int i=0; i < numElem; i++) 
	{
		data[i] = embree::Vec3f(inData[3*i+0], inData[3*i+1], inData[3*i+2]);
//		cout << data[i] << "\n";
	}

	return m_device->rtNewData("immutable_managed", numElem*sizeof(embree::Vec3f), data);
}

embree::Handle<embree::Device::RTData> 
	EmbreeViewportRenderer::loadVec3iArray(int *inData, unsigned int numElem)
{
	embree::Vec3i *data = NULL;

	if (inData == NULL) return NULL;

	data = (embree::Vec3i*) embree::alignedMalloc(numElem*sizeof(embree::Vec3i));
	for (unsigned int i=0; i < numElem; i++)
	{
		data[i] = embree::Vec3i(inData[3*i+0], inData[3*i+1], inData[3*i+2]);
//		cout << data[i] << "\n";
	}

	return m_device->rtNewData("immutable_managed", numElem*sizeof(embree::Vec3i), data);
}

embree::Handle<embree::Device::RTData> 
	EmbreeViewportRenderer::loadVec4fArray(float *inData, unsigned int numElem)
{
	embree::Vec4f *data = NULL;

	if (inData == NULL) return NULL;

	data = (embree::Vec4f*) embree::alignedMalloc(numElem*sizeof(embree::Vec4f));
	for (unsigned int i=0; i < numElem; i++) 
	{
		data[i] = embree::Vec4f(inData[4*i+0], inData[4*i+1], inData[4*i+2], inData[4*i+3]);
	}

	return m_device->rtNewData("immutable_managed", numElem*sizeof(embree::Vec4f), data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene conversion methods
////////////////////////////////////////////////////////////////////////////////////////////////////

void
EmbreeViewportRenderer::convertLight(const MDagPath &dagPath)
{
	//char buffer[1024];

	MStatus status;
	MString pname = dagPath.fullPathName() ;

	// Get the light's transform node
	MMatrix  matrix = dagPath.inclusiveMatrix();

	// Get its attributes
	MFnLight thislight(dagPath, &status);
	MColor color = thislight.color();
	float intensity = thislight.intensity();
	MColor lightIntensity = thislight.lightIntensity();

	// Make intensity match mental ray better
	intensity *= 2.5;

	if ( dagPath.hasFn( MFn::kPointLight ))	
	{
		// Create point light object
		embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("pointlight");
		const embree::Vector3f P((float)matrix.matrix[3][0], 
			(float)matrix.matrix[3][1], 
			(float)matrix.matrix[3][2]);

		// For some reason R & B are mixed up in Embree
		const embree::Color I((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));

		MFnNonAmbientLight nonAmb(dagPath, &status);
		int decayRate = nonAmb.decayRate();
	
		m_device->rtSetFloat3(light, "P", P.x, P.y, P.z);
		m_device->rtSetFloat3(light, "I", I.r, I.g, I.b);
		m_device->rtSetInt1(light, "decayRate", decayRate);
		m_device->rtCommit(light);
		m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL)); // &primXform[0]));
	} 
	else if ( dagPath.hasFn( MFn::kAmbientLight) )
	{
		// Create ambient light object
		embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("ambientlight");
		// For some reason R & B are mixed up in Embree
		const embree::Color L((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));
		m_device->rtSetFloat3(light, "L", L.r, L.g, L.b);
		m_device->rtCommit(light);
		m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL));
	}
	else if ( dagPath.hasFn( MFn::kDirectionalLight) )
	{
		// Create directional light object
		MFloatVector lightDirection = thislight.lightDirection(0, MSpace::kWorld);

        embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("directionallight");
        const embree::Vector3f D((float)lightDirection.x, (float)lightDirection.y, 
			(float)lightDirection.z);
		// For some reason R & B are mixed up in Embree
        const embree::Color E((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));
        m_device->rtSetFloat3(light, "D", D.x, D.y, D.z);
        m_device->rtSetFloat3(light, "E", E.r, E.g, E.b);
        m_device->rtCommit(light);
        m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL)); // &primXform[0]));
	}
	else if ( dagPath.hasFn( MFn::kSpotLight) )
	{
        // Create spot light object
		MFnSpotLight spotlight(dagPath, &status);
		double coneAngle = 57.2957795 * spotlight.coneAngle();
		double penumbraAngle = 57.2957795 * spotlight.penumbraAngle();
		MFloatVector lightDirection = thislight.lightDirection(0, MSpace::kWorld);

		embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("spotlight");
		const embree::Vector3f P((float)matrix.matrix[3][0], 
			(float)matrix.matrix[3][1], 
			(float)matrix.matrix[3][2]);
		const embree::Color I((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));
        const embree::Vector3f D((float)lightDirection.x, (float)lightDirection.y, 
			(float)lightDirection.z);
		MFnNonAmbientLight nonAmb(dagPath, &status);
		int decayRate = nonAmb.decayRate();

		m_device->rtSetFloat3(light, "P", P.x, P.y, P.z);
		m_device->rtSetFloat3(light, "I", I.r, I.g, I.b);
		m_device->rtSetFloat3(light, "D", D.x, D.y, D.z);
		m_device->rtSetInt1(light, "decayRate", decayRate);
		m_device->rtSetFloat1(light, "angleMin", coneAngle);
		m_device->rtSetFloat1(light, "angleMax", coneAngle+penumbraAngle);
		m_device->rtCommit(light);
		m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL));
	}
    else if ( dagPath.hasFn( MFn::kAreaLight) )
    {
        // Create "area light" object (really just a single triangle)
        
        // Further empirical intensity correction
        intensity *= 100.0;
        
        // We don't map this to multiple triangle lights so that we don't get
        // our light index count screwed up
        embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("trianglelight");
        // For some reason R & B are mixed up in Embree
		const embree::Color L((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));
                
        // Area lights start out when created pointing at 0,0,-1
        // So we need to create a light triangle perpendicular to this,
        // rotated and scaled appropriately
        MVector vec1(1.0, 0.0, 0.0);
        MVector vec2(0.0, 1.0, 0.0);
        MVector vec3 = vec1 * matrix;
        MVector vec4 = vec2 * matrix;
        
        const embree::Vector3f v0((float)matrix.matrix[3][0], 
                                  (float)matrix.matrix[3][1], 
                                  (float)matrix.matrix[3][2]);
        const embree::Vector3f v1(v0.x+vec3.x, v0.y+vec3.y, v0.z+vec3.z);
        const embree::Vector3f v2(v0.x+vec4.x, v0.y+vec4.y, v0.z+vec4.z);
        
        m_device->rtSetFloat3(light, "v0", v0.x, v0.y, v0.z);
        m_device->rtSetFloat3(light, "v1", v1.x, v1.y, v1.z);
        m_device->rtSetFloat3(light, "v2", v2.x, v2.y, v2.z);
		m_device->rtSetFloat3(light, "L", L.r, L.g, L.b);
        m_device->rtCommit(light);
        m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL));
    }
}

// ------------------------------------------------------------

// Try to find the material shader associated with a given node
MObject EmbreeViewportRenderer::findObjectMaterialShader( MObject& setNode )
{
    // Interpret the set as a generic dependency node
	MFnDependencyNode graphNode(setNode);
    
    // Try to find a shader plug associated with this node
	MPlug shaderPlug = graphNode.findPlug("surfaceShader");
			
    // If there is one, get the shader from it
	if (!shaderPlug.isNull()) 
    {			
		MPlugArray connectedPlugs;
		bool asSrc = false;
		bool asDst = true;
        // And then the shader(s) connected to the plug
		shaderPlug.connectedTo( connectedPlugs, asDst, asSrc );

        // We don't support more than one connected shader, or none
		if (connectedPlugs.length() != 1)
			MGlobal::displayError("Error getting shader");
		else 
            // Return the attached shader
			return connectedPlugs[0].node();
	}			
	
	return MObject::kNullObj;
}

// ------------------------------------------------------------

void EmbreeViewportRenderer::getMaterialData(const MDagPath &currObject, 
	int haveTexture, SceneData &materialInfo)
{
	std::string diffuseTextureFile;
    std::string specularTextureFile;
    std::string bumpTextureFile;
	float diffColor[3] = {1.0, 1.0, 1.0};  // Needed to make diffuse textures visible
	float transparentColor[3] = {0.0, 0.0, 0.0};
	float specularColor[3] = {1.0, 1.0, 1.0};  // Used to make specular textures visible
    float absorption[3] = {0.0, 0.0, 0.0};
    float roughness = 1.0f;  // pure diffuse (0 = pure specular)
	float refractIndex = 1.5f;
    float diffusionExponent = 100.0f;
    float transmissionDepth = 9999999.9f;
    MStatus status;
    
    diffuseTextureFile.clear();
    specularTextureFile.clear();
    bumpTextureFile.clear();
    
	MFnMesh mesh(currObject);
	MObjectArray sets;
	MObjectArray comps;
	unsigned int instanceNum = currObject.instanceNumber();
    
    // Try to get at shaders associated with this node
    status = mesh.getConnectedSetsAndMembers(instanceNum, sets, comps, true);
    
    // Somehow this mesh object was improperly set up
    if (status == MS::kFailure) 
    {
		MGlobal::displayError("ERROR : MFnMesh::getConnectedSetsAndMembers");
        return;
    }
	
	for ( unsigned i=0; i<sets.length(); i++ ) 
	{
		MObject set = sets[i];  // get one of the set members

		// Scrape material properties from the first material node associated with the
		// surface
		MObject shaderNode = findObjectMaterialShader(set);
		if (shaderNode != MObject::kNullObj)
		{
			// Diffuse color
			MPlug colorPlug = MFnDependencyNode(shaderNode).findPlug("color", &status);
			if (status != MS::kFailure)
			{   
                // Get any texture associated with the diffuse color
				MItDependencyGraph It( colorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream);
				if( !It.isDone() && haveTexture)
				{
					// We have a texture associated with this material
					// Get the filename
					MObject				MayaTexture = It.thisNode();
					MString filename;
					MFnDependencyNode( MayaTexture).findPlug( "fileTextureName").getValue( filename);
					if( filename.length())
						diffuseTextureFile = filename.asChar();
				}
                else
                {
                    // Get the diffuse color only when we do not have textures
                    MObject data;
                    colorPlug.getValue( data);
                    MFnNumericData val(data);
                    val.getData( diffColor[0], diffColor[1], diffColor[2]);
                }
			}  // got color data

			// Transparency
			MPlug transparencyPlug = MFnDependencyNode(shaderNode).findPlug("transparency", &status);
			if (status != MS::kFailure)
			{
				MObject data;
				transparencyPlug.getValue( data);
				MFnNumericData val(data);
				val.getData( transparentColor[0], transparentColor[1], transparentColor[2]);
			}

			// Specular reflection
			MPlug specularColorPlug = MFnDependencyNode(shaderNode).findPlug("specularColor", &status);
			if (status != MS::kFailure)
			{
                // Get any texture associated with the specular color
				MItDependencyGraph It( specularColorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream);
				if( !It.isDone() && haveTexture)
				{
					// We have a texture associated with this material
					// Get the filename
					MObject				MayaTexture = It.thisNode();
					MString filename;
					MFnDependencyNode( MayaTexture).findPlug( "fileTextureName").getValue( filename);
					if( filename.length())
						specularTextureFile = filename.asChar();
				}
                else
                {
                    // Get the specular color only when we do not have textures
                    MObject data;
                    specularColorPlug.getValue( data);
                    MFnNumericData val(data);
                    val.getData( specularColor[0], specularColor[1], specularColor[2]);
                }
			}

			// Index of refraction
            // NOTE:  To do metals via Uber this would need to be something like a color.
            // Since "incandescence" has already been subverted, that leaves
            // "ambience" for this if such support is desired.  Not implemented
            // here at this time.
			MPlug refIndexPlug = MFnDependencyNode(shaderNode).findPlug("refractiveIndex", &status);
			if (status != MS::kFailure)
			{
				refIndexPlug.getValue( refractIndex );
			}
            
            // Bump map (if present)
            MPlug bumpPlug = MFnDependencyNode(shaderNode).findPlug("normalCamera", &status);
			if (status == MS::kSuccess)
            {
				MItDependencyGraph It( bumpPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream);
				if( !It.isDone() && haveTexture)
				{
					// We have a texture associated with this bump
					// Get the filename
					MObject				MayaTexture = It.thisNode();
					MString filename;
					MFnDependencyNode( MayaTexture).findPlug( "fileTextureName").getValue( filename);
                    if( filename.length())
						bumpTextureFile = filename.asChar();
                }
            }

            // Absorption for metals - subverting "incandescence" to get this
            // Good values seem less than 5
            // Copper:  4.24 2.58 2.33 with refraction 0.22 0.95 1.17
            // Gold:  4.10 2.73 1.92 with refraction 0.16 0.35 1.60
            // Silver: 4.59 3.35 2.33 with refraction 0.14 0.12 0.16
            // Currently we have no way to set the refraction components - we just
            // use the traditional index of refraction
			MPlug absorptionPlug = MFnDependencyNode(shaderNode).findPlug("incandescence", &status);
			if (status != MS::kFailure)
			{
				MObject data;
				absorptionPlug.getValue( data);
				MFnNumericData val(data);
				val.getData( absorption[0], absorption[1], absorption[2]);
                // Scale to get into range of "good" values
                absorption[0] *= 5.0f;
                absorption[1] *= 5.0f;
                absorption[2] *= 5.0f;
			}
            
			// Translucence switch (>0 turns on, and does nothing else)
			MPlug translucencePlug = MFnDependencyNode(shaderNode).findPlug("translucence", &status);
			if (status != MS::kFailure)
			{
                float translucence = 0.0f;
				translucencePlug.getValue( translucence );
               
                if (translucence > 0.0f)
                {
                    // Get transmissionDepth
                    MPlug translucenceDepthPlug = MFnDependencyNode(shaderNode).findPlug("translucenceDepth", &status);
                    if (status != MS::kFailure)
                    {
                        translucenceDepthPlug.getValue( transmissionDepth );
                    }
                    
                    // Take over translucenceFocus to use as the diffusionExponent
                    // Unfortunately, since translucenceFocus only goes from 0 to 1,
                    // we need to multiply by 100 to get a better exponent
                    MPlug translucenceFocusPlug = MFnDependencyNode(shaderNode).findPlug("translucenceFocus", &status);
                    if (status != MS::kFailure)
                    {
                        translucenceFocusPlug.getValue( diffusionExponent );
                        diffusionExponent *= 100.0f;
                    }
                }  // Do translucence
			}
            
			// Figure out what sort of shading model is being used since
			// everything reports MFn::kLambert and nothing else in Maya 2014
			int lam, phng, blnn, phnge;
			MFnLambertShader lambert(shaderNode, &status);
			lam = (status == MS::kSuccess);   // Seems to always be true...even for PhongE
			MFnPhongShader phong(shaderNode, &status);
			phng = (status == MS::kSuccess); 
			MFnPhongEShader phonge(shaderNode, &status);
			phnge = (status == MS::kSuccess);
			MFnBlinnShader blinn(shaderNode, &status);
			blnn = (status == MS::kSuccess);

			if (phng) {
				// "Plastic" 
				// Maya:  The valid range is 2 to infinity. The slider range is 2 
				// (broad highlight, not very shiny surface) to 100 (small highlight, 
				// very shiny surface), though you can type in a higher value.
				float cosPower = 0.01f;
				MPlug cosinePowerPlug = MFnDependencyNode(shaderNode).findPlug("cosinePower", &status);
				if (status != MS::kFailure)
				{
					MObject data;
					cosinePowerPlug.getValue( cosPower );
                    // In "Uber" roughness=0 for perfectly specular, 1.0 for fully diffuse
                    // So we map the cosPower range of 100 to 0 to 0 to 1
					roughness = 100.0f-cosPower/100.0f;
                    if (roughness < 0.0)
                        roughness = 0.0;
				}	
			} else if (blnn) {
				// "Metal" 
				// Maya: The valid range is 0 (no highlight) to 0.999 (broad highlight,
                // not very shiny surface). A value of 0.1 produces a small highlight
                // (very shiny surface)
				float eccentricity = 0.0f;
				MPlug eccentricityPlug = MFnDependencyNode(shaderNode).findPlug("eccentricity", &status);
				if (status != MS::kFailure)
				{
					MObject data;
					eccentricityPlug.getValue( eccentricity );
                    // In "Uber" roughness=0 for perfectly specular, 1.0 for fully diffuse
                    // So this is a good match
					roughness = eccentricity;
				}
			} else if (phnge) {
				// We either have a PhongE or Anisotropic shader - assume the former
				// More "plastic" 
				// Also has "highlightSize" and "whiteness" attributes
				MPlug roughnessPlug = MFnDependencyNode(shaderNode).findPlug("roughness", &status);
				if (status != MS::kFailure)
				{
					MObject data;
                    // In "Uber" roughness=0 for perfectly specular, 1.0 for fully diffuse
                    // Which is roughly how Maya sees it
					roughnessPlug.getValue( roughness );
				}	
			}
    
			materialInfo.haveTexture = haveTexture;  // Do we have UVs?
			materialInfo.diffuseTextureFile = diffuseTextureFile;
			materialInfo.specularTextureFile = specularTextureFile;
			materialInfo.bumpTextureFile = bumpTextureFile;
            
			materialInfo.roughness = roughness;
			materialInfo.refractIndex = refractIndex;
            materialInfo.diffusionExponent = diffusionExponent;
            materialInfo.transmissionDepth = transmissionDepth;
            
			materialInfo.diffcolor[0] = diffColor[0]; materialInfo.diffcolor[1] = diffColor[1]; materialInfo.diffcolor[2] = diffColor[2];
			materialInfo.transparentColor[0] = transparentColor[0]; materialInfo.transparentColor[1] = transparentColor[1]; materialInfo.transparentColor[2] = transparentColor[2];
			materialInfo.specularColor[0] = specularColor[0]; materialInfo.specularColor[1] = specularColor[1]; materialInfo.specularColor[2] = specularColor[2];
            materialInfo.absorption[0] = absorption[0]; materialInfo.absorption[1] = absorption[1]; materialInfo.absorption[2] = absorption[2]; 

			break;  // For now, one shader per object
		} // non-null shader node
	}  // for all connected sets and members
}

// ------------------------------------------------------------

int EmbreeViewportRenderer::checkMaterialProperties(const MDagPath &currObject, 
	int haveTexture, SceneData &oldSceneInfo)
{
	int replacementNeeded = 0;
	SceneData currMaterialData;

	// Get information on the light from Maya
	getMaterialData(currObject, haveTexture, currMaterialData);

	// Look for changes
	if (oldSceneInfo.haveTexture != currMaterialData.haveTexture) replacementNeeded = 1;
	if (oldSceneInfo.diffuseTextureFile != currMaterialData.diffuseTextureFile) replacementNeeded = 1;
	if (oldSceneInfo.specularTextureFile != currMaterialData.specularTextureFile) replacementNeeded = 1;
	if (oldSceneInfo.bumpTextureFile != currMaterialData.bumpTextureFile) replacementNeeded = 1;

	if (oldSceneInfo.diffcolor[0] != currMaterialData.diffcolor[0]) replacementNeeded = 1;
	if (oldSceneInfo.diffcolor[1] != currMaterialData.diffcolor[1]) replacementNeeded = 1;
	if (oldSceneInfo.diffcolor[2] != currMaterialData.diffcolor[2]) replacementNeeded = 1;
	if (oldSceneInfo.transparentColor[0] != currMaterialData.transparentColor[0]) replacementNeeded = 1;
	if (oldSceneInfo.transparentColor[1] != currMaterialData.transparentColor[1]) replacementNeeded = 1;
	if (oldSceneInfo.transparentColor[2] != currMaterialData.transparentColor[2]) replacementNeeded = 1;
	if (oldSceneInfo.specularColor[0] != currMaterialData.specularColor[0]) replacementNeeded = 1;
	if (oldSceneInfo.specularColor[1] != currMaterialData.specularColor[1]) replacementNeeded = 1;
	if (oldSceneInfo.specularColor[2] != currMaterialData.specularColor[2]) replacementNeeded = 1;
	if (oldSceneInfo.absorption[0] != currMaterialData.absorption[0]) replacementNeeded = 1;
	if (oldSceneInfo.absorption[1] != currMaterialData.absorption[1]) replacementNeeded = 1;
	if (oldSceneInfo.absorption[2] != currMaterialData.absorption[2]) replacementNeeded = 1;
    
	if (oldSceneInfo.roughness != currMaterialData.roughness) replacementNeeded = 1;
	if (oldSceneInfo.refractIndex != currMaterialData.refractIndex) replacementNeeded = 1;
	if (oldSceneInfo.diffusionExponent != currMaterialData.diffusionExponent) replacementNeeded = 1;
	if (oldSceneInfo.transmissionDepth != currMaterialData.transmissionDepth) replacementNeeded = 1;

	return replacementNeeded;
}

// ------------------------------------------------------------

void EmbreeViewportRenderer::convertObjectMaterial(const MDagPath &dagPath, 
	int haveTexture, embree::Handle<embree::Device::RTMaterial> &material)
{
	SceneData materialInfo;
    static embree::Handle<embree::Device::RTTexture> texture;
    static embree::Handle<embree::Device::RTData> data;
    texture = NULL;
    data = NULL;
	getMaterialData(dagPath, haveTexture, materialInfo);

    // Create an "Uber" shader with the current values we have pulled from Maya
    // We reverse B and R since, well, that is what looks right
	material = m_device->rtNewMaterial("Uber");
	m_device->rtSetFloat3(material, "diffuse", 
				materialInfo.diffcolor[2], materialInfo.diffcolor[1], materialInfo.diffcolor[0]);
	m_device->rtSetFloat3(material, "transmission", 
				materialInfo.transparentColor[2], materialInfo.transparentColor[1], materialInfo.transparentColor[0]);
	m_device->rtSetFloat3(material, "specular", 
				materialInfo.specularColor[2], materialInfo.specularColor[1], materialInfo.specularColor[0]);
	m_device->rtSetFloat3(material, "absorption", 
				materialInfo.absorption[2], materialInfo.absorption[1], materialInfo.absorption[0]);
    m_device->rtSetFloat1(material, "roughness", materialInfo.roughness);
	m_device->rtSetFloat3(material, "refraction", materialInfo.refractIndex, 
                                    materialInfo.refractIndex, materialInfo.refractIndex);
	m_device->rtSetFloat1(material, "diffusionExponent", materialInfo.diffusionExponent);
	m_device->rtSetFloat1(material, "transmissionDepth", materialInfo.transmissionDepth);
    // For the present these are hard-coded since I don't have a good
    // idea for how to get them out of Maya.  One of the mental ray 
    // textures?  Text file we read?  Environment variables?
#if 1
    // wax
    // works best with transmissionDepth = 25.0, diffusionExponent = 1.0,
    // transmission = 0.9f, 0.9f, 0.9f
    static float dcoeff[] = {1.0f, 1.0f, 1.0f, 1.0f};
    data = loadVec4fArray(&dcoeff[0], 1);
    m_device->rtSetArray(material, "diffusionCoefficients", "float4", data, 1, sizeof(embree::Vec4f), 0);

#else   
    // Carnival glass
    // works best with transmissionDepth = 65.0, diffusionExponent = 60.0
    static float dcoeff[] = {0.133f, 0.155f, 0.149f, 0.0064f,
            0.100f, 0.236f, 0.244f, 0.0484f,
            0.118f, 0.198f, 0.000f, 0.1870f,
            0.113f, 0.007f, 0.007f, 0.5670f,
            0.358f, 0.304f, 0.200f, 1.9900f,
            0.078f, 0.070f, 0.040f, 7.4100f};
    embree::Handle<embree::Device::RTData> data = loadVec4fArray(&dcoeff[0], 6);
    m_device->rtSetArray(material, "diffusionCoefficients", "float4", data, 6, sizeof(embree::Vec4f), 0);

#endif

    // Load and assign the texture files if we have texture coordinates *and* the file
	// Only ppm, pfm, and jpeg supported unless you build with USE_IMAGEMAGICK
	if ((1 == haveTexture) && (!materialInfo.diffuseTextureFile.empty())) 
    {
        texture = embree::rtLoadTexture(materialInfo.diffuseTextureFile);
		m_device->rtSetTexture(material, "diffuseMap", texture);
	}
	if ((1 == haveTexture) && (!materialInfo.specularTextureFile.empty())) 
    {
        texture = embree::rtLoadTexture(materialInfo.specularTextureFile);
		m_device->rtSetTexture(material, "specularMap", texture);
	}
	if ((1 == haveTexture) && (!materialInfo.bumpTextureFile.empty())) 
    {
        texture = embree::rtLoadTexture(materialInfo.bumpTextureFile);
		m_device->rtSetTexture(material, "bumpMap", texture);
	}
    
    // Commit the material to the device
    m_device->rtCommit(material);
}

// ------------------------------------------------------------

void EmbreeViewportRenderer::convertObject( const MDagPath &dagPath)
{
	char buffer[1024];

	if ( dagPath.hasFn( MFn::kMesh ))
	{
		MObject object = dagPath.node();
		MFnMesh mesh(object);

		// Does this surface have texture coordinates?
		bool haveTexture = false;
		int	numUVsets = mesh.numUVSets();
		MString uvSetName;
		MObjectArray textureInstances;
		if (numUVsets > 0)
		{
			mesh.getCurrentUVSetName( uvSetName );
			MStatus status = mesh.getAssociatedUVSetTextures(uvSetName, textureInstances);
			if (status == MS::kSuccess && textureInstances.length())
			{
				haveTexture = true;
			}
		}

		bool useNormals = true;

		// Add the requirements to what we will query from the mesh.
		MGeometryRequirements requirements;
		requirements.addPosition();
		if (useNormals)
			requirements.addNormal();
		if (haveTexture)
			requirements.addTexCoord( uvSetName );

        // We are ignoring colors, tangents and binormals for now

        // Now get the object information we requested
		MGeometry geom = MGeometryManager::getGeometry( dagPath, requirements, NULL );

		unsigned int numPrims = geom.primitiveArrayCount();

		MString ccname = dagPath.fullPathName() ;

        // If there are any triangles in the mesh, extract and use what we need
		if (numPrims)
		{
			// Load the first element in the primitive array
			const MGeometryPrimitive prim = geom.primitiveArray(0);
			unsigned int numElem = prim.elementCount();

			if (numElem)
			{
				// Is the data in floats, ints, etc.?
				MGeometryData::ElementType elemType = prim.dataType();

				int *idx = (int *) prim.data();

				if (elemType != MGeometryData::kInt32) 
				{
					sprintf(buffer, "%s: Error!  Data type %d (kInt32=%d, kUnsignedInt32 =%d):  0=%d, 1=%d, 2=%d", 
							ccname.asChar(), elemType, 
							MGeometryData::kInt32,
							MGeometryData::kUnsignedInt32,
							idx[0], idx[1], idx[2]);
					MGlobal::displayInfo(buffer);
				}

				// Get the position data
				const MGeometryData pos = geom.position();
				float * posPtr = (float * )pos.data();

				bool haveData = idx && posPtr;

				MGeometryPrimitive::DrawPrimitiveType primType = prim.drawPrimitiveType();

                // We only are accepting triangle data
				if (primType != MGeometryPrimitive::kTriangles)
				{
					MString pname = dagPath.fullPathName() ;
					sprintf(buffer, "%s: Error!  Primitive type: %d (Invalid=%d, Triangles=%d)", 
							pname.asChar(), primType,
							MGeometryPrimitive::kInvalidIndexType,
							MGeometryPrimitive::kTriangles);
					MGlobal::displayInfo(buffer);
					haveData = false;
				}

				// Get the normals data
				float * normPtr = NULL;
				int numNorm = 0;
				if (useNormals)
				{
					const MGeometryData norm = geom.normal();
					normPtr = (float * )norm.data();
					numNorm = norm.elementCount();
				}

				// Get the texture coordinate data
				float *uvPtr = NULL;
				if (haveTexture)
				{
					const MGeometryData uvs = geom.texCoord( uvSetName );
					uvPtr = (float *)uvs.data();
				}
                
                // For now, we will ignore colors, binormals and tangents
                
                // If we have geometry, import it into the Embree sample renderer
				if (haveData)
				{
                    embree::Handle<embree::Device::RTShape> mesh = NULL;
                    embree::Handle<embree::Device::RTData> positions = NULL;
                    embree::Handle<embree::Device::RTData> triangles = NULL;
                    embree::Handle<embree::Device::RTData> normals = NULL;
                    embree::Handle<embree::Device::RTData> uvcoords = NULL;

					// Get the mesh's transform node
					MMatrix  matrix = dagPath.inclusiveMatrix();

					// Rotate everything on X, Y, and Z axes by 180, and get the
                    // transform from the rightmost column
					float primXform[12] = {
						(float)matrix.matrix[0][0], -(float)matrix.matrix[1][0], -(float)matrix.matrix[2][0],
						-(float)matrix.matrix[0][1], (float)matrix.matrix[1][1], -(float)matrix.matrix[2][1],
						-(float)matrix.matrix[0][2], -(float)matrix.matrix[1][2], (float)matrix.matrix[2][2],
			  			(float)matrix.matrix[3][0], (float)matrix.matrix[3][1], (float)matrix.matrix[3][2]
					};

					//  ------ Now load mesh data
					// Create a Embree material for the mesh
					embree::Handle<embree::Device::RTMaterial> material;
					convertObjectMaterial(dagPath, haveTexture, material);

					// Create a mesh object
					mesh = m_device->rtNewShape("trianglemesh");

					// Transfer the position data
					unsigned int numPos = pos.elementCount();
					positions = loadVec3fArray(posPtr, numPos);
					m_device->rtSetArray(mesh, "positions", "float3", positions, numPos, sizeof(embree::Vec3f), 0);

					// Triangle indices
					triangles = loadVec3iArray(idx, numElem/3);
					m_device->rtSetArray(mesh, "indices", "int3", triangles, numElem/3, sizeof(embree::Vec3i), 0);

					// Normals
					if (useNormals)
					{
						normals = loadVec3fArray(normPtr, numNorm);
						m_device->rtSetArray(mesh, "normals", "float3", normals, numNorm, sizeof(embree::Vec3f), 0);
					}

					// UV coordinates
					if (haveTexture)
					{
						uvcoords = loadVec2fArray(uvPtr, numPos);
						m_device->rtSetArray(mesh, "texcoords", "float2", uvcoords, numPos, sizeof(embree::Vec2f), 0);
					}

					// Color data - TDB

					// Finish up the mesh
					m_device->rtSetString(mesh,"accel", "default");
					m_device->rtSetString(mesh,"builder", "default");
					m_device->rtSetString(mesh,"traverser", "default");
					m_device->rtCommit(mesh);
					m_device->rtClear(mesh);

					// And store the Embree shape primitive object
					m_prims.push_back(m_device->rtNewShapePrimitive(mesh, material, &primXform[0]));
				}   // Have object data
			}  // There are triangles in this primitive
		}  // There are primitives
	} // If this is a mesh node
}

// ------------------------------

// Gather book-keeping data for an object
void EmbreeViewportRenderer::getObjectData(MDagPath &currObject, SceneData &objectInfo)
{
	MMatrix  matrix = currObject.inclusiveMatrix();
	MGeometryRequirements requirements;
	requirements.addPosition();
	MGeometry geom = MGeometryManager::getGeometry( currObject, requirements, NULL );
	MStatus status;
	MFnDagNode dagNode(currObject, &status);
	MBoundingBox box = dagNode.boundingBox();
	unsigned int numPrims = geom.primitiveArrayCount();
	unsigned int numIndices = 0;
	unsigned int numPos = 0;
	MBoundingBox bbox(box.min(), box.max());
	bool haveTexture = false;

	if (numPrims)
	{
		const MGeometryPrimitive prim = geom.primitiveArray(0);
		numIndices = prim.elementCount();
		const MGeometryData pos = geom.position();
		numPos = pos.elementCount();
	}

	// Do we have texture coordinates?
	MObject object = currObject.node();
	MFnMesh mesh(object);
		
	int	numUVsets = mesh.numUVSets();
	MString uvSetName;
	MObjectArray textures;
	if (numUVsets > 0)
	{
		mesh.getCurrentUVSetName( uvSetName );
		MStatus status = mesh.getAssociatedUVSetTextures(uvSetName, textures);
		if (status == MS::kSuccess && textures.length())
		{
			haveTexture = true;
		}
	}

	objectInfo.numPrimitives = numPrims;
	objectInfo.numIndices = numIndices;
	objectInfo.numPositions = numPos;
	objectInfo.transform = matrix;
	objectInfo.boundBox = bbox;
	objectInfo.haveTexture = haveTexture;
}

// ---------------------------------

// Gather book-keeping data for a light 
void EmbreeViewportRenderer::getLightData(MDagPath &currLight, SceneData &lightInfo)
{
	// Get information on the light from Maya
	MMatrix  matrix = currLight.inclusiveMatrix();
	MStatus status;
	MFnLight thislight(currLight, &status);
	MColor color = thislight.color();
	float intensity = thislight.intensity();
	MFloatVector lightDirection = MFloatVector::zero;
	float coneAngle = 0;
	float penumbraAngle = 0;
	int decayRate = 0;

	if ( currLight.hasFn( MFn::kDirectionalLight) )
	{
		// Special case for directional light
		lightDirection = thislight.lightDirection(0, MSpace::kWorld);
	}
	else if ( currLight.hasFn( MFn::kSpotLight) )
	{
		MFnSpotLight spotlight(currLight, &status);
		lightDirection = thislight.lightDirection(0, MSpace::kWorld);
		coneAngle = 57.2957795 * spotlight.coneAngle();
		penumbraAngle = 57.2957795 * spotlight.penumbraAngle();
	}
    else if ( currLight.hasFn( MFn::kAreaLight) )
    {
        lightDirection = thislight.lightDirection(0, MSpace::kWorld);
    }

	if (currLight.hasFn( MFn::kNonAmbientLight) )
	{
		MFnNonAmbientLight nonAmb(currLight, &status);
		decayRate = nonAmb.decayRate();
	}

	lightInfo.transform = matrix;
	lightInfo.color = color;
	lightInfo.intensity = intensity;
	lightInfo.lightDirection = lightDirection;
	lightInfo.coneAngle = coneAngle;
	lightInfo.penumbraAngle = penumbraAngle;
	lightInfo.decayRate = decayRate;
}

// ------------------------------
int EmbreeViewportRenderer::checkObjectProperities(MDagPath &currObject, SceneData &oldSceneInfo, int i,
												   int *materialUpdated) 
{
	int replacementNeeded = 0;
	SceneData currObjectData;

	// Get information on the object from Maya
	getObjectData(currObject, currObjectData);
    
	// Look for changes in the object itself
	if (oldSceneInfo.numPrimitives != currObjectData.numPrimitives) replacementNeeded = 1;
	if (oldSceneInfo.numIndices != currObjectData.numIndices) replacementNeeded = 1;
	if (oldSceneInfo.numPositions != currObjectData.numPositions) replacementNeeded = 1;
	if (oldSceneInfo.transform != currObjectData.transform) replacementNeeded = 1;
	if (oldSceneInfo.boundBox.center() != currObjectData.boundBox.center()) replacementNeeded = 1;
	if (oldSceneInfo.boundBox.min() != currObjectData.boundBox.min()) replacementNeeded = 1;
	if (oldSceneInfo.boundBox.max() != currObjectData.boundBox.max()) replacementNeeded = 1;
    if (oldSceneInfo.haveTexture != currObjectData.haveTexture) replacementNeeded = 1;
	if (oldSceneInfo.lindex != i) replacementNeeded = 1;

	// If materials change, do not force an object rebuild - just replace material
	if (0 == replacementNeeded) 
	{
		int replaceMaterial;

		replaceMaterial = checkMaterialProperties(currObject, currObjectData.haveTexture, oldSceneInfo);

		if (replaceMaterial && (NULL != m_render_scene))
		{
			// Grab the changed material and import into the Embree sample renderer
			embree::Handle<embree::Device::RTMaterial> material;
			convertObjectMaterial(currObject, currObjectData.haveTexture, material);
			m_device->rtCommit(material);

			// Update the recorded material data for change detection next time
			getMaterialData(currObject, currObjectData.haveTexture, oldSceneInfo);

			// Now replace the material in the scene
			m_device->rtUpdateObjectMaterial(m_render_scene, material, oldSceneInfo.eindex);

			// And update the information in the object database
			std::string currObjectName = currObject.fullPathName().asChar();
			std::pair<std::map<std::string, SceneData>::iterator, bool> retstat;
			m_objectDatabase.erase(currObjectName);  // Remove old information
			retstat = m_objectDatabase.insert(std::pair<std::string, SceneData>(currObjectName, oldSceneInfo)); // add new

			*materialUpdated = 1;
		}
	}

	return replacementNeeded;
}

// ------------------------------
void EmbreeViewportRenderer::recordObjectProperties(MDagPath &currObject, int i, int eindex)
{
	std::string currObjectName = currObject.fullPathName().asChar();
	SceneData currObjectData;

	// Get object data from Maya
	getObjectData(currObject, currObjectData);

	// Get material data
	MObject object = currObject.node();
	MFnMesh mesh(object);
	bool haveTexture = false;
	int	numUVsets = mesh.numUVSets();
	MString uvSetName;
	MObjectArray textureInstances;
	if (numUVsets > 0)
	{
		mesh.getCurrentUVSetName( uvSetName );
		MStatus status = mesh.getAssociatedUVSetTextures(uvSetName, textureInstances);
		if (status == MS::kSuccess && textureInstances.length())
		{
			haveTexture = true;
		}
	}
	getMaterialData(currObject, haveTexture, currObjectData);

	// Record the object's properties for change detection
	currObjectData.lindex = i;		
	currObjectData.eindex = eindex;

	// Add this record to the object database
	std::pair<std::map<std::string, SceneData>::iterator, bool> retstat;
	retstat = m_objectDatabase.insert(std::pair<std::string, SceneData>(currObjectName, currObjectData));
}

// ---------------------------------
int EmbreeViewportRenderer::checkLightProperities(MDagPath &currLight, SceneData &oldSceneInfo, int i) 
{
	int replacementNeeded = 0;
	SceneData currLightData;

	// Get information on the light from Maya
	getLightData(currLight, currLightData);

	// Look for changes
	if (oldSceneInfo.transform != currLightData.transform) replacementNeeded = 1;
	if (oldSceneInfo.color != currLightData.color) replacementNeeded = 1;
	if (oldSceneInfo.intensity != currLightData.intensity) replacementNeeded = 1;
	if (oldSceneInfo.lightDirection != currLightData.lightDirection) replacementNeeded = 1;
	if (oldSceneInfo.coneAngle != currLightData.coneAngle) replacementNeeded = 1;
	if (oldSceneInfo.penumbraAngle != currLightData.penumbraAngle) replacementNeeded = 1;
	if (oldSceneInfo.decayRate != currLightData.decayRate) replacementNeeded = 1;
	if (oldSceneInfo.lindex != i) replacementNeeded = 1;

	return replacementNeeded;
}

// ---------------------------------

void EmbreeViewportRenderer::recordLightProperties(MDagPath &currLight, int i, int eindex)
{
	SceneData currLightData;
	std::string currLightName = currLight.fullPathName().asChar();

	// Get information on the light from Maya
	getLightData(currLight, currLightData);

	// Record the object's properties for change detection
	currLightData.lindex = i;		
	currLightData.eindex = eindex;

	// Add this record to the object database
	std::pair<std::map<std::string, SceneData>::iterator, bool> retstat;
	retstat = m_lightDatabase.insert(std::pair<std::string, SceneData>(currLightName, currLightData));
}

// ------------------------------

int	 EmbreeViewportRenderer::convertMayaObjectsToEmbree(std::vector<MDagPath> &currentObjects, 
								int objectReplacementNeeded,
								int *materialUpdated)
{
	int i;
	int numObjects = currentObjects.size();
	int replacementNeeded = objectReplacementNeeded;
	std::string currObjectName;

	// If "replacement needed" is already true, just rebuild 
	// everything rather than trying to find if there were changes
	if (0 == objectReplacementNeeded)
	{
		// Check to see if any of the objects changed
        // Yes, this implementation is relatively brute-force, but I have not found
        // any other way to tell if object or material has changed without
        // revisiting them all and looking for high-level changes
		for (i = 0; i < numObjects; i++)
		{
			std::map<std::string, SceneData>::iterator oldInfoIter;
			SceneData oldSceneInfo;

			// Get an object and its name
			MDagPath currObject = currentObjects[i];
			currObjectName = currObject.fullPathName().asChar();

			// Load the information we have on that object
			oldInfoIter = m_objectDatabase.find(currObjectName);
			if (oldInfoIter == m_objectDatabase.end())
			{
				// Never seen this object before
				replacementNeeded = 1;
				break;   // No point in checking further
			}
			else
			{
				// Seen this object before - let's make sure it hasn't changed
				oldSceneInfo = oldInfoIter->second;

				int needReplacement = checkObjectProperities(currObject, oldSceneInfo, i, 
														   materialUpdated);

				if (needReplacement == 1) 
                {
                    replacementNeeded = 1;
					break;   // There was a change - no point in checking further
                }
			} // check object
		}  // all objects
	}  // Check if objects changed

	// If nothing changed, we are done
	if (0 == replacementNeeded)
		return 0;

	// Something changed - rebuild the object databases
    // Ideally, we just replace the changed object
	m_objectDatabase.clear();
	m_prims.clear();

	for (i = 0; i < numObjects; i++)
	{
		// Get the next object
		MDagPath currObject = currentObjects[i];

		// Build the Embree version of the object
		convertObject(currObject);

		// Record the object's properties for change detection
		// Objects are loaded before lights, so that an object's list 
		// and embree indexes are the same
		recordObjectProperties(currObject, i, i);
	}  // retranslate all objects

	return 1;
}

// ---------------------------------

int	EmbreeViewportRenderer::convertMayaLightsToEmbree(std::vector<MDagPath> &currentLights, 
			int lightReplacementNeeded)
{
	int i;
	int numLights = currentLights.size();
	int replacementNeeded = lightReplacementNeeded;
	std::string currLightName;

	// If "replacement needed" is already true, flush the database and do 
    // a full rebuild
	if (0 == lightReplacementNeeded) 
	{
		// Now look through all the lights and see if any changed
		for (i = 0; i < numLights; i++)
		{
			std::map<std::string, SceneData>::iterator oldInfoIter;
			SceneData oldSceneInfo;

			// Get a light and its name
			MDagPath currLight = currentLights[i];
			currLightName = currLight.fullPathName().asChar();

			// Load the information we have on that light in our database
			oldInfoIter = m_lightDatabase.find(currLightName);
			if (oldInfoIter == m_lightDatabase.end())
			{
                // Never seen this light before
                replacementNeeded = 1;
                break;  // No point checking further
			}
			else
			{
                // Look for changes in this light
                oldSceneInfo = oldInfoIter->second;

                int needReplacement = checkLightProperities(currLight, oldSceneInfo, i);

                if (needReplacement == 1)
                {
                    replacementNeeded = 1;
                    break;  // There was a change - no point in checking further
                }
			} // seen the light before
		} // for all lights
	} // Check for light changes

	// If nothing changed, we are done
	if (0 == replacementNeeded)
		return 0;

	// Something changed - rebuild the light databases
    // Again, ideally we just swap out the changed light
	m_lightDatabase.clear();
	m_lights.clear();

	for (i = 0; i < numLights; i++)
	{
		// Get a light and its name
		MDagPath currLight = currentLights[i];

		// Build the Embree version of the object
		convertLight(currLight);

		// Record the object's properties for change detection
		// Lights are loaded after objects, so their embree index is
		// picks up after the objects
		recordLightProperties(currLight, i, m_prims.size()+i);
	}  // retranslate all lights

	return 1;
}

// ------------------------------
// ------
// Scan the entire DAG for leaves that are objects or lights
// This is necessary since off-camera objects or lights can
// effect the lighting in the view of the camera)
// -------
bool EmbreeViewportRenderer::collectSceneObjects (const MRenderingInfo &renderInfo, 
		std::vector<MDagPath> &currentObjects, std::vector<MDagPath> &currentLights)
{
	MStatus status;
	MItDag dagIterator( MItDag::kDepthFirst, MFn::kInvalid, &status);
	if (status != MS::kSuccess)
		return false;

	for ( ; !dagIterator.isDone() ; dagIterator.next() ) 
	{
		MDagPath dagPath;
		status = dagIterator.getPath(dagPath);
		if (status != MS::kSuccess) break;

		if (0 != dagPath.childCount()) continue;  // Only leaf nodes

		if (dagPath.isValid())
		{
			//
			// Draw surfaces (currently, only meshes)
			//
			if ( dagPath.hasFn( MFn::kMesh)
//						|| path.hasFn( MFn::kNurbsSurface) ||
//						path.hasFn( MFn::kSubdiv) )
					)
			{
				currentObjects.push_back(dagPath);
			}  // Draw surfaces
			//
			// Lights
			//
			else if (dagPath.hasFn( MFn::kLight) ||
				dagPath.hasFn( MFn::kAmbientLight) ||
				dagPath.hasFn( MFn::kNonAmbientLight) ||
				dagPath.hasFn( MFn::kAreaLight) ||
				dagPath.hasFn( MFn::kLinearLight ) ||
				dagPath.hasFn( MFn::kNonExtendedLight ) ||
				dagPath.hasFn( MFn::kDirectionalLight ) ||
				dagPath.hasFn( MFn::kPointLight ) ||
				dagPath.hasFn( MFn::kSpotLight ) )
			{
				currentLights.push_back(dagPath);
			}
		}  // path is valid
	}  // Iterate entire scene

	return true;
}



embree::Handle<embree::Device::RTScene> EmbreeViewportRenderer::createSceneFull()
{
    embree::Handle<embree::Device::RTScene> scene = m_device->rtNewScene(m_scene.c_str());
    m_device->rtSetString(scene,"accel",m_accel.c_str());
    m_device->rtSetString(scene,"builder",m_builder.c_str());
    m_device->rtSetString(scene,"traverser",m_traverser.c_str());

	int num_objs = m_prims.size();

	// Load the objects we have collected into the scene
    for (size_t i=0; i<num_objs; i++) 
		m_device->rtSetPrimitive(scene,i,m_prims[i]);

	// Load the lights we have collected into the scene
	for (size_t i=0; i<m_lights.size(); i++) 
		m_device->rtSetPrimitive(scene,i+num_objs,m_lights[i]);

	m_device->rtCommit(scene);
    return scene;
}

void EmbreeViewportRenderer::updateSceneLightsOnly(embree::Handle<embree::Device::RTScene> scene)
{
	int num_objs = m_prims.size();

	// Set up the lights again
	for (size_t i=0; i<m_lights.size(); i++) 
		m_device->rtSetPrimitive(scene,i+num_objs,m_lights[i]);

	m_device->rtCommit(scene);  // This will possibly result in a memory leak in singleray_device
}

// Ideally we would also have an updateSceneOneObject method that just replaces
// the one object that changed (translated, mesh change).  To get there we need 
// new logic in the single-ray renderer and new logic above to deal with 
// changes of a single mesh

// ------------------------------- 

void EmbreeViewportRenderer::createCamera(const embree::AffineSpace3f& space,
	  const double aspectRatio, viewPortInfo *vpInfo)
{
    /*! pinhole camera */
    if (vpInfo->camRadius == 0.0f)
    {
      embree::Handle<embree::Device::RTCamera> camera = m_device->rtNewCamera("pinhole");
      m_device->rtSetTransform(camera, "local2world", embree::copyToArray(space));
      m_device->rtSetFloat1(camera, "angle", vpInfo->camFieldOfView);
      m_device->rtSetFloat1(camera, "aspectRatio", aspectRatio);
      m_device->rtCommit(camera);
	  vpInfo->camera = camera;
    }
    /*! depth of field camera */
    else
    {
      embree::Handle<embree::Device::RTCamera> camera = m_device->rtNewCamera("depthoffield");
      m_device->rtSetTransform(camera, "local2world", copyToArray(space));
      m_device->rtSetFloat1(camera, "angle", vpInfo->camFieldOfView);
      m_device->rtSetFloat1(camera, "aspectRatio", aspectRatio);
      m_device->rtSetFloat1(camera, "lensRadius", vpInfo->camRadius);
      m_device->rtSetFloat1(camera, "focalDistance", embree::length(vpInfo->camLookAt - vpInfo->camPos));
      m_device->rtCommit(camera);
      vpInfo->camera = camera;
    }
}

// ----------------------------------

MStatus EmbreeViewportRenderer::updateCamera(const MRenderingInfo &renderInfo, 
	viewPortInfo *vpInfo, int newCamera)
{
	MStatus status;
	const MDagPath &cameraPath = renderInfo.cameraPath();
	int needToRebuildCamera = 0;

	// Get the camera
	MFnCamera camera(cameraPath, &status);
	if (MS::kSuccess != status) {
            return status;
    }

	// Get camera parameters
	MPoint eyePoint = camera.eyePoint(MSpace::kWorld);
	double aspectRatio = camera.aspectRatio();
	MPoint lookAt = camera.centerOfInterestPoint(MSpace::kWorld);
	MVector viewUp = camera.upDirection(MSpace::kWorld);

	if (1 == newCamera)
		needToRebuildCamera = 1;

	// Did it change?  If so, use out checking to populate a new camera
	if (eyePoint != vpInfo->oldeye) {
		vpInfo->accumulate = 0;
		needToRebuildCamera = 1;
		vpInfo->oldeye = eyePoint;
	}
	if (lookAt != vpInfo->oldlookAt) {
		vpInfo->accumulate = 0;
		needToRebuildCamera = 1;
		vpInfo->oldlookAt = lookAt;
	}
	if (viewUp != vpInfo->oldeyeUp) {
		vpInfo->accumulate = 0;
		needToRebuildCamera = 1;
		vpInfo->oldeyeUp = viewUp;
	}

	if (0 == needToRebuildCamera)
		return status;   // No change in camera

	// Yes, it did, so update..

	// Field of view - Maya returns radians, we want degrees
	vpInfo->camFieldOfView = 57.2957795 * camera.verticalFieldOfView();
    
	// Location of the camera
	vpInfo->camPos.x = eyePoint.x;  
	vpInfo->camPos.y = eyePoint.y;  
	vpInfo->camPos.z = eyePoint.z; 

	// LookAt vector
	vpInfo->camLookAt.x = lookAt.x;
	vpInfo->camLookAt.y = lookAt.y;
	vpInfo->camLookAt.z = lookAt.z;
    
	// Up vector - negated because that makes the result match Maya viewport
	vpInfo->camUp.x = -viewUp.x;
	vpInfo->camUp.y = -viewUp.y;
	vpInfo->camUp.z = -viewUp.z;

	// Create a new updated camera, and drop the old one (by virtue of reference counting)
	createCamera(embree::AffineSpace3f::lookAtPoint(vpInfo->camPos, 
		vpInfo->camLookAt, vpInfo->camUp),
		aspectRatio, vpInfo);

	return status;
}

// ---------------------------
MStatus	EmbreeViewportRenderer::GetViewportHandles(const MRenderingInfo &renderInfo, 
		embree::Handle<embree::Device::RTCamera> &curr_camera, 
		embree::Handle<embree::Device::RTFrameBuffer> &curr_frameBuffer,
		int &accumulate)
{
	MStatus status;
	const MDagPath &cameraPath = renderInfo.cameraPath();
	const MRenderTarget &renderTarget = renderInfo.renderTarget();
	int width = renderTarget.width();
	int height = renderTarget.height();
	int i;
	int newCamera = 0;


	// Get the camera
	MFnCamera camera(cameraPath, &status);
	if (MS::kSuccess != status) {
            return status;
    }

	// And its name
	MString camName = camera.fullPathName();

	// Have we seen it before?
	for (i = 0; i < VIEWPORTARRAYSIZE; i++)
	{
		if (camName == m_viewportArray[i].fullCameraPathName)
			break;
		if (-1 == m_viewportArray[i].oldwidth)
			break;   // We reached the last initialized record
	}

	if (i == VIEWPORTARRAYSIZE)
		return MS::kFailure;   // Too many cameras to keep track of
	
	if (-1 == m_viewportArray[i].oldwidth)
	{
		// We have a new camera to track
		m_viewportArray[i].fullCameraPathName = camName;
		newCamera = 1;
	}

	// Adapt frame buffer to viewport changes
	if ((1 == newCamera) ||
		(width != m_viewportArray[i].oldwidth) || 
		(height != m_viewportArray[i].oldheight)) 
	{
		m_viewportArray[i].frameBuffer = m_device->rtNewFrameBuffer(m_format.c_str(), width, height, m_numBuffers);
		m_viewportArray[i].oldheight = height;
		m_viewportArray[i].oldwidth = width;
		m_viewportArray[i].accumulate = 0;
	}

	// Update the camera as necessary
	status = updateCamera(renderInfo, &m_viewportArray[i], newCamera);

	if (MS::kSuccess != status) {
            return status;
    }

    // Remember which camera we are not looking at
	curr_camera = m_viewportArray[i].camera;
	curr_frameBuffer = m_viewportArray[i].frameBuffer;

	if (0 == m_viewportArray[i].accumulate)
		accumulate = 0;   // Camera or frame buffer changed
	// else camera and frame buffer did not change, so obey current state of accumulate

	m_viewportArray[i].accumulate = m_refine;  // Reset this for next time around

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering method
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Description:
//		Render directly to current Embree target.
//
bool EmbreeViewportRenderer::renderInEmbreeSampleRenderer( const MRenderingInfo &renderInfo )
{
	MStatus status;
	const MRenderTarget &renderTarget = renderInfo.renderTarget();
	int width = renderTarget.width();
	int height = renderTarget.height();
	std::vector<MDagPath>  currentObjects;
	std::vector<MDagPath>  currentLights;
	int objectReplacementNeeded = 0;
	int lightReplacementNeeded = 0;
	int materialUpdated = 0;
	embree::Handle<embree::Device::RTCamera> curr_camera;
	embree::Handle<embree::Device::RTFrameBuffer> curr_frameBuffer;

	// Accumulate multiple renders in the frame buffer unless the 
    // scene or camera have changed
	int accumulate = m_refine;

	// Figure out what objects we have in the scene this time around
	collectSceneObjects(renderInfo, currentObjects, currentLights);

	// Adapt to changes in the scene from last time we were here
    
	// High-level "have things changed?" tests
    // -- different number of objects?
	if (currentObjects.size() != m_oldnumObjects) {
		accumulate = 0;
		m_oldnumObjects = currentObjects.size();
		objectReplacementNeeded = 1; //BVH rebuild flag
	}
    else
        if (0 == currentObjects.size())
            return true;   // Nothing to render - pretend success

    // -- different number of lights?
	if (currentLights.size() != m_oldnumLights) {
		accumulate = 0;
		m_oldnumLights = currentLights.size();
		lightReplacementNeeded = 1; // light replacement flag
	}

	// Now dive down into the objects and check them as we decide
	// if they need to be created/recreated
	objectReplacementNeeded = convertMayaObjectsToEmbree(currentObjects, objectReplacementNeeded,
		&materialUpdated);

	lightReplacementNeeded = convertMayaLightsToEmbree(currentLights, lightReplacementNeeded);

	if (1 == objectReplacementNeeded)
	{
		// Changes in objects - need to rebuild the BVH data structures and re-create the scene
		// Kill the existing scene
		if (m_render_scene)
		{
			m_device->rtClear(m_render_scene);
			m_render_scene = NULL;
			embree::rtClearTextureCache();
			embree::rtClearImageCache();
		}
		
		m_render_scene = createSceneFull();

		accumulate = 0;
	}
	else if ((1 == lightReplacementNeeded) && m_render_scene)
	{
		// Changes to lights - just replace the lights without rebuilding the scene from
		// scratch
		updateSceneLightsOnly(m_render_scene);

		accumulate = 0;
	}
	else if (1 == materialUpdated)
		accumulate = 0;    // Reset framebuffer on material update

	// Set up the frame buffer and camera used for rendering based on the
	// active Maya viewport
	status = GetViewportHandles(renderInfo, curr_camera, curr_frameBuffer, accumulate);

	if (status != MS::kSuccess)
		return false;

    // Now ray trace the scene!
	m_device->rtRenderFrame(m_renderer,
		curr_camera, 
		m_render_scene,
		m_tonemapper,
		curr_frameBuffer,
		accumulate);

	m_device->rtSwapBuffers(curr_frameBuffer);

	// Transfer result to Maya frame buffer
	unsigned char *cPtr = (unsigned char *)m_device->rtMapFrameBuffer(curr_frameBuffer);
	MImage image;

	image.setPixels(cPtr, width, height);

	// And display in Maya
	status = renderTarget.writeColorBuffer(image, 0, 0, false);

	// Clean up
	m_device->rtUnmapFrameBuffer(curr_frameBuffer);

	if (status != MS::kSuccess)
		return false;
	else
		return true;
}

