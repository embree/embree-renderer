// ======================================================================== //
// Copyright 2009-2014 Intel Corporation                                    //
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

#ifndef EmbreeViewportRenderer_h_
#define EmbreeViewportRenderer_h_

#include "device/handle.h"
#include "sys/platform.h"
#include "sys/ref.h"
#include "math/math.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/col3.h"
#include "math/affinespace.h"
#include "image/image.h"
#include "device/device.h"
#include "device/handle.h"
#include "sys/taskscheduler.h"
#include "lexers/streamfilters.h"
#include "lexers/parsestream.h"
#include "device/loaders/loaders.h"

#include <maya/MViewportRenderer.h>
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MRenderingInfo.h>
#include <maya/MFnCamera.h>
#include <maya/MFnLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFnNonAmbientLight.h>
#include <maya/MAngle.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MFloatVector.h>
#include <maya/MFnSet.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnReflectShader.h>
#include <maya/MFnPhongEShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnBlinnShader.h>

#include <maya/MItDag.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MBoundingBox.h>
#include <maya/MFnNumericData.h>
#include <maya/MItDependencyGraph.h>

#include <stdio.h>
#include <maya/MIOStream.h>

#include <maya/MDrawTraversal.h>
#include <maya/MGeometryManager.h>
#include <maya/MGeometry.h>
#include <maya/MGeometryData.h>
#include <maya/MGeometryPrimitive.h>
#include <maya/MHwTextureManager.h>
#include <maya/MImageFileInfo.h>
#include <maya/MEulerRotation.h>

#include <maya/MFnSingleIndexedComponent.h>

#include <iostream>
#include <vector>
#include <map>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#define VIEWPORTARRAYSIZE 16

// Keep track of scene data
class SceneData
{
   friend ostream &operator<<(ostream &, const SceneData &);

   public:
	  // object properties
      int numPrimitives;
      int numIndices;
	  int numPositions;
	  MBoundingBox boundBox;  // how much object editing will this catch?
	  MMatrix transform;
      
	  // Light properties
	  MColor color;
	  float intensity;  
	  MFloatVector lightDirection;
	  float coneAngle;
	  float penumbraAngle;
	  int decayRate;
      
	  // Material properties
	  int haveTexture;
	  std::string diffuseTextureFile;
      std::string specularTextureFile;
      std::string bumpTextureFile;
	  float diffcolor[3]; 
	  float transparentColor[3]; 
	  float specularColor[3]; 
	  float absorption[3];
      float roughness;  
	  float refractIndex; 
      float diffusionExponent;
      float transmissionDepth;
      
	  // Book keeping
	  int eindex;   // Embree index
	  int lindex;   // List index

      SceneData();
      SceneData(const SceneData &);
      ~SceneData(){};
      SceneData &operator=(const SceneData &rhs);
      int operator==(const SceneData &rhs) const;
      int operator<(const SceneData &rhs) const;
};

typedef struct viewPortInfo
{
	int oldwidth;
	int oldheight;
	MPoint oldeye;
	MVector oldlookAt;
	MVector oldeyeUp;
	embree::Handle<embree::Device::RTFrameBuffer> frameBuffer;
	embree::Handle<embree::Device::RTCamera> camera;
	MString  fullCameraPathName;
	embree::Vector3f camPos; 
	embree::Vector3f camLookAt; 
	embree::Vector3f camUp;
	float camFieldOfView;
	float camRadius;
	int accumulate;
} viewPortInfo;


class MDagPath;
class MBoundingBox;

//
// Plugin viewport renderer using the Embree API.
//
class EmbreeViewportRenderer : public MViewportRenderer
{
public:
	EmbreeViewportRenderer( const MString & name = "EmbreeViewportRenderer" );
	virtual ~EmbreeViewportRenderer();

	// Required virtual overrides from MViewportRenderer
	//
	virtual	MStatus	initialize();
	virtual	MStatus	uninitialize();
	virtual MStatus	render( const MRenderingInfo &renderInfo );
	virtual bool	nativelySupports( MViewportRenderer::RenderingAPI api,
										  float version );
	virtual bool	override( MViewportRenderer::RenderingOverride override );

    // Scene-conversion code and variables
protected:
	bool    convertLight(const MDagPath &dagPath);
	bool    convertSurface( const MDagPath &dagPath);
	bool    drawBounds( const MDagPath &dagPath,
                    const MBoundingBox &box);
	void    convertSurfaceMaterial(const MDagPath &dagPath, 
                    int haveTexture, 
                    embree::Handle<embree::Device::RTMaterial> &material);
	bool    collectSceneObjects (const MRenderingInfo &renderInfo, 
                    std::vector<MDagPath> &currentObjects,
                    std::vector<MDagPath> &currentLights);
	int     convertMayaObjectsToEmbree(std::vector<MDagPath> &currentObjects, 
                    int objectReplacementNeeded,
                    int *materialUpdated);
	int		convertMayaLightsToEmbree(std::vector<MDagPath> &currentLights, 
                    int lightReplacementNeeded);
	bool    renderToTarget( const MRenderingInfo &renderInfo );

