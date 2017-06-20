//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//
//  Simple program to print all the materials in a detector on
//  a straight line between two given points
// 
//  Author     : F.Gaede, DESY
//  Author     : M.Frank, CERN
//
//==========================================================================

#include "TError.h"

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialManager.h"
#include "main.h"

using namespace dd4hep;
using namespace dd4hep::rec;

int main_wrapper(int argc, char** argv)   {
  struct Handler  {
    Handler() { SetErrorHandler(Handler::print); }
    static void print(int level, Bool_t abort, const char *location, const char *msg)  {
      if ( level > kInfo || abort ) ::printf("%s: %s\n", location, msg);
    }
    static void usage()  {
      std::cout << " usage: materialScan compact.xml x0 y0 z0 x1 y1 z1 " << std::endl 
                << "        -> prints the materials on a straight line between the two given points ( unit is cm) "  
                << std::endl;
      exit(1);
    }
  } _handler;

  if( argc != 8 ) Handler::usage();
   
  Vector3D p0, p1, end, direction;
  std::string inFile =  argv[1];
  std::stringstream sstr;
  sstr << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] << " " << "NONE";
  sstr >> p0.array()[0] >> p0.array()[1] >> p0.array()[2];
  sstr >> p1.array()[0] >> p1.array()[1] >> p1.array()[2];
  if ( !sstr.good() ) Handler::usage();

  setPrintLevel(WARNING);
  Detector& description = Detector::getInstance();
  description.fromCompact(inFile);
  direction = (p1-p0).unit();

  MaterialManager matMgr( description.world().volume()  ) ;
  const MaterialVec& materials = matMgr.materialsBetween(p0, p1);
  double sum_x0 = 0;
  double sum_lambda = 0;
  double path_length = 0;
  const char* fmt1 = " | %5d %-20s %3.0f %8.3f %8.4f %11.4f  %11.4f %10.3f %8.2f %11.6f %11.6f  (%7.2f,%7.2f,%7.2f)\n";
  const char* fmt2 = " | %5d %-20s %3.0f %8.3f %8.4f %11.6g  %11.6g %10.3f %8.2f %11.6f %11.6f  (%7.2f,%7.2f,%7.2f)\n";
  const char* line = " +--------------------------------------------------------------------------------------------------------------------------------------------------\n";

  ::printf("%s + Material scan between: x_0 = (%7.2f,%7.2f,%7.2f) [cm] and x_1 = (%7.2f,%7.2f,%7.2f) [cm] : \n%s",
           line,p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],line);
  ::printf(" |     \\   %-11s        Atomic                 Radiation   Interaction               Path   Integrated  Integrated    Material\n","Material");
  ::printf(" | Num. \\  %-11s   Number/Z   Mass/A  Density    Length       Length    Thickness   Length      X0        Lambda      Endpoint  \n","Name");
  ::printf(" | Layer \\ %-11s            [g/mole]  [g/cm3]     [cm]        [cm]          [cm]      [cm]     [cm]        [cm]     (     cm,     cm,     cm)\n","");
  ::printf("%s",line);

  for( unsigned i=0,n=materials.size();i<n;++i){
    TGeoMaterial* mat =  materials[i].first->GetMaterial();
    double length = materials[i].second;
    double nx0 = length / mat->GetRadLen();
    sum_x0 += nx0;
    double nLambda = length / mat->GetIntLen();
    sum_lambda += nLambda;
    path_length += length;
    end = path_length * direction;
    const char* fmt = mat->GetRadLen() >= 1e5 ? fmt2 : fmt1;
    ::printf(fmt, i+1, mat->GetName(), mat->GetZ(), mat->GetA(),
             mat->GetDensity(), mat->GetRadLen(), mat->GetIntLen(), 
             length, path_length, sum_x0, sum_lambda, end[0], end[1], end[2]);
    //mat->Print();
  }
  printf("%s",line);
  const MaterialData& avg = matMgr.createAveragedMaterial(materials);
  const char* fmt = avg.radiationLength() >= 1e5 ? fmt2 : fmt1;
  end = path_length * direction;
  ::printf(fmt,0,"Average Material",avg.Z(),avg.A(),avg.density(), 
           avg.radiationLength(), avg.interactionLength(),
           path_length, path_length, 
           path_length/avg.radiationLength(), 
           path_length/avg.interactionLength(),
           end[0], end[1], end[2]);
  printf("%s",line);
  return 0;
}
