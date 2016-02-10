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
#include "DD4hep/Errors.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"

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
 
/// Default destructor
ConditionsManager::~ConditionsManager()   {
  access()->clear();
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
Handle<ConditionsManager::Loader> ConditionsManager::loader()  const   {
  return Handle<Loader>(access()->m_loader.get());
}

/// Register new IOV type if it does not (yet) exist.
pair<bool, const IOVType*> 
ConditionsManager::registerIOVType(size_t iov_type, const string& iov_name)   {
  return access()->registerIOVType(iov_type, iov_name);
}
      
/// Access IOV by its type
const IOVType* ConditionsManager::iovType (size_t iov_type) const  {
  return access()->iovType(iov_type);
}

/// Access IOV by its name
const IOVType* ConditionsManager::iovType (const string& iov_name) const   {
  return access()->iovType(iov_name);
}

/// Age conditions, which are no longer used and to be removed eventually
void ConditionsManager::age()   {
  access()->age();
}

/// Clean conditions, which are above the age limit.
void ConditionsManager::clean()   {
  access()->clean();
}

/// Full cleanup of all managed conditions.
void ConditionsManager::clear()   {
  access()->clear();
}

/// Retrieve a condition given a Detector Element and the conditions name
Condition 
ConditionsManager::get(DetElement detector,
                       const string& cond,
                       const IOV& req_validity)
{
  return access()->get(detector, cond, req_validity);
}

/// Retrieve a condition given a Detector Element path and the conditions name
Condition 
ConditionsManager::get(const string& detector_path, 
                       const string& cond,
                       const IOV& req_validity)
{
  Interna::ConditionsManagerObject* o = access();
  DetElement detector = Geometry::DetectorTools::findDaughterElement(o->m_lcdd.world(), detector_path);
  return o->get(detector, cond, req_validity);
}

/// Push all pending updates to the conditions store
void ConditionsManager::pushUpdates()   {
  access()->pushUpdates();
}

/// Lock the internal data structures. This disables calls to "register", etc.
void ConditionsManager::lock()   {
  access()->locked = 1;
}
 
/// Unlock the internal data structures. This enables calls to "register", etc.
void ConditionsManager::unlock()   {
  access()->locked = 0;
}

/// Enable all updates to the clients with the defined IOV
void ConditionsManager::enable(const IOV& required_validity)   {
  access()->enable(required_validity);
}
 
/// Prepare all updates to the clients with the defined new IOV. Changes are not yet applied
void ConditionsManager::prepare(const IOV& required_validity)   {
  access()->prepare(required_validity);
}
 
/// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
Condition 
ConditionsManager::get(const string& conditions_path, const IOV& req_validity)
{
  size_t idx = conditions_path.rfind(".");
  if ( idx != string::npos )  {
    string detector_path  = conditions_path.substr(0,idx);
    string condition_name = conditions_path.substr(idx+1, string::npos);
    return this->get(detector_path,condition_name, req_validity);
  }
  except("ConditionsManager","+++ Badly formed conditions path %s. [%s]",
         conditions_path.c_str(),Errors::linkRange().c_str());
  return Condition();
}

/// Retrieve a condition given a Detector Element and the conditions name
RangeConditions 
ConditionsManager::getRange(DetElement detector,
                            const std::string& condition_name,
                            const IOV& req_range_validity)
{
  return access()->getRange(detector, condition_name, req_range_validity);
}

/// Retrieve a condition given a Detector Element path and the conditions name
RangeConditions 
ConditionsManager::getRange(const std::string& detector_path, 
                            const std::string& condition_name,
                            const IOV& req_range_validity)
{
  Interna::ConditionsManagerObject* o = access();
  DetElement detector = Geometry::DetectorTools::findDaughterElement(o->m_lcdd.world(), detector_path);
  return o->getRange(detector, condition_name, req_range_validity);
}

/// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
RangeConditions 
ConditionsManager::getRange(const std::string& conditions_path,
                            const IOV& req_range_validity)
{
  size_t idx = conditions_path.rfind(".");
  if ( idx != string::npos )  {
    string detector_path  = conditions_path.substr(0,idx);
    string condition_name = conditions_path.substr(idx+1, string::npos);
    return this->getRange(detector_path,condition_name, req_range_validity);
  }
  except("ConditionsManager","+++ Badly formed conditions path %s. [%s]",
         conditions_path.c_str(),Errors::linkRange().c_str());
  return RangeConditions();
}
