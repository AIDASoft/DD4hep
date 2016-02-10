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
#ifndef DDCOND_CONDITIONSDATALOADER_H
#define DDCOND_CONDITIONSDATALOADER_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/ComponentProperties.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files
#include <list>
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class Entry;
    class ConditionsDataLoader;
    typedef std::list<Entry*> ConditionsStack;

    /// Interface for a generic conditions loader
    /** 
     *  Common function for all loader.
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsDataLoader : public NamedObject, public PropertyConfigurable   {
    public:
      typedef std::vector<std::string> Sources;
      typedef ConditionsDataLoader base_t;

    protected:
      /// Reference to main detector description object
      LCDD&    m_lcdd;
      /// Reference to conditions manager used to queue update requests
      ConditionsManager m_mgr;
      /// Property: input data source definitions
      Sources  m_sources;

    protected:
      /// Queue update to manager.
      Condition queueUpdate(Entry* data);
      /// Push update to manager.
      void pushUpdates();

    public:
      /// Default constructor
      ConditionsDataLoader(LCDD& lcdd, ConditionsManager mgr, const std::string nam);
      /// Default destructor
      virtual ~ConditionsDataLoader();
      /// Add data source definition to loader
      void addSource(const std::string& source);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load(DetElement det,
                          const std::string& cond,
                          const IOV& req_validity,
                          RangeConditions& conditions) = 0;
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range(DetElement det, 
                                const std::string& cond,
                                const IOV& req_validity,
                                RangeConditions& conditions) = 0;
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSDATALOADER_H    */
