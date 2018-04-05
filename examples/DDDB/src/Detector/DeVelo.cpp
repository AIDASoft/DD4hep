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
#include "Detector/DeVelo.h"
#include "DD4hep/Printout.h"

using namespace gaudi::detail;

/// Printout method to stdout
void DeVeloStaticObject::print(int indent, int flg)  const    {
  std::string prefix = DE::indent(indent);
  this->DeStaticObject::print(indent,flg);
  printout(INFO, "DeVeloStatic",
           "%s+ Sides: %ld Modules:%ld Supports: %ld Sensors: %ld Cut:%g",
           prefix.c_str(), sides.size(), modules.size(), supports.size(),
           sensors[DeVeloFlags::ALL].size(), sensitiveVolumeCut);
  printout(INFO, "DeVeloStatic",
           "%s+ Sensors: R:%ld (L:%ld R:%ld) Phi:%ld (L:%ld R:%ld) rPhi:%ld (L:%ld R:%ld) PU:%ld (L:%ld R:%ld)",
           prefix.c_str(), 
           sensors[DeVeloFlags::ALL].size(),
           rSensors[DeVeloFlags::ALL].size(),
           rSensors[DeVeloFlags::LEFT].size(),
           rSensors[DeVeloFlags::RIGHT].size(),
           phiSensors[DeVeloFlags::ALL].size(),
           phiSensors[DeVeloFlags::LEFT].size(),
           phiSensors[DeVeloFlags::RIGHT].size(),
           rphiSensors[DeVeloFlags::ALL].size(),
           rphiSensors[DeVeloFlags::LEFT].size(),
           rphiSensors[DeVeloFlags::RIGHT].size(),
           puSensors[DeVeloFlags::ALL].size(),
           puSensors[DeVeloFlags::LEFT].size(),
           puSensors[DeVeloFlags::RIGHT].size()
           );
  if ( flg&DePrint::CHILDREN )   {
    std::for_each(sides.begin(), sides.end(),
                  [indent,flg](const DeVeloGenericStatic& s) { if (s.isValid()) s.print(indent+1,flg); });
  }
}

/// Initialization of sub-classes
void DeVeloStaticObject::initialize()    {
  this->DeStaticObject::initialize();
  sensitiveVolumeCut = param<double>("sensitiveVolumeCut");
}

/// Printout method to stdout
void DeVeloObject::print(int indent, int flg)  const    {
  std::string prefix = DE::indent(indent);
  this->DeIOVObject::print(indent,flg&~DePrint::CHILDREN);
  printout(INFO, "DeVelo",
           "%s+ Sides: %ld Modules:%ld Supports: %ld Sensors: %ld",
           prefix.c_str(), sides.size(), modules.size(), supports.size(),
           sensors[DeVeloFlags::ALL].size());
  printout(INFO, "DeVelo",
           "%s+ Sensors: R:%ld (L:%ld R:%ld) Phi:%ld (L:%ld R:%ld) rPhi:%ld (L:%ld R:%ld) PU:%ld (L:%ld R:%ld)",
           prefix.c_str(), 
           sensors[DeVeloFlags::ALL].size(),
           rSensors[DeVeloFlags::ALL].size(),
           rSensors[DeVeloFlags::LEFT].size(),
           rSensors[DeVeloFlags::RIGHT].size(),
           phiSensors[DeVeloFlags::ALL].size(),
           phiSensors[DeVeloFlags::LEFT].size(),
           phiSensors[DeVeloFlags::RIGHT].size(),
           rphiSensors[DeVeloFlags::ALL].size(),
           rphiSensors[DeVeloFlags::LEFT].size(),
           rphiSensors[DeVeloFlags::RIGHT].size(),
           puSensors[DeVeloFlags::ALL].size(),
           puSensors[DeVeloFlags::LEFT].size(),
           puSensors[DeVeloFlags::RIGHT].size()
           );
  std::for_each(sides.begin(), sides.end(),
                [indent,flg](const DeVeloGeneric& s) { if (s.isValid()) s.print(indent+1,flg); });
}

/// Initialization of sub-classes
void DeVeloObject::initialize()    {
  this->DeIOVObject::initialize();
  vp_static = de_static;
}
