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
#ifndef DETECTOR_DECONDITIONCALLDEFS_H 
#define DETECTOR_DECONDITIONCALLDEFS_H 1

// Framework include files
#include "DD4hep/DetElement.h"
#include "DD4hep/ConditionDerived.h"

/// Gaudi namespace declaration
namespace gaudi   {

  /// Base class to share common type definitions
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeConditionCallDefs {
  public:
    typedef dd4hep::DetElement                   DetElement;
    typedef dd4hep::Condition                    Condition;
    typedef dd4hep::ConditionKey                 ConditionKey;
    typedef dd4hep::ConditionKey::KeyMaker       KeyMaker;
    typedef dd4hep::cond::ConditionResolver      Resolver;
    typedef dd4hep::cond::ConditionUpdateContext Context;
    typedef dd4hep::DDDB::DDDBCatalog            Catalog;
  };

}      // End namespace gaudi
#endif // DETECTOR_DECONDITIONCALLDEFS_H