	void    updateSceneLightsOnly(embree::Handle<embree::Device::RTScene> scene);

	MObject     findShader( MObject& setNode );
	int     checkMaterialProperties(const MDagPath &currObject, int haveTexture, SceneData &oldSceneInfo);
	void    getMaterialData(const MDagPath &currObject, int haveTexture, SceneData &materialInfo);

	void    getObjectData(MDagPath &currObject, SceneData &objectInfo);
	int     checkObjectProperities(MDagPath &currObject, SceneData &oldSceneInfo, int i,
                    int *materialUpdated);
	void    recordObjectProperties(MDagPath &currObject, int i, int eindex);

	void    getLightData(MDagPath &currLight, SceneData &lightInfo);
	int     checkLightProperities(MDagPath &currLight, SceneData &oldSceneInfo, int i);
	void    recordLightProperties(MDagPath &currLight, int i, int eindex);
	MStatus     GetViewportHandles(const MRenderingInfo &renderInfo, 
                    embree::Handle<embree::Device::RTCamera> &curr_camera, 
                    embree::Handle<embree::Device::RTFrameBuffer> &curr_frameBuffer,
                    int &accumulate);

	embree::Handle<embree::Device::RTScene>   createSceneFull();
	void createCamera(const embree::AffineSpace3f& space, 
                    const double aspectRatio,
                    viewPortInfo *vpInfo);
	MStatus     updateCamera(const MRenderingInfo &renderInfo, 
                    viewPortInfo *vpInfo, 
                    int newCamera);

	embree::Handle<embree::Device::RTData> 
            loadVec2fArray(float *inData, unsigned int numElem);
	embree::Handle<embree::Device::RTData> 
            loadVec3fArray(float *inData, unsigned int numElem);
	embree::Handle<embree::Device::RTData> 
            loadVec3iArray(int *inData, unsigned int numElem);
	embree::Handle<embree::Device::RTData> 
            loadVec4fArray(float *inData, unsigned int numElem);

	// Embree state for this instance
	/* rendering device and handles */
	embree::Device*     m_device;
	embree::Handle<embree::Device::RTRenderer>      m_renderer;
	embree::Handle<embree::Device::RTToneMapper>    m_tonemapper;
	embree::Handle<embree::Device::RTImage>         m_backplate;
	embree::Handle<embree::Device::RTScene>         m_render_scene;
	std::vector<embree::Handle<embree::Device::RTPrimitive> >   m_prims;
	std::vector<embree::Handle<embree::Device::RTPrimitive> >   m_lights;

	/* rendering settings */
	std::string     m_scene;
	std::string     m_accel;
	std::string     m_builder;
	std::string     m_traverser;
	int             m_depth;
	int             m_spp;

	/* output settings */
	int             m_numBuffers;
	int             m_refine;
	float           m_gamma;
	bool            m_vignetting;
	bool            m_fullscreen;
	size_t          m_width;
	size_t          m_height;
	std::string     m_format;
	std::string     m_rtcore_cfg;
	size_t          m_numThreads;

	/* Information used for change detection */
	int             m_oldnumObjects;
	int             m_oldnumLights;
	viewPortInfo    m_viewportArray[VIEWPORTARRAYSIZE];
	std::map<std::string, SceneData>    m_objectDatabase;
	std::map<std::string, SceneData>    m_lightDatabase;
    
    // Misc
    int             m_plugintype;

	RenderingAPI	m_API;		// Rendering API
	float			m_Version;	// Embree version number as float.
};

//
// ---------- Derived classes, one for each supported device type
//
class EmbreeViewportRendererXeonSingle : public EmbreeViewportRenderer
{
public:
	EmbreeViewportRendererXeonSingle();
	void createRenderDevice();
};

class EmbreeViewportRendererXeonISPC : public EmbreeViewportRenderer
{
public:
	EmbreeViewportRendererXeonISPC();
	void createRenderDevice();
};

class EmbreeViewportRendererXeonPhiSingle : public EmbreeViewportRenderer
{
public:
	EmbreeViewportRendererXeonPhiSingle();
	void createRenderDevice();
};

class EmbreeViewportRendererXeonPhiISPC : public EmbreeViewportRenderer
{
public:
        EmbreeViewportRendererXeonPhiISPC();
        void createRenderDevice();
};

#endif /* EmbreeViewportRenderer_h_ */

