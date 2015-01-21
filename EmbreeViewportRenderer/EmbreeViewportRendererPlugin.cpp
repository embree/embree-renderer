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

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MFnPlugin.h>

#include <stdio.h>

///////////////////////////////////////////////////
// Register the Embree renderers during plugin initialization/shutdown
///////////////////////////////////////////////////

static EmbreeViewportRendererXeonSingle     *g_EmbreeViewportRendererXeonSingle = 0;
static EmbreeViewportRendererXeonPhiSingle  *g_EmbreeViewportRendererXeonPhiSingle = 0;
static EmbreeViewportRendererXeonISPC       *g_EmbreeViewportRendererXeonISPC = 0;
static EmbreeViewportRendererXeonPhiISPC    *g_EmbreeViewportRendererXeonPhiISPC = 0;
static EmbreeViewportRendererHybridISPC     *g_EmbreeViewportRendererHybridISPC = 0;

MStatus initializePlugin( MObject obj )
{
	MStatus   status = MStatus::kFailure;

#ifdef SINGLEXEON
	//
	// Register the Intel(R) Xeon(R) Processor Single-ray renderer
	//
    MFnPlugin plugin( obj, "IntelSX", "2014", "Any");
	g_EmbreeViewportRendererXeonSingle = new EmbreeViewportRendererXeonSingle();
	if (g_EmbreeViewportRendererXeonSingle)
	{
		g_EmbreeViewportRendererXeonSingle->createRenderDevice();
		status = g_EmbreeViewportRendererXeonSingle->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register Single-Ray Embree Renderer for Intel(R) Xeon(R) processors properly.");
		}
	}
#endif

#ifdef SINGLEXEONPHI
	//
	// Register the Intel(R) Xeon Phi(TM) coprocessor Single-ray renderer
	//
	MFnPlugin plugin( obj, "IntelSXP", "2014", "Any");
	g_EmbreeViewportRendererXeonPhiSingle = new EmbreeViewportRendererXeonPhiSingle();
	if (g_EmbreeViewportRendererXeonPhiSingle)
	{
		g_EmbreeViewportRendererXeonPhiSingle->createRenderDevice();
		status = g_EmbreeViewportRendererXeonPhiSingle->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register Single-Ray Embree Renderer for the Intel(R) Xeon Phi(TM) coprocessor properly.");
		}
	}
#endif
    
#ifdef ISPCXEON
	//
	// Register the Intel(R) Xeon(R) Processor ISPC renderer
	//
	MFnPlugin plugin( obj, "IntelIX", "2014", "Any");
	g_EmbreeViewportRendererXeonISPC = new EmbreeViewportRendererXeonISPC();
	if (g_EmbreeViewportRendererXeonISPC)
	{
		g_EmbreeViewportRendererXeonISPC->createRenderDevice();
		status = g_EmbreeViewportRendererXeonISPC->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register ISPC Embree Renderer for Intel(R) Xeon(R) processors properly.");
		}
	}
#endif

#ifdef ISPCXEONPHI
	//
	// Register the Intel(R) Xeon Phi(TM) coprocessor ISPC renderer
	//
	MFnPlugin plugin( obj, "IntelIXP", "2014", "Any");
	g_EmbreeViewportRendererXeonPhiISPC = new EmbreeViewportRendererXeonPhiISPC();
	if (g_EmbreeViewportRendererXeonPhiISPC)
	{
		g_EmbreeViewportRendererXeonPhiISPC->createRenderDevice();
		status = g_EmbreeViewportRendererXeonPhiISPC->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register ISPC Embree Renderer for the Intel(R) Xeon Phi(TM) coprocessor properly.");
		}
	}
#endif
    
#ifdef ISPCHYBRID
	//
	// Register the Hybrid ISPC renderer
	//
	MFnPlugin plugin( obj, "IntelIH", "2014", "Any");
	g_EmbreeViewportRendererHybridISPC = new EmbreeViewportRendererHybridISPC();
	if (g_EmbreeViewportRendererHybridISPC)
	{
		g_EmbreeViewportRendererHybridISPC->createRenderDevice();
		status = g_EmbreeViewportRendererHybridISPC->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register Hybrid ISPC Embree Renderer properly.");
		}
	}
#endif
	return status;
}

MStatus uninitializePlugin( MObject obj )
{
	MStatus   status = MStatus::kSuccess;

	MFnPlugin plugin( obj );

	//
	// Deregister the Intel(R) Xeon(R) Processor Single-ray renderer
	//
	if (g_EmbreeViewportRendererXeonSingle)
	{
		status = g_EmbreeViewportRendererXeonSingle->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister Single-Ray Embree Renderer for Intel(R) Xeon(R) processors properly.");
		}
	}
	g_EmbreeViewportRendererXeonSingle = 0;

	//
	// Deregister the Intel(R) Xeon Phi(TM) coprocessor Single-ray renderer
	//
	if (g_EmbreeViewportRendererXeonPhiSingle)
	{
		status = g_EmbreeViewportRendererXeonPhiSingle->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister Single-Ray Embree Renderer for the Intel(R) Xeon Phi(TM) coprocessor properly.");
		}
	}
	g_EmbreeViewportRendererXeonPhiSingle = 0;
    
	//
	// Deregister the Intel(R) Xeon(R) Processor ISPC renderer
	//
	if (g_EmbreeViewportRendererXeonISPC)
	{
		status = g_EmbreeViewportRendererXeonISPC->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister ISPC Embree Renderer for Intel(R) Xeon(R) processors properly.");
		}
	}
	g_EmbreeViewportRendererXeonISPC = 0;
    
	//
	// Deregister the Intel(R) Xeon Phi(TM) coprocessor ISPC renderer
	//
	if (g_EmbreeViewportRendererXeonPhiISPC)
	{
		status = g_EmbreeViewportRendererXeonPhiISPC->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister ISPC Embree Renderer for the Intel(R) Xeon Phi(TM) coprocessor properly.");
		}
	}
	g_EmbreeViewportRendererXeonPhiISPC = 0;
    
	//
	// Deregister the Hybrid ISPC renderer
	//
	if (g_EmbreeViewportRendererHybridISPC)
	{
		status = g_EmbreeViewportRendererHybridISPC->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister Hybrid ISPC Embree Renderer properly.");
		}
	}
	g_EmbreeViewportRendererHybridISPC = 0;
    
	return status;
}

