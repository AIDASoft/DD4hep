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
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace DD4hep::Conditions;

/// Interface to partially scan data content of the conditions mapping
void ConditionsMap::scan(DetElement   detector,
                         itemkey_type lower,
                         itemkey_type upper,
                         const Processor&   processor) const
{
  /// Heklper to implement partial scans.
  struct Scanner : public Processor   {
    key_type lower, upper;
    const Processor& processor;
    /// Constructor
    Scanner(key_type low, key_type up, const Processor& p) : lower(low), upper(up), processor(p) {}
    /// Conditions callback for object processing
    virtual int process(Condition c)  const override  {
      key_type h = c->hash;
      if ( h >= lower && h <= upper )
        return processor(c);
      return 0;
    }
  };
  if ( detector.isValid() )   {
    ConditionKey::detkey_type det_key = detector.key();
    key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    Scanner  scn(low,up,processor);
    this->scan(scn);
    return;
  }
  DD4hep::except("ConditionsMap","Cannot scan conditions map for conditions of an invalid detector element!");
}

/// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
std::vector<Condition> ConditionsMap::get(DetElement detector,
                                          itemkey_type lower,
                                          itemkey_type upper)  const   {
  /// Heklper to implement partial scans.
  struct Scanner : public Processor   {
    key_type lower, upper;
    std::vector<Condition>& result;
    /// Constructor
    Scanner(key_type low, key_type up, std::vector<Condition>& r) : lower(low), upper(up), result(r) {}
    /// Conditions callback for object processing
    virtual int process(Condition c)  const override  {
      key_type h = c->hash;
      if ( h >= lower && h <= upper )   {
        result.push_back(c);
        return 1;
      }
      return 0;
    }
  };
  std::vector<Condition> result;
  if ( detector.isValid() )   {
    ConditionKey::detkey_type det_key = detector.key();
    key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    Scanner  scn(low,up,result);
    this->scan(scn);
    return result;
  }
  DD4hep::except("ConditionsMap","Cannot scan conditions map for conditions of an invalid detector element!");
  return result;
}

/// Insert a new entry to the map
template <typename T>
bool ConditionsMapping<T>::insert(DetElement detector, unsigned int key, Condition condition)   {
  auto res = data.insert(std::make_pair(ConditionKey(detector,key).hash,condition));
  return res.second;
}

/// Interface to access conditions by hash value
template <typename T>
Condition ConditionsMapping<T>::get(DetElement detector, unsigned int key) const   {
  auto res = data.find(ConditionKey(detector,key).hash);
  return (res == data.end()) ? Condition() : res->second;
}

/// Interface to scan data content of the conditions mapping
template <typename T>
void ConditionsMapping<T>::scan(const Processor& processor) const  {
  for( const auto& i : data )
    processor(i);
}

/// Interface to partially scan data content of the conditions mapping
template <typename T>
void ConditionsMapping<T>::scan(DetElement   detector,
                                itemkey_type lower,
                                itemkey_type upper,
                                const Processor&   processor) const   {
  if ( detector.isValid() )   {
    ConditionKey::detkey_type det_key = detector.key();
    key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    typename T::const_iterator first = data.lower_bound(low);
    for(; first != data.end() && (*first).first <= up; ++first )
      processor((*first).second);
    return;
  }
  DD4hep::except("ConditionsMap","Cannot scan conditions map for conditions of an invalid detector element!");
}


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    /// Specialization: Insert a new entry to the map
    template <>
    bool ConditionsMapping<std::multimap<Condition::key_type,Condition> >
    ::insert(DetElement detector, unsigned int key, Condition condition)   {
      data.insert(std::make_pair(ConditionKey(detector,key).hash,condition));
      return true;
    }

    /// Specialization: Interface to partially scan data content of the conditions mapping
    template <>
    void ConditionsMapping<std::unordered_map<Condition::key_type,Condition> >
    ::scan(DetElement detector, itemkey_type lower, itemkey_type upper, const Processor& proc) const   {
      this->ConditionsMap::scan(detector, lower, upper, proc);
    }


    template class ConditionsMapping<std::map<Condition::key_type,Condition> >;
    template class ConditionsMapping<std::multimap<Condition::key_type,Condition> >;
    template class ConditionsMapping<std::unordered_map<Condition::key_type,Condition> >;

  }       /* End namespace Conditions               */
}         /* End namespace DD4hep                   */
