// $Id: $
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
#ifndef DD4HEP_GEOMETRY_CONDITIONINTERNA_H
#define DD4HEP_GEOMETRY_CONDITIONINTERNA_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/BasicGrammar.h"
#include "DD4hep/NamedObject.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    // Forward declarations
    class DetElement;
    class DetElementObject;
  }

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    // Forward declarations
    class ConditionsLoader;
    class ConditionsPool;
    class Condition;
    class IOVType;
    class IOV;

    /// Class describing an opaque conditions data block
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class BlockData : public Block   {
      friend class Condition;
      friend class Object;

    private:
      enum {
        PLAIN_DATA = 1<<0,
        ALLOC_DATA = 1<<1,
        BOUND_DATA = 1<<2
      } _DataTypes;
      /// Data buffer: plain data are allocated directly on this buffer
      unsigned int data[16];
      /// Destructor function -- only set if the object is valid
      void (*destruct)(void*);
      /// Constructor function -- only set if the object is valid
      void (*copy)(void*,const void*);

    public:
      /// Data buffer type: Must be a bitmap!
      int type;

      /// Standard initializing constructor
      BlockData();
      /// Standard Destructor
      ~BlockData();
      /// Move the data content: 'from' will be reset to NULL
      void move(BlockData& from);
      /// Set data value
      void bind(const BasicGrammar* grammar,
                void (*ctor)(void*,const void*),
                void (*dtor)(void*));
      /// Set data value
      void assign(const void* ptr,const std::type_info& typ);
    };

    /// The data class behind a conditions container handle.
    /**
     *  See ConditionsInterna.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
       */
    class ConditionsLoader  {
    protected:
      /// Protected destructor
      virtual ~ConditionsLoader();
    public:
      /// Access the conditions loading
      virtual Condition get(Geometry::DetElement element, const std::string& key, const IOV& iov) = 0;
    };

    /// Conditions internal namespace declaration
    /** Internally defined datastructures are not presented to the
     *  user directly, but are used by dedicated views.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    namespace Interna {

      // Forward declarations
      class ConditionContainer;
      class ConditionObject;
      
      /// The data class behind a conditions handle.
      /**
       *  See ConditionsInterna.cpp for the implementation.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class ConditionObject : public NamedObject {
      public:
        enum ConditionState {
          INACTIVE = 0,
          ACTIVE = 1
        };
        /// Condition value (in string form)
        std::string value;
        /// Condition validity (in string form)
        std::string validity;
        /// Condition address
        std::string address;
        /// Comment string
        std::string comment;
        /// The detector element
        Geometry::DetElement detector;
        /// Data block
        BlockData data;
        /// Reference to conditions pool
        ConditionsPool* pool;
        /// Interval of validity
        const IOV* iov;
        /// Hash value of the name
        int hash;
        /// Flags
        int flags;
        /// Reference count
        int refCount;
        /// Standard constructor
        ConditionObject();
        /// Standard Destructor
        virtual ~ConditionObject();
        /// Move data content: 'from' will be reset to NULL
        ConditionObject& move(ConditionObject& from);
        /// Access safely the IOV
        const IOV* iov_data() const;
        /// Access safely the IOV-type
        const IOVType* iov_type() const;
        /// Check if object is already bound....
        bool is_bound()  const  {  return data.is_bound(); }
      };

      /// The data class behind a conditions container handle.
      /**
       *  See ConditionsInterna.cpp for the implementation.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class ConditionContainer : public NamedObject {
      private:
        // We only allow creation and deletion by the central detector element
        friend class Geometry::DetElement;
        friend class Geometry::DetElementObject;

        /// Standard constructor
        ConditionContainer();
        /// Default destructor
        virtual ~ConditionContainer();

      public:
        /// Clear all conditions. Auto-delete of all existing entries
        void lock();
        void unlock();
        void removeElements();
        void add(Condition condition);
        void remove(int condition_hash);
        size_t size() const { return entries.size(); }
#ifdef __CINT__
        Handle<NamedObject> detector;
#else
        typedef Container::Entries Entries;
        Entries& elements() { return entries; }
        const Entries& elements() const { return entries; }
      public:
        /// The hosting detector element
        DetElement detector;
#endif
      private:
        /// Conditions container declaration
        Container::Entries entries;
        // std::map<std::string,Condition> entries;
      };

    } /* End namespace Interna    */

    template <typename T> static void copyObject(void* t, const void* s)  {
      *((T*)t) = *((const T*)s);
    }
    template <typename T> static void destructObject(void* p)  {
      T* t = (T*)p;
      t->~T();
    }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& Block::get() {
      if (!grammar || (grammar->type() != typeid(T))) { throw std::bad_cast(); }
      return *(T*)pointer;
    }
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& Block::get() const {
      if (!grammar || (grammar->type() != typeid(T))) { throw std::bad_cast(); }
      return *(T*)pointer;
    }
    /// Bind the data of the conditions object to a given format.
    template <typename T> Condition& Condition::bind()   {
      Object* o = access();
      BlockData& b = o->data;
      b.bind(&BasicGrammar::instance<T>(),copyObject<T>,destructObject<T>);
      new(b.pointer) T();
      if ( !o->value.empty() ) b.fromString(o->value);
      return *this;
    }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& Condition::get() {
      return access()->data.get<T>();
    }
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& Condition::get() const {
      return access()->data.get<T>();
    }

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#define DD4HEP_DEFINE_CONDITIONS_TYPE(x)            \
  namespace DD4hep { namespace Conditions  {        \
      template Condition& Condition::bind<x>();     \
      template x& Condition::get<x>();              \
      template const x& Condition::get<x>() const;  \
    }}

#define DD4HEP_DEFINE_CONDITIONS_CONT(x)        \
  DD4HEP_DEFINE_CONDITIONS_TYPE(x)              \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::vector<x>) \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::list<x>)   \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::set<x>)    \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::deque<x>)  \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::int_map_t)  \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::ulong_map_t) \
  DD4HEP_DEFINE_CONDITIONS_TYPE(DD4hep::Primitive<x>::string_map_t)

#define DD4HEP_DEFINE_CONDITIONS_U_CONT(x)      \
  DD4HEP_DEFINE_CONDITIONS_CONT(x)              \
  DD4HEP_DEFINE_CONDITIONS_CONT(unsigned x)

#endif    /* DD4HEP_GEOMETRY_CONDITIONINTERNA_H    */
