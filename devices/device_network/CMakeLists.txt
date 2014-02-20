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

ADD_LIBRARY(device_network SHARED network_device.cpp)
TARGET_LINK_LIBRARIES(device_network sys image)

IF (USE_IMAGE_MAGICK)
  ADD_DEFINITIONS(-DUSE_IMAGEMAGICK -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=8)
  INCLUDE_DIRECTORIES(${ImageMagick_Magick++_INCLUDE_DIR})
  SET(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} ${ImageMagick_Magick++_LIBRARY})
ENDIF()

ADD_EXECUTABLE(renderer_server network_server.cpp network_server_main.cpp)
TARGET_LINK_LIBRARIES(renderer_server lexers device loaders ${ADDITIONAL_LIBRARIES})

