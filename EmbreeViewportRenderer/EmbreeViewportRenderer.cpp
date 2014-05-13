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
   texturefile.clear();
   haveTexture = 0;
   diffcolor[0] = 1.0; diffcolor[1] = 1.0; diffcolor[2] = 1.0; 
   roughness = 0.0f;
   refractIndex = 1.5f;
   transparentColor[0] = 0.0; transparentColor[1] = 0.0; transparentColor[2] = 0.0;
   specularColor[0] = 0.0; specularColor[1] = 0.0; specularColor[2] = 0.0;

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
   texturefile = copyin.texturefile;
   haveTexture = copyin.haveTexture;
   diffcolor[0] = copyin.diffcolor[0]; diffcolor[1] = copyin.diffcolor[1]; diffcolor[2] = copyin.diffcolor[2];
   roughness = copyin.roughness;
   refractIndex = copyin.refractIndex;
   transparentColor[0] = copyin.transparentColor[0]; transparentColor[1] = copyin.transparentColor[1]; transparentColor[2] = copyin.transparentColor[2];
   specularColor[0] = copyin.specularColor[0]; specularColor[1] = copyin.specularColor[1]; specularColor[2] = copyin.specularColor[2];

   eindex = copyin.eindex;
   lindex = copyin.lindex;
}

