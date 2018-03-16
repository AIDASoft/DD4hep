//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================

// Framework include files
#include "Detector/DeVP.h"
#include "DD4hep/Printout.h"

using namespace gaudi::detail;


/// Printout method to stdout
void DeVPStaticObject::print(int indent, int flg)  const    {
  std::string prefix = DE::indent(indent);
  printout(INFO, "DeVPStatic",
           "%s*+++++ VP detector element for %s  Sides:%ld Supports:%ld Modules:%ld Ladders:%ld Sensors:%ld Cut:%g",
           prefix.c_str(), detector.path().c_str(),
           sides.size(), supports.size(), modules.size(), ladders.size(), sensors.size(),
           sensitiveVolumeCut);
  this->DeVPGenericStaticObject::print(indent,flg);
  std::for_each(sensors.begin(), sensors.end(), [indent,flg](const DeVPSensorStatic& s) { s.print(indent+1,flg); });
}

/// Initialization of sub-classes
void DeVPStaticObject::initialize()    {
  this->DeVPGenericStaticObject::initialize();
  sensitiveVolumeCut = param<double>("sensitiveVolumeCut");
}

/// Printout method to stdout
void gaudi::DeVPStatic::print(int indent, int flg)  const    {
  access()->print(indent, flg);
}

/// Printout method to stdout
void DeVPObject::print(int indent, int flg)  const    {
  std::string prefix = DE::indent(indent);
  printout(INFO, "DeVP",
           "%s*+++++ VP detector element for %s  Sides:%ld Supports:%ld Modules:%ld Ladders:%ld Sensors:%ld",
           prefix.c_str(), detector.path().c_str(),
           sides.size(), supports.size(), modules.size(), ladders.size(), sensors.size());
  this->DeVPGenericObject::print(indent,flg);
  std::for_each(sensors.begin(), sensors.end(), [indent,flg](const DeVPSensor& s) {
      if ( s.isValid() ) s.print(indent+1,flg);   });
}

/// Initialization of sub-classes
void DeVPObject::initialize()    {
  this->DeVPGenericObject::initialize();
  vp_static = de_static;
}


/// Printout method to stdout
void gaudi::DeVP::print(int indent, int flg)  const    {
  access()->print(indent,flg);
}

#if 0
static void test()   {
  using namespace gaudi;
  DeVP devp;
  DetectorElement<DeVP> detElem(devp);
  std::cout << "TEST: Got detector element:" << detElem.detector().path()
            << detElem.detectorAlignment().name()
            << std::endl;
}
#endif
