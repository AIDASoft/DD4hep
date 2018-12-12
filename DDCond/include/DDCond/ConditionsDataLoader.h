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
#ifndef DDCOND_CONDITIONSDATALOADER_H
#define DDCOND_CONDITIONSDATALOADER_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/ComponentProperties.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files
#include <list>
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class Entry;
    typedef std::list<Entry*> ConditionsStack;
    class ConditionsSlice;
    class ConditionsDescriptor;

    /// Interface for a generic conditions loader
    /** 
     *  Common function for all loader.
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsDataLoader : public NamedObject, public PropertyConfigurable   {
    public:
      typedef std::pair<std::string, IOV> Source;
      typedef std::vector<Source>         Sources;
      typedef ConditionsDataLoader        base_t;
      typedef Condition::key_type         key_type;

      typedef std::map<key_type,Condition>                          LoadedItems;
      typedef std::vector<std::pair<key_type,ConditionsLoadInfo*> > RequiredItems;

    protected:
      /// Reference to main detector description object
      Detector&         m_detector;
      /// Reference to conditions manager used to queue update requests
      ConditionsManager m_mgr;
      /// Property: input data source definitions
      Sources           m_sources;

    protected:
      /// Queue update to manager.
      //Condition queueUpdate(Entry* data);
      /// Push update to manager.
      void pushUpdates();

    public:
      /// Default constructor
      ConditionsDataLoader(Detector& description, ConditionsManager mgr, const std::string nam);
      /// Default destructor
      virtual ~ConditionsDataLoader();
      /// Optional initialization for sub-classes
      virtual void initialize()    {}
      /// Access conditions manager
      ConditionsManager manager() const  {  return m_mgr; }
      /// Access to properties
      Property& operator[](const std::string& property_name);
      /// Access to properties (CONST)
      const Property& operator[](const std::string& property_name)  const;
      /// Add data source definition to loader
      void addSource(const std::string& source);
      /// Add data source definition to loader for data corresponding to a given IOV
      void addSource(const std::string& source, const IOV& iov);
#if 0
      /// Load  a condition set given the conditions key according to their validity
      virtual size_t load_single(key_type         key,
                                 const IOV&       req_validity,
                                 RangeConditions& conditions) = 0;
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range( key_type         key,
                                 const IOV&       req_validity,
                                 RangeConditions& conditions) = 0;
#endif
      /// Load a number of conditions items from the persistent medium according to the required IOV
      virtual size_t load_many(  const IOV&       req_validity,
                                 RequiredItems&   work,
                                 LoadedItems&     loaded,
                                 IOV&             combined_validity) = 0;
    };
  }        /* End namespace cond         */
}          /* End namespace dd4hep             */
#endif     /* DDCOND_CONDITIONSDATALOADER_H    */
