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

    /// ConditionsMap class.
    /**
     *  Access mechanisms of DD4hep conditions for utilities
     *  ===================================================
     *
     *  The conditions map class is the basic interface to manage/access conditions
     *  in DD4hep. It's main use is to provide a common interface to utilities using
     *  DD4hep conditions, such as scanners, selectors, printers etc.
     *  Such utilities often require access to conditions sets based on individual 
     *  DetElement instances.
     *  
     *  Access to conditions is solely supported using this interface
     *  -- All utilities must use this interface.
     *  -- Any concrete implementation using conditions/alignment utilities
     *     must implement this interface
     *  -- Basic implmentation using STL map, multimap and unordered_map are provided.
     *  -- A special no-op implementation of this interface shall be provided to access
     *     "default" alignment conditions.
     *     This implementation shall fall-back internally to the DetElement::nominal() alignment.
     *     Known clients: VolumeManager (hence: DDG4, DDRec, etc.)
     *
     *  Though this sounds like a trivial change, the consequences concern the entire conditions
     *  and alignment handling. This interface decouples entirely the core part of DD4hep
     *  from the conditons cache handling and the alignment handling.
     *
     *  Based on this interface most utilities used to handle conditions, detectors scans
     *  to visit DetElement related condition sets, alignment and conditions printers etc.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsMap   {
    public:
      typedef Condition::key_type     key_type;
      typedef Condition::detkey_type  detkey_type;
      typedef Condition::itemkey_type itemkey_type;
      typedef Condition::Processor    Processor;
      enum {
        FIRST_ITEM =  0x0,
        LAST_ITEM  = ~0x0        
      };
      enum {
        FIRST_KEY  =  0x0ULL,
        LAST_KEY   = ~0x0ULL        
      };

    public:
      /// Standard destructor
      virtual ~ConditionsMap() = default;
      /// Insert a new entry to the map. The detector element key and the item key make a unique global conditions key
      virtual bool insert(DetElement detector, itemkey_type key, Condition condition) = 0;
      /// Interface to access conditions by hash value. The detector element key and the item key make a unique global conditions key
      virtual Condition get(DetElement detector, itemkey_type key) const = 0;
      /// Interface to scan data content of the conditions mapping
      virtual void scan(const Processor& processor) const = 0;

      /** Partial implementations for utilities accessing DetElement conditions      */

      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      /** Note: This default implementation uses 
       *        void scan(DetElement detector,
       *                  itemkey_type lower,
       *                  itemkey_type upper,
       *                  const Processor& collector)
       *        The performance depends on the concrete implementation of the scan method!
       */
      virtual std::vector<Condition> get(DetElement detector,
                                         itemkey_type lower,
                                         itemkey_type upper)  const;
      
      /// Interface to partially scan data content of the conditions mapping
      /** Note: This default implementation assumes unordered containers and hence is
       *        not the most efficient implementation!
       *        Internally it uses "scan(Processor& processor)"
       *        the subselection hence is linearly depending of the number of elements.
       *
       *        This default implementation uses 
       *        std::vector<Condition> get(DetElement detector,
       *                                  itemkey_type lower,
       *                                  itemkey_type upper)
       *        The performance depends on the concrete implementation of the scan method!
       *        
       *        Using ordered maps with "lower_bound(key)" this can be greatly improved.
       *        See the concrete implementations below.
       */
      virtual void scan(DetElement       detector,
                        itemkey_type     lower,
                        itemkey_type     upper,
                        const Processor& processor) const;
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
      virtual bool insert(DetElement detector, itemkey_type key, Condition condition)  override;
      /// Interface to access conditions by hash value
      virtual Condition get(DetElement detector, itemkey_type key) const  override;
      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      virtual std::vector<Condition> get(DetElement detector,
                                         itemkey_type lower,
                                         itemkey_type upper)  const override  {
        return this->ConditionsMap::get(detector,lower,upper);
      }
      /// Interface to scan data content of the conditions mapping
      virtual void scan(const Processor& processor) const  override;
      /// Interface to partially scan data content of the conditions mapping
      virtual void scan(DetElement         detector,
                        itemkey_type       lower,
                        itemkey_type       upper,
                        const Processor&   processor) const  override;
    };
    /// Concrete implementation of the conditions map using a left-right map
    typedef ConditionsMapping<std::map<Condition::key_type,Condition> > ConditionsTreeMap;
    /// Concrete implementation of the conditions map using a multimap
    typedef ConditionsMapping<std::multimap<Condition::key_type,Condition> > ConditionsMultiMap;
    /// Concrete implementation of the conditions map using a hashmap
    typedef ConditionsMapping<std::unordered_map<Condition::key_type,Condition> > ConditionsHashMap;
    
  }       /* End namespace Conditions               */
}         /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITIONSMAP_H        */
