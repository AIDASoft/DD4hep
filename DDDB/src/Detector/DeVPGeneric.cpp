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
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVPGenStatic", "%s*========== %ld Sensors %s",
             DE::indent(indent).c_str(), sensors.size(), detector.path().c_str());
  }
  DeStaticObject::print(indent, flg);
}


/// Initialization of sub-classes
void DeVPGenericObject::initialize()    {
  this->DeIOVObject::initialize();
}

/// Printout method to stdout
void DeVPGenericObject::print(int indent, int flg)  const   {
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVPGen", "%s*========== %ld Sensors %s",
             DE::indent(indent).c_str(), sensors.size(), detector.path().c_str());
  }
  DeIOVObject::print(indent, flg);
}
