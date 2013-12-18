// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
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

The Embree Example Renderer is a photo-realistic renderer that builds
on Embree, a collection of high performance ray tracing kernels. The
renderer is used to demonstrate how Embree is used in practice and to
measure Embree's performance in a realistic application scenario. The
Embree Example Renderer is not a full featured renderer and not
designed to be used for production renderering.

The Embree Example Renderer is released as Open Source under the
Apache 2.0 license.

--- Supported Platforms ---

The Embree Example Renderer runs on Windows, Linux and MacOSX, each in
32bit and 64bit modes. The code compiles with the Intel Compiler, the
Microsoft Compiler and with GCC.

The Xeon Phi version of the Embree Example Renderer only works under
Linux in 64bit mode. For compilation of the the Xeon Phi code the
Intel Compiler is required. The host side code compiles with GCC and
the Intel Compiler.

--- Compiling the Embree Example Render on Linux and MacOSX ---

For compilation under Linux and MacOSX you have to install Embree,
CMake (for compilation) the developer version of GLUT (for display)
and we recommend installing the ImageMagick and OpenEXR developer
packages (for reading and writing images).

The Embree ray tracing kernels can be downloaded from
embree.github.git. Follow the install instructions in the README.txt
to install Embree. We recommend properly installing Embree kernels on
your machine using "sudo make install".

Under MaxOSX, the remaining dependencies can be installed using MacPorts:

   sudo port install cmake freeglut openexr ImageMagick

Under Linux you can install the dependencies using yum:

   sudo yum install cmake.x86_64
   sudo yum install freeglut.x86_64 freeglut-devel.x86_64
   sudo yum install libXmu.x86_64 libXi.x86_64 libXmu-devel.x86_64 libXi-devel.x86_64
   sudo yum install OpenEXR.x86_64 OpenEXR-devel.x86_64
   sudo yum install ImageMagick.x86_64 ImageMagick-c++.x86_64 ImageMagick-devel.x86_64 ImageMagick-c++-devel.x86_64 

To compile the Embree Example Renderer using CMake create a build
directory and execute ccmake .. inside this directory.

   mkdir build
   cd build
   ccmake ..

This will open a configuration dialog where you should set the build
mode to “Release” and the compiler to either GCC or ICC. You can
configure which parts of the Embree Example Renderer to build:

   BUILD_SINGLE_RAY_DEVICE     : Single ray device for CPU operating
                                 on single rays.
   BUILD_SINGLE_RAY_DEVICE_XEON_PHI : Single ray device for Xeon Phi 
                                 operating on single rays.
   BUILD_ISPC_DEVICE           : ISPC CPU device operating on ray
                                 packets of size 4 (SSE) or 8 (AVX).
   BUILD_ISPC_DEVICE_XEON_PHI  : ISPC device for Xeon Phi accelerator
                                 operating on ray packets of size 16
   BUILD_NETWORK_DEVICE        : Network device to render on render server

When enabling any ISPC renderer, you also have to install ISPC (see
below). If you select BUILD_ISPC_DEVICE, you should select which
instructions sets to enable for ISPC:

   TARGET_SSE2   : generate SSE2 code path for ISPC code
   TARGET_SSE41  : generate SSE4.1 code path for ISPC code
   TARGET_AVX    : generate AVX code path for ISPC code
   TARGET_AVX2   : generate AVX2 code path for ISPC code

All target ISAs you select when compiling the Embree Example Render,
have also to be enabled when compiling Embree. Due to some limitation
of ISPC you have to enable more than one target ISA if you also
enabled more than one target ISA when compiling Embree.

If you installed Embree, CMake will find it automatically and set
the EMBREE_INCLUDE_PATH and EMBREE_LIBRARY variables.

If you do not want to install Embree on your system some additional
configurations are required to use Embree from its build folder. Set
the EMBREE_INCLUDE_PATH to the embree_directory/include folder and the
EMBREE_LIBRARY to embree_directory/build/libembree.2.1.0.dylib for
MacOSX or embree_directory/build/libembree.so.2.1.0 for Linux. Under
Linux you have to additionally add embree_directory/build to your
LD_LIBRARY_PATH. This last step is not required under MacOSX.

