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
#ifndef DD4HEP_GEOMETRY_CONDITIONSMAP_H
#define DD4HEP_GEOMETRY_CONDITIONSMAP_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/Detector.h"

// C/C++ include files
#include <map>
#include <unordered_map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    /// ConditionsMap class
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsMap   {
    public:
      typedef Condition::key_type key_type;
    public:
      /// Standard destructor
      virtual ~ConditionsMap() = default;
      /// Insert a new entry to the map
      virtual bool insert(DetElement detector, unsigned int key, Condition condition) = 0;
      /// Interface to access conditions by hash value
      virtual Condition get(DetElement detector, unsigned int key) const = 0;
      /// Interface to scan data content of the conditions mapping
      virtual void scan(Condition::Processor& processor) const = 0;
    };

    /// Concrete ConditionsMap implementation class using externally defined containers
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template <typename T>
    class ConditionsMapping : virtual public ConditionsMap   {
    public:
      /// The actual data container
      T   data;
    public:
      /// Standard constructor to construct an empty object
      ConditionsMapping() = default;
      /// Standard destructor
      virtual ~ConditionsMapping() = default;
      /// No copy constructor
      ConditionsMapping(const ConditionsMapping& copy) = delete;
      /// No assignment
      ConditionsMapping& operator=(const ConditionsMapping& copy) = delete;
      /// Insert a new entry to the map
      virtual bool insert(DetElement detector, unsigned int key, Condition condition);
      /// Interface to access conditions by hash value
      virtual Condition get(DetElement detector, unsigned int key) const;
      /// Interface to scan data content of the conditions mapping
      virtual void scan(Condition::Processor& processor) const;
    };
    /// Concrete implementation of the conditions map using a left-right map
    typedef ConditionsMapping<std::map<Condition::key_type,Condition> >           ConditionsTreeMap;
    /// Concrete implementation of the conditions map using a hashmap
    typedef ConditionsMapping<std::unordered_map<Condition::key_type,Condition> > ConditionsHashMap;
    
  }       /* End namespace Conditions               */
}         /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITIONSMAP_H        */
