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
#ifndef DDCOND_CONDITIONSIOVPOOL_H
#define DDCOND_CONDITIONSIOVPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Forward declarations
    class ConditionsDataLoader;

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
      typedef ConditionsPool* Entry;
      typedef std::map<std::pair<int,int>, Entry > Entries;
      Entries        entries;
    public:
      /// Default constructor
      ConditionsIOVPool();
      /// Default destructor
      virtual ~ConditionsIOVPool();
      /// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
      void __find(DetElement detector,
                  const std::string& condition_name,
                  const IOV& req_validity,
                  RangeConditions& result);
      /// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
      void __find_range(DetElement detector,
                        const std::string& condition_name,
                        const IOV& req_validity,
                        RangeConditions& result);
      /// Select all ACTIVE conditions, which do no longer match the IOV requirement
      void __select_expired(const IOV& required_validity, 
                            RangeConditions& result);
      void __update_expired(ConditionsDataLoader& loader,
                            ConditionsPool& pool,
                            RangeConditions& expired,
                            const IOV& required_validity);
   };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /*  DDCOND_CONDITIONSIOVPOOL_H   */