Press c (for configure) and g (for generate) to generate a Makefile
and leave the configuration. The code can now be compiled by executing
make. The executables will be generated in the build folder.

      make

--- Compiling Embree on Windows ---

For compilation under Windows you first have to install the Embree ray
tracing kernels from embree.github.git. Follow the install
instructions in the README.txt to install Embree. After installation
you have to set the EMBREE_INSTALL_DIR environment variable to the
main folder of Embree.

Use the Visual Studio 2008 solution file to compile the Embree Example
Renderer. Inside Visual Studio you can switch between the Microsoft
Compiler and the Intel Compiler by right clicking on the solution and
then selecting the compiler. The project compiles with both compilers
in 32 bit and 64 bit mode. We recommend using 64 bit mode and the
Intel Compiler for best performance.

By default, the solution file requires ISPC to be installed properly
(see below). For compiling the solution without ISPC, simply delete
the device_ispc project from the solution file.

We recommend enabling syntax highlighting for the .ispc source 
and .isph header files. To do so open Visual Studio 2008, go to 
Tools -> Options -> Text Editor -> File Extension and add the isph
and ispc extension for the "Microsoft Visual C++" editor.

--- Installing ISPC ---

For the ISPC projects of Embree to work you have to install ISPC from
ispc.github.com. Best use ISPC v1.5.0 as we used that version for
testing. You can download precompiled ISPC binaries or compile ISPC
from sources. We recommend using the precompiled binaries. After
installing ISPC you have to put the ispc executable permanently into
your path:

  export PATH=path-to-ispc:$PATH

--- Running the Embree Example Renderer ---

This section describes how to run the Embree Example Renderer. Execute
renderer -help for a complete list of parameters. The renderer comes
with a few simple test scenes, each consisting of a scene file (.xml
or .obj) and a command script file (.ecs). The command script file
contains command line parameters that set the camera parameters,
lights and render settings. The following command line will render the
Cornell box scene with 16 samples per pixel and write the resulting
image to the file cb.tga in the current directory:

   renderer -c ../../models/cornell_box.ecs -spp 16 -o cb.tga

To interactively display the same scene, enter the following command:

   renderer -c ../../models/cornell_box.ecs

A window will open and you can control the camera using the mouse and
keyboard. Pressing c in interactive mode outputs the current camera
parameters, pressing r enables or disables the progressive refinement
mode.

By default the renderer uses the single ray device. For selecting a
different device use the -device command line parameter at the very 
beginning:

  renderer -device singleray -c ../../models/cornell_box.ecs
  renderer -device ispc -c ../../models/cornell_box.ecs

  renderer -device singleray_xeonphi -c ../../models/cornell_box.ecs
  renderer -device ispc_xeonphi -c ../../models/cornell_box.ecs

--- Using Embree renderer in network mode ---

For using the network device start the embree server on some machine:

  renderer_server

Make sure that port 8484 is not blocked by the firewall. Now you can 
connect from a second machine to the network server:

  renderer -connect ip_of_network_server -c ../../models/cornell_box.ecs
  
--- Navigation ---

The navigation in the interactive display mode follows the camera
orbit model, where the camera revolves around the current center of
interest. The camera navigation assumes the y-axis to point
upwards. If your scene is modelled using the z-axis as up axis we
recommend rotating the scene.

	LMB: Rotate around center of interest
	MMB: Pan
	RMB: Dolly (move camera closer or away from center of interest)
	Strg+LMB: Pick center of interest
	Strg+Shift+LMB: Pick focal distance
	Alt+LMB: Roll camera around view direction
	L: Decrease lens radius by one world space unit
	Shift+L: Increase lens radius by one world space unit

--- Contact ---

Please contact embree_support@intel.com if you have questions related to
Embree or if you want to report a bug.
