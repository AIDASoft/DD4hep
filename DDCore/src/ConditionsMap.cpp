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
#include "DD4hep/ConditionsMap.h"

using namespace DD4hep::Conditions;

/// Insert a new entry to the map
template <typename T>
bool ConditionsMapping<T>::insert(DetElement detector, unsigned int key, Condition condition)   {
  ConditionKey k(detector,key);
  auto res = data.insert(std::make_pair(k.hash,condition));
  return res.second;
}

/// Interface to access conditions by hash value
template <typename T>
Condition ConditionsMapping<T>::get(DetElement detector, unsigned int key) const   {
  ConditionKey k(detector,key);
  auto res = data.find(k.hash);
  if ( res == data.end() )
    return Condition();
  return res->second;
}

/// Interface to scan data content of the conditions mapping
template <typename T>
void ConditionsMapping<T>::scan(Condition::Processor& processor) const  {
  for( const auto& i : data )
    processor(i);
}

template class ConditionsMapping<std::map<Condition::key_type,Condition> >;
template class ConditionsMapping<std::unordered_map<Condition::key_type,Condition> >;
