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
#ifndef DD4HEP_DDCOND_CONDITIONSCONTENT_H
#define DD4HEP_DDCOND_CONDITIONSCONTENT_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionDerived.h"

// C/C++ include files
#include <memory>
#include <unordered_map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

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
      int refCount = 0;
    protected:
      /// Default destructor. 
      virtual ~ConditionsLoadInfo();
    public:
      /// Default constructor
      ConditionsLoadInfo();
      /// Move constructor
      ConditionsLoadInfo(ConditionsLoadInfo&& copy) = delete;
      /// Copy constructor
      ConditionsLoadInfo(const ConditionsLoadInfo& copy) = delete;
      /// Assignment operator
      ConditionsLoadInfo& operator=(const ConditionsLoadInfo& copy) = delete;
      ConditionsLoadInfo* addRef()  {  ++refCount;  return this; }
      void release() {   --refCount; if ( refCount <= 0 ) delete this;  }
      virtual const std::type_info& type() const = 0;
      virtual const void*           ptr()  const = 0;
      virtual ConditionsLoadInfo*   clone()  const = 0;
      virtual std::string           toString() const = 0;
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
      template <typename T> class LoadInfo : public ConditionsLoadInfo {
      public:
        T info;
        LoadInfo(const T& i) : info(i) {}
        LoadInfo()                  = default;
        LoadInfo(const LoadInfo& c) = delete;
        virtual ~LoadInfo()         = default;
        LoadInfo& operator=(const LoadInfo& copy) = delete;
        virtual const std::type_info& type()   const  override
        { return typeid(T);                      }
        virtual const void*           ptr()    const  override
        { return &info;                          }
        virtual ConditionsLoadInfo*   clone()  const  override
        { return new LoadInfo<T>(info);          }
        virtual std::string           toString() const override;
      };
      
      // Looks like tree-maps are a bit slower.
      typedef std::map<Condition::key_type,ConditionDependency* > Dependencies;
      typedef std::map<Condition::key_type,ConditionsLoadInfo* >  Conditions;
      //typedef std::unordered_map<Condition::key_type,ConditionDependency* > Dependencies;
      //typedef std::unordered_map<Condition::key_type,ConditionsLoadInfo* >  Conditions;

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
      Conditions& conditions()              { return m_conditions;   }
      /// Access to the real condition entries to be loaded (CONST)
      const Conditions& conditions() const  { return m_conditions;   }
      /// Access to the derived condition entries to be computed
      Dependencies& derived()               { return m_derived;      }
      /// Access to the derived condition entries to be computed (CONST)
      const Dependencies& derived() const   { return m_derived;      }
      /// Clear the conditions content definitions
      void clear();
      /// Merge the content of "to_add" into the this content
      void merge(const ConditionsContent& to_add);
      /// Remove a condition from the content
      bool remove(Condition::key_type condition);
      /// Add a new conditions key representing a real (not derived) condition
      std::pair<Condition::key_type, ConditionsLoadInfo*>
      insertKey(Condition::key_type hash);
      /// Add a new conditions key. T must inherit from class ConditionsContent::Info
      std::pair<Condition::key_type, ConditionsLoadInfo*>
      addLocationInfo(Condition::key_type hash, ConditionsLoadInfo* info);
      /// Add a new conditions key. T must inherit from class ConditionsContent::Info
      template <typename T> std::pair<Condition::key_type, ConditionsLoadInfo*>
      addLocation(Condition::key_type hash, const T& info)   {
        return addLocationInfo(hash, new LoadInfo<T>(info));
      }
      /// Add a new shared conditions dependency.
      template <typename T> std::pair<Condition::key_type, ConditionsLoadInfo*>
      addLocation(DetElement de, Condition::itemkey_type item, const T& info)   {
        return addLocationInfo(ConditionKey(de, item).hash, new LoadInfo<T>(info));
      }
      /// Add a new shared conditions dependency
      std::pair<Condition::key_type, ConditionDependency*>
      addDependency(ConditionDependency* dep);
      /// Add a new conditions dependency (Built internally from arguments)
      std::pair<Condition::key_type, ConditionDependency*>
      addDependency(DetElement de, Condition::itemkey_type item, ConditionUpdateCall* callback);
    };

    template <> inline
    std::string ConditionsContent::LoadInfo<std::string>::toString() const   {
      return this->info;
    }

  }        /* End namespace cond               */
}          /* End namespace dd4hep                   */
#endif     /* DD4HEP_DDCOND_CONDITIONSCONTENT_H      */
