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
#ifndef DD4HEP_CONDITIONS_CONDITIONS_H
#define DD4HEP_CONDITIONS_CONDITIONS_H

// Framework include files
#include "DD4hep/OpaqueData.h"
#include "DD4hep/Handle.h"
#include "DD4hep/IOV.h"

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Grammar definition for type binding
  class BasicGrammar;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    // Forward declarations
    class DetElement;
    class LCDD;
  }

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    // The DetElement is a central object. We alias it here.
    using Geometry::DetElement;
    using Geometry::LCDD;

    // Forward declarations
    class ConditionsManagerObject;
    class ConditionsManager;
    class ConditionsLoader;
    class UserPool;

    /// Conditions internal namespace
    namespace Interna  {
      class ConditionContainer;
      class ConditionObject;
    }

    /// Main condition object handle.
    /**
     *  This objects allows access to the data block and
     *  the interval of validity for a single condition.
     *
     *  Note:
     *  Conditions may be shared between several DetElement objects.
     *  Hence, the back-link to the DetElement structure cannot be
     *  set - it would be ambiguous.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Condition: public Handle<Interna::ConditionObject> {
    public:
      /// Standard object type
      typedef Interna::ConditionObject    Object;
      /// Forward definition of the key type
      typedef unsigned int                key_type;
      /// Forward definition of the iov type
      typedef IOV                         iov_type;
      /// Forward definition of the object properties
      typedef unsigned int                mask_type;

    public:
      enum StringFlags  {
        WITH_IOV         = 1<<0,
        WITH_ADDRESS     = 1<<1,
        WITH_TYPE        = 1<<2,
        WITH_COMMENT     = 1<<4,
        WITH_DATATYPE    = 1<<5,
        WITH_DATA        = 1<<6,
        NO_NAME          = 1<<20,
        NONE
      };
      enum ConditionState {
        INACTIVE         = 0,
        ACTIVE           = 1<<0,
        CHECKED          = 1<<2,
        DERIVED          = 1<<3,
        TEMPERATURE      = 1<<4,
        PRESSURE         = 1<<5,
        ALIGNMENT        = 1<<6,
        // Keep bit 7-15 for other generic types
        // Bit 16-31 is reserved for user classifications
        USER_FLAGS_FIRST = 1<<16,
        USER_FLAGS_LAST  = 1<<31
      };

      /// Abstract base for processing callbacks to conditions objects
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Processor {
      public:
        /// Default constructor
        Processor();
        /// Default destructor
        virtual ~Processor() = default;
        /// Conditions callback for object processing
        virtual int operator()(Condition c) = 0;
      };

      /// Default constructor
      Condition() = default;
      /// Copy constructor
      Condition(const Condition& c) = default;
      /// Initializing constructor
      Condition(Object* p);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Condition(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Initializing constructor for a pure, undecorated conditions object
      Condition(const std::string& name, const std::string& type);
      /// Assignment operator
      Condition& operator=(const Condition& c) = default;

      /// Output method
      std::string str(int with_data=WITH_IOV|WITH_ADDRESS|WITH_DATATYPE)  const;

      /** Data block (bound type)         */
      /// Access the data type
      int dataType()  const;
      /// Access the IOV block
      OpaqueData& data()  const;

      /** Interval of validity            */
      /// Access the IOV type
      const IOVType& iovType()  const;
      /// Access the IOV block
      const iov_type& iov()  const;

      /** Direct data items in string form */
      /// Access the type field of the condition
      const std::string& type()  const;
      /// Access the comment field of the condition
      const std::string& comment()  const;
      /// Access the value field of the condition as a string
      const std::string& value()  const;
      /// Access the address string [e.g. database identifier]
      const std::string& address()  const;

      /** Conditions meta-data   */
      /// Access to the type information
      const std::type_info& typeInfo() const;
      /// Access to the grammar type
      const BasicGrammar& descriptor() const;
      /// Hash identifier
      key_type key()  const;
      
      /** Conditions handling */
      /// Re-evaluate the conditions data according to the previous bound type definition
      Condition& rebind();

      /** Bind the data of the conditions object to a given format.
       *
       *  Note: The type definition is possible exactly once.
       *  Any further rebindings MUST match the identical type.
       */
      template <typename T> T& bind();
      /** Set and bind the data of the conditions object to a given format.
       *
       *  Note: The type definition is possible exactly once.
       *  Any further rebindings MUST match the identical type.
       */
      template <typename T> T& bind(const std::string& val);
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> const T& get() const;
      /// Check if object is already bound....
      bool is_bound()  const  {  return isValid() ? data().is_bound() : false;  }
    };

    /// Initializing constructor
    inline Condition::Condition(Condition::Object* p) : Handle<Condition::Object>(p)  {
    }

    /// Container class for condition handles aggregated by a detector element
    /**
     *  Note: The conditions container is owner by the detector element
     *        On deletion the detector element will destroy the container
     *        and all associated entries.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Container : public Handle<Interna::ConditionContainer> {

    public:
      /// Abstract base for processing callbacks to container objects
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Processor {
      public:
        /// Default constructor
        Processor();
        /// Default destructor
        virtual ~Processor() = default;
        /// Container callback for object processing
        virtual int operator()(Container container) = 0;
      };

      /// Standard object type
      typedef Interna::ConditionContainer      Object;
      /// Forward definition of the key type
      typedef Condition::key_type              key_type;
      /// Forward definition of the iov type
      typedef Condition::iov_type              iov_type;
      /// Forward definition of the mapping type
      typedef std::pair<key_type, std::string> key_value;
      /// Definition of the keys
      typedef std::map<key_type, key_value>    Keys;

    public:
      /// Default constructor
      Container() = default;
      /// Constructor to be used when reading the already parsed object
      Container(const Container& c) = default;

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Handle<Q>& e) : Handle<Object>(e) {}

      /// Assignment operator
      Container& operator=(const Container& c) = default;

      /// Access the number of conditons keys available for this detector element
      size_t numKeys() const;

      /// Known keys of conditions in this container
      const Keys&  keys()  const;
      
      /// Access to condition objects by key and IOV. 
      Condition get(const std::string& condition_key, const iov_type& iov);

      /// Access to condition objects directly by their hash key. 
      Condition get(key_type condition_key, const iov_type& iov);

      /// Access to condition objects. Only conditions in the pool are accessed.
      Condition get(const std::string& condition_key, const UserPool& pool);

      /// Access to condition objects. Only conditions in the pool are accessed.
      Condition get(key_type condition_key, const UserPool& pool);
    };

    /// Key definition to optimize ans simplyfy the access to conditions entities
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionKey  {
    public:
      /// Forward definition of the key type
      typedef Condition::key_type      key_type;

      /// String representation of the key object
      std::string  name;
      /// Hashed key representation
      key_type     hash;

    public:
      /// Default constructor
      ConditionKey() : hash(0)  {}
      /// Constructor from string
      ConditionKey(const std::string& compare);
      /// Constructor from string
      ConditionKey(const std::string& s, key_type h) : name(s), hash(h) {}
      /// Copy constructor
      ConditionKey(const ConditionKey& c) : name(c.name), hash(c.hash) {}

      /// Hash code generation from input string
      static key_type hashCode(const char* value);
      /// Hash code generation from input string
      static key_type hashCode(const std::string& value);

      /// Assignment operator from the string representation
      ConditionKey& operator=(const std::string& value);
      /// Assignment operator from object copy
      ConditionKey& operator=(const ConditionKey& key);
      /// Equality operator using key object
      bool operator==(const ConditionKey& compare)  const;
      /// Equality operator using hash value
      bool operator==(const key_type compare)  const;
      /// Equality operator using the string representation
      bool operator==(const std::string& compare)  const;

      /// Operator less (for map insertions) using key object
      bool operator<(const ConditionKey& compare)  const;
      /// Operator less (for map insertions) using hash value
      bool operator<(const key_type compare)  const;
      /// Operator less (for map insertions) using the string representation
      bool operator<(const std::string& compare)  const;

      /// Automatic conversion to the string representation of the key object
      operator const std::string& ()  const  {  return name;     }
      /// Automatic conversion to the hashed representation of the key object
      operator key_type () const             {  return hash;     }
    };

    /// Hash code generation from input string
    inline ConditionKey::key_type ConditionKey::hashCode(const char* value)
    {   return hash32(value);    }

    /// Hash code generation from input string
    inline ConditionKey::key_type ConditionKey::hashCode(const std::string& value)
    {   return hash32(value);    }

    /// Assignment operator from object copy
    inline ConditionKey& ConditionKey::operator=(const ConditionKey& key)  {
      if ( this != &key )  {
        hash  = key.hash;
        name  = key.name;
      }
      return *this;
    }

    /// Equality operator using key object
    inline bool ConditionKey::operator==(const ConditionKey& compare)  const
    {  return hash == compare.hash;                            }

    /// Equality operator using hash value
    inline bool ConditionKey::operator==(const key_type compare)  const
    {  return hash == compare;                                 }

    /// Operator less (for map insertions) using key object
    inline bool ConditionKey::operator<(const ConditionKey& compare)  const
    {  return hash < compare.hash;                             }

    /// Operator less (for map insertions) using hash value
    inline bool ConditionKey::operator<(const key_type compare)  const
    {  return hash < compare;                                  }

    /// Access the key of the condition
    ConditionKey make_key(Condition c);

    // Utility type definitions
    typedef std::vector<Condition>          RangeConditions;
    typedef std::pair<RangeConditions,bool> RangeStatus;

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_CONDITIONS_CONDITIONS_H */
