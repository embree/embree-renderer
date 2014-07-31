@ECHO ON&& SETLOCAL&& PUSHD "%~dp0"&& SETLOCAL ENABLEDELAYEDEXPANSION&& SETLOCAL ENABLEEXTENSIONS&& SET V=5&& IF NOT "!V!"=="5" (ECHO DelayedExpansion Failed&& GOTO :EOF)

REM
REM  ----- NOTE NOTE NOTE ----
REM  Be sure to create the "objs" and "ispcgen" directories in the Embree AND
REM  Embree sample renderer directories before running this script
REM
REM  Run this script in an "Intel Composer XE 2013 SP1 Intel(R) 64 Visual Studio 2010" command window

REM

SET CC=icl
REM SET ISPCDIR=C:\Users\cwcongdo\Documents\ispc-v1.6.0-windows
SET ISPCDIR=C:\Users\cwcongdo\Documents\ispc-v1.6dev-mic_win_cross
REM SET ISPC=%ISPCDIR%\ispc_wintophixcompile.exe
SET ISPC=%ISPCDIR%\ispc.exe

SET EMBREEROOT=C:\Users\cwcongdo\Documents\dcc_render-embree
SET EMBREERENDROOT=C:\Users\cwcongdo\Documents\dcc_render-embree-renderer
SET EOBJDIR=%EMBREEROOT%\objs
SET EKERNISPCDIR=%EMBREEROOT%\ispcgen

SET EMBREEINCLUDE=-I%EMBREEROOT% -I%EMBREEROOT%\common -I%EMBREEROOT%\include
SET EKERNELSDIR=%EMBREEROOT%\kernels
SET EKERNELSPHIDIR=%EMBREEROOT%\kernels\xeonphi
SET EKERNCOMDIR=%EMBREEROOT%\kernels\common

SET EMBREECFLAGS=/Qmic -c -g -debug inline-debug-info -D__SPINLOCKS__ -restrict -Wall -fasm-blocks -fPIC -O3 -DNDEBUG -mCG_lrb_num_threads=4 -mCG_lrb_num_threads=4 -fp-model fast -fimf-precision=low -fasm-blocks -no-inline-max-total-size -inline-factor=200 -fPIC  -fma  -restrict -no-prec-div -no-prec-sqrt -mGLOB_default_function_attrs="use_vec_for_imul=on;use_fast_math=on;gather_scatter_loop_jknzd=on;gather_scatter_loop_unroll=2;use_gather_scatter_hint=on;c_lrb_avoid_vector_insts_with_rip=on;c_avoid_bank_conflicts=on;c_sch_nop_insertion=on;c_avoid_movz_and_movs=off;c_avoid_integer_ciscization=on;avoid_stall_between_all_insts=on;avoid_long_vector_ints=on;avoid_loads_with_extend=on;smart_mem_conflicts=on" -mP2OPT_hlo_prefetch=F %EMBREEINCLUDE%

SET EMBREECFLAGSISPC=/Qmic -c -g -debug inline-debug-info -Dembree_xeonphi_EXPORTS -D__SPINLOCKS__ -D__TARGET_XEON_PHI__ -restrict -Wall -fasm-blocks -fPIC -O3 -DNDEBUG -mCG_lrb_num_threads=4 -mCG_lrb_num_threads=4 -fp-model fast -fimf-precision=low -fasm-blocks -no-inline-max-total-size -inline-factor=200 -fPIC  -fma  -restrict -no-prec-div -no-prec-sqrt -mGLOB_default_function_attrs="use_vec_for_imul=on;use_fast_math=on;gather_scatter_loop_jknzd=on;gather_scatter_loop_unroll=2;use_gather_scatter_hint=on;c_lrb_avoid_vector_insts_with_rip=on;c_avoid_bank_conflicts=on;c_sch_nop_insertion=on;c_avoid_movz_and_movs=off;c_avoid_integer_ciscization=on;avoid_stall_between_all_insts=on;avoid_long_vector_ints=on;avoid_loads_with_extend=on;smart_mem_conflicts=on" -mP2OPT_hlo_prefetch=F  %EMBREEINCLUDE%
		  
REM EMBREEISPCFLAGS = --arch=x86-64 --pic -O3 --target=generic-16 --emit-c++ --c++-include-file=$(ISPCDIR)\examples\intrinsics\knc.h --woff --opt=fast-math --opt=force-aligned-memory 
SET EMBREEISPCFLAGS=--arch=x86-64  -O1 --target=generic-16 --emit-c++ --c++-include-file=%ISPCDIR%\examples\intrinsics\knc.h --woff --opt=fast-math --opt=force-aligned-memory 
REM SET EMBREEISPCFLAGS=--arch=x86-64  -O1 --target=generic-16 --emit-c++ --c++-include-file=%ISPCDIR%\examples\intrinsics\knc-i1x16.h --woff --opt=fast-math --opt=force-aligned-memory 
SET EMBREEISPCINCLUDE=-I%EMBREEROOT% -I%EKERNELSPHIDIR%

