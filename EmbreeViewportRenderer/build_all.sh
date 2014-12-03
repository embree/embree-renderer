#!/bin/csh -x
#
# ======================================================================== //
# Copyright 2009-2014 Intel Corporation                                    //
#                                                                          //
# Licensed under the Apache License, Version 2.0 (the "License");          //
# you may not use this file except in compliance with the License.         //
# You may obtain a copy of the License at                                  //
#                                                                          //
#     http://www.apache.org/licenses/LICENSE-2.0                           //
#                                                                          //
# Unless required by applicable law or agreed to in writing, software      //
# distributed under the License is distributed on an "AS IS" BASIS,        //
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
# See the License for the specific language governing permissions and      //
# limitations under the License.                                           //
# ======================================================================== //
#
set MAYAPLUGINS = /usr/autodesk/maya2014-x64/devkit/plug-ins/
set MAYAINCLUDE = /usr/autodesk/maya2014-x64/include
set MAYALIBS = /usr/autodesk/maya2014-x64/lib
set EMBREEROOT = /nfs/pdx/home/cwcongdo/Programming/dcc_render-embree
set RENDERROOT = /nfs/pdx/home/cwcongdo/Programming/dcc_render-dcc_render-ccongdon-embree-renderer
set INTELCOMPOSERXEROOT = /opt/intel/composer_xe_2015.0.090
# set INTELCOMPOSERXEROOT = /opt/intel/composer_xe_2013_sp1.3.174
set INTELCOMPILERLIBS = $INTELCOMPOSERXEROOT/compiler/lib/intel64

echo ""
echo ""

cc -c -I$RENDERROOT -I$RENDERROOT/common -I$EMBREEROOT -I$RENDERROOT/devices -I../EmbreeViewportRenderer -I$MAYAINCLUDE -I/usr/X11R6/include  -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g  ../EmbreeViewportRenderer/EmbreeViewportRenderer.cpp -o ../build/EmbreeViewportRenderer.o

echo ""
echo " --------- Build the single-ray viewport renderer for Intel(r) Xeon(r) plugin"
echo ""

cc -c -I$RENDERROOT -I$RENDERROOT/common -I$EMBREEROOT -I$RENDERROOT/devices -I../EmbreeViewportRenderer -I$MAYAINCLUDE -I/usr/X11R6/include  -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g  -DSINGLEXEON ../EmbreeViewportRenderer/EmbreeViewportRendererPlugin.cpp -o ../build/EmbreeViewportRendererPluginSX.o

rm -f ../build/EmbreeViewportRendererSX.so

cc  -o ../build/EmbreeViewportRendererSX.so -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g   -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords -Wl,-Bsymbolic -shared -Wl,--version-script=$MAYAPLUGINS/linux_plugin.map -L../build -L$MAYALIBS   ../build/EmbreeViewportRenderer.o ../build/EmbreeViewportRendererPluginSX.o -L$MAYALIBS   -lOpenMayaRender -lOpenMaya -lOpenMayaUI -lFoundation ../build/libsys.a ../build/liblexers.a ../build/libloaders.a ../build/libimage.a ../build/libdevice.a ../build/liblexers.a ../build/libsys.a -lpthread -ldl   $INTELCOMPILERLIBS/libimf.a $INTELCOMPILERLIBS/libirc.a

echo ""
echo " --------- Build the single-ray viewport renderer for Intel(r) Xeon Phi(tm) plugin"
echo ""

cc -c -I$RENDERROOT -I$RENDERROOT/common -I$EMBREEROOT -I$RENDERROOT/devices -I../EmbreeViewportRenderer -I$MAYAINCLUDE -I/usr/X11R6/include  -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g  -DSINGLEXEONPHI ../EmbreeViewportRenderer/EmbreeViewportRendererPlugin.cpp -o ../build/EmbreeViewportRendererPluginSXP.o

rm -f ../build/EmbreeViewportRendererSXP.so

cc  -o ../build/EmbreeViewportRendererSXP.so -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g   -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords -Wl,-Bsymbolic -shared -Wl,--version-script=$MAYAPLUGINS/linux_plugin.map -L../build -L$MAYALIBS   ../build/EmbreeViewportRenderer.o ../build/EmbreeViewportRendererPluginSXP.o -L$MAYALIBS   -lOpenMayaRender -lOpenMaya -lOpenMayaUI -lFoundation ../build/libsys.a ../build/liblexers.a ../build/libloaders.a ../build/libimage.a ../build/libdevice.a ../build/liblexers.a ../build/libsys.a -lpthread -ldl  $INTELCOMPILERLIBS/libimf.a $INTELCOMPILERLIBS/libirc.a 

