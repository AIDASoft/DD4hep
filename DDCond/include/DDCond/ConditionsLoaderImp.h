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
#ifndef DDCOND_CONDITIONSLOADERIMP_H
#define DDCOND_CONDITIONSLOADERIMP_H

// Framework include files
#include "DDCond/ConditionsPool.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Concrete class to interface conditions loading from DetElements
    /** 
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsLoaderImp : public ConditionsLoader  {
    protected:
      /// Reference to conditions manager
      ConditionsManager m_manager;
      /// Referenec count
      long              m_refCount;
    public:
      /// Default constructor
      ConditionsLoaderImp(ConditionsManager mgr);
      /// Default destructor
      virtual ~ConditionsLoaderImp();
      /// Addreference count. Use object
      virtual void addRef();
      /// Release object. The client may not use any reference any further.
      virtual void release();
      /// Access the conditions loading mechanism
      virtual Condition get(key_type key, const iov_type& iov);
      /// Access the conditions loading mechanism. Only conditions in the user pool will be accessed.
      virtual Condition get(key_type key, const UserPool& pool);
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSDATALOADERIMP_H    */
