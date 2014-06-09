#-
# ==========================================================================
# Copyright (c) 2011 Autodesk, Inc.
# All rights reserved.
# 
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information written by Autodesk, Inc., and are
# protected by Federal copyright law. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without
# the prior written consent of Autodesk, Inc.
# Modifications by Charles Congdon of Intel for the Embree plugin 05/29/14
# ==========================================================================
#+

ifndef INCL_BUILDRULES

#
# Include platform specific build settings
#
MAYAPLUGINS := /usr/autodesk/maya2014-x64/devkit/plug-ins/
MAYAINCLUDE := /usr/autodesk/maya2014-x64/include
MAYALIBS := /usr/autodesk/maya2014-x64/lib
EMBREEROOT := /nfs/pdx/home/cwcongdo/Programming/dcc_render-embree
RENDERROOT := /nfs/pdx/home/cwcongdo/Programming/dcc_render-dcc_render-ccongdon-embree-renderer
INTELCOMPOSERXEROOT := /opt/intel/composer_xe_2013_sp1.3.174
INTELCOMPILERLIBS := $(INTELCOMPOSERXEROOT)/compiler/lib/intel64
TOP := ..
include buildrules


#
# Always build the local plug-in when make is invoked from the
# directory.
#
all : plugins

endif

#
# Variable definitions
#

SRCDIR := $(TOP)/EmbreeViewportRenderer
DSTDIR := $(TOP)/build

EmbreeViewportRenderer_SOURCES  := $(SRCDIR)/EmbreeViewportRenderer.cpp $(SRCDIR)/EmbreeViewportRendererPlugin.cpp
EmbreeViewportRenderer_OBJECTS  := $(DSTDIR)/EmbreeViewportRenderer.o $(DSTDIR)/EmbreeViewportRendererPluginSX.o $(DSTDIR)/EmbreeViewportRendererPluginSXP.o $(DSTDIR)/EmbreeViewportRendererPluginIX.o $(DSTDIR)/EmbreeViewportRendererPluginIXP.o
EmbreeViewportRendererSX_PLUGIN   := $(DSTDIR)/EmbreeViewportRendererSX.$(EXT)
EmbreeViewportRendererSXP_PLUGIN   := $(DSTDIR)/EmbreeViewportRendererSXP.$(EXT)
EmbreeViewportRendererIX_PLUGIN   := $(DSTDIR)/EmbreeViewportRendererIX.$(EXT)
EmbreeViewportRendererIXP_PLUGIN   := $(DSTDIR)/EmbreeViewportRendererIXP.$(EXT)
EmbreeViewportRenderer_MAKEFILE := $(SRCDIR)/Makefile

#
# Include the optional per-plugin Makefile.inc
#
#    The file can contain macro definitions such as:
#       {pluginName}_EXTRA_CFLAGS
#       {pluginName}_EXTRA_C++FLAGS
#       {pluginName}_EXTRA_INCLUDES
#       {pluginName}_EXTRA_LIBS
-include $(SRCDIR)/Makefile.inc


#
# Set target specific flags.
#
#CFLAGS   := $(CFLAGS) -g -debug inline-debug-info
CFLAGS   := $(CFLAGS) -g 
#$(EmbreeViewportRendererSX_PLUGIN): CFLAGS   := $(CFLAGS) -DSINGLEXEON  $(EmbreeViewportRenderer_EXTRA_CFLAGS)
#$(EmbreeViewportRendererSX_PLUGIN): C++FLAGS := $(C++FLAGS) -DSINGLEXEON $(EmbreeViewportRenderer_EXTRA_C++FLAGS)
#$(EmbreeViewportRendererSX_PLUGIN): CFLAGS   := $(CFLAGS) -DISPCXEON  $(EmbreeViewportRenderer_EXTRA_CFLAGS)
#$(EmbreeViewportRendererSX_PLUGIN): C++FLAGS := $(C++FLAGS) -DISPCXEON $(EmbreeViewportRenderer_EXTRA_C++FLAGS)
#$(EmbreeViewportRenderer_OBJECTS): INCLUDES := -I$(EMBREEROOT) -I$(EMBREEROOT)/include -I$(EMBREEROOT)/common -I$(RENDERROOT) -I$(RENDERROOT)/common -I$(RENDERROOT)/devices $(INCLUDES) $(EmbreeViewportRenderer_EXTRA_INCLUDES)
$(EmbreeViewportRenderer_OBJECTS): INCLUDES := -I$(RENDERROOT) -I$(RENDERROOT)/common -I$(EMBREEROOT) -I$(RENDERROOT)/devices $(INCLUDES) $(EmbreeViewportRenderer_EXTRA_INCLUDES)

