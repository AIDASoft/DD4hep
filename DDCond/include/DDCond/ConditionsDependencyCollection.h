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
#ifndef DDCOND_CONDITIONSDEPENDENCYCOLLECTION_H
#define DDCOND_CONDITIONSDEPENDENCYCOLLECTION_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionDerived.h"

// C++ include files
#include <map>
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class UserPool;
    class ConditionDependency;
    
    /// Collection of condition dependencies.
    /** 
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsDependencyCollection     {
    public:

      /// Forward definition of the dependency type
      typedef ConditionDependency                Dependency;
      struct Holder  {
        int         flag;
        Dependency* dep;
        Holder(Dependency* d) : flag(0), dep(d) {}
        Holder& operator=(Dependency* d) { dep = d; flag=0; return *this; }
        operator int()  const            { return dep ? 1 : 0;            }
        operator Dependency*&()          { return dep;                    }
        Dependency* operator->()  const  { return dep;                    }
        Dependency* get()  const         { return dep;                    }
      };

      /// Forward definition of the dependency container type
      typedef std::map<Condition::key_type,Holder> Dependencies;
      typedef Dependencies::const_iterator         const_iterator;
      typedef Dependencies::iterator               iterator;
      typedef Dependencies::key_type               key_type;
      typedef Dependencies::mapped_type            mapped_type;

      /// Functor base to create views
      /** 
       *
       *  \author  M.Frank
       *  \version 1.0
       */
      class Functor  {
      public:
        /// Forward definition of the dependency type
        typedef ConditionsDependencyCollection::Dependency   Dependency;
        /// Forward definition of the dependency container type
        typedef ConditionsDependencyCollection::Dependencies Dependencies;
      };
      /// Dependency container
      Dependencies dependencies;
    public:
      /// Initializing constructor
      ConditionsDependencyCollection();
      /// Initializing constructor
      template <typename I> ConditionsDependencyCollection(I first, I last)
      { std::for_each(first, last, [this](typename I::reference& v) { this->insert(v); });  }
      /// Initializing constructor
      template <typename I, typename R>
      ConditionsDependencyCollection(I first, I last, R (*func)(typename I::reference val))
      { std::for_each(first, last, [this,func](typename I::reference v) { this->insert(func(v)); }); }
      /// Copy constructor
      ConditionsDependencyCollection(const ConditionsDependencyCollection& copy);
      /// Default destructor
      virtual ~ConditionsDependencyCollection();
      /// Clear the dependency container. Destroys the contained stuff
      void clear();
      /// Mini-container interface: emptyness check
      bool empty()  const            {  return dependencies.empty();      }
      /// Mini-container interface: number of entries
      size_t size()  const           {  return dependencies.size();       }
      /// Mini-container interface: begin of iteration
      iterator       begin()         {  return dependencies.begin();      }
      /// Mini-container interface: begin of iteration (const)
      const_iterator begin()  const  {  return dependencies.begin();      }
      /// Mini-container interface: end of iteration
      iterator       end()           {  return dependencies.end();        }
      /// Mini-container interface: end of iteration (const)
      const_iterator end()    const  {  return dependencies.end();        }
      /// Mini-container interface: find element by key 
      iterator       find(Condition::key_type key)
      {  return dependencies.find(key);                                   }
      /// Mini-container interface: find element by key (const)
      const_iterator find(Condition::key_type key) const
      {  return dependencies.find(key);                                   }
      /// Mini-container interface: insert element by key
      std::pair<iterator,bool> insert(const Dependencies::value_type& entry);
      /// Insert new element by key
      std::pair<iterator,bool> insert(Dependency* dep);
      /// Insert a range of items with a item converting adjustment
      template <typename I> void insert(I first, I last)
      {  std::for_each(first,last,[this](typename I::reference v) { this->insert(v); }); }
      /// Insert a range of items with a item converting adjustment
      template <typename I, typename R>
      void insert(I first, I last, R (*func)(typename I::reference val))
      {  std::for_each(first, last, [this,func](typename I::reference v) { this->insert(func(v)); }); }
      /// Create view by application of functor
      template <typename T> void for_each(const T& function)  const
      {  std::for_each(dependencies.begin(),dependencies.end(),function); }
    };

    /// Collection of DetElements from the conditions dependencies.
    /** 
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    template <typename Data, typename Oper>
    class DependencyCollector : public ConditionsDependencyCollection::Functor    {
    public:
      /// Reference to collection result (Object owned by caller)
      Data& user_data;
      /// Reference to the collection operator (Object owned by caller)
      const Oper& user_oper;
      /// Initializing constructor
      DependencyCollector(Data& d, const Oper& o) : user_data(d), user_oper(o)    {}
      /// Default callback to work on dependency items. May be overloaded if necessary
      void operator()(const Dependencies::value_type& e) const
      {  user_oper(user_data, e);                                                  }
    };
  }        /* End namespace Conditions                   */
}          /* End namespace DD4hep                       */

#endif     /* DDCOND_CONDITIONSDEPENDENCYCOLLECTION_H    */
