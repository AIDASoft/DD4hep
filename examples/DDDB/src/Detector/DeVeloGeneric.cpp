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

/// Initialization of sub-classes
void DeVeloGenericStaticObject::initialize()    {
  this->DeStaticObject::initialize();
}

/// Printout method to stdout
void DeVeloGenericStaticObject::print(int indent, int flg)  const   {
  std::string prefix = DE::indent(indent);
  DeStaticObject::print(indent, flg&~DePrint::CHILDREN);
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVeloGen", "%s+ >> %ld child(ren) %ld Sensor(s)",
             prefix.c_str(), children.size(), sensors.size());
  }
  if ( flg&DePrint::CHILDREN )   {
    for(size_t i=0, e=children.size(); e==0 && i<sensors.size(); ++i)
      sensors[i]->print(indent+1, flg);
    for(size_t i=0, e=children.size(); e>0 && i<sensors.size(); ++i)   {
      printout(INFO,"DeVeloGen", "%s+   >> Sensor[%ld] %s",
               prefix.c_str(),i,sensors[i]->detector.path().c_str());
    }
    for(auto c : children) c->print(indent+1,flg);
  }
}

/// Initialization of sub-classes
void DeVeloGenericObject::initialize()    {
  this->DeIOVObject::initialize();
}

/// Printout method to stdout
void DeVeloGenericObject::print(int indent, int flg)  const   {
  std::string prefix = DE::indent(indent);
  DeIOVObject::print(indent, flg&~DePrint::CHILDREN);
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeIOVVeloGen", "%s+ >> %ld child(ren) %ld Sensor(s)",
             prefix.c_str(), children.size(), sensors.size());
  }
  for(size_t i=0, e=children.size(); e==0 && i<sensors.size(); ++i)
    sensors[i]->print(indent+1, flg);
  for(size_t i=0, e=children.size(); e>0 && i<sensors.size(); ++i)   {
    printout(INFO,"DeIOVVeloGen", "%s+   >> [%ld] %s",
             prefix.c_str(), i, sensors[i]->detector.path().c_str());
  }
  for(auto c : children) c->print(indent+1,flg);
}
