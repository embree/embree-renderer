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
//
// Plug-in functions
//
///////////////////////////////////////////////////

static EmbreeViewportRendererXeonSingle		*g_EmbreeViewportRendererXeonSingle = 0;
static EmbreeViewportRendererXeonISPC		*g_EmbreeViewportRendererXeonISPC = 0;
static EmbreeViewportRendererXeonPhiSingle	*g_EmbreeViewportRendererXeonPhiSingle = 0;
static EmbreeViewportRendererXeonPhiISPC	*g_EmbreeViewportRendererXeonPhiISPC = 0;

MStatus initializePlugin( MObject obj )
{
	MStatus   status = MStatus::kFailure;

#ifdef SINGLEXEON
	//
	// Register the Xeon Single-ray renderer
	//
        MFnPlugin plugin( obj, "IntelSX", "2014", "Any");
	g_EmbreeViewportRendererXeonSingle = new EmbreeViewportRendererXeonSingle();
	if (g_EmbreeViewportRendererXeonSingle)
	{
		g_EmbreeViewportRendererXeonSingle->createRenderDevice();
		status = g_EmbreeViewportRendererXeonSingle->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register Single-Ray Embree Renderer for Xeon properly.");
		}
	}
#endif

#ifdef ISPCXEON
	//
	// Register the Xeon ISPC renderer
	//
	MFnPlugin plugin( obj, "IntelIX", "2014", "Any");
	g_EmbreeViewportRendererXeonISPC = new EmbreeViewportRendererXeonISPC();
	if (g_EmbreeViewportRendererXeonISPC)
	{
		g_EmbreeViewportRendererXeonISPC->createRenderDevice();
		status = g_EmbreeViewportRendererXeonISPC->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register ISPC Embree Renderer for Xeon properly.");
		}
	}
#endif

#ifdef SINGLEXEONPHI
	//
	// Register the Xeon Phi Single-ray renderer
	//
	MFnPlugin plugin( obj, "IntelSXP", "2014", "Any");
	g_EmbreeViewportRendererXeonPhiSingle = new EmbreeViewportRendererXeonPhiSingle();
	if (g_EmbreeViewportRendererXeonPhiSingle)
	{
		g_EmbreeViewportRendererXeonPhiSingle->createRenderDevice();
		status = g_EmbreeViewportRendererXeonPhiSingle->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register Single-Ray Embree Renderer for Xeon Phi properly.");
		}
	}
#endif

#ifdef ISPCXEONPHI
	//
	// Register the Xeon Phi ISPC renderer
	//
	MFnPlugin plugin( obj, "IntelIXP", "2014", "Any");
	g_EmbreeViewportRendererXeonPhiISPC = new EmbreeViewportRendererXeonPhiISPC();
	if (g_EmbreeViewportRendererXeonPhiISPC)
	{
		g_EmbreeViewportRendererXeonPhiISPC->createRenderDevice();
		status = g_EmbreeViewportRendererXeonPhiISPC->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register ISPC Embree Renderer for Xeon Phi properly.");
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
	// Deregister the Xeon Single-ray renderer
	//
	if (g_EmbreeViewportRendererXeonSingle)
	{
		status = g_EmbreeViewportRendererXeonSingle->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister Single-Ray Embree Renderer for Xeon properly.");
		}
	}
	g_EmbreeViewportRendererXeonSingle = 0;

	//
	// Deregister the Xeon ISPC renderer
	//
	if (g_EmbreeViewportRendererXeonISPC)
	{
		status = g_EmbreeViewportRendererXeonISPC->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister ISPC Embree Renderer for Xeon properly.");
		}
	}
	g_EmbreeViewportRendererXeonISPC = 0;

	//
	// Deregister the Xeon Phi Single-ray renderer
	//
	if (g_EmbreeViewportRendererXeonPhiSingle)
	{
		status = g_EmbreeViewportRendererXeonPhiSingle->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister Single-Ray Embree Renderer for Xeon Phi properly.");
		}
	}
	g_EmbreeViewportRendererXeonPhiSingle = 0;

	//
	// Deregister the Xeon Phi ISPC renderer
	//
	if (g_EmbreeViewportRendererXeonPhiISPC)
	{
		status = g_EmbreeViewportRendererXeonPhiISPC->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister ISPC Embree Renderer for Xeon Phi properly.");
		}
	}
	g_EmbreeViewportRendererXeonPhiISPC = 0;

	return status;
}

