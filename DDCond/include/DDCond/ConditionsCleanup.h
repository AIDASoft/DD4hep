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
#ifndef DDCOND_CONDITIONSCLEANUP_H
#define DDCOND_CONDITIONSCLEANUP_H

// Framework include files
#include "DDCond/ConditionsPool.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /// Base class to handle conditions cleanups
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsCleanup  {
    public:
      /// Default constructor
      ConditionsCleanup() = default;
      /// Copy constructor
      ConditionsCleanup(const ConditionsCleanup& c) = default;
      /// Default destructor
      virtual ~ConditionsCleanup() = default;
      /// Assignment operator
      ConditionsCleanup& operator=(const ConditionsCleanup& c) = default;
      /// Request cleanup operation of IOV POOL
      virtual bool operator()(const ConditionsIOVPool& iov_pool)  const;
      /// Request cleanup operation of regular conditiions pool
      virtual bool operator()(const ConditionsPool& pool)  const;
    };

   /// Base class to handle conditions cleanups
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsFullCleanup : public ConditionsCleanup {
    public:
      /// Default constructor
      ConditionsFullCleanup() = default;
      /// Copy constructor
      ConditionsFullCleanup(const ConditionsFullCleanup& c) = default;
      /// Default destructor
      virtual ~ConditionsFullCleanup() = default;
      /// Assignment operator
      ConditionsFullCleanup& operator=(const ConditionsFullCleanup& c) = default;
      /// Request cleanup operation of IOV POOL
      virtual bool operator()(const ConditionsIOVPool& iov_pool)  const  override;
      /// Request cleanup operation of regular conditiions pool
      virtual bool operator()(const ConditionsPool& pool)  const  override;
    };
  } /* End namespace cond                   */
} /* End namespace dd4hep                   */

#endif     /* DDCOND_CONDITIONSCLEANUP_H    */
