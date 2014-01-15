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

#include <stdio.h>
#include "xmlWriter.h"
#include "sys/platform.h"
#include "sys/filename.h"
#include "math/vec3.h"
#include "math/col3.h"
#include "image/image.h"

std::map<std::string,std::string> g_bumpMaps;

void writeMaterialTextures(FILE *file, const Material &material) {

    if (!material.map_d.empty())  fprintf(file, "                    <texture name=\"map_d\">\"%s\"</texture>\n",  material.map_d.c_str());
    if (!material.map_Ka.empty()) fprintf(file, "                    <texture name=\"map_Ka\">\"%s\"</texture>\n", material.map_Ka.c_str());
    if (!material.map_Kd.empty()) fprintf(file, "                    <texture name=\"map_Kd\">\"%s\"</texture>\n", material.map_Kd.c_str());
    if (!material.map_Ks.empty()) fprintf(file, "                    <texture name=\"map_Ks\">\"%s\"</texture>\n", material.map_Ks.c_str());
    if (!material.map_Ns.empty()) fprintf(file, "                    <texture name=\"map_Ns\">\"%s\"</texture>\n", material.map_Ns.c_str());
    if (!material.map_Bump.empty()) {
      embree::FileName bump_in = material.map_Bump;
      embree::FileName bump_out = embree::FileName(bump_in.dropExt().str()+"ump").addExt("."+bump_in.ext());
      g_bumpMaps[bump_in.str()] = bump_out.str();
      fprintf(file, "                    <texture name=\"map_Bump\">\"%s\"</texture>\n", bump_out.c_str());
    }
}

void writeMaterial(FILE *file, const Material &material) 
{
  if (material.name == "6_Eyelash")
  {
    fprintf(file, "            <material name=\"%s\">\n",material.name.c_str());
    fprintf(file, "                <code>\"EyeLash\"</code>\n");
    fprintf(file, "                <parameters>\n");
    fprintf(file, "                    <float3 name=\"lashColor\"> %7.4f %7.4f %7.4f</float3>\n", material.Kd.r, material.Kd.g, material.Kd.b);
    if (!material.map_d.empty()) fprintf(file, "                    <texture name=\"map_opacity\">\"%s\"</texture>\n", material.map_d.c_str());
    fprintf(file, "                </parameters>\n");
    fprintf(file, "            </material>\n");
  }
  else if (material.name == "Top")
  {
    fprintf(file, "            <material file=\"materials/knitted_white.xml\">\n");
    fprintf(file, "                <float2 name=\"scale\">0.3 0.3</float2>\n");
    fprintf(file, "            </material>\n");
  }
  else if (material.name == "B25STop") 
  {
    fprintf(file, "            <material file=\"materials/synthetic.xml\"/>\n");
  }
  else
  {
    fprintf(file, "            <material name=\"%s\">\n",material.name.c_str());
    fprintf(file, "                <code>\"OBJ\"</code>\n");
    fprintf(file, "                <parameters>\n");
    fprintf(file, "                    <float  name=\"d\"> %7.4f</float>\n", material.d);
    fprintf(file, "                    <float3 name=\"Ka\">%7.4f %7.4f %7.4f</float3>\n", material.Ka.r, material.Ka.g, material.Ka.b);
    fprintf(file, "                    <float3 name=\"Kd\">%7.4f %7.4f %7.4f</float3>\n", material.Kd.r, material.Kd.g, material.Kd.b);
    fprintf(file, "                    <float3 name=\"Ks\">%7.4f %7.4f %7.4f</float3>\n", material.Ks.r, material.Ks.g, material.Ks.b);
    fprintf(file, "                    <float  name=\"Ns\">%7.4f</float>\n", material.Ns);
    writeMaterialTextures(file, material);
    fprintf(file, "                </parameters>\n");
    fprintf(file, "            </material>\n");
  }
}

