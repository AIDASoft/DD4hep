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
    class Condition;

    ///   Conditions internal namespace declaration
    /**  Internally defined datastructures are not presented to the
     *   user directly, but are used by dedicated views.
     *
     */
    namespace ConditionsInterna {
      class ConditionContainer;
      class ConditionObject;
      class Entry;
      class IOV;

      /// Class describing the interval of validty
      /**
       *
       * \author  M.Frank
       * \version 1.0
       * \ingroup DD4HEP_GEOMETRY
       */
      class IOV   {
        friend class Condition;
      public:
        enum {
          UNKNOWN_IOV = 1<<0
        } _IOVTypes;
        /// IOV buffer type: Must be a bitmap!
        int type;
        int data[4];
        int _spare;
        /// Initializing constructor
        IOV(int t=UNKNOWN_IOV);
        /// Standard Destructor
        ~IOV();
        /// Move the data content: 'from' will be reset to NULL
        void move(IOV& from);
        /// Create IOV data from string representation
        void fromString(const std::string& rep);
        /// Create string representation of the IOV
        std::string str();
      };

      /// Class describing an opaque conditions data block
      /**
       *
       * \author  M.Frank
       * \version 1.0
       * \ingroup DD4HEP_GEOMETRY
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

      /// The intermediate conditions data
      /**
       *
       * \author  M.Frank
       * \version 1.0
       * \ingroup DD4HEP_GEOMETRY
       */
      class Entry : public NamedObject  {
      public:
        /// Reference to the detector element
        DetElement detector;
        /// Conditions type (was the tag in the XML)
        std::string type;
        /// The actual conditions data
        std::string value;
        /// The validity string to be interpreted by the updating engine
        std::string validity;
        /// Default constructor
        Entry() {}
        /// Initializing constructor
        Entry(const DetElement& det, const std::string& nam, const std::string& typ, const std::string& valid);
        /// Copy constructor
        Entry(const Entry& c);
        /// Default destructor
        virtual ~Entry();
        /// Assignment operator
        Entry& operator=(const Entry& c);
      };

      /// The data class behind a conditions handle.
      /**
       *
       * \author  M.Frank
       * \version 1.0
       * \ingroup DD4HEP_GEOMETRY
       */
      class ConditionObject : public NamedObject {
      public:
        /// Condition value (in string form)
        std::string value;
        /// Condition validity (in string form)
        std::string validity;
        /// Condition address
        std::string address;
        /// Comment string
        std::string comment;
        /// The detector element
        DetElement detector;
        /// Data block
        BlockData data;
        /// Interval of validity
        IOV iov;
        /// Standard constructor
        ConditionObject();
        /// Standard Destructor
        virtual ~ConditionObject();
        /// Move data content: 'from' will be reset to NULL
        ConditionObject& move(ConditionObject& from);
      };

      /// The data class behind a conditions container handle.
      /**
       *
       * \author  M.Frank
       * \version 1.0
       * \ingroup DD4HEP_GEOMETRY
       */
      class ConditionContainer : public NamedObject {
      public:
#ifndef __CINT__
        /// Container definition
        typedef std::map<std::string, DD4hep::Geometry::Condition> Entries;
#endif
        std::map<std::string,DD4hep::Geometry::Condition> entries;
        /// Standard constructor
        ConditionContainer();
        /// Default destructor
        virtual ~ConditionContainer();
        /// Clear all conditions. Auto-delete of all existing entries
        void removeElements();
      };

    } /* End namespace ConditionsInterna    */

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
      ConditionsInterna::BlockData& b = o->data;
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
  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#define DD4HEP_DEFINE_CONDITIONS_TYPE(x)            \
  namespace DD4hep { namespace Geometry  {          \
      template Condition& Condition::bind<x>();     \
      template x& Condition::get<x>();              \
      template const x& Condition::get<x>() const;  \
    }}

#define DD4HEP_DEFINE_CONDITIONS_CONT(x)        \
  DD4HEP_DEFINE_CONDITIONS_TYPE(x)              \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::vector<x>) \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::list<x>)   \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::set<x>)

#define DD4HEP_DEFINE_CONDITIONS_U_CONT(x)      \
  DD4HEP_DEFINE_CONDITIONS_CONT(x)              \
  DD4HEP_DEFINE_CONDITIONS_CONT(unsigned x)


#endif    /* DD4HEP_GEOMETRY_CONDITIONINTERNA_H    */