REM
REM   -------------- Build process for the embree kernels --------------
REM

REM
REM   --- Embree kernel ISPC build
REM
FOR %%a IN (%EKERNCOMDIR%\*.ispc) DO %ISPC% %EMBREEISPCFLAGS% %EMBREEISPCINCLUDE% -h %EKERNISPCDIR%\%%~na_ispc.h -MMM %EKERNISPCDIR%\%%~na.dev.idep -o %EKERNISPCDIR%\%%~na.dev.cpp %EKERNCOMDIR%\%%~na.ispc

REM
REM   --- Embree kernel C++ file builds
REM 

set OLDDIR = %CD%
cd  %EOBJDIR%

%CC% %EKERNISPCDIR%\*.dev.cpp %EMBREECFLAGSISPC% -I%EKERNELSPHIDIR% -I%EKERNELSDIR% -I%EKERNISPCDIR%

%CC%  %EMBREEROOT%\common\sys\*.cpp %EMBREECFLAGS%
%CC%  %EMBREEROOT%\common\sys\sync\*.cpp %EMBREECFLAGS%
%CC%  %EMBREEROOT%\common\sys\stl\*.cpp %EMBREECFLAGS%
%CC%  %EMBREEROOT%\common\simd\mic*.cpp %EMBREECFLAGS%
%CC%  %EMBREEROOT%\kernels\common\*.cpp %EMBREECFLAGS% -I%EKERNELSDIR% -I%EKERNELSPHIDIR%
%CC%  %EMBREEROOT%\kernels\xeonphi\builders\*.cpp %EMBREECFLAGS% -I%EKERNELSDIR% -I%EKERNELSPHIDIR%
%CC%  %EMBREEROOT%\kernels\xeonphi\bvh4hair\*.cpp %EMBREECFLAGS% -I%EKERNELSDIR% -I%EKERNELSPHIDIR%
%CC%  %EMBREEROOT%\kernels\xeonphi\bvh4i\*.cpp %EMBREECFLAGS% -I%EKERNELSDIR% -I%EKERNELSPHIDIR%
%CC%  %EMBREEROOT%\kernels\xeonphi\bvh4mb\*.cpp %EMBREECFLAGS% -I%EKERNELSDIR% -I%EKERNELSPHIDIR%
%CC%  %EMBREEROOT%\kernels\xeonphi\geometry\*.cpp %EMBREECFLAGS% -I%EKERNELSDIR% -I%EKERNELSPHIDIR%

cd %OLDDIR%

REM
REM   -------------- Build process for the render devices --------------
REM

REM -- SET RENDINCLUDE=-I%EMBREERENDROOT% -I%EMBREERENDROOT%\common -I%EMBREERENDROOT%\include 

SET ROBJDIR=%EMBREERENDROOT%\objs
SET RISPCDIR=%EMBREERENDROOT%\ispcgen
SET RENDDEVDIR=%EMBREERENDROOT%\devices
SET RENDISPCDEVDIR=%RENDDEVDIR%\device_ispc
REM SET COIROOTDIR="C:\Program Files\Intel\MPSS\k1om-mpss-linux\usr\include\intel-coi"

SET RENDINCLUDE=-I%EMBREERENDROOT%\common -I%RENDDEVDIR% -I%EMBREEROOT%\include -I%EMBREERENDROOT%
SET RENDCOIDEVINCL=%RENDINCLUDE% -I%RENDDEVDIR%\device_single -I%RENDDEVDIR%\device_ispc -I"C:\Program Files\Intel\MPSS\k1om-mpss-linux\usr\include\intel-coi"
SET RENDSINGDEVINCL=%RENDINCLUDE% -I%RENDDEVDIR%\device_singleray
SET RENDISPCDEVINCL=%RENDINCLUDE% -I%RENDDEVDIR%\device_ispc -I%RISPCDIR%

SET RENDISPCINCLUDE=-I%RENDISPCDEVDIR% -I%EMBREEROOT%\include

