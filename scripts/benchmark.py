#!/usr/bin/python

## ======================================================================== ##
## Copyright 2009-2012 Intel Corporation                                    ##
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

# Embree Test Scripts
# ===================

# Windows
# -------

# Prerequisites:
#   Install Python 3.2+
#   Install Visual Studio 2010 or 2008
#   Install Intel C++ Compiler
#   Check out Embree into <embree_dir>
#   Download Embree test scenes from shared drive /Repo/ModelRepository/Embree/*.tgz and unpack in <model_dir>

# Instructions:
#   Open the "Visual Studio x64 Cross Tools Command Prompt (2010)"
#   cd <embree_dir>
#   mkdir TEST
#   <python_dir>\python.exe <embree_dir>\scripts\benchmark.py compile windows
#   <python_dir>\python.exe <embree_dir>\scripts\benchmark.py render windows <model_dir> TEST
#   <python_dir>\python.exe <embree_dir>\scripts\benchmark.py extract windows TEST
#   Check the generated output for "!" (Warning or worse), "Crash", "Error" and unusually low performance.

# Linux and OS X
# --------------

# Prerequisites:
#   Install Python 2.6+
#   Install Intel C++ Compiler
#   Check out Embree into <embree_dir>
#   Download Embree test scenes from shared drive /Repo/ModelRepository/Embree/*.tgz and unpack in <model_dir>

# Instructions:
#   Open a shell
#   cd <embree_dir>
#   mkdir TEST
#   ./scripts/benchmark.py compile unix
#   ./scripts/benchmark.py render unix <model_dir> TEST
#   ./scripts/benchmark.py extract unix TEST
#   Check the generated output for red text (Warning or worse), "Crash", "Error" and unusually low performance.

import sys
import os
import re

dash = '/'

########################## configuration ##########################

oss = ['windows', 'unix']

#compilers_win = ['V100']
compilers_win = ['ICC']
#compilers_win  = ['V100', 'ICC']
compilers_unix = ['ICC']
#compilers_unix = ['GCC', 'ICC']
#compilers_unix = ['GCC', 'CLANG', 'ICC']
compilers      = []

#builds = ['Debug']
builds = ['Release']
#builds = ['Release', 'Debug']
#builds = ['Release']
#builds = ['ReleaseAVX']
#builds = ['ReleaseAVX2']
#builds = ['ReleaseAVX2', 'ReleaseAVX', 'Release']
#builds = ['ReleaseAVX2', 'ReleaseAVX', 'Release', 'Debug']

#platforms_win  = ['win32']
platforms_win  = ['x64']
#platforms_win  = ['win32', 'x64']
platforms_unix = ['x64']
platforms      = []

devices = [ 'singleray', 'ispc' ]
scenes = ['conference']
#scenes = ['conference', 'crown', 'headlight', 'nightgown', 'powerplant']
#scenes = ['conference', 'courtyard', 'crown', 'e87', 'e89', 'e89_engine', 'headlight', 'loftcube', 'nightgown', 'powerplant', 'stanford', 'xyz_dragon']

#isas_win  = ['sse41']
#isas_win = ['sse3', 'sse41']
#isas_win = ['sse3', 'sse41', 'avx']
#isas_win = ['sse3', 'sse41', 'avx', 'avx2']
isas4_win = ['sse3', 'sse41']
isas8_win = ['avx']
#isas8_win = ['avx', 'avx2']
isas_win  = isas4_win + isas8_win

#isas4_unix = []
isas4_unix = ['sse3', 'sse41']
isas8_unix = ['avx']
isas16_unix = ['knc']
#isas8_unix = ['avx', 'avx2']
isas_unix  = isas4_unix + isas8_unix + isas16_unix
#isas_unix = ['sse3', 'sse41', 'avx', 'avx2']
isas4 = []
isas8 = []
isas  = []

modelDir  = ''
testDir = ''
embreeDirLinux = '~/Projects/embree_v21'
try:
  embreeDirWindows = os.environ['EMBREE_INSTALL_DIR']
