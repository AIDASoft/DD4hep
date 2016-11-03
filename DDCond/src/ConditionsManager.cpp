// $Id$
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsManager.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Access specialization
    template <> ConditionsManager ConditionsManager::from<LCDD>(LCDD& host)  {
      Object* obj = host.extension<Object>();
      if ( obj ) return ConditionsManager(obj);
      except("ConditionsManager","+++ Failed to access manager from LCDD.");
      return ConditionsManager();
    }
  }
}

/// Default constructor
ConditionsManager::ConditionsManager(LCDD& lcdd)  {
  assign(new Object(lcdd), "ConditionsManager","");
}
 
ConditionsManager& ConditionsManager::initialize()   {
  access()->initialize();
  return *this;
}

/// Access to the property manager
PropertyManager& ConditionsManager::properties()  const   {
  return access()->properties();
}

/// Access to properties
Property& ConditionsManager::operator[](const std::string& property_name) const    {
  return access()->properties().property(property_name);
}

/// Access the conditions loader
Handle<ConditionsManager::Loader> ConditionsManager::loader()  const    {
  return access()->loader();
}

/// Register new IOV type if it does not (yet) exist.
pair<bool, const IOVType*> 
ConditionsManager::registerIOVType(size_t iov_type, const string& iov_name)   {
  return access()->registerIOVType(iov_type, iov_name);
}

/// Access IOV by its name
const IOVType* ConditionsManager::iovType (const string& iov_name) const   {
  return access()->iovType(iov_name);
}

/// Access conditions multi IOV pool by iov type
ConditionsIOVPool* ConditionsManager::iovPool(const IOVType& iov_type)  const   {
  return access()->m_rawPool[iov_type.type];
}

/// Access the used/registered IOV types
const vector<const IOVType*> ConditionsManager::iovTypesUsed() const  {
  Object* obj = access();
  vector<const IOVType*> result;
  const IOVTypes& types = obj->iovTypes();
  for(IOVTypes::const_iterator i=types.begin(); i!=types.end(); ++i)
    if ( int((*i).type) != IOVType::UNKNOWN_IOV ) result.push_back(&(*i));
  return result;
}

/// Register IOV with type and key
ConditionsPool* ConditionsManager::registerIOV(const IOVType& typ, IOV::Key key)   {
  return access()->registerIOV(typ, key);
}

/// Create IOV from string
void ConditionsManager::fromString(const string& iov_str, IOV& iov)  {
  access()->fromString(iov_str, iov);
}

/// Register new condition with the conditions store. Unlocked version, not multi-threaded
bool ConditionsManager::registerUnlocked(ConditionsPool* pool, Condition cond)   {
  return access()->registerUnlocked(pool, cond);
}

/// Clean conditions, which are above the age limit.
void ConditionsManager::clean(const IOVType* typ, int max_age)   {
  access()->clean(typ, max_age);
}

/// Full cleanup of all managed conditions.
void ConditionsManager::clear()   {
  access()->clear();
}

/// Prepare all updates to the clients with the defined new IOV. Changes are not yet applied
long ConditionsManager::prepare(const IOV& required_validity, dd4hep_ptr<UserPool>& user_pool)   {
  return access()->prepare(required_validity, user_pool);
}

/// Prepare all updates to the clients with the defined IOV
long ConditionsManager::prepare(const IOV& required_validity,
				dd4hep_ptr<UserPool>& user_pool,
				const Dependencies& dependencies,
				bool verify_dependencies)  {
  return access()->prepare(required_validity, user_pool, dependencies, verify_dependencies);
}