ostream &operator<<(ostream &output, const SceneData &aaa)
{
   output << "Scene Data element:" << endl;
   output << aaa.numPrimitives << ' ' << aaa.numIndices << ' ' << aaa.numPositions << endl;
   output << aaa.boundBox.center() << ' ' << aaa.boundBox.width() << ' ' << aaa.boundBox.height() << ' ' << aaa.boundBox.depth() << endl;
   output << aaa.transform << endl;
   output << aaa.color << ' ' << aaa.intensity << '  ' << aaa.lightDirection << endl;
   output << aaa.coneAngle << ' ' << aaa.penumbraAngle << ' ' << aaa.decayRate << endl;
   output << "Embree Index " << aaa.eindex << "  List Index " << aaa.lindex << endl;
   if (aaa.haveTexture)
	   output << "Texture name " << aaa.texturefile << endl;
   output << "roughness=" << aaa.roughness << " refractindex= " << aaa.refractIndex << endl;
   output << "diffcolor = " << aaa.diffcolor[0] << "," << aaa.diffcolor[1] << "," << aaa.diffcolor[2] << endl; 
   output << "transparentColor = " << aaa.transparentColor[0] << "," << aaa.transparentColor[1] << "," << aaa.transparentColor[2] << endl; 
   output << "specularColor = " << aaa.specularColor[0] << "," << aaa.specularColor[1] << "," << aaa.specularColor[2] << endl; 
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
   this->texturefile = rhs.texturefile;
   this->roughness = rhs.roughness;
   this->refractIndex = rhs.refractIndex;
   this->diffcolor[0] = rhs.diffcolor[0]; this->diffcolor[1] = rhs.diffcolor[1]; this->diffcolor[2] = rhs.diffcolor[2]; 
   this->transparentColor[0] = rhs.transparentColor[0]; this->transparentColor[1] = rhs.transparentColor[1]; this->transparentColor[2] = rhs.transparentColor[2]; 
   this->specularColor[0] = rhs.specularColor[0]; this->specularColor[1] = rhs.specularColor[1]; this->specularColor[2] = rhs.specularColor[2]; 

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
   if( this->texturefile != rhs.texturefile) return 0;
   if( this->roughness != rhs.roughness) return 0;
   if( this->refractIndex != rhs.refractIndex) return 0;
   if( this->diffcolor[0] != rhs.diffcolor[0]) return 0; if( this->diffcolor[1] != rhs.diffcolor[1]) return 0; if( this->diffcolor[2] != rhs.diffcolor[2]) return 0;
   if( this->transparentColor[0] != rhs.transparentColor[0]) return 0; if( this->transparentColor[1] != rhs.transparentColor[1]) return 0; if( this->transparentColor[2] != rhs.transparentColor[2]) return 0;
   if( this->specularColor[0] != rhs.specularColor[0]) return 0; if( this->specularColor[1] != rhs.specularColor[1]) return 0; if( this->specularColor[2] != rhs.specularColor[2]) return 0;


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
   if( this->roughness < rhs.roughness) return 1;
   if( this->refractIndex < rhs.refractIndex) return 1;
   if( this->diffcolor[0] < rhs.diffcolor[0]) return 1;   if( this->diffcolor[1] < rhs.diffcolor[1]) return 1;   if( this->diffcolor[2] < rhs.diffcolor[2]) return 1;
   if( this->transparentColor[0] < rhs.transparentColor[0]) return 1;   if( this->transparentColor[1] < rhs.transparentColor[1]) return 1;   if( this->transparentColor[2] < rhs.transparentColor[2]) return 1;
   if( this->specularColor[0] < rhs.specularColor[0]) return 1;   if( this->specularColor[1] < rhs.specularColor[1]) return 1;   if( this->specularColor[2] < rhs.specularColor[2]) return 1;
  return 0;
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonSingle::EmbreeViewportRendererXeonSingle( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonSingle")
{
	// Set the ui name
	fUIName.set( "Single-Ray Embree Renderer for Xeon");
}

void EmbreeViewportRendererXeonSingle::createRenderDevice()
{
	if (m_device == NULL) 
	{
      m_device = embree::Device::rtCreateDevice("default",   // single ray device
	          m_numThreads,m_rtcore_cfg.c_str());
	}
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonISPC::EmbreeViewportRendererXeonISPC( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonISPC")
{
	// Set the ui name
	fUIName.set( "ISPC Embree Renderer for Xeon");
}

void EmbreeViewportRendererXeonISPC::createRenderDevice()
{
	if (m_device == NULL) 
	{
      m_device = embree::Device::rtCreateDevice("ispc",   // ispc_device
	          m_numThreads,m_rtcore_cfg.c_str());
	}
}

// ------------------------------------------------------------------
EmbreeViewportRendererXeonPhiSingle::EmbreeViewportRendererXeonPhiSingle( )
	: EmbreeViewportRenderer("EmbreeViewportRendererXeonPhiSingle")
{
	// Set the ui name
	fUIName.set( "Single-Ray Embree Renderer for Xeon Phi");
}

void EmbreeViewportRendererXeonPhiSingle::createRenderDevice()
{
	if (m_device == NULL) 
	{
//      m_device = embree::Device::rtCreateDevice("ispc_knc",   // COI
      m_device = embree::Device::rtCreateDevice("singleray_knc",   // COI
	          m_numThreads,m_rtcore_cfg.c_str());
	}
}

// ------------------------------------------------------------------

// Gets called when the plugin is *loaded* in the plugin manager
EmbreeViewportRenderer::EmbreeViewportRenderer( const MString & name )
:	MViewportRenderer( name )
{
	// This renderer overrides all drawing
	fRenderingOverride = MViewportRenderer::kOverrideThenUI;
//	fRenderingOverride = MViewportRenderer::kOverrideAllDrawing;

	// Set API and version number
	m_API = MViewportRenderer::kSoftware;
	m_Version = 2.2f;

	/* rendering device and handles */
	m_device = NULL;
	embree::g_device = NULL;
	m_renderer = NULL;
	m_tonemapper = NULL;
//	m_frameBuffer = NULL;
	m_backplate = NULL;
	m_render_scene = NULL;
//	m_camera = NULL;

	/* rendering settings */
	m_scene = "default";
	m_accel = "default";
	m_builder = "default";
	m_traverser = "default";
	m_depth = -1;                       //!< recursion depth - -1 sets default
	//  m_spp = 1;                          //!< samples per pixel for ordinary rendering
	m_spp = 4;  //!< samples per pixel for ordinary rendering

	/* output settings */
	m_numBuffers = 2;                   //!< number of buffers of the framebuffer
	m_refine = 1;                       //!< refinement mode - 1 = refine with each render, 0 = reset
	m_gamma = 1.0f;
	m_vignetting = false;
	m_fullscreen = false;
	m_width = 512;
	m_height = 512;
	m_format = "RGBA8";
	m_rtcore_cfg = "";   // used in rtcInit for the device - lotsa possible tweaks
	m_numThreads = 16;

	// Change detection
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
   // Unititialize gets called automatically when the plugin is *unloaded* by the plugin manager
	// uninitialize();
}

/* virtual */
// Gets called when the plugin is first used after being loaded in the plugin manager
MStatus
EmbreeViewportRenderer::initialize()
{
	char buffer[256];

sprintf(buffer, "EmbreeViewportRenderer::initialize");
MGlobal::displayInfo(buffer);

	if (NULL == m_device)
		return MStatus::kFailure;  // Device not created successfully
	else
		embree::g_device = m_device;  // WARNING WARNING - yes, the renderer needs this global set

sprintf(buffer, "2 - device=%lp", m_device );
MGlobal::displayInfo(buffer);

    /* create global objects */
	m_renderer = m_device->rtNewRenderer("pathtracer");

sprintf(buffer, "renderer=%lp", m_renderer );
MGlobal::displayInfo(buffer);

    if (m_depth >= 0) m_device->rtSetInt1(m_renderer, "maxDepth", m_depth);
    m_device->rtSetInt1(m_renderer, "sampler.spp", m_spp);
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
//	char buffer[256];

//sprintf(buffer, "EmbreeViewportRenderer::uninitialize");
//MGlobal::displayInfo(buffer);
	m_backplate = NULL;
//	m_frameBuffer = NULL;
	m_tonemapper = NULL;
	m_renderer = NULL;
	m_prims.clear();
	m_lights.clear();
	m_render_scene = NULL;
//	m_camera = NULL;
	embree::rtClearTextureCache();
	embree::rtClearImageCache();
	// Necessary to make sure that the Embree core shuts down fully
	if (m_device != NULL) {
		delete m_device;  // This will shut down the Embree thread pool
		m_device = NULL;
		embree::g_device = NULL;  // WARNING WARNING - anyone still using this is in trouble
	}

//    m_oldwidth=0;
//   m_oldheight=0;
	for (int i = 0; i < VIEWPORTARRAYSIZE; i++)
	{
		m_viewportArray[i].oldwidth=-1;
		m_viewportArray[i].oldheight=0;
		m_viewportArray[i].oldeye = MPoint(0,0,0);
		m_viewportArray[i].oldlookAt = MVector(0,0,0);
		m_viewportArray[i].oldeyeUp = MVector(0,0,0);
//		m_viewportArray[i].frameBuffer = NULL;
//		m_viewportArray[i].camera = NULL;
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

/* virtual */
MStatus
EmbreeViewportRenderer::render(const MRenderingInfo &renderInfo)
{
	// We don't care what renderInfo.renderingAPI() is used - we will use this one
	renderToTarget( renderInfo );
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
//		data[i] = embree::Vec3i(inData[3*i+2], inData[3*i+1], inData[3*i+0]);
//		cout << data[i] << "\n";
	}

	return m_device->rtNewData("immutable_managed", numElem*sizeof(embree::Vec3i), data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering methods
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
EmbreeViewportRenderer::convertLight(const MDagPath &dagPath)
{
	char buffer[1024];

	MStatus status;
	MString pname = dagPath.fullPathName() ;
#if 0
	sprintf(buffer, "%s is type %d (kLight=%d, kPoint=%d)", pname.asChar(),  dagPath.apiType(), 
		MFn::kLight,  MFn::kPointLight);
	MGlobal::displayInfo(buffer);
#endif
	// Get the light's transform node
	MMatrix  matrix = dagPath.inclusiveMatrix();

	float primXform[12] = {
		(float)matrix.matrix[0][0], -(float)matrix.matrix[1][0], -(float)matrix.matrix[2][0],
		-(float)matrix.matrix[0][1], (float)matrix.matrix[1][1], -(float)matrix.matrix[2][1],
		-(float)matrix.matrix[0][2], -(float)matrix.matrix[1][2], (float)matrix.matrix[2][2],
		(float)matrix.matrix[3][0], (float)matrix.matrix[3][1], (float)matrix.matrix[3][2]
	};

	// Get its attributes
	MFnLight thislight(dagPath, &status);
	MColor color = thislight.color();
	float intensity = thislight.intensity();
	MColor lightIntensity = thislight.lightIntensity();

	// Make intensity match mental ray better
	intensity *= 2.5;

//	cout << pname.asChar() << "Light Color:  " << color << "\n";

	if ( dagPath.hasFn( MFn::kPointLight ))	
	{
		// Create point light object
		embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("pointlight");
		const embree::Vector3f P((float)matrix.matrix[3][0], 
			(float)matrix.matrix[3][1], 
			(float)matrix.matrix[3][2]);
 //       const embree::Color I((float)(color.r * intensity),(float)(color.g * intensity),
 //			(float)(color.b * intensity));
		// For some reason R & B are mixed up in Embree
		const embree::Color I((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));
//		cout << "Point Light " << P << I << "\n";
		MFnNonAmbientLight nonAmb(dagPath, &status);
		int decayRate = nonAmb.decayRate();
	
		m_device->rtSetFloat3(light, "P", P.x, P.y, P.z);
		m_device->rtSetFloat3(light, "I", I.r, I.g, I.b);
		m_device->rtSetInt1(light, "decayRate", decayRate);
		m_device->rtCommit(light);
		m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL)); // &primXform[0]));
#if 0
		sprintf(buffer, "Set up point light %s", pname.asChar());
		MGlobal::displayInfo(buffer);
#endif
	} 
	else if ( dagPath.hasFn( MFn::kAmbientLight) )
	{
		// Create ambient light object
		embree::Handle<embree::Device::RTLight> light = m_device->rtNewLight("ambientlight");
		// For some reason R & B are mixed up in Embree
		const embree::Color L((float)(color.b * intensity),(float)(color.g * intensity),
			(float)(color.r * intensity));
//		cout << "Ambient Color " << L << "\n";
		m_device->rtSetFloat3(light, "L", L.r, L.g, L.b);
		m_device->rtCommit(light);
		m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL));
#if 0
		sprintf(buffer, "Set up ambient light %s", pname.asChar());
		MGlobal::displayInfo(buffer);
#endif
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
//				cout "Directional " << D << " " << E << "\n";
        m_device->rtSetFloat3(light, "D", D.x, D.y, D.z);
        m_device->rtSetFloat3(light, "E", E.r, E.g, E.b);
        m_device->rtCommit(light);
        m_lights.push_back(m_device->rtNewLightPrimitive(light, NULL, NULL)); // &primXform[0]));
#if 0
		sprintf(buffer, "Set up directional light %s", pname.asChar());
		MGlobal::displayInfo(buffer);
#endif
	}
	else if ( dagPath.hasFn( MFn::kSpotLight) )
	{
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
#if 0
		cout << I << "\n";
		sprintf(buffer, "Set up spot light %s, %f %f", pname.asChar(), coneAngle, coneAngle+penumbraAngle);
		MGlobal::displayInfo(buffer);
#endif
	}


	return true;
}

// ------------------------------------------------------------

MObject EmbreeViewportRenderer::findShader( MObject& setNode )
//
//  Description:
//      Find the shading node for the given shading group set node.
//
{
	MFnDependencyNode fnNode(setNode);
	MPlug shaderPlug = fnNode.findPlug("surfaceShader");
			
	if (!shaderPlug.isNull()) {			
		MPlugArray connectedPlugs;
		bool asSrc = false;
		bool asDst = true;
		shaderPlug.connectedTo( connectedPlugs, asDst, asSrc );

		if (connectedPlugs.length() != 1)
			MGlobal::displayError("Error getting shader");
		else 
			return connectedPlugs[0].node();
	}			
	
	return MObject::kNullObj;
}

// ------------------------------------------------------------

void EmbreeViewportRenderer::getMaterialData(const MDagPath &currObject, 
	int haveTexture, SceneData &materialInfo)
{
	std::string texturefile;
	float diffColor[3] = {1.0, 1.0, 1.0};
	float roughness = 0.0f;  // pure specular
	float refractIndex = 1.5f;
	float transparentColor[3] = {0.0, 0.0, 0.0};
	float specularColor[3] = {0.0, 0.0, 0.0};

	MFnMesh fnMesh(currObject);
	MObjectArray sets;
	MObjectArray comps;
	unsigned int instanceNum = currObject.instanceNumber();
	if (!fnMesh.getConnectedSetsAndMembers(instanceNum, sets, comps, true))
		MGlobal::displayError("ERROR : MFnMesh::getConnectedSetsAndMembers");
	
	texturefile.clear();
	for ( unsigned i=0; i<sets.length(); i++ ) 
	{
		MObject set = sets[i];  // all sets
		MObject comp = comps[i];  // components in the corresponding set

		MStatus status;
		MFnSet fnSet( set, &status );
		if (status == MS::kFailure) {
			MGlobal::displayError("ERROR: MFnSet::MFnSet");
			continue;
		}

		// Scrape material properties from the first material node associated with the
		// surface
		MObject shaderNode = findShader(set);
		if (shaderNode != MObject::kNullObj)
		{
			// Color data
			MPlug colorPlug = MFnDependencyNode(shaderNode).findPlug("color", &status);
			if (status != MS::kFailure)
			{
				MItDependencyGraph It( colorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream);
				if( !It.isDone())
				{
					// We have a texture associated with this material
					// Get the filename
					MObject				MayaTexture = It.thisNode();
					MString filename;
					MFnDependencyNode( MayaTexture).findPlug( "fileTextureName").getValue( filename);
					if( filename.length())
						texturefile = filename.asChar();
#if 0
char buffer[1024];
MString pname = dagPath.fullPathName() ;
sprintf(buffer, "%s: texture %s", pname.asChar(), filename.asChar());
MGlobal::displayInfo(buffer);
#endif
				}
				else 
				{
					// Only basic colors
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
				MObject data;
				specularColorPlug.getValue( data);
				MFnNumericData val(data);
				val.getData( specularColor[0], specularColor[1], specularColor[2]);
			}

			// Index of refraction
			MPlug refIndexPlug = MFnDependencyNode(shaderNode).findPlug("refractiveIndex", &status);
			if (status != MS::kFailure)
			{
				MObject data;
				refIndexPlug.getValue( refractIndex );
			}

			// Figure out what sort of shading model is being used since
			// everything reports MFn::kLambert and nothing else in Maya 2014
			int lam, phng, blnn, phnge;
			MFnLambertShader lambert(shaderNode, &status);
			lam = (status == MS::kSuccess);   // Seems to always be one...even for PhongE
			MFnPhongShader phong(shaderNode, &status);
			phng = (status == MS::kSuccess); 
			MFnPhongEShader phonge(shaderNode, &status);
			phnge = (status == MS::kSuccess);
			MFnBlinnShader blinn(shaderNode, &status);
			blnn = (status == MS::kSuccess);
#if 0
char buffer[1026];
MString pname = dagPath.fullPathName() ;
sprintf(buffer, "%s/%d: lambert %d/%d, phong %d/%d, blinn %d/%d phonge %d ", pname.asChar(), i,
shaderNode.hasFn(MFn::kLambert), lam,
shaderNode.hasFn(MFn::kPhong), phng, 
shaderNode.hasFn(MFn::kBlinn), blnn, phnge);
MGlobal::displayInfo(buffer);
#endif

// char buffer[1026];

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
					roughness = 100.0f-cosPower/100.0f;
				}	
#if 0
MString pname = dagPath.fullPathName() ;
sprintf(buffer, "%s:  cosPower = %f", pname.asChar(), cosPower);
MGlobal::displayInfo(buffer);
#endif
			} else if (blnn) {
				// "Metal" 
				// Maya: The valid range is 0 (no highlight) to 0.999 (broad highlight, 
				// not very shiny surface)
				float eccentricity = 0.0f;
				MPlug eccentricityPlug = MFnDependencyNode(shaderNode).findPlug("eccentricity", &status);
				if (status != MS::kFailure)
				{
					MObject data;
					eccentricityPlug.getValue( eccentricity );
					roughness = eccentricity;
					// ??? Maya's funky remapping of eccentricity into cosinePower.
					//Material.Power = (eccentricity < 0.03125f) ? 128.0f : 4.0f / eccentricity;
				}
#if 0
MString pname = dagPath.fullPathName() ;
sprintf(buffer, "%s:  eccentricity = %f", pname.asChar(), eccentricity);
MGlobal::displayInfo(buffer);
#endif
			} else if (phnge) {
				// We either have a PhongE or Anisotropic shader - assume the former
				// More "plastic" 
				// Also has "highlightSize" and "whiteness" attributes
				MPlug roughnessPlug = MFnDependencyNode(shaderNode).findPlug("roughness", &status);
				if (status != MS::kFailure)
				{
					MObject data;
					roughnessPlug.getValue( roughness );
				}	
#if 0
MString pname = dagPath.fullPathName() ;
sprintf(buffer, "%s:  roughness = %f", pname.asChar(), roughness);
MGlobal::displayInfo(buffer);
#endif
			}

			materialInfo.haveTexture = haveTexture;
			materialInfo.texturefile = texturefile;
			materialInfo.roughness = roughness;
			materialInfo.refractIndex = refractIndex;
			materialInfo.diffcolor[0] = diffColor[0]; materialInfo.diffcolor[1] = diffColor[1]; materialInfo.diffcolor[2] = diffColor[2];
			materialInfo.transparentColor[0] = transparentColor[0]; materialInfo.transparentColor[1] = transparentColor[1]; materialInfo.transparentColor[2] = transparentColor[2];
			materialInfo.specularColor[0] = specularColor[0]; materialInfo.specularColor[1] = specularColor[1]; materialInfo.specularColor[2] = specularColor[2];
			break;  // Start with one shader per object
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
	if (oldSceneInfo.texturefile != currMaterialData.texturefile) replacementNeeded = 1;
	if (oldSceneInfo.roughness != currMaterialData.roughness) replacementNeeded = 1;
	if (oldSceneInfo.refractIndex != currMaterialData.refractIndex) replacementNeeded = 1;
	if (oldSceneInfo.diffcolor[0] != currMaterialData.diffcolor[0]) replacementNeeded = 1;
	if (oldSceneInfo.diffcolor[1] != currMaterialData.diffcolor[1]) replacementNeeded = 1;
	if (oldSceneInfo.diffcolor[2] != currMaterialData.diffcolor[2]) replacementNeeded = 1;
	if (oldSceneInfo.transparentColor[0] != currMaterialData.transparentColor[0]) replacementNeeded = 1;
	if (oldSceneInfo.transparentColor[1] != currMaterialData.transparentColor[1]) replacementNeeded = 1;
	if (oldSceneInfo.transparentColor[2] != currMaterialData.transparentColor[2]) replacementNeeded = 1;
	if (oldSceneInfo.specularColor[0] != currMaterialData.specularColor[0]) replacementNeeded = 1;
	if (oldSceneInfo.specularColor[1] != currMaterialData.specularColor[1]) replacementNeeded = 1;
	if (oldSceneInfo.specularColor[2] != currMaterialData.specularColor[2]) replacementNeeded = 1;

	return replacementNeeded;
}

// ------------------------------------------------------------

void EmbreeViewportRenderer::convertSurfaceMaterial(const MDagPath &dagPath, 
	int haveTexture, embree::Handle<embree::Device::RTMaterial> &material)
{
	SceneData materialInfo;

	getMaterialData(dagPath, haveTexture, materialInfo);

	material = m_device->rtNewMaterial("Uber");
	m_device->rtSetFloat3(material, "diffColor", 
				materialInfo.diffcolor[2], materialInfo.diffcolor[1], materialInfo.diffcolor[0]);
	m_device->rtSetFloat3(material, "reflColor", 
				materialInfo.specularColor[2], materialInfo.specularColor[1], materialInfo.specularColor[0]);
	m_device->rtSetFloat1(material, "roughness", materialInfo.roughness);
	m_device->rtSetFloat3(material, "transColor", 
				materialInfo.transparentColor[2], materialInfo.transparentColor[1], materialInfo.transparentColor[0]);
	m_device->rtSetFloat1(material, "etaInside", materialInfo.refractIndex);
//			m_device->rtSetFloat3(material, "realRefract", 0.19f, 0.45f, 1.50f); // gold
//			m_device->rtSetFloat3(material, "imagRefract", 3.06f, 2.40f, 1.88f);

	if ((1 == haveTexture) && (!materialInfo.texturefile.empty())) {  // need texture coordinates *and* a file
		// Load and assign the texture file
		// WARNING!!!
		// Note:  only ppm and pfm supported unless you build with USE_IMAGEMAGICK
		m_device->rtSetTexture(material, "diffTexture", embree::rtLoadTexture(materialInfo.texturefile));
	}
}

// ------------------------------------------------------------

bool EmbreeViewportRenderer::convertSurface( const MDagPath &dagPath)
{
	bool drewSurface = false;
	char buffer[1024];

	/// TODO:  At some point pay attention to "active" and "templated"

	if ( dagPath.hasFn( MFn::kMesh ))
	{
		MObject object = dagPath.node();
		MFnMesh mesh(object);

		// Figure out texturing
		//
		bool haveTexture = false;
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

		bool haveColors = false;
		int	numColors = mesh.numColorSets();
		MString colorSetName;
		if (numColors > 0)
		{
			haveColors = true;
			mesh.getCurrentColorSetName(colorSetName);
		}

		bool useNormals = true;
//		bool useNormals = false;

		// Setup our requirements needs.
		MGeometryRequirements requirements;
		requirements.addPosition();
		if (useNormals)
			requirements.addNormal();
		if (haveTexture)
			requirements.addTexCoord( uvSetName );
		if (haveColors)
			requirements.addColor( colorSetName );
		/*
		// Test for tangents and binormals
		bool testBinormal = false;
		if (testBinormal)
			requirements.addBinormal( uvSetName );
		bool testTangent= false;
		if (testTangent)
			requirements.addTangent( uvSetName );
		*/

		MGeometry geom = MGeometryManager::getGeometry( dagPath, requirements, NULL );

		unsigned int numPrims = geom.primitiveArrayCount();

		MString ccname = dagPath.fullPathName() ;
#if 0
		sprintf(buffer, "%s consists of %d primitives", 
				ccname.asChar(), numPrims);
		MGlobal::displayInfo(buffer);
#endif

		if (numPrims)
		{
			/*   MGeometryPrimitive is a class describes the topology used for
				accessing MGeometryData.
				Topology is specified as a set of index values which references into
				data elements in an MGeometryData. Index values can be assumed to be
				stored in contiguous memory.*/
			const MGeometryPrimitive prim = geom.primitiveArray(0);
			unsigned int numElem = prim.elementCount();
#if 0
			sprintf(buffer, "%s - %d elements", 
					ccname.asChar(), numElem);
			MGlobal::displayInfo(buffer);
#endif

			if (numElem)
			{
				// Is the data in floats, ints, etc.
				MGeometryData::ElementType elemType = prim.dataType();

//				unsigned int *idx = (unsigned int *) prim.data();
				int *idx = (int *) prim.data();

				if (elemType != MGeometryData::ElementType::kInt32) 
				{
					sprintf(buffer, "%s: Error!  Data type %d (kInt32=%d, kUnsignedInt32 =%d):  0=%d, 1=%d, 2=%d", 
							ccname.asChar(), elemType, 
							MGeometryData::ElementType::kInt32,
							MGeometryData::ElementType::kUnsignedInt32,
							idx[0], idx[1], idx[2]);
					MGlobal::displayInfo(buffer);
				}

				// Get the position data
				const MGeometryData pos = geom.position();
				float * posPtr = (float * )pos.data();

				bool haveData = idx && posPtr;

				MGeometryPrimitive::DrawPrimitiveType primType = prim.drawPrimitiveType();

				if (primType != MGeometryPrimitive::DrawPrimitiveType::kTriangles)
				{
					MString pname = dagPath.fullPathName() ;
					sprintf(buffer, "%s: Error!  Primitive type: %d (Invalid=%d, Triangles=%d)", 
							pname.asChar(), primType,
							MGeometryPrimitive::DrawPrimitiveType::kInvalidIndexType,
							MGeometryPrimitive::DrawPrimitiveType::kTriangles);
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

				unsigned int numColorComponents = 4;
				float *clrPtr = NULL;
				if (haveColors)
				{
					const MGeometryData clrs = geom.color( colorSetName );
					clrPtr = (float *)clrs.data();
				}
				/*
				else if (testBinormal)
				{
					const MGeometryData binorm = geom.binormal( uvSetName );
					clrPtr = (float *)binorm.data();
					numColorComponents = 3;
				}
				else if (testTangent)
				{
					const MGeometryData tang = geom.tangent( uvSetName );
					clrPtr = (float *)tang.data();
					numColorComponents = 3;
				}
				*/
#if 0
			sprintf(buffer, "%s - haveData=%d", 
					ccname.asChar(), haveData);
			MGlobal::displayInfo(buffer);
#endif
				if (haveData)
				{
					drewSurface = true;

					// Get the mesh's transform node
					MMatrix  matrix = dagPath.inclusiveMatrix();

					// Rotate everything on X, Y, and Z axes by 180
					float primXform[12] = {
						(float)matrix.matrix[0][0], -(float)matrix.matrix[1][0], -(float)matrix.matrix[2][0],
						-(float)matrix.matrix[0][1], (float)matrix.matrix[1][1], -(float)matrix.matrix[2][1],
						-(float)matrix.matrix[0][2], -(float)matrix.matrix[1][2], (float)matrix.matrix[2][2],
			  			(float)matrix.matrix[3][0], (float)matrix.matrix[3][1], (float)matrix.matrix[3][2]
					};
//embree::AffineSpace3f testmatrix = embree::copyFromArray((const float *)&primXform[0]);
//cout << ccname.asChar() << " " << testmatrix << "\n";
					//  ------ Now load mesh data
					// Create material for mesh
					embree::Handle<embree::Device::RTMaterial> material;
					convertSurfaceMaterial(dagPath, haveTexture, material);
					m_device->rtCommit(material);

	// Record size of material?

					// Create a mesh object
					embree::Handle<embree::Device::RTShape> mesh = m_device->rtNewShape("trianglemesh");

					// Transfer the position data
					unsigned int numPos = pos.elementCount();
					embree::Handle<embree::Device::RTData> positions = loadVec3fArray(posPtr, numPos);
					m_device->rtSetArray(mesh, "positions", "float3", positions, numPos, sizeof(embree::Vec3f), 0);

					// Triangle indices
					embree::Handle<embree::Device::RTData> triangles = loadVec3iArray(idx, numElem/3);
					m_device->rtSetArray(mesh, "indices", "int3", triangles, numElem/3, sizeof(embree::Vec3i), 0);

					// Normals
					if (useNormals)
					{
						embree::Handle<embree::Device::RTData> normals = loadVec3fArray(normPtr, numNorm);
						m_device->rtSetArray(mesh, "normals", "float3", normals, numNorm, sizeof(embree::Vec3f), 0);
					}

					// UV coordinates
					if (haveTexture)
					{
						embree::Handle<embree::Device::RTData> uvcoords = loadVec2fArray(uvPtr, numPos);
						m_device->rtSetArray(mesh, "texcoords", "float2", uvcoords, numPos, sizeof(embree::Vec2f), 0);
					}

					// Color data - TDB

					// Finish up the mesh
					m_device->rtSetString(mesh,"accel", "default");
					m_device->rtSetString(mesh,"builder", "default");
					m_device->rtSetString(mesh,"traverser", "default");
					m_device->rtCommit(mesh);
					m_device->rtClear(mesh);

					// And commit it to the scene
					m_prims.push_back(m_device->rtNewShapePrimitive(mesh, material, &primXform[0]));
//					m_prims.push_back(m_device->rtNewShapePrimitive(mesh, material, NULL));
#if 0
					MString pname = dagPath.fullPathName() ;
					sprintf(buffer, "Set mesh up %s to draw: Tris=%d, positions=%d, normals=%d", pname.asChar(),
						numElem/3, numPos, numNorm);
					MGlobal::displayInfo(buffer);
#endif
				}   // Have object data
			}  // There are triangles in this primitive
		}  // There are primitives
	} // If this is a mesh node

	return drewSurface;
}

bool
EmbreeViewportRenderer::drawBounds( const MDagPath &dagPath,
								    const MBoundingBox &box)
{

	return true;
}

//
// Uncomment #define to see how pruning works
//
//#define _DEBUG_TRAVERSAL_PRUNING

// Use a custom traverser class.
class MsurfaceDrawTraversal : public MDrawTraversal
{
	virtual bool		filterNode( const MDagPath &traversalItem )
	{
		bool prune = false;
		char buffer[1024];

		//
		// Check to only prune shapes, not transforms.
		//
#if defined(_DEBUG_TRAVERSAL_PRUNING)
		MString pname = traversalItem.fullPathName() ;
#endif
		if ( traversalItem.childCount() == 0)
		{
			if ( !traversalItem.hasFn( MFn::kMesh) &&
//				!traversalItem.hasFn( MFn::kNurbsSurface) &&
//				!traversalItem.hasFn( MFn::kSubdiv) &&
//				!traversalItem.hasFn( MFn::kSketchPlane ) &&
//				!traversalItem.hasFn( MFn::kGroundPlane ) &&
				!traversalItem.hasFn( MFn::kLight) &&
				!traversalItem.hasFn( MFn::kAmbientLight) &&
				!traversalItem.hasFn( MFn::kNonAmbientLight) &&
				!traversalItem.hasFn( MFn::kAreaLight) &&
				!traversalItem.hasFn( MFn::kLinearLight ) &&
				!traversalItem.hasFn( MFn::kNonExtendedLight ) &&
				!traversalItem.hasFn( MFn::kDirectionalLight ) &&
				!traversalItem.hasFn( MFn::kPointLight ) &&
				!traversalItem.hasFn( MFn::kSpotLight )
				)
			{
#if defined(_DEBUG_TRAVERSAL_PRUNING)
				sprintf(buffer, "Prune path [ %s ]", pname.asChar());
				MGlobal::displayInfo(buffer);
#endif
				prune = true;
			}
			else
			{
				prune = false;
#if defined(_DEBUG_TRAVERSAL_PRUNING)
//				sprintf(buffer, "Don't prune path [ %s ]", pname.asChar());
//				MGlobal::displayInfo(buffer);
#endif
			}
		}
		else
		{
#if defined(_DEBUG_TRAVERSAL_PRUNING)
			//sprintf(buffer, "Not leaf node? [ %s ]", pname.asChar());
			//MGlobal::displayInfo(buffer);
#endif
		}
		return prune;
	}
};

// ------------------------------

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
	unsigned int numIndices;
	unsigned int numPos;
	MBoundingBox bbox(box.min(), box.max());
	bool haveTexture = false;

	if (numPrims)
	{
		const MGeometryPrimitive prim = geom.primitiveArray(0);
		numIndices = prim.elementCount();
		const MGeometryData pos = geom.position();
		numPos = pos.elementCount();
	}

	// Do we have textures?
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
int EmbreeViewportRenderer::checkObjectProperities(MDagPath &currObject, SceneData &oldSceneInfo, int i) 
{
	int replacementNeeded = 0;
	SceneData currObjectData;

	// Get information on the light from Maya
	getObjectData(currObject, currObjectData);

	// Look for changes
	if (oldSceneInfo.numPrimitives != currObjectData.numPrimitives) replacementNeeded = 1;
	if (oldSceneInfo.numIndices != currObjectData.numIndices) replacementNeeded = 1;
	if (oldSceneInfo.numPositions != currObjectData.numPositions) replacementNeeded = 1;
	if (oldSceneInfo.transform != currObjectData.transform) replacementNeeded = 1;
	if (oldSceneInfo.boundBox.center() != currObjectData.boundBox.center()) replacementNeeded = 1;
	if (oldSceneInfo.boundBox.min() != currObjectData.boundBox.min()) replacementNeeded = 1;
	if (oldSceneInfo.boundBox.max() != currObjectData.boundBox.max()) replacementNeeded = 1;
	if (oldSceneInfo.lindex != i) replacementNeeded = 1;

	if (0 == replacementNeeded)
		replacementNeeded = checkMaterialProperties(currObject, currObjectData.haveTexture, oldSceneInfo);

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
								int objectReplacementNeeded)
{
	int i;
	int numObjects = currentObjects.size();
	int replacementNeeded = objectReplacementNeeded;
	std::string currObjectName;
	char buffer[1024];

	// If "replacement needed" is already true, just rebuild 
	// everything rather than trying to find if there were changes
	if (0 == objectReplacementNeeded)
	{
		// Check to see if any of the objects changed
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

				replacementNeeded = checkObjectProperities(currObject, oldSceneInfo, i);

				if (replacementNeeded == 1)
					break;   // There was a change - no point in checking further
			} // check object
		}  // all objects
	}  // Check if objects changed

	// If nothing changed, we are done
	if (0 == replacementNeeded)
		return 0;

	// Something changed - rebuild the object databases
	m_objectDatabase.clear();
	m_prims.clear();
//	embree::rtClearTextureCache();
//  embree::rtClearImageCache();

	for (i = 0; i < numObjects; i++)
	{
		// Get the next object
		MDagPath currObject = currentObjects[i];

		// Build the Embree version of the object
		convertSurface(currObject);

		// Record the object's properties for change detection
		recordObjectProperties(currObject, i, m_prims.size());
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

	// If "replacement needed" is already true, flush the database and do a full rebuild
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

				replacementNeeded = checkLightProperities(currLight, oldSceneInfo, i);

				if (replacementNeeded == 1)
					break;  // There was a change - no point in checking further
			} // seen the light before
		} // for all lights
	} // Check for changes

	// If nothing changed, we are done
	if (0 == replacementNeeded)
		return 0;

	// Something changed - rebuild the light databases
	m_lightDatabase.clear();
	m_lights.clear();

	for (i = 0; i < numLights; i++)
	{
		// Get a light and its name
		MDagPath currLight = currentLights[i];

		// Build the Embree version of the object
		convertLight(currLight);

		// Record the object's properties for change detection
		recordLightProperties(currLight, i, m_lights.size());
	}  // retranslate all lights

	return 1;
}

// ------------------------------

bool EmbreeViewportRenderer::collectSceneObjects (const MRenderingInfo &renderInfo, 
		std::vector<MDagPath> &currentObjects, std::vector<MDagPath> &currentLights)
{
	const MRenderTarget &renderTarget = renderInfo.renderTarget();
	const MDagPath &cameraPath = renderInfo.cameraPath();
	char buffer[1024];

//#define _DEBUG_TRAVERSAL_PRUNING
	// ------
	// Scan the entire DAG for leaves that are objects or lights
	// This is necessary since off-camera objects or lights can
	// effect the lighting in the view of the camera)
	// -------
	MStatus status;
	MItDag dagIterator( MItDag::kDepthFirst, MFn::kInvalid, &status);
	if (status != MS::kSuccess)
		return false;

	for ( ; !dagIterator.isDone() ; dagIterator.next() ) 
	{
		MDagPath dagPath;
		status = dagIterator.getPath(dagPath);
		if (status != MS::kSuccess) break;
#if defined(_DEBUG_TRAVERSAL_PRUNING)
				MString pname = dagPath.fullPathName() ;
				sprintf(buffer, "Visiting [ %s ] - children=%d", pname.asChar(),
					dagPath.childCount());
				MGlobal::displayInfo(buffer);
#endif
		if (0 != dagPath.childCount()) continue;  // Only leaf nodes

		if (dagPath.isValid())
		{
			//
			// Draw surfaces (polys, nurbs, subdivs)
			//
			if ( dagPath.hasFn( MFn::kMesh)
//						|| path.hasFn( MFn::kNurbsSurface) ||
//						path.hasFn( MFn::kSubdiv) ||
//						path.hasFn( MFn::kSketchPlane ) ||
//						path.hasFn( MFn::kGroundPlane)
					)
			{
#if defined(_DEBUG_TRAVERSAL_PRUNING)
				MString pname = dagPath.fullPathName() ;
				sprintf(buffer, "Translating mesh [ %s ]", pname.asChar());
				MGlobal::displayInfo(buffer);
#endif
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
#if defined(_DEBUG_TRAVERSAL_PRUNING)
				MString pname = dagPath.fullPathName() ;
				sprintf(buffer, "Translating light [ %s ]", pname.asChar());
				MGlobal::displayInfo(buffer);
#endif
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

	// Set up the objects
    for (size_t i=0; i<num_objs; i++) 
		m_device->rtSetPrimitive(scene,i,m_prims[i]);

	// Set up the lights
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
	char buffer[1024];
	MStatus status;
	const MDagPath &cameraPath = renderInfo.cameraPath();
	const MRenderTarget &renderTarget = renderInfo.renderTarget();
	//const MMatrix & view = renderInfo.viewMatrix(); 
	int width = renderTarget.width();
	int height = renderTarget.height();
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

	// Did it change?  Also use this to populate a new camera
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
//	MVector viewDirection = camera.viewDirection(MSpace::kWorld);
	// LookAt vector
	vpInfo->camLookAt.x = lookAt.x;
	vpInfo->camLookAt.y = lookAt.y;
	vpInfo->camLookAt.z = lookAt.z;
	// Up vector - negated because that makes the result match Maya viewport
	vpInfo->camUp.x = -viewUp.x;
	vpInfo->camUp.y = -viewUp.y;
	vpInfo->camUp.z = -viewUp.z;
/*
	sprintf(buffer, "campos=%f, %f, %f, lookat=%f %f %f", eyePoint.x, eyePoint.y, eyePoint.z,
		lookAt.x, lookAt.y, lookAt.z);
	MGlobal::displayInfo(buffer);
*/
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

	curr_camera = m_viewportArray[i].camera;
	curr_frameBuffer = m_viewportArray[i].frameBuffer;

	if (0 == m_viewportArray[i].accumulate)
		accumulate = 0;   // Camera or frame buffer changed
	// else camera and frame buffer did not change, so obey current state of accumulate

	m_viewportArray[i].accumulate = m_refine;  // Reset this for next time around

	return status;
}

// ---------------------------

bool EmbreeViewportRenderer::renderToTarget( const MRenderingInfo &renderInfo )
//
// Description:
//		Render directly to current Embree target.
//
{
	char buffer[1024];
	MStatus status;
	bool cStatus;
	const MRenderTarget &renderTarget = renderInfo.renderTarget();
	int width = renderTarget.width();
	int height = renderTarget.height();
	std::vector<MDagPath>  currentObjects;
	std::vector<MDagPath>  currentLights;
	int objectReplacementNeeded = 0;
	int lightReplacementNeeded = 0;
	embree::Handle<embree::Device::RTCamera> curr_camera;
	embree::Handle<embree::Device::RTFrameBuffer> curr_frameBuffer;

	// Accumulate multiple renders in the frame buffer unless the scene or camera have changed
	int accumulate = m_refine;

	// Figure out what objects we have in the scene this time around
	collectSceneObjects(renderInfo, currentObjects, currentLights);

	// Adapt to changes in the scene from last time we were here
	// High-level "have things changed?" tests
	if (currentObjects.size() != m_oldnumObjects) {
		accumulate = 0;
		m_oldnumObjects = currentObjects.size();
		objectReplacementNeeded = 1; //BVH rebuild flag
	}

	if (currentLights.size() != m_oldnumLights) {
		accumulate = 0;
		m_oldnumLights = currentLights.size();
		lightReplacementNeeded = 1; // light replacement flag
	}

	// Now dive down into the objects and check them as we decide
	// if they need to be created/recreated
	objectReplacementNeeded = convertMayaObjectsToEmbree(currentObjects, objectReplacementNeeded);

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

#if 0
		sprintf(buffer, "Primitives to render=%d, Lights to Render=%d", m_prims.size(),
			m_lights.size());
		MGlobal::displayInfo(buffer);
#endif
		accumulate = 0;
	}
	else if ((1 == lightReplacementNeeded) && m_render_scene)
	{
		// Changes to lights - just replace the lights without rebuilding the scene from
		// scratch
		updateSceneLightsOnly(m_render_scene);
#if 0
		sprintf(buffer, "Lights to Render=%d", m_lights.size());
		MGlobal::displayInfo(buffer);
#endif
		accumulate = 0;
	}

	// Set up the frame buffer and camera used for rendering based on
	// the Maya camera in use.
	status = GetViewportHandles(renderInfo, curr_camera, curr_frameBuffer, accumulate);

#if 0
		sprintf(buffer, "Primitives to render=%d, Lights to Render=%d, accumulate=%d", m_prims.size(),
			m_lights.size(), accumulate);
		MGlobal::displayInfo(buffer);
#endif

	if (status != MS::kSuccess)
		return false;
	/*
	// Adapt frame buffer to viewport changes
	if ((width != m_oldwidth) || (height != m_oldheight)) {
		m_frameBuffer = m_device->rtNewFrameBuffer(m_format.c_str(), width, height, m_numBuffers);
		m_oldheight = height;
		m_oldwidth = width;
		accumulate = 0;
	}

	// Update the camera as necessary
	status = updateCamera(renderInfo, &accumulate);

	// render into framebuffer
	m_device->rtRenderFrame(m_renderer,
		m_camera, 
		m_render_scene,
		m_tonemapper,
		m_frameBuffer,
		accumulate);
*/
	// render into framebuffer
	m_device->rtRenderFrame(m_renderer,
		curr_camera, 
		m_render_scene,
		m_tonemapper,
		curr_frameBuffer,
		accumulate);

//	m_device->rtSwapBuffers(m_frameBuffer);
	m_device->rtSwapBuffers(curr_frameBuffer);

	// Transfer result to Maya frame buffer
//	unsigned char *cPtr = (unsigned char *)m_device->rtMapFrameBuffer(m_frameBuffer);
	unsigned char *cPtr = (unsigned char *)m_device->rtMapFrameBuffer(curr_frameBuffer);
	MImage image;

	image.setPixels(cPtr, width, height);

	// And display in Maya
	status = renderTarget.writeColorBuffer(image, 0, 0, false);

	// Clean up
//	m_device->rtUnmapFrameBuffer(m_frameBuffer);
	m_device->rtUnmapFrameBuffer(curr_frameBuffer);

	if (status != MS::kSuccess)
		return false;
	else
		return true;
}

