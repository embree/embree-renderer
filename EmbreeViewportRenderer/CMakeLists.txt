## ======================================================================== ##
## Copyright 2009-2014 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

FIND_PATH(MAYA_INCLUDE_PATH maya/MViewportRenderer.h /usr/autodesk/maya2014-x64/include)
FIND_PATH(MAYA_LIB_PATH libOpenMayalib.a /usr/autodesk/maya2014-x64/lib)
FIND_PATH(MAYA_PLUGIN_PATH linux_plugin.map /usr/autodesk/maya2014-x64/devkit/plug-ins)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${MAYA_INCLUDE_PATH} ../common ../devices)

LINK_DIRECTORIES(${MAYA_LIB_PATH})

#definitions seen in buildconfig in the top-level Maya devkit/plug-ins directory
add_definitions(-DREQUIRE_IOSTREAM -DAW_NEW_IOSTREAMS -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO)
add_definitions(-D_GNU_SOURCE -DLINUX_64)
add_definitions(-fPIC -fno-strict-aliasing -Wall)
add_definitions(-Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char)
add_definitions(-pthread -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords)

SET (CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic -shared -Wl,--version-script=${MAYA_PLUGIN_PATH}/linux_plugin.map")


SET(BUILD_SINGLE_MAYA_PLUGIN off CACHE BOOL "Enable to build Single-Ray Maya Plugin for Intel(r) Xeon(r)")
SET(BUILD_ISPC_MAYA_PLUGIN off CACHE BOOL "Enable to build ISPC Maya Plugin for Intel(r) Xeon(r)")
SET(BUILD_SINGLEPHI_MAYA_PLUGIN off CACHE BOOL "Enable to build Single-Ray Maya Plugin for Intel(r) Xeon Phi(tm)")
SET(BUILD_ISPCPHI_MAYA_PLUGIN off CACHE BOOL "Enable to build ISPC Maya Plugin for Intel(r) Xeon Phi(tm)")

ADD_SUBDIRECTORY(EmbreeViewportRendererSX)
ADD_SUBDIRECTORY(EmbreeViewportRendererIX)
ADD_SUBDIRECTORY(EmbreeViewportRendererSXP)
ADD_SUBDIRECTORY(EmbreeViewportRendererIXP)
