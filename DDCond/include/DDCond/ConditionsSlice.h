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
#ifndef DD4HEP_DDCOND_CONDITIONSSLICE_H
#define DD4HEP_DDCOND_CONDITIONSSLICE_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionDerived.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsDependencyCollection.h"

// C/C++ include files


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class UserPool;
    class ConditionsSlice;

    /// Conditions slice object. Defines which conditions should be loaded by the ConditionsManager.
    /**
     *  Object contains set of required conditions keys to be loaded to the user pool.
     *  It alkso contains the load information for the required conditions (conditions addresses).
     *  The address objects depend on the actual loader mechanism and must be specified the user.
     *  The information is then chained through the calls and made availible to the loader object.
     *
     *  On return it contains the individual condition load information.
     *
     *  Referenced by: ConditonsUserPool, ConditionsManager
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsSlice  {
    public:

      /// Base class for data loading information.
      /**
       *   Must be specialized to fit the needs of the concrete ConditionsDataLoader object.
       *
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      struct Info {
        /// Default destructor. 
        virtual ~Info();
        virtual const std::type_info& type() const = 0;
        virtual const void*           ptr()  const = 0;
        template<typename T> T*       data() const {  return (T*)ptr(); }
      };

      /// Concrete class for data loading information.
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      template <typename T> struct LoadInfo : public Info {
        T info;
        LoadInfo()           = default;
        LoadInfo(const T& i) : info(i) {}
        virtual ~LoadInfo()  = default;
        LoadInfo& operator=(const LoadInfo& copy) {
          if ( &copy != this ) info = copy.info;
          return *this;
        }
        virtual const std::type_info& type() const { return typeid(T); }
        virtual const void*           ptr() const  { return &info;     }
      };
      template <typename T> static LoadInfo<T> loadInfo(const T& t)
      { return LoadInfo<T>(t);                                         }
      
      /// Slice entry class. Describes all information to load a condition.
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      struct Entry  {
        friend class ConditionsSlice;
      private:
        /// Default assignment operator
        Entry& operator=(const Entry& copy) = delete;
        /// Copy constructor (special!)
        Entry(const Entry& copy, Info* l);
      protected:
      public:
        ConditionKey          key;
        ConditionDependency*  dependency = 0;
        Condition::mask_type  mask       = 0;
        Info*                 loadinfo   = 0;
        /// Default constructor
        Entry() = default;
        /// Initializing constructor
        Entry(const ConditionKey& k, Info* l);
        /// Default destructor. 
        virtual ~Entry();
        /// Clone the entry
        virtual Entry* clone();
      };

      /// Concrete slice entry class. Includes the load information
      /**
       *   T must be specialized and must 
       *
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      template <typename T> class ConditionsLoaderEntry : public Entry, public T
      {
        friend class ConditionsSlice;
        /// No default constructor
        ConditionsLoaderEntry() = delete;
        /// Default assignment operator
        ConditionsLoaderEntry& operator=(const ConditionsLoaderEntry& copy) = delete;
        /// Copy constructor
        ConditionsLoaderEntry(const ConditionsLoaderEntry& copy)
          : Entry(copy, this), T(copy) { }
      private:
        /// Initializing constructor
        ConditionsLoaderEntry(const ConditionKey& k, const T& d)
          : Entry(k, this), T(d)       { }
        virtual ~ConditionsLoaderEntry() = default;
        virtual Entry* clone()   { return new ConditionsLoaderEntry(*this); }
        virtual const void* data() const {  return (T*)this;  }
      };
      
      typedef Condition::key_type                key_type;
      typedef ConditionDependency                Dependency;
      typedef ConditionsDependencyCollection     Dependencies;
      typedef std::map<key_type,Entry*>          ConditionsProxy;
      
    public:
      /// Reference to the conditions manager.
      /** Not used by the object, simple for convenience.
       * Then all actora are lumped together.
       */
      ConditionsManager    manager;

    protected:
      /// Reference to the user pool
      dd4hep_ptr<UserPool> m_pool;
      ConditionsProxy      m_conditions;
      ConditionsProxy      m_derived;
      Dependencies         m_dependencies;
      IOV                  m_iov;
      /// Default assignment operator
      ConditionsSlice& operator=(const ConditionsSlice& copy) = delete;

      /// Add a new conditions entry
      bool insert_condition(Entry* entry);

    public:
      /// Default constructor
      ConditionsSlice() : manager(), m_iov(0) {}
      /// Initializing constructor
      ConditionsSlice(ConditionsManager m, const IOV& value) : manager(m), m_iov(value) {}
      /// Copy constructor (Special, partial copy only. Hence no assignment!)
      ConditionsSlice(const ConditionsSlice& copy);
      /// Default destructor. 
      virtual ~ConditionsSlice();
      /// Required IOV
      const IOV& iov() const                     { return m_iov;          }
      /// Set a new IOV
      void setNewIOV(const IOV& value)           { m_iov = value;         }
      /// Access the user condition pool
      dd4hep_ptr<UserPool>& pool()               { return m_pool;         }
      /// Access dependency list
      const Dependencies& dependencies() const   { return m_dependencies; }
      /// Access to the real condition entries to be loaded
      const ConditionsProxy& conditions() const  { return m_conditions;   }
      /// Access to the derived condition entries to be computed
      const ConditionsProxy& derived() const     { return m_derived;      }
      /// Number of entries
      size_t size() const  { return m_conditions.size()+m_derived.size(); }
      /// Clear the container. Destroys the contained stuff
      void clear();
      /// Clear the conditions access and the user pool.
      void reset();
      /// Add a new conditions dependency collection
      void insert(const Dependencies& deps);
      /// Add a new shared conditions dependency
      bool insert(Dependency* dependency);
      /// Add a new conditions key. T must inherot from class ConditionsSlice::Info
      template <typename T=Info> bool insert(const ConditionKey& key, const T& loadinfo)   {
        Entry* e = new ConditionsLoaderEntry<T>(key,loadinfo);
        return insert_condition(e);
      }
    };

    /// Populate the conditions slice from the conditions manager (convenience)
    ConditionsSlice* createSlice(ConditionsManager mgr, const IOVType& typ);
  }        /* End namespace Conditions               */
}          /* End namespace DD4hep                   */
#endif     /* DD4HEP_DDCOND_CONDITIONSSLICE_H        */
