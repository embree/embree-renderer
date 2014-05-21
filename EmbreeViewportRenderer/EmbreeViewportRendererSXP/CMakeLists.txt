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

IF (BUILD_SINGLEPHI_MAYA_PLUGIN AND BUILD_SINGLERAY_DEVICE_XEON_PHI)
    add_definitions(-DSINGLEXEONPHI=1)
    add_library ( EmbreeViewportRendererSXP SHARED ../EmbreeViewportRenderer.cpp ../EmbreeViewportRendererPlugin.cpp)
    target_link_libraries (EmbreeViewportRendererSXP libOpenMayalib.a libmocap.a sys lexers loaders image device)
ENDIF()


