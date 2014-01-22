// ======================================================================== //
//                 INTEL CORPORATION PROPRIETARY INFORMATION                //
//   This software is supplied under the terms of a license agreement or    //
//   nondisclosure agreement with Intel Corporation and may not be copied   //
//    or disclosed except in accordance with the terms of that agreement.   //
//        Copyright (C) 2012 Intel Corporation. All Rights Reserved.        //
// ======================================================================== //

#include <stdio.h>
#include <stdlib.h>
#include "objLoader.h"
#include "xmlWriter.h"

int main(int argc, char **argv) {

    /*! all file names must be specified on the command line */
    if (argc != 4) printf("  USAGE:  obj2xml <infile.OBJ> <outfile.xml> <outfile.bin>\n"), exit(1);

    /*! load the OBJ file */
    std::vector<Mesh> model = loadOBJ(argv[1]);

    /*! write the Embree XML shell and binary scene data */
    writeXML(argv[2], argv[3], model);

}

