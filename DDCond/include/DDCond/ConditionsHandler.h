// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_CONDITIONS_CONDITIONSHANDLER_H
#define DD4HEP_CONDITIONS_CONDITIONSHANDLER_H

// Framework include files
#include "DDCond/ConditionsStack.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Forward declarations
    class ConditionsStack;
    class LCDD;

    /// Class caching all known conditions operations for one LCDD instance.
    /**
     *  Internally the instances are fragmented to subdetectors defined
     *  by the next-to-top level detector elements.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionsHandler  {
      friend class LCDD;

    protected:
      LCDD&       m_lcdd;
      /// Reference count
      int         m_refCount;

    protected:
      /// Default constructor
      ConditionsHandler(LCDD& lcdd);
      /// Default destructor
      virtual ~ConditionsHandler();
      /// Population entry: Apply a complete stack of ordered conditionss to the geometry structure
      void apply(ConditionsStack& stack);

    public:
      /// Create and install a new instance tree
      static void install(LCDD& lcdd);
      /// Unregister and delete a tree instance
      static void uninstall(LCDD& lcdd);

      /// Functor callback
      static void triggerUpdate(DetElement detector);
      
      /// Add reference count
      int addRef();
      /// Release object. If reference count goes to NULL, automatic deletion is triggered.
      int release();
      /// Open a new transaction stack (Note: only one stack allowed!)
      void openTransaction();
      /// Close existing transaction stack and apply all conditionss
      void closeTransaction();
    };

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_CONDITIONS_CONDITIONSHANDLER_H       */