except KeyError:
  embreeDirWindows = ""

########################## compiling ##########################

def compile(OS,compiler,platform,isas,build,simd):
  if OS == 'windows':
  
#   if simd == 8: build += AVX2'
    if simd == 8: build += 'AVX'

    cfg = '/p:Configuration=' + build + ';'
    cfg += 'Platform=' + platform + ';'
    cfg += 'PlatformToolset=';
#   if (compiler == 'ICC'): cfg += '"Intel C++ Compiler 12.1" '
#   if (compiler == 'ICC'): cfg += '"Intel C++ Compiler XE 12.1" '
    if (compiler == 'ICC'): cfg += '"Intel C++ Compiler XE 14.0" '
    elif (compiler == 'V100'): cfg += 'v100 '
    else: 
      sys.stderr.write('unknown compiler: ' + compiler + '\n')
      sys.exit(1)

    # first compile Embree
    command =  'msbuild ' + embreeDirWindows + '\\embree_vs2010.sln' + ' ' + cfg + ' /t:Clean'
    os.system(command)
    command =  'msbuild ' + embreeDirWindows + '\\embree_vs2010.sln' + ' ' + cfg
    os.system(command)
    command =  'msbuild ' + embreeDirWindows + '\\embree_vs2010.sln' + ' ' + cfg
    os.system(command)

    # now compile the Embree renderer      
    command =  'msbuild embree-renderer_vs2010.sln' + ' ' + cfg + ' /t:Clean'
    os.system(command)
    command =  'msbuild embree-renderer_vs2010.sln' + ' ' + cfg
    os.system(command)
    command =  'msbuild embree-renderer_vs2010.sln' + ' ' + cfg
    os.system(command)
  
    #command += '/t:rebuild /verbosity:n'
  else:

    if (platform != 'x64'):
      sys.stderr.write('unknown platform: ' + platform + '\n')
      sys.exit(1)

    if   (compiler == 'ICC'  ): compilerOption = ' -D COMPILER=ICC'
    elif (compiler == 'GCC'  ): compilerOption = ' -D COMPILER=GCC'
    elif (compiler == 'CLANG'): compilerOption = ' -D COMPILER=CLANG'
    else:
      sys.stderr.write('unknown compiler: ' + compiler + '\n')
      sys.exit(1)

    # first compile Embree
    command = 'pushd ' + embreeDirLinux + '; mkdir build; cd build; cmake'
    command += compilerOption
    command += ' -D RTCORE_BACKFACE_CULLING=OFF'
    command += ' -D RTCORE_ENABLE_RAY_MASK=OFF'
    command += ' -D RTCORE_ENABLE_SPINLOCKS=OFF'
    command += ' -D RTCORE_ENABLE_STAT_COUNTERS=OFF'
    command += ' -D RTCORE_ENABLE_TASKLOGGER=OFF'
    command += ' -D RTCORE_EXIT_ON_ERROR=ON'
    command += ' -D RTCORE_FIX_RAYS=OFF'

    if (compiler == 'ICC'):
      command += ' -D TARGET_SSE41=ON';
      command += ' -D TARGET_AVX=ON'
      command += ' -D TARGET_AVX2=ON'
    else:
      command += ' -D TARGET_SSE41=OFF'
      command += ' -D TARGET_AVX=OFF'
      command += ' -D TARGET_AVX2=OFF'

    if ('knc' in isas): command += ' -D TARGET_XEON_PHI=ON';
    else:               command += ' -D TARGET_XEON_PHI=OFF';

    command += ' -D CMAKE_BUILD_TYPE=' + build
    command += ' ..; make clean; make -j 8';
    os.system(command)

    # now compile Embree Renderer
    command = 'popd; mkdir build; cd build; cmake '

    command += ' -D TARGET_SSE2=ON'
    if (compiler == 'ICC'):
      command += ' -D TARGET_SSE41=ON'
      if (simd == 8):
        command += ' -D TARGET_AVX=ON'
        command += ' -D TARGET_AVX2=ON'
      else:
        command += ' -D TARGET_AVX=OFF'
        command += ' -D TARGET_AVX2=OFF'
    else:
      command += ' -D TARGET_SSE41=OFF'
      command += ' -D TARGET_AVX=OFF'
      command += ' -D TARGET_AVX2=OFF'

    if ('knc' in isas):
      command += ' -D BUILD_ISPC_DEVICE_XEON_PHI=ON';
      command += ' -D BUILD_SINGLERAY_DEVICE_XEON_PHI=ON';
    else:
      command += ' -D BUILD_ISPC_DEVICE_XEON_PHI=OFF';
      command += ' -D BUILD_SINGLERAY_DEVICE_XEON_PHI=OFF';

    command += ' -D BUILD_SINGLERAY_DEVICE=ON';
    command += ' -D BUILD_ISPC_DEVICE=ON';
    command += ' -D CMAKE_BUILD_TYPE=' + build
    command += ' ..; make clean; make -j 8';
    os.system(command)