SET RENDCFLAGS=/Qmic -c -restrict -Wall -fasm-blocks -fPIC -O3 -D__WINDOWS_MIC__ -DNDEBUG -mCG_lrb_num_threads=4 -mCG_lrb_num_threads=4 -fp-model fast -fimf-precision=low -fasm-blocks -no-inline-max-total-size -inline-factor=200 -fPIC  -fma  -restrict -no-prec-div -no-prec-sqrt -mGLOB_default_function_attrs="use_vec_for_imul=on;use_fast_math=on;gather_scatter_loop_jknzd=on;gather_scatter_loop_unroll=2;use_gather_scatter_hint=on;c_lrb_avoid_vector_insts_with_rip=on;c_avoid_bank_conflicts=on;c_sch_nop_insertion=on;c_avoid_movz_and_movs=off;c_avoid_integer_ciscization=on;avoid_stall_between_all_insts=on;avoid_long_vector_ints=on;avoid_loads_with_extend=on;smart_mem_conflicts=on" -mP2OPT_hlo_prefetch=F
SET RENDCFLAGSISPC=/Qmic -c -restrict -Wall -fasm-blocks -fPIC -O3 -DNDEBUG -mCG_lrb_num_threads=4 -mCG_lrb_num_threads=4 -fp-model fast -fimf-precision=low -fasm-blocks -no-inline-max-total-size -inline-factor=200 -fPIC  -fma  -restrict -no-prec-div -no-prec-sqrt -mGLOB_default_function_attrs="use_vec_for_imul=on;use_fast_math=on;gather_scatter_loop_jknzd=on;gather_scatter_loop_unroll=2;use_gather_scatter_hint=on;c_lrb_avoid_vector_insts_with_rip=on;c_avoid_bank_conflicts=on;c_sch_nop_insertion=on;c_avoid_movz_and_movs=off;c_avoid_integer_ciscization=on;avoid_stall_between_all_insts=on;avoid_long_vector_ints=on;avoid_loads_with_extend=on;smart_mem_conflicts=on"  -mP2OPT_hlo_prefetch=F  %RENDINCLUDE% -I%RENDDEVDIR% -I%RENDISPCDEVDIR% -I%RISPCDIR%
			   
SET RENDISPCFLAGS=-D__MIC__ --arch=x86-64  -O1 --target=generic-16 --emit-c++ --c++-include-file=%ISPCDIR%\examples\intrinsics\knc.h --wno-perf --opt=fast-math --opt=force-aligned-memory 
REM SET RENDISPCFLAGS=-D__MIC__ --arch=x86-64  -O1 --target=generic-16 --emit-c++ --c++-include-file=%ISPCDIR%\examples\intrinsics\knc-i1x16.h --wno-perf --opt=fast-math --opt=force-aligned-memory 

REM
REM   --- Renderer ISPC builds
REM

FOR /R %RENDISPCDEVDIR% %%a IN (*.ispc) DO %ISPC% %RENDISPCFLAGS% %RENDISPCINCLUDE% -h %RISPCDIR%\%%~na_ispc.h -MMM %RISPCDIR%\%%~na.dev.idep -o %RISPCDIR%\%%~na.dev.cpp %%a

REM
REM   --- Renderer C++ file builds
REM

cd %RISPCDIR%

DEL framebuffer_rgb8.dev.cpp
%CC% *.dev.cpp %RENDCFLAGSISPC%

cd %ROBJDIR%

%CC% %EMBREERENDROOT%\common\simd\mic*.cpp %RENDCFLAGS% %RENDINCLUDE%
FOR /R %EMBREERENDROOT%\common\sys %%a IN (*.cpp) DO %CC% %%a %RENDCFLAGS% %RENDINCLUDE%
%CC% %EMBREERENDROOT%\common\lexers\*.cpp %RENDCFLAGS% %RENDINCLUDE%

REM
REM -- COI server
REM
%CC% %EMBREERENDROOT%\devices\device_coi\coi_server.cpp  %RENDCFLAGS% %RENDCOIDEVINCL%

REM
REM -- The single ray device
REM
FOR /R %EMBREERENDROOT%\devices\device_singleray %%a IN (*.cpp) DO %CC% %%a %RENDCFLAGS% %RENDSINGDEVINCL%

REM
REM -- CPP portions of ISPC device
REM
FOR /R %EMBREERENDROOT%\devices\device_ispc %%a IN (*.cpp) DO %CC% %%a %RENDCFLAGS% %RENDISPCDEVINCL%

cd %ROBJDIR%

set PATH=C:\Program Files (x86)\Intel\Composer XE 2013 SP1\bin\intel64_mic;%PATH%

REM
REM  -- Build single-ray device for KNC
REM

"C:\Program Files (x86)\Intel\Composer XE 2013 SP1\bin\intel64_mic\icc.exe" -static-intel -rdynamic -fPIC -fPIC -o device_singleray_knc singleray_device.o hdrilight.o trianglemesh_normals.o trianglemesh_full.o sampler.o distribution1d.o distribution2d.o pathtraceintegrator.o filter.o debugrenderer.o integratorrenderer.o progress.o coi_server.o %EOBJDIR%\*.o -lpthread -ldl "C:\Program Files\Intel\MPSS\k1om-mpss-linux\usr\lib64\libcoi_device.so" -Wl,-rpath,"C:\Program Files\Intel\MPSS\k1om-mpss-linux\usr\lib64"  -zmuldefs

REM
REM  -- Build ISPC device for KNC
REM

"C:\Program Files (x86)\Intel\Composer XE 2013 SP1\bin\intel64_mic\icc.exe" -static-intel -rdynamic -fPIC -fPIC -o device_ispc_knc ispc_device.o trianglemesh.o sphere.o  coi_server.o %RISPCDIR%\*.o %EOBJDIR%\*.o -lpthread -ldl "C:\Program Files\Intel\MPSS\k1om-mpss-linux\usr\lib64\libcoi_device.so" -Wl,-rpath,"C:\Program Files\Intel\MPSS\k1om-mpss-linux\usr\lib64"  -zmuldefs

EXIT /B

