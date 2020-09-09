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
#ifndef DDCOND_CONDITIONSIOVPOOL_H
#define DDCOND_CONDITIONSIOVPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"

// C/C++ include files
#include <map>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /// Pool of conditions satisfying one IOV type (epoch, run, fill, etc)
    /** 
     *  Purely internal class to the conditions manager implementation.
     *  Not at all to be accessed by clients!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsIOVPool  {
    public:
      /// Shortcut name for the actual container elements
      typedef std::shared_ptr<ConditionsPool> Element;
      /// Shortcut name for the actual conditions container
      typedef std::map<IOV::Key, Element >    Elements;      

      /// Container of IOV dependent conditions pools
      Elements elements;     //! Not ROOT persistent
      /// Reference to the IOV container
      const IOVType* type;   //! Not ROOT persistent
      
    public:
      /// Default constructor
      ConditionsIOVPool(const IOVType* type);
      /// Default destructor
      virtual ~ConditionsIOVPool();
      /// Retrieve  a condition set given the key according to their validity
      size_t select(Condition::key_type key, const IOV& req_validity, RangeConditions& result);
      /// Retrieve  a condition set given the key according to their validity
      size_t selectRange(Condition::key_type key, const IOV& req_validity, RangeConditions& result);
      /// Select all ACTIVE conditions, which do match the IOV requirement
      size_t select(const IOV& req_validity, RangeConditions& valid, IOV& cond_validity);
      /// Select all ACTIVE conditions, which do match the IOV requirement
      size_t select(const IOV&              req_validity,
                    const ConditionsSelect& valid,
                    IOV&                    cond_validity);
      /// Select all ACTIVE conditions pools, which do match the IOV requirement
      size_t select(const IOV& req_validity, Elements& valid, IOV& cond_validity);
      /// Select all ACTIVE conditions pools, which do match the IOV requirement (faster)
      size_t select(const IOV& req_validity, Elements& valid);
      /// Select all ACTIVE conditions pools, which do match the IOV requirement
      size_t select(const IOV& req_validity, std::vector<Element>& valid, IOV& cond_validity);
      /// Select all ACTIVE conditions pools, which do match the IOV requirement (faster)
      size_t select(const IOV& req_validity, std::vector<Element>& valid);

      /// Remove all key based pools with an age beyon the minimum age. 
      /** @return Number of conditions cleaned up and removed.                       */
      int clean(int max_age);

      /// Invoke cache cleanup with user defined policy
      /** @return pair<Number of pools cleared, Number of conditions cleaned up and removed> */
      int clean(const ConditionsCleanup& cleaner);
    };

  } /* End namespace cond             */
} /* End namespace dd4hep                   */

#endif // DDCOND_CONDITIONSIOVPOOL_H