def compileLoop(OS):
    for compiler in compilers:
      for platform in platforms:
        for build in builds:
          print(OS + ' ' + compiler + ' ' + platform + ' ' + build)
          compile(OS,compiler,platform,isas,build,4)
          compile(OS,compiler,platform,isas,build,8)

########################## rendering ##########################

def configName(OS, scene, compiler, platform, build, device, isa):
  return OS + '_' + compiler + '_' + platform + '_' + build + '_' + scene + '_' + device + '_' + isa 

def render(OS, scene, compiler, platform, build, device, isa):
  if OS == 'windows': executable = platform + '\\' + build + '\\' + 'renderer.exe'
  else:               executable = 'build' + '/' + 'renderer' 
  base = configName(OS, scene, compiler, platform, build, device, isa)
  logFile = testDir + dash + base + '.log'
  if OS == 'windows': imageFile = testDir + dash + base + '.tga'
  else:               imageFile = testDir + dash + base + '.jpg'
  if isa == 'knc'   : device = device + '_xeon_phi'
  if not os.path.exists(logFile):
    command = executable
    command += ' -rtcore benchmark=1' + ',isa=' + isa + ' -device ' + device
    command += ' -c ' + modelDir + dash + scene + dash + scene + '_test.ecs'
    command += ' -spp 16 -frames 2'
    command += ' -o ' + imageFile + ' > ' + logFile
    os.system(command)

def renderLoop(OS):
    for compiler in compilers:
      for platform in platforms:
        for build in builds:
          if isas4 <> []:
            compile(OS,compiler,platform,isas,build,4)
            for scene in scenes:
              for device in devices:
                for isa in isas4:
                  print(compiler + ' ' + platform + ' ' + build + ' ' + scene + ' ' + device + ' ' + isa)
                  render(OS, scene, compiler, platform, build, device, isa)
          if isas8 <> []:
            compile(OS,compiler,platform,isas,build,8)
            for scene in scenes:
              for device in devices:
                for isa in isas8:
                  print(compiler + ' ' + platform + ' ' + build + ' ' + scene + ' ' + device + ' ' + isa)
                  render(OS, scene, compiler, platform, build, device, isa)

########################## data extraction ##########################

buildTime = {}
memory = {}
mrps = {}
num = {}
warning = {}
error = {}
crash = {}

# Extract a floating point value from a string
def extractFloat(string, prefix, suffix):
  regexp = prefix + '[+-]? *(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?' + suffix
  values = re.findall(regexp, string)
  if (len(values) != 1):
    sys.stderr.write("Error: could not extract floating point value");
    sys.exit(1)
  return float(values[0].replace(prefix, '').replace(suffix, ''))

