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
#ifndef DD4HEP_CONDITIONS_CONDITIONSSTACK_H
#define DD4HEP_CONDITIONS_CONDITIONSSTACK_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Memory.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    namespace ConditionsInterna {
      // Forward declarations
      class Entry;
    }

    /// Implementation of a stack of conditions assembled before application
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionsStack {
    public:
      friend class dd4hep_ptr<ConditionsStack>;

      typedef ConditionsInterna::Entry Entry;
      typedef std::map<std::string,Entry*> Stack;

    protected:
      /// The subdetector specific map of conditions caches
      Stack m_stack;

      /// Standard constructor
      ConditionsStack();
      /// Standard destructor
      virtual ~ConditionsStack();

    public:
      /// Static client accessor
      static ConditionsStack& get(); 
      /// Create an conditions stack instance. The creation of a second instance will be refused.
      static void create();
      /// Check existence of conditions stack
      static bool exists();
      /// Push new entry into the stack
      void insert(dd4hep_ptr<Entry>& data);
      /// Clear data content and remove the slignment stack
      void release();
      /// Access size of the conditions stack
      size_t size() const  {  return m_stack.size(); }
      /// Retrieve an conditions entry of the current stack
      dd4hep_ptr<Entry> pop();
      /// Get all path entries to be aligned. Note: transient!
      std::vector<const Entry*> entries() const;
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_CONDITIONSSTACK_H  */
