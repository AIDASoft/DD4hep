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

/// Initialization of sub-classes
void DeVPGenericStaticObject::initialize()    {
  this->DeStaticObject::initialize();
}

/// Printout method to stdout
void DeVPGenericStaticObject::print(int indent, int flg)  const   {
  std::string prefix = DE::indent(indent);
  DeStaticObject::print(indent, flg);
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVPGenStatic", "%s+ >> %ld Sensor(s)", prefix.c_str(), sensors.size());
  }
  for(size_t i=0; i<sensors.size(); ++i)   {
    if ( sensors[i].isValid() )  {
      printout(INFO,"DeVeloGenStatic", "%s+   >> [%ld] %s",
               prefix.c_str(), i, sensors[i]->detector.path().c_str());
    }
  }
}

/// Initialization of sub-classes
void DeVPGenericObject::initialize()    {
  this->DeIOVObject::initialize();
}

/// Printout method to stdout
void DeVPGenericObject::print(int indent, int flg)  const   {
  std::string prefix = DE::indent(indent);
  DeIOVObject::print(indent, flg);
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVPGen", "%s+ >> %ld Sensor(s)", prefix.c_str(), sensors.size());
  }
  for(size_t i=0; i<sensors.size(); ++i)   {
    if ( sensors[i].isValid() )  {
      printout(INFO,"DeVeloGenStatic", "%s+   >> [%ld] %s",
               prefix.c_str(), i, sensors[i]->detector.path().c_str());
    }
  }
}
