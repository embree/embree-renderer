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

PROJECT(embree)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
MARK_AS_ADVANCED(CMAKE_INSTALL_PREFIX)
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH)
MARK_AS_ADVANCED(CLEAR CMAKE_VERBOSE_MAKEFILE)

MARK_AS_ADVANCED(CMAKE_OSX_ARCHITECTURES)
MARK_AS_ADVANCED(CMAKE_OSX_DEPLOYMENT_TARGET)
MARK_AS_ADVANCED(CMAKE_OSX_SYSROOT)
MARK_AS_ADVANCED(GLUT_cocoa_LIBRARY)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/common/cmake ${CMAKE_MODULE_PATH})

##############################################################
# ISA configuration
##############################################################

SET(TARGET_SSE2  ON CACHE BOOL "Generate code path for SSE2")
SET(TARGET_SSE41 ON CACHE BOOL "Generate code path for SSE41")
SET(TARGET_AVX   ON CACHE BOOL "Generate code path for AVX")
SET(TARGET_AVX2  ON CACHE BOOL "Generate code path for AVX2")

SET(ISPC_TARGETS "")
IF (TARGET_SSE2 STREQUAL "ON")
  SET(ISPC_TARGETS ${ISPC_TARGETS} "sse2")
ENDIF()

IF (TARGET_SSE41 STREQUAL "ON")
  SET(ISPC_TARGETS ${ISPC_TARGETS} "sse4")
ENDIF()

IF (TARGET_AVX STREQUAL "ON")
  SET(ISPC_TARGETS ${ISPC_TARGETS} "avx")
ENDIF()

IF (TARGET_AVX2 STREQUAL "ON")
  SET(ISPC_TARGETS ${ISPC_TARGETS} "avx2")
ENDIF()

SET(ISPC_TARGETS "${ISPC_TARGETS}")
STRING(REGEX REPLACE ";" "," ISPC_TARGETS "${ISPC_TARGETS}")

##############################################################
# Compiler
##############################################################

SET(COMPILER "GCC" CACHE STRING "Set to GCC, CLANG, or ICC")

IF (COMPILER STREQUAL "GCC") 
  INCLUDE (gcc)
ELSEIF (COMPILER STREQUAL "CLANG") 
  INCLUDE (clang)
ELSEIF (COMPILER STREQUAL "ICC") 
  INCLUDE (icc)
ELSE ()
  MESSAGE(FATAL_ERROR "Unknown compiler specified: " ${COMPILER})
ENDIF ()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type: Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_VERBOSE_MAKEFILE false CACHE BOOL "Enables verbose mode.")

INCLUDE (ispc)

##############################################################
# search paths
##############################################################

LINK_DIRECTORIES(${EXECUTABLE_OUTPUT_PATH})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/common)

##############################################################
# output paths
##############################################################

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

##############################################################
# find Embree RTCore
##############################################################

FIND_PACKAGE(embree)
IF(EMBREE_FOUND)
  INCLUDE_DIRECTORIES(${EMBREE_INCLUDE_PATH})
  INCLUDE_DIRECTORIES_ISPC(${EMBREE_INCLUDE_PATH})
ENDIF(EMBREE_FOUND)

##############################################################
# directories to compile
##############################################################

SET(TARGET_CPU "xeon")
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(devices)
ADD_SUBDIRECTORY(tools/obj2xml)
ADD_SUBDIRECTORY(tools/xml2obj)
 
