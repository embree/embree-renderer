// ======================================================================== //
//                 INTEL CORPORATION PROPRIETARY INFORMATION                //
//   This software is supplied under the terms of a license agreement or    //
//   nondisclosure agreement with Intel Corporation and may not be copied   //
//    or disclosed except in accordance with the terms of that agreement.   //
//        Copyright (C) 2012 Intel Corporation. All Rights Reserved.        //
// ======================================================================== //

#ifndef __XML_WRITER_H__
#define __XML_WRITER_H__

#include "objLoader.h"

/*! write out scene geometry and materials in Embree XML format */
void writeXML(const char *xmlFileName, const char *binFileName, const std::vector<Mesh> &model);

#endif // __XML_WRITER_H__