def initAverage():
  for OS in oss:
    if OS == 'windows':
      compilers = compilers_win
      platforms = platforms_win
      isas = isas_win
    else:
      compilers = compilers_unix
      platforms = platforms_unix
      isas = isas_unix
    for compiler in compilers:
      buildTime[compiler] = 0
      memory[compiler] = 0
      mrps[compiler] = 0
      num [compiler] = 0
    for platform in platforms:
      buildTime[platform] = 0
      memory[platform] = 0
      mrps[platform] = 0
      num [platform] = 0
    for isa in isas:
      buildTime[isa] = 0
      memory[isa] = 0
      mrps[isa] = 0
      num[isa] = 0
    for compiler in compilers:
      for platform in platforms:
        for build in builds:
          for device in devices:
            for isa in isas:
              base = configName(OS, 'average', compiler, platform, build, device, isa)
              buildTime[base] = 0
              memory[base] = 0
              mrps[base] = 0
              num[base] = 0
              warning[base] = False
              error[base] = False
              crash[base] = False

# Extract data for on test configuration
def extract(OS, scene, compiler, platform, build, device, isa):
  base = configName(OS, scene, compiler, platform, build, device, isa)
  avg = configName(OS, 'average', compiler, platform, build, device, isa)
  logFileName = testDir + dash + base + '.log'
  warning[base] = False
  error[base] = False
  crash[base] = False
  buildTime[base] = 'N/A'
  memory[base] = 'N/A'
  mrps[base] = 'N/A'
  try:
    logFile = open(logFileName, 'r')
    crash[base] = True # assume a crash until mrps data was extracted successfully
    buildTime[base] = 'Crash'
    memory[base] = 'Crash'
    mrps[base] = 'Crash'
    for line in logFile:
      line = line.replace('mrps','Mrps')
      if line.count('Error') > 0:
        error[base] = True
        buildTime[base] = 'Error'
        memory[base] = 'Error'
        mrps[base] = 'Error'
        break
      if line.count('Warning') > 0:
        warning[base] = True
      if line.count('BENCHMARK_BUILD ') == 1:
        line = line[16:]
        numbers = map(float, line.split(" "))
        buildTime[base] = numbers[0]
        memory[base] = 1E-6*numbers[2]
      elif line.count('Mrps') > 0:
        mrps[base] = extractFloat(line, '', ' Mrps')
        crash[base] = False

      if mrps[base] != 'Crash':
        buildTime[compiler] += buildTime[base]
        memory[compiler] += memory[base]
        mrps[compiler] += mrps[base]
        num[compiler] += 1

        buildTime[platform] += buildTime[base]
        memory[platform] += memory[base]
        mrps[platform] += mrps[base]
        num[platform] += 1

        buildTime[isa] += buildTime[base]
        memory[isa] += memory[base]
        mrps[isa] += mrps[base]
        num[isa] += 1

        buildTime[avg] += buildTime[base]
        memory[avg] += memory[base]
        mrps[avg] += mrps[base]
        num[avg] += 1

  except IOError :
    print('cannot open ' + logFileName)

# Extract all data
def extractLoop():
  initAverage()
  for OS in oss:
    if OS == 'windows':
      compilers = compilers_win
      platforms = platforms_win
      isas = isas_win
    else:
      compilers = compilers_unix
      platforms = platforms_unix
      isas = isas_unix
    for compiler in compilers:
      for platform in platforms:
        for build in builds:
          for scene in scenes:
            for device in devices:
              for isa in isas:
                extract(OS, scene, compiler, platform, build, device, isa)

def printAccelRowForScene(OS,scene,compiler,platform,build,device,isa):
  line  = '  ' + '{0:<13}'.format(device) + ' | '
  line += '  ' + '{0:<8}'.format(isa) + ' | '
  base = configName(OS, scene, compiler, platform, build, device, isa)
  if (num.has_key(base)): N = num[base] 
  else: N = 1
  if type(buildTime[base]) == float: line += ('%#6i' % (buildTime[base]/N)) + ' '
  else: line += '{0:>6}'.format(buildTime[base]) + ' '
  if type(memory[base]) == float: line += ('%#7.1f' %  (memory[base]/N)) + ' '
  else: line += '{0:>7}'.format(memory[base]) + ' '
  if type(mrps[base]) == float: line += ('%#6.1f' %  (mrps[base]/N))
  else: line += '{0:>6}'.format(mrps[base])
  line += ' | '
  print(line)
 