echo ""
echo " --------- Build the ISPC viewport renderer for Intel(r) Xeon(r) plugin"
echo ""

cc -c -I$RENDERROOT -I$RENDERROOT/common -I$EMBREEROOT -I$RENDERROOT/devices -I../EmbreeViewportRenderer -I$MAYAINCLUDE -I/usr/X11R6/include  -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g  -DISPCXEON ../EmbreeViewportRenderer/EmbreeViewportRendererPlugin.cpp -o ../build/EmbreeViewportRendererPluginIX.o

rm -f ../build/EmbreeViewportRendererIX.so

cc  -o ../build/EmbreeViewportRendererIX.so -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g   -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords -Wl,-Bsymbolic -shared -Wl,--version-script=$MAYAPLUGINS/linux_plugin.map -L../build -L$MAYALIBS   ../build/EmbreeViewportRenderer.o ../build/EmbreeViewportRendererPluginIX.o -L$MAYALIBS   -lOpenMayaRender -lOpenMaya -lOpenMayaUI -lFoundation ../build/libsys.a ../build/liblexers.a ../build/libloaders.a ../build/libimage.a ../build/libdevice.a ../build/liblexers.a ../build/libsys.a -lpthread -ldl  $INTELCOMPILERLIBS/libimf.a $INTELCOMPILERLIBS/libirc.a

echo ""
echo " --------- Build the ISPC viewport renderer for Intel(r) Xeon Phi(tm) plugin"
echo ""

cc -c -I$RENDERROOT -I$RENDERROOT/common -I$EMBREEROOT -I$RENDERROOT/devices -I../EmbreeViewportRenderer -I$MAYAINCLUDE -I/usr/X11R6/include  -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g  -DISPCXEONPHI ../EmbreeViewportRenderer/EmbreeViewportRendererPlugin.cpp -o ../build/EmbreeViewportRendererPluginIXP.o

rm -f ../build/EmbreeViewportRendererIXP.so

cc  -o ../build/EmbreeViewportRendererIXP.so -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g   -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords -Wl,-Bsymbolic -shared -Wl,--version-script=$MAYAPLUGINS/linux_plugin.map -L../build -L$MAYALIBS   ../build/EmbreeViewportRenderer.o ../build/EmbreeViewportRendererPluginIXP.o -L$MAYALIBS   -lOpenMayaRender -lOpenMaya -lOpenMayaUI -lFoundation ../build/libsys.a ../build/liblexers.a ../build/libloaders.a ../build/libimage.a ../build/libdevice.a ../build/liblexers.a ../build/libsys.a -lpthread -ldl  $INTELCOMPILERLIBS/libimf.a $INTELCOMPILERLIBS/libirc.a

echo ""
echo " --------- Build the hybrid ISPC viewport renderer"
echo ""

cc -c -I$RENDERROOT -I$RENDERROOT/common -I$EMBREEROOT -I$RENDERROOT/devices -I../EmbreeViewportRenderer -I$MAYAINCLUDE -I/usr/X11R6/include  -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g  -DISPCHYBRID ../EmbreeViewportRenderer/EmbreeViewportRendererPlugin.cpp -o ../build/EmbreeViewportRendererPluginIH.o

rm -f ../build/EmbreeViewportRendererIH.so

cc  -o ../build/EmbreeViewportRendererIH.so -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread -g   -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords -Wl,-Bsymbolic -shared -Wl,--version-script=$MAYAPLUGINS/linux_plugin.map -L../build -L$MAYALIBS   ../build/EmbreeViewportRenderer.o ../build/EmbreeViewportRendererPluginIH.o -L$MAYALIBS   -lOpenMayaRender -lOpenMaya -lOpenMayaUI -lFoundation ../build/libsys.a ../build/liblexers.a ../build/libloaders.a ../build/libimage.a ../build/libdevice.a ../build/liblexers.a ../build/libsys.a -lpthread -ldl  $INTELCOMPILERLIBS/libimf.a $INTELCOMPILERLIBS/libirc.a

echo ""
echo " --------- Build complete"
echo ""