void writeMeshData(FILE *file, const Mesh &mesh) 
{
  if (mesh.material.name == "5_Cornea") return;

  fwrite(&mesh.positions[0], sizeof(Vec3f), mesh.positions.size(), file);
  fwrite(&mesh.normals[0],   sizeof(Vec3f), mesh.normals.size(),   file);
  fwrite(&mesh.texcoords[0], sizeof(Vec2f), mesh.texcoords.size(), file);
  fwrite(&mesh.triangles[0], sizeof(Vec3i), mesh.triangles.size(), file);

}

void writeMeshHeader(FILE *file, const Mesh &mesh, size_t meshID, size_t &offset) 
{
  if (mesh.material.name == "5_Cornea") return;

    fprintf(file, "        <TriangleMesh id=\"%zu\" name=\"mesh%zu\">\n", meshID, meshID);
    writeMaterial(file, mesh.material);
    fprintf(file, "            <positions ofs=\"%zu\" size=\"%zu\"/>\n", offset, mesh.positions.size());  offset += mesh.positions.size() * sizeof(Vec3f);
    fprintf(file, "            <normals   ofs=\"%zu\" size=\"%zu\"/>\n", offset, mesh.normals.size());    offset += mesh.normals.size()   * sizeof(Vec3f);
    fprintf(file, "            <texcoords ofs=\"%zu\" size=\"%zu\"/>\n", offset, mesh.texcoords.size());  offset += mesh.texcoords.size() * sizeof(Vec2f);
    fprintf(file, "            <triangles ofs=\"%zu\" size=\"%zu\"/>\n", offset, mesh.triangles.size());  offset += mesh.triangles.size() * sizeof(Vec3i);
    fprintf(file, "        </TriangleMesh>\n\n");

}

namespace embree {
void convertBumpMap(const FileName& inFile, const FileName& outFile)
{
  /* skip if output file already exists */
  if (FILE* f = fopen(outFile.c_str(),"r")) {
    fclose(f); return;
  }
  
  std::cout << "converting " << inFile.str() << " " << std::flush;
  Ref<Image> in = loadImage(inFile);
  Ref<Image> out = new Image3f(in->width,in->height);
  for (size_t y=0; y<in->height; y++) {
    if (y % 100 == 0) std::cout << "." << std::flush;
    for (size_t x=0; x<in->width; x++) {
      float h00 = in->get(x+0,y+0).r;
      float h01 = in->get(x+0,min(y+1,in->height-1)).r;
      float h10 = in->get(min(x+1,in->width-1),y+0).r;
      Vec3f n = Vec3f(0.5f)+0.5f*normalize(cross(Vec3f(1.0f,0.0f,h10-h00),Vec3f(0.0f,1.0f,h01-h00)));
      out->set(x,y,Color4(n.x,n.y,n.z,0.0f));
    }
  }
  std::cout << " [DONE]" << std::endl;
  storeImage(out,outFile);
}
}

void writeXML(const char *xmlFileName, const char *binFileName, const std::vector<Mesh> &model) {

    /*! open the Embree XML file */
    FILE *xmlFile = fopen(xmlFileName, "w");  if (!xmlFile) printf("  ERROR:  unable to open %s\n", xmlFileName), exit(1);

    /*! open the Embree BIN file */
    FILE *binFile = fopen(binFileName, "w");  if (!binFile) printf("  ERROR:  unable to open %s\n", binFileName), exit(1);

    /*! write the XML header */
    fprintf(xmlFile, "<?xml version=\"1.0\"?>\n\n<scene>\n    <Group id=\"0\" name=\"\">\n\n");

    /*! write the XML triangle mesh meta data */
    for (size_t i=0, offset=0 ; i < model.size() ; i++) writeMeshHeader(xmlFile, model[i], i + 1, offset);

    /*! write the triangle mesh binary data */
    for (size_t i=0 ; i < model.size() ; i++) writeMeshData(binFile, model[i]);  fclose(binFile);

    /*! write the XML footer */
    fprintf(xmlFile, "    </Group>\n</scene>\n");  fclose(xmlFile);

    /*! convert all bump maps */
    for (std::map<std::string,std::string>::iterator i=g_bumpMaps.begin(); i != g_bumpMaps.end(); i++) 
      embree::convertBumpMap(i->first,i->second);
}

