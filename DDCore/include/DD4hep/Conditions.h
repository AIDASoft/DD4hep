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
#ifndef DD4HEP_GEOMETRY_CONDITION_H
#define DD4HEP_GEOMETRY_CONDITION_H

// Framework include files
#include "DD4hep/Handle.h"

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
    class ConditionsManager;
    class ConditionsLoader;
    class IOVType;
    class IOV;

    /// Conditions intrnal namespace
    namespace Interna  {
      class ConditionContainer;
      class ConditionObject;
    }

    class IOVType   {
    public:
      enum { UNKNOWN_IOV = ~0x0 } _IOVTypes;

      unsigned int type;
      std::string  name;
      IOVType() : type(UNKNOWN_IOV), name() {}
      ~IOVType() {}
      std::string str() const;
    };

    /// Class describing the interval of validty
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class IOV   {
      friend class Condition;
    private:
      /// No IOC copies!
      explicit IOV(const IOV&) {}
      /// Initializing constructor: Does not set reference to IOVType !
      explicit IOV();
    public:
      /// Key definition
      typedef std::pair<int,int> Key;

      const IOVType* iovType;
      Key            keyData;
      int            optData;
      /// IOV buffer type: Must be a bitmap!
      unsigned int   type;

      /// Initializing constructor
      explicit IOV(const IOVType* typ);
      /// Standard Destructor
      ~IOV();
      /// Move the data content: 'from' will be reset to NULL
      void move(IOV& from);
      /// Create string representation of the IOV
      std::string str() const;
      /// Check if the IOV corresponds to a range
      bool has_range() const    {  return keyData.first != keyData.second;  }
      /// Check if the IOV corresponds to a range
      bool is_discrete() const  {  return keyData.first == keyData.second;  }
      /// Get the local key of the IOV
      Key  key() const {  return keyData; }
      /// Check for validity containment
      /** Check if the caller 'iov' is of the same type and the range 
       *  is fully conained by the caller.
       */
      bool contains(const IOV& iov)  const;
      static bool same_type(const IOV& iov, const IOV& test)  {
        unsigned int typ1 = iov.iovType ? iov.iovType->type : iov.type;
        unsigned int typ2 = test.iovType ? test.iovType->type : test.type;
        return typ1 == typ2;
      }
      static bool key_is_contained(const IOV::Key& key, const IOV::Key& test)
      {   return key.first >= test.first && key.second <= test.second;      }
      static bool key_overlaps_lower_end(const IOV::Key& key, const IOV::Key& test)
      {   return key.first <= test.second && key.first >= test.first;       }
      static bool key_overlaps_higher_end(const IOV::Key& key, const IOV::Key& test)
      {   return key.second >= test.first && key.second <= test.second;     }

    };
    

    /// Class describing an opaque conditions data block
    /**
     *  Access methods are templated. Once the access is fixed
     *  on the first call, the data type may not be changed anymore.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Block   {
      /// Access only through the conditions class!
      friend class Condition;

    private:
    protected:
      /// Standard initializing constructor
      Block();
      /// Standard Destructor
      virtual ~Block();

    protected:
      /// Data type
      const BasicGrammar* grammar;
      /// Pointer to object data
      void* pointer;

    public:
      /// Create data block from string representation
      void fromString(const std::string& rep);
      /// Create string representation of the data block
      std::string str();
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> inline T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> inline const T& get() const;
    };

    /// Main condition object handle.
    /**
     *  This objects allows access to the data block and
     *  the interval of validity for a single condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Condition: public Handle<Interna::ConditionObject> {
    public:
      typedef Interna::ConditionObject Object;

    public:
      /// Default constructor
      Condition();
      /// Copy constructor
      Condition(const Condition& c);
      /// Initializing constructor
      Condition(Object* p);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Condition(const Handle<Q>& e)
        : Handle<Object>(e) {
      }
      /// Initializing constructor
      Condition(const std::string& name, const std::string& type);
      /// Assignment operator
      Condition& operator=(const Condition& c);

      /** Data block (bound type)         */
      /// Access the data type
      int dataType()  const;
      /// Access the IOV block
      Block& block()  const;

      /** Interval of validity            */
      /// Access the IOV type
      const IOVType& iovType()  const;
      /// Access the IOV block
      const IOV& iov()  const;

      /** Direct data items in string form */
      /// Access the name of the condition
      const std::string& name()  const;
      /// Access the type field of the condition
      const std::string& type()  const;
      /// Access the comment field of the condition
      const std::string& comment()  const;
      /// Access the validity field of the condition as a string
      const std::string& validity()  const;
      /// Access the value field of the condition as a string
      const std::string& value()  const;
      /// Access the address string [e.g. database identifier]
      const std::string& address()  const;
      /// Access the hosting detector element
      Geometry::DetElement detector()  const;

      /** Conditions meta-data   */
      /// Access to the type information
      const std::type_info& typeInfo() const;
      /// Access to the grammar type
      const BasicGrammar& descriptor() const;

      /** Conditions handling */
      /// Re-evaluate the conditions data according to the previous bound type definition
      Condition& rebind();

      /** Bind the data of the conditions object to a given format.
       *
       *  Note: The type definition is possible exactly once.
       *  Any further rebindings MUST match the identical type.
       */
      template <typename T> Condition& bind();
      /// Generic getter. Specify the exact type, not a polymorph type
      template <typename T> T& get();
      /// Generic getter (const version). Specify the exact type, not a polymorph type
      template <typename T> const T& get() const;
    };

    /// Initializing constructor
    inline Condition::Condition(Condition::Object* p) : Handle<Condition::Object>(p)  {
    }

    /// Copy constructor
    inline Condition::Condition(const Condition& c) : Handle<Condition::Object>(c)   {
    }

    /// Default constructor
    inline Condition::Condition() : Handle<Condition::Object>()   {
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
      /// Standard object type
      typedef Interna::ConditionContainer Object;
      /// Definition of the conditions container of this detector element
      //typedef std::map<std::string, Condition> Entries;
      typedef std::map<int, Condition> Entries;

    public:
      /// Default constructor
      Container();
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Container& c) : Handle<Object>(c) {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Access the number of conditons available for this detector element
      size_t count() const;
      /// Access the full map of conditons
      Entries& entries() const;
      /// Clear all attached DetElement conditions.
      void removeElements() const;
      /// Access to condition objects. No loading undertaken. The condition must be present
      Condition operator[](const std::string& key);
      /// Access to condition objects directly by their hash key. 
      /// No loading undertaken. The condition must be present
      Condition operator[](int hash_key);
    };

    /// Default constructor
    inline Container::Container() : Handle<Object>() {
    }

    // Utility type definitions
    typedef std::vector<Condition> RangeConditions;
    typedef std::pair<RangeConditions,bool> RangeStatus;

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITION_H    */
