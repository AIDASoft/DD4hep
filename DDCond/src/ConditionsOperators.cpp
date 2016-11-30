//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsOperators.h"

// C/C++ include files
#include <cstring>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using DD4hep::Geometry::LCDD;

/// Select all condition from the conditions manager registered at the LCDD object
size_t Operators::collectAllConditions(LCDD& lcdd, RangeConditions& conditions)   {
  ConditionsManager manager = ConditionsManager::from(lcdd);
  return collectAllConditions(manager, conditions);
}

/// Select all condition from the conditions manager registered at the LCDD object
size_t Operators::collectAllConditions(ConditionsManager manager, RangeConditions& conditions)   {
  typedef vector<const IOVType*> _T;
  typedef ConditionsIOVPool::Elements _E;
  const _T types = manager.iovTypesUsed();
  size_t num_conditions = 0;
  for( _T::const_iterator i = types.begin(); i != types.end(); ++i )    {
    const IOVType* type = *i;
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        const _E& e = pool->elements;
        for (_E::const_iterator j=e.begin(); j != e.end(); ++j)  {
          ConditionsPool* cp = (*j).second;
          RangeConditions rc;
          cp->select_all(rc);
          for(RangeConditions::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)
            conditions.push_back(*ic);
          num_conditions += rc.size();
        }
      }
    }
  }
  return num_conditions;
}

/// Select all condition from the conditions manager registered at the LCDD object
size_t Operators::collectAllConditions(LCDD& lcdd, std::map<int,Condition>& conditions)   {
  ConditionsManager manager = ConditionsManager::from(lcdd);
  return collectAllConditions(manager, conditions);
}

/// Select all condition from the conditions manager registered at the LCDD object
size_t Operators::collectAllConditions(ConditionsManager manager, std::map<int,Condition>& conditions)   {
  typedef vector<const IOVType*> _T;
  typedef ConditionsIOVPool::Elements _E;
  const _T types = manager.iovTypesUsed();
  size_t num_conditions = 0;
  for( _T::const_iterator i = types.begin(); i != types.end(); ++i )    {
    const IOVType* type = *i;
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        const _E& e = pool->elements;
        for (_E::const_iterator j=e.begin(); j != e.end(); ++j)  {
          ConditionsPool* cp = (*j).second;
          RangeConditions rc;
          cp->select_all(rc);
          for(RangeConditions::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)
            conditions.insert(make_pair((*ic)->hash,*ic));
          num_conditions += rc.size();
        }
      }
    }
  }
  return num_conditions;
}