# Print data in formatted ASCII tables
def printScene(scene):
  tableWidth = 85 + 24
  line = scene + ' '
  while (len(line) < tableWidth+8): line = line + '='
  print(line)
  
  line = ''
  while (len(line) < 29): line = line + ' '
  line += '|  build  memory   mrps |'
  print(line)

  for OS in oss:
    if OS == 'windows':
      compilers = compilers_win
      platforms = platforms_win
      isas = isas_win
    else:
      compilers = compilers_unix
      platforms = platforms_unix
      isas = isas_unix

    for platform in platforms:
     for compiler in compilers:
      for build in builds:    
          line = compiler + ' ' + platform + ' ' + build + ' '
          while (len(line) < tableWidth): line = line + '.'
          print(line)
          for device in devices:
            for isa in isas:
              printAccelRowForScene(OS,scene,compiler,platform,build,device,isa)

  line = '';
  while (len(line) < tableWidth): line = line + "="
  print(line + '\n\n')

########################## command line parsing ##########################

def printUsage():
  sys.stderr.write('Usage: ' + sys.argv[0] + ' compile <os>\n')
  sys.stderr.write('       ' + sys.argv[0] + ' render  <os> <modelDir> <testDir>\n')
  sys.stderr.write('       ' + sys.argv[0] + ' extract <os> <testDir>\n')
  sys.exit(1)

if len(sys.argv) < 3: printUsage()
mode = sys.argv[1]
OS = sys.argv[2]

if OS == 'windows':
  dash = '\\'
  compilers = compilers_win
  platforms = platforms_win
  isas = isas_win
  isas4 = isas4_win
  isas8 = isas8_win

else:
  dash = '/'
  compilers = compilers_unix
  platforms = platforms_unix
  isas = isas_unix
  isas4 = isas4_unix
  isas8 = isas8_unix

if mode == 'compile':
  compileLoop(OS)
  sys.exit(1)

if mode == 'render':
  if len(sys.argv) < 5: printUsage()
  modelDir  = sys.argv[3]
  testDir = sys.argv[4]
  renderLoop(OS)
  sys.exit(1)

if mode == 'extract':
  dash = '/'
  if len(sys.argv) < 4: printUsage()
  testDir = sys.argv[3]
  if OS != 'all': oss = [OS]
  extractLoop()

  # Print scenes
  for scene in scenes:
    printScene(scene)

  printScene('average')

  line = ''
  while (len(line) < 16): line = line + ' '
  line += '|  build  memory   mrps |'
  print(line)

  print('Average by Compiler............................')

  for compiler in compilers:
    averageBuildTime = buildTime[compiler]/num[compiler]
    averageMemory = memory[compiler]/num[compiler]
    averageMrps = mrps[compiler]/num[compiler]
    line = ''
    line += ('%#6i' % averageBuildTime) + ' '
    line += ('%#7.1f' % averageMemory) + ' '
    line += ('%#6.1f' % averageMrps)
    line += ' | '

    print('  '+'{0:<14}'.format(compiler) + '| ' + line)

  print('')

  line = ''
  while (len(line) < 16): line = line + ' '
  line += '|  build  memory   mrps |'
  print(line)

  print('Average by ISA .................................')

  for isa in isas:
    averageBuildTime = buildTime[isa]/num[isa]
    averageMemory = memory[isa]/num[isa]
    averageMrps = mrps[isa]/num[isa]
    line = ''
    line += ('%#6i' % averageBuildTime) + ' '
    line += ('%#7.1f' % averageMemory) + ' '
    line += ('%#6.1f' % averageMrps)
    line += ' | '

    print('  '+'{0:<14}'.format(isa) + '| ' + line)
  
  print('')

  sys.exit(1)
