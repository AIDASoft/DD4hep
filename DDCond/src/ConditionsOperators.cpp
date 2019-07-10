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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsOperators.h"

// C/C++ include files
#include <cstring>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

/// Select all condition from the conditions manager registered at the Detector object
size_t Operators::collectAllConditions(Detector& description, RangeConditions& conditions)   {
  ConditionsManager manager = ConditionsManager::from(description);
  return collectAllConditions(manager, conditions);
}

/// Select all condition from the conditions manager registered at the Detector object
size_t Operators::collectAllConditions(ConditionsManager manager, RangeConditions& conditions)   {
  const auto types = manager.iovTypesUsed();
  size_t num_conditions = 0;
  for( auto type : types )  {
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        for( auto& cp : pool->elements )  {
          RangeConditions rc;
          cp.second->select_all(rc);
          for( auto c : rc )
            conditions.emplace_back(c);
          num_conditions += rc.size();
        }
      }
    }
  }
  return num_conditions;
}

/// Select all condition from the conditions manager registered at the Detector object
size_t Operators::collectAllConditions(Detector& description, std::map<int,Condition>& conditions)   {
  ConditionsManager manager = ConditionsManager::from(description);
  return collectAllConditions(manager, conditions);
}

/// Select all condition from the conditions manager registered at the Detector object
size_t Operators::collectAllConditions(ConditionsManager manager, std::map<int,Condition>& conditions)   {
  const auto types = manager.iovTypesUsed();
  size_t num_conditions = 0;
  for( auto type : types )  {
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        for( auto& cp : pool->elements )  {
          RangeConditions rc;
          cp.second->select_all(rc);
          for( auto c : rc )
            conditions.emplace(c->hash,c);
          num_conditions += rc.size();
        }
      }
    }
  }
  return num_conditions;
}

