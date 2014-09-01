## ======================================================================== ##
## Copyright 2009-2013 Intel Corporation                                    ##
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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET (SOURCES 
    api/singleray_device.cpp
    lights/hdrilight.cpp   
    shapes/trianglemesh_normals.cpp   
    shapes/trianglemesh_full.cpp       
    samplers/sampler.cpp
    samplers/distribution1d.cpp
    samplers/distribution2d.cpp
    integrators/pathtraceintegrator.cpp
    filters/filter.cpp
    renderers/debugrenderer.cpp
    renderers/integratorrenderer.cpp
    renderers/progress.cpp
    )

IF (__XEON__)
 
  IF (APPLE)
    SET(BUILD_SINGLERAY_DEVICE_TARGET "SSE2" CACHE STRING "ISA for single ray device (SSE2, SSSE3, SSE4.1, SSE4.2, AVX, AVX2)")
  ELSE()
    SET(BUILD_SINGLERAY_DEVICE_TARGET "SSSE3" CACHE STRING "ISA for single ray device (SSSE3, SSE4.1, SSE4.2, AVX, AVX2)")
  ENDIF()

  IF (BUILD_SINGLERAY_DEVICE_TARGET STREQUAL "SSE2")
    SET(SINGLE_RAY_FLAGS ${FLAGS_SSE2})
  ELSEIF(BUILD_SINGLERAY_DEVICE_TARGET STREQUAL "SSSE3")
    SET(SINGLE_RAY_FLAGS ${FLAGS_SSSE3})
  ELSEIF (BUILD_SINGLERAY_DEVICE_TARGET STREQUAL "SSE4.1")
    SET(SINGLE_RAY_FLAGS ${FLAGS_SSE41})
  ELSEIF (BUILD_SINGLERAY_DEVICE_TARGET STREQUAL "SSE4.2")
    SET(SINGLE_RAY_FLAGS ${FLAGS_SSE42})
  ELSEIF (BUILD_SINGLERAY_DEVICE_TARGET STREQUAL "AVX")
    SET(SINGLE_RAY_FLAGS ${FLAGS_AVX})
  ELSEIF (BUILD_SINGLERAY_DEVICE_TARGET STREQUAL "AVX2")
    SET(SINGLE_RAY_FLAGS ${FLAGS_AVX2})
  ELSE()
    MESSAGE(FATAL_ERROR "Invalid single ray target ${BUILD_SINGLERAY_DEVICE_TARGET}")
  ENDIF()

  ADD_LIBRARY(device_singleray SHARED ${SOURCES})
  SET_TARGET_PROPERTIES(device_singleray PROPERTIES COMPILE_FLAGS "${SINGLE_RAY_FLAGS}")
  TARGET_LINK_LIBRARIES(device_singleray sys ${EMBREE_LIBRARY} image)

ELSE ()
  INCLUDE(icc_mic)
  ADD_EXECUTABLE(device_singleray_knc ${SOURCES})
  TARGET_LINK_LIBRARIES(device_singleray_knc sys_knc ${EMBREE_LIBRARY_MIC} coi_server_knc)
ENDIF ()
