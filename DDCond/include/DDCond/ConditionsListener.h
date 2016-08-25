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
#ifndef DD4HEP_CONDITIONS_CONDITIONSLISTENER_H
#define DD4HEP_CONDITIONS_CONDITIONSLISTENER_H

// Framework include files
#include "DD4hep/Conditions.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Forward declarations
  class IOV;
  class IOVType;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Forward declarations
    class ConditionsPool;

    /// Base class to be implemented by objects to listen on condition callbacks
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsListener  {
    public:
      /// Default constructor
      ConditionsListener();
      /// Default destructor
      virtual ~ConditionsListener();
      /// ConditionsListener dummy implementation: onRegister new IOV type
      virtual void onRegisterIOVType(const IOVType* /* type */, void* /* param */)  { }
      /// ConditionsListener dummy implementation: onRegister new condition
      virtual void onRegisterCondition(Condition /* cond */, void* /* param */)  { }
      /// ConditionsListener dummy implementation: onRegister new conditions pool
      virtual void onRegisterPool(ConditionsPool* /* pool */, void* /* param */)  { }

      /// ConditionsListener dummy implementation: onRemove a condition
      virtual void onRemoveCondition(Condition /* cond */, void* /* param */)  { }
      /// ConditionsListener dummy implementation: onRemove a conditions pool
      virtual void onRemovePool(ConditionsPool* /* pool */, void* /* param */)  { }
     };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_CONDITIONSLISTENER_H  */