depend_EmbreeViewportRenderer:     INCLUDES := $(INCLUDES) $(EmbreeViewportRenderer_EXTRA_INCLUDES)

LFLAGS   := $(LFLAGS) -L$(DSTDIR) -L$(MAYALIBS) $(EmbreeViewportRenderer_EXTRA_LFLAGS) 
LIBS     := $(LIBS)   -lOpenMayaRender -lOpenMaya -lOpenMayaUI -lFoundation $(DSTDIR)/libsys.a $(DSTDIR)/liblexers.a $(DSTDIR)/libloaders.a $(DSTDIR)/libimage.a $(DSTDIR)/libdevice.a $(DSTDIR)/liblexers.a $(DSTDIR)/libsys.a -lpthread -ldl $(EmbreeViewportRenderer_EXTRA_LIBS) 
# If you build the sample renderer code with ICC, you need to link against the following libraries
LIBS     := $(LIBS) $(INTELCOMPILERLIBS)/libimf.a $(INTELCOMPILERLIBS)/libirc.a

#
# Rules definitions
#

.PHONY: depend_EmbreeViewportRenderer clean_EmbreeViewportRenderer Clean_EmbreeViewportRenderer


$(EmbreeViewportRendererSX_PLUGIN): $(DSTDIR)/EmbreeViewportRenderer.o $(DSTDIR)/EmbreeViewportRendererPluginSX.o 
	-rm -f $@
	$(LD) -o $@ $(LFLAGS) $^ $(LIBS)
	
$(EmbreeViewportRendererIX_PLUGIN): $(DSTDIR)/EmbreeViewportRenderer.o $(DSTDIR)/EmbreeViewportRendererPluginIX.o 
	-rm -f $@
	$(LD) -o $@ $(LFLAGS) $^ $(LIBS)

$(DSTDIR)/EmbreeViewportRenderer.o :  $(SRCDIR)/EmbreeViewportRenderer.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@

$(DSTDIR)/EmbreeViewportRendererPluginSX.o :  $(SRCDIR)/EmbreeViewportRendererPlugin.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) -DSINGLEXEON $< -o $@
	
$(DSTDIR)/EmbreeViewportRendererPluginIX.o :  $(SRCDIR)/EmbreeViewportRendererPlugin.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) -DISPCXEON $< -o $@

depend_EmbreeViewportRenderer :
	makedepend $(INCLUDES) $(MDFLAGS) -f$(DSTDIR)/Makefile $(EmbreeViewportRenderer_SOURCES)

clean_EmbreeViewportRenderer:
	-rm -f $(EmbreeViewportRenderer_OBJECTS)

Clean_EmbreeViewportRenderer:
	-rm -f $(EmbreeViewportRenderer_MAKEFILE).bak $(EmbreeViewportRenderer_OBJECTS) $(EmbreeViewportRendererSX_PLUGIN) $(EmbreeViewportRendererSXP_PLUGIN) $(EmbreeViewportRendererIXP_PLUGIN) $(EmbreeViewportRendererIXP_PLUGIN)


plugins: $(EmbreeViewportRendererSX_PLUGIN) $(EmbreeViewportRendererIX_PLUGIN)
depend:	 depend_EmbreeViewportRenderer
clean:	 clean_EmbreeViewportRenderer
Clean:	 Clean_EmbreeViewportRenderer

