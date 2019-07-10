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
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace dd4hep;

/// Interface to partially scan data content of the conditions mapping
void ConditionsMap::scan(DetElement   detector,
                         Condition::itemkey_type lower,
                         Condition::itemkey_type upper,
                         const Condition::Processor&   processor) const
{
  /// Helper to implement partial scans.
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  struct Scanner : public Condition::Processor   {
    Condition::key_type lower, upper;
    const Condition::Processor& processor;
    /// Constructor
    Scanner(Condition::key_type low, Condition::key_type up, const Condition::Processor& p)
      : lower(low), upper(up), processor(p) {}
    /// Conditions callback for object processing
    virtual int process(Condition c)  const override  {
      Condition::key_type h = c->hash;
      if ( h >= lower && h <= upper )
        return processor(c);
      return 0;
    }
  };
  if ( detector.isValid() )   {
    Condition::detkey_type det_key = detector.key();
    Condition::key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    Condition::key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    Scanner  scn(low,up,processor);
    this->scan(scn);
    return;
  }
  dd4hep::except("ConditionsMap","Cannot scan conditions map for conditions of an invalid detector element!");
}

/// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
std::vector<Condition> ConditionsMap::get(DetElement detector,
                                          Condition::itemkey_type lower,
                                          Condition::itemkey_type upper)  const   {
  /// Helper to implement partial scans.
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  struct Scanner : public Condition::Processor   {
    Condition::key_type lower, upper;
    std::vector<Condition>& result;
    /// Constructor
    Scanner(Condition::key_type low, Condition::key_type up, std::vector<Condition>& r) : lower(low), upper(up), result(r) {}
    /// Conditions callback for object processing
    virtual int process(Condition c)  const override  {
      Condition::key_type h = c->hash;
      if ( h >= lower && h <= upper )   {
        result.emplace_back(c);
        return 1;
      }
      return 0;
    }
  };
  std::vector<Condition> result;
  if ( detector.isValid() )   {
    Condition::detkey_type det_key = detector.key();
    Condition::key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    Condition::key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    Scanner  scn(low,up,result);
    this->scan(scn);
    return result;
  }
  dd4hep::except("ConditionsMap","Cannot scan conditions map for conditions of an invalid detector element!");
  return result;
}

/// Insert a new entry to the map
template <typename T>
bool ConditionsMapping<T>::insert(DetElement detector, Condition::itemkey_type key, Condition condition)   {
  auto res = data.emplace(ConditionKey(detector,key).hash,condition);
  return res.second;
}

/// Interface to access conditions by hash value
template <typename T>
Condition ConditionsMapping<T>::get(DetElement detector, Condition::itemkey_type key) const   {
  auto res = data.find(ConditionKey(detector,key).hash);
  return (res == data.end()) ? Condition() : res->second;
}

/// Interface to scan data content of the conditions mapping
template <typename T>
void ConditionsMapping<T>::scan(const Condition::Processor& processor) const  {
  for( const auto& i : data )
    processor(i);
}

/// Interface to partially scan data content of the conditions mapping
template <typename T>
void ConditionsMapping<T>::scan(DetElement   detector,
                                Condition::itemkey_type lower,
                                Condition::itemkey_type upper,
                                const Condition::Processor&   processor) const   {
  if ( detector.isValid() )   {
    Condition::detkey_type det_key = detector.key();
    Condition::key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    Condition::key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    typename T::const_iterator first = data.lower_bound(low);
    for(; first != data.end() && (*first).first <= up; ++first )
      processor((*first).second);
    return;
  }
  dd4hep::except("ConditionsMap","Cannot scan conditions map for conditions of an invalid detector element!");
}


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Specialization: Insert a new entry to the map
  template <>
  bool ConditionsMapping<std::multimap<Condition::key_type,Condition> >
  ::insert(DetElement detector, Condition::itemkey_type key, Condition condition)   {
    data.emplace(ConditionKey(detector,key).hash,condition);
    return true;
  }

  /// Specialization: Interface to partially scan data content of the conditions mapping
  template <>
  void ConditionsMapping<std::unordered_map<Condition::key_type,Condition> >
  ::scan(DetElement detector, Condition::itemkey_type lower,
         Condition::itemkey_type upper,
         const Condition::Processor& proc) const
  {
    this->ConditionsMap::scan(detector, lower, upper, proc);
  }


  template class ConditionsMapping<std::map<Condition::key_type,Condition> >;
  template class ConditionsMapping<std::multimap<Condition::key_type,Condition> >;
  template class ConditionsMapping<std::unordered_map<Condition::key_type,Condition> >;
}         /* End namespace dd4hep                   */
