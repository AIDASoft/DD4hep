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
    class ConditionsDescriptor;

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
        /// Default destructor. 
        virtual ~ConditionsLoadInfo();
        virtual const std::type_info& type() const = 0;
        virtual const void*           ptr()  const = 0;
        template<typename T> T*       data() const {  return (T*)ptr(); }
      };

    /// Slice entry class. Describes all information to load a condition.
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     */
    class ConditionsDescriptor  {
      friend class ConditionsSlice;
    private:
      int                   refCount   = 0;
    public:
      ConditionKey          key;
      ConditionDependency*  dependency = 0;
      ConditionsLoadInfo*   loadinfo   = 0;

      // These are mine. Do not even dare to call any of these two!
      ConditionsDescriptor* addRef() { ++refCount;  return this;           }
      void release()                 { if ( --refCount <= 0 ) delete this; }

    private:
      /// Default constructor
      ConditionsDescriptor() = delete;
      /// Default destructor. 
      virtual ~ConditionsDescriptor();
      /// Copy constructor (special!)
      ConditionsDescriptor(const ConditionsDescriptor& copy) = delete;
      /// Default assignment operator
      ConditionsDescriptor& operator=(const ConditionsDescriptor& copy) = delete;

    protected:
      /// Initializing constructor
      ConditionsDescriptor(const ConditionKey& k, ConditionsLoadInfo* l);
      /// Initializing constructor
      ConditionsDescriptor(ConditionDependency* d, ConditionsLoadInfo* l);
    };
    
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

      typedef ConditionsDescriptor Descriptor;
      
      /// Concrete class for data loading information.
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      template <typename T> struct LoadInfo : public ConditionsLoadInfo {
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
      /// Concrete class for NO data loading information.
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      struct NoLoadInfo : public ConditionsLoadInfo {
        NoLoadInfo()           = default;
        NoLoadInfo(const NoLoadInfo& i) = default;
        virtual ~NoLoadInfo()  = default;
        NoLoadInfo& operator=(const NoLoadInfo& copy)  = default;
        virtual const std::type_info& type() const { return typeid(void); }
        virtual const void*           ptr() const  { return 0;            }
      };
      
      /// Concrete slice entry class. Includes the load information
      /**
       *   T must be specialized and must 
       *
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       */
      template <typename T> class ConditionsLoaderDescriptor : public Descriptor, public T
      {
        friend class ConditionsSlice;
        /// No default constructor
        ConditionsLoaderDescriptor() = delete;
        /// Default assignment operator
        ConditionsLoaderDescriptor& operator=(const ConditionsLoaderDescriptor& copy) = delete;
        /// Copy constructor
        ConditionsLoaderDescriptor(const ConditionsLoaderDescriptor& copy) = delete;

      private:
        /// Initializing constructor
        ConditionsLoaderDescriptor(const ConditionKey& k, const T& d)
          : Descriptor(k, this), T(d)       { }
        virtual ~ConditionsLoaderDescriptor() = default;
        virtual const void* data() const {  return (T*)this;  }
      };
      
      typedef Condition::key_type            key_type;
      typedef ConditionDependency            Dependency;
      typedef std::map<key_type,Descriptor*> ConditionsProxy;

    public:
      /// Reference to the conditions manager.
      /** Not used by the object, simple for convenience.
       *  Then all actors are lumped together, which are used by the client code.
       */
      ConditionsManager    manager;
      /// Reference to the user pool managing all conditions of this slice
      dd4hep_ptr<UserPool> pool;

    protected:
      /// Container of conditions required by this slice
      ConditionsProxy      m_conditions;
      /// Container of derived conditions required by this slice
      ConditionsProxy      m_derived;
      /// If flag conditonsManager["OutputUnloadedConditions"]=true: will contain conditions not loaded
      ConditionsProxy      m_missingConditions;
      /// If flag conditonsManager["OutputUnloadedConditions"]=true: will contain conditions not computed
      ConditionsProxy      m_missingDerivations;
      
      /// Default assignment operator
      ConditionsSlice& operator=(const ConditionsSlice& copy) = delete;

      /// Add a new conditions descriptor entry
      bool insert_condition(Descriptor* entry);
      /// Add a new condition to the user pool
      bool insert_condition(Condition condition);

    public:
      /// Default constructor
      ConditionsSlice() = delete;
      /// Initializing constructor
      ConditionsSlice(ConditionsManager m);
      /// Copy constructor (Special, partial copy only. Hence no assignment!)
      ConditionsSlice(const ConditionsSlice& copy);
      /// Default destructor. 
      virtual ~ConditionsSlice();
      /// Access to the real condition entries to be loaded
      const ConditionsProxy& conditions() const  { return m_conditions;   }
      /// Access to the derived condition entries to be computed
      const ConditionsProxy& derived() const     { return m_derived;      }
      /// Number of entries
      size_t size() const  { return m_conditions.size()+m_derived.size(); }
      /// Access the map of missing conditions (only valid after preparation)
      ConditionsProxy& missingConditions()  { return m_missingConditions; }
      /// Access the map of missing computational conditions (only valid after preparation)
      ConditionsProxy& missingDerivations() { return m_missingDerivations;}
      /// Clear the container. Destroys the contained stuff
      void clear();
      /// Clear the conditions access and the user pool.
      void reset();
      /// Remove a new shared conditions dependency
      bool remove(Dependency* dependency);
      /// Remove a condition from the slice
      bool remove(Condition condition);

      /// Add a new conditions dependency collection
      void insert(const ConditionsDependencyCollection& deps);
      /// Add a new shared conditions dependency
      bool insert(Dependency* dependency);

      /// Add a new conditions key. T must inherot from class ConditionsSlice::Info
      template <typename T>
      bool insert(const ConditionKey& key, const T& loadinfo)   {
        Descriptor* e = new ConditionsLoaderDescriptor<T>(key,loadinfo);
        return insert_condition(e);
      }
      /// Add a condition directly to the slice
      /** Note: The user pool must exist. This call ONLY allows to update the user pool.
       */
      bool insert(Condition condition)   {
        return insert_condition(condition);
      }
      /// Create load-info object
      template <typename T> static LoadInfo<T> loadInfo(const T& t) { return LoadInfo<T>(t);  }
      
      /// Create slice entry for external usage
      template <typename T> static
      Descriptor* createDescriptor(const ConditionKey& key, const T& loadinfo)   {
        Descriptor* e = new ConditionsLoaderDescriptor<T>(key,loadinfo);
        return e;
      }
    };

    /// Populate the conditions slice from the conditions manager (convenience)
    ConditionsSlice* createSlice(ConditionsManager mgr, const IOVType& typ);
  }        /* End namespace Conditions               */
}          /* End namespace DD4hep                   */
#endif     /* DD4HEP_DDCOND_CONDITIONSSLICE_H        */
