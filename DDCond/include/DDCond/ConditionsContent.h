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
#ifndef DD4HEP_DDCOND_CONDITIONSCONTENT_H
#define DD4HEP_DDCOND_CONDITIONSCONTENT_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionDerived.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Base class for data loading information.
    /**
     *   Must be specialized to fit the needs of the concrete ConditionsDataLoader object.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     */
    class ConditionsLoadInfo {
    public:
      /// Default constructor
      ConditionsLoadInfo() = default;
      /// Copy constructor
      ConditionsLoadInfo(const ConditionsLoadInfo& copy) = default;
      /// Default destructor. 
      virtual ~ConditionsLoadInfo();
      /// Assignment operator
      ConditionsLoadInfo& operator=(const ConditionsLoadInfo& copy) = default;

      virtual const std::type_info& type() const = 0;
      virtual const void*           ptr()  const = 0;
      template<typename T> T*       data() const {  return (T*)ptr(); }
    };

    /// Conditions content object. Defines which conditions should be loaded by the ConditionsManager.
    /**
     *  Object contains set of required conditions keys to be loaded to the user pool.
     *  It also contains the load information for the required conditions (conditions addresses).
     *  The address objects depend on the actual loader mechanism and must be specified the user.
     *  The information is then chained through the calls and made availible to the loader object.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsContent  {
    public:
      /// Concrete class for data loading information.
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      template <typename T> struct LoadInfo : public ConditionsLoadInfo {
        T info;
        LoadInfo(const T& i) : info(i) {}
        LoadInfo()                  = default;
        LoadInfo(const LoadInfo& c) = default;
        virtual ~LoadInfo()         = default;
        LoadInfo& operator=(const LoadInfo& copy) = default;
        virtual const std::type_info& type() const { return typeid(T); }
        virtual const void*           ptr()  const { return &info;     }
      };
     
      typedef Condition::key_type                      key_type;
      typedef std::map<key_type,ConditionDependency* > Dependencies;
      typedef std::map<key_type,ConditionsLoadInfo* >  Conditions;

    protected:
      /// Container of conditions required by this content
      Conditions        m_conditions;
      /// Container of derived conditions required by this content
      Dependencies      m_derived;

    private:
      /// Default assignment operator
      ConditionsContent& operator=(const ConditionsContent& copy) = delete;
      /// Copy constructor
      ConditionsContent(const ConditionsContent& copy) = delete;

    public:
      /// Default constructor
      ConditionsContent();
      /// Default destructor. 
      virtual ~ConditionsContent();
      /// Access to the real condition entries to be loaded
      const Conditions& conditions() const  { return m_conditions;   }
      /// Access to the derived condition entries to be computed
      const Dependencies& derived() const   { return m_derived;      }
      /// Clear the conditions content definitions
      void clear();
      /// Remove a condition from the content
      bool remove(key_type condition);
      /// Add a new conditions key representing a real (not derived) condition
      bool insertKey(key_type hash)   {
        return m_conditions.insert(std::make_pair(hash,(ConditionsLoadInfo*)0)).second;
      }
      /// Add a new conditions key. T must inherit from class ConditionsContent::Info
      template <typename T> bool insertKey(key_type hash, const T& info)   {
        ConditionsLoadInfo* i = new LoadInfo<T>(info);
        bool ret = m_conditions.insert(std::make_pair(hash,i)).second;
        if ( !ret ) delete i;
        return ret;
      }
      /// Add a new shared conditions dependency
      bool insertDependency(ConditionDependency* dep)   {
        bool ret = m_derived.insert(std::make_pair(dep->key(),dep)).second;
        if ( ret ) dep->addRef();
        return ret;
      }
      /// Create load-info object
      template <typename T> static LoadInfo<T> loadInfo(const T& t) { return LoadInfo<T>(t);  }
    };
  }        /* End namespace Conditions               */
}          /* End namespace DD4hep                   */
#endif     /* DD4HEP_DDCOND_CONDITIONSCONTENT_H      */
