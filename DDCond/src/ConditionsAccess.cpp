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
#include "DDCond/ConditionsAccess.h"
#include "DDCond/ConditionsManagerObject.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default destructor
ConditionsAccess::~ConditionsAccess()   {
}

/// Access IOV by its name
const IOVType* ConditionsAccess::iovType (const string& iov_name) const   {
  return access()->iovType(iov_name);
}

/// Access the used/registered IOV types
const vector<const IOVType*> ConditionsAccess::iovTypesUsed() const  {
  Object* obj = access();
  vector<const IOVType*> result;
  const Object::IOVTypes& types = obj->iovTypes();
  for(const auto& i : types )
    if ( int(i.type) != IOVType::UNKNOWN_IOV ) result.push_back(&i);
  return result;
}

/// Retrieve a condition given a Detector Element and the conditions name
Condition 
ConditionsAccess::get(Condition::key_type key, const Condition::iov_type& req_validity)  {
  return access()->get(key, req_validity);
}

/// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
Condition 
ConditionsAccess::get(const string& path, const Condition::iov_type& req_validity)  {
  Condition::key_type key = ConditionKey::hashCode(path);
  return access()->get(key, req_validity);
}

/// Retrieve a condition given a Detector Element and the conditions name
RangeConditions 
ConditionsAccess::getRange(Condition::key_type key, const Condition::iov_type& req_validity)  {
  return access()->getRange(key, req_validity);
}

/// Retrieve a condition given a Detector Element path and the conditions name
RangeConditions 
ConditionsAccess::getRange(const string& path, const Condition::iov_type& req_validity)  {
  Condition::key_type key = ConditionKey::hashCode(path);
  return access()->getRange(key, req_validity);
}
