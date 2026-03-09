//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
/* 
 Plugin invocation:
 ==================
 This plugin behaves like a main program.
 Invoke the plugin with something like this:

 geoPluginRun -destroy -plugin DD4hep_PrintField -opt [-opt]

*/
// Framework include files
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Detector.h>
#include <XML/Conversions.h>
#include <XML/XML.h>
#include <iostream>
#include <fstream>
#include <cerrno>

using namespace dd4hep;

namespace  {
  class Point {
  public:
    Position position;
  };
}
template <> void Converter<Point>::operator()(xml_h e) const {
  xml_dim_t p = e;
  Point pt = { Position(p.x(0e0), p.y(0e0), p.z(0e0)) };
  auto* points = (std::vector<Point>*)this->param;
  points->emplace_back( pt );
}

static int  print_field(Detector& detector, int argc, char** argv)  {
  std::string fname;
  for( int i = 0; i < argc && argv[i]; ++i )  {
    if ( 0 == ::strncmp("-points",argv[i],4) )
      fname = argv[++i];
    else  {
      break;
    }
  }
  if( fname.empty() )  {
    std::cout <<
      "Usage: -plugin DD4hep_PrintField  -arg [-arg]                                \n\n"
      "     Print electro magnetic field at various points supplied by xml input    \n\n"
      "     -point <string>    Input file with global point coordinates               \n"
      "     Arguments given: " << arguments(argc,argv) << std::endl << std::flush;
    ::exit(EINVAL);
  }
  xml::DocumentHolder doc(xml::DocumentHandler().load(fname));
  xml_h root = doc.root();
  if( !root.ptr() )  {
    printout(ALWAYS, "PrintField","+++ XML input %s does not exist. Exiting.", fname.c_str());
    ::exit(EINVAL);
  }
  printout(ALWAYS, "PrintField","+++ =========================================================");
  printout(ALWAYS, "PrintField","+++ Electro-magnetic field strength at selected points from %s", fname.c_str());
  printout(ALWAYS, "PrintField","+++ =========================================================");
  std::vector<Point> points;
  double vm = dd4hep::volt/dd4hep::meter;
  OverlayedField field = detector.field();
  xml_coll_t(root, _U(point)).for_each(Converter<Point>(detector, &points));
  for( std::size_t i=0; i< points.size(); ++i )  {
    const auto& pos = points[i].position;
    double value[6] = { 0e0, 0e0, 0e0, 0e0, 0e0, 0e0 };
    field.electromagneticField(pos, value);
    printout(ALWAYS, "PrintField","+++ "
             "Position: %7.2f %7.2f %7.2f [cm] "
             "electric field: %7.2e %7.2e %7.2e [V/m] "
             "magnetic field: %7.2e %7.2e %7.2e [tesla]",
             pos.x()/dd4hep::cm, pos.y()/dd4hep::cm, pos.z()/dd4hep::cm,
             value[0]/vm, value[1]/vm, value[2]/vm,
             value[3]/dd4hep::tesla,
             value[4]/dd4hep::tesla,
             value[5]/dd4hep::tesla );
  }

  return 1;
}
DECLARE_APPLY(DD4hep_PrintField,print_field)
