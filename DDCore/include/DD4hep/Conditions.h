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
#ifndef DD4HEP_CONDITIONS_CONDITIONS_H
#define DD4HEP_CONDITIONS_CONDITIONS_H

// Framework include files
#include "DD4hep/IOV.h"
#include "DD4hep/Handle.h"
#include "DD4hep/OpaqueData.h"

// C/C++ include files
#include <vector>

#define DD4HEP_CONDITIONKEY_HAVE_NAME 1

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class BasicGrammar;
  class DetElement;
  class Detector;

  /// Conditions internal namespace
  namespace detail  {
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
  class Condition: public Handle<detail::ConditionObject> {
  public:
    /// Forward definition of the key type
    typedef unsigned long long int key_type;
    /// High part of the key identifies the detector element
    typedef unsigned int           detkey_type;
    /// Low part of the key identifies the item identifier
    typedef unsigned int           itemkey_type;
    /// Forward definition of the object properties
    typedef unsigned int           mask_type;

  public:
    enum StringFlags  {
      WITH_IOV           = 1<<0,
      WITH_ADDRESS       = 1<<1,
      WITH_TYPE          = 1<<2,
      WITH_COMMENT       = 1<<4,
      WITH_DATATYPE      = 1<<5,
      WITH_DATA          = 1<<6,
      NO_NAME            = 1<<20,
      NONE
    };
    enum ConditionState {
      INACTIVE            = 0,
      ACTIVE              = 1<<0,
      CHECKED             = 1<<2,
      DERIVED             = 1<<3,
      ONSTACK             = 1<<4,
      // Flags for specific conditions
      TEMPERATURE         = 1<<5,
      TEMPERATURE_DERIVED = TEMPERATURE|DERIVED,
      PRESSURE            = 1<<6,
      PRESSURE_DERIVED    = PRESSURE|DERIVED,
      ALIGNMENT_DELTA     = 1<<7,
      ALIGNMENT_DERIVED   = ALIGNMENT_DELTA|DERIVED,
      // Keep bit 8-15 for other generic types
      // Bit 16-31 is reserved for user classifications
      USER_FLAGS_FIRST    = 1<<16,
      USER_FLAGS_LAST     = 1<<31
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
      /// Processing callback
      virtual int process(Condition c)  const = 0;
      /// Conditions callback for object processing
      virtual int operator()(Condition c)  const
      {  return this->process(c);                   }
      /// Conditions callback for object processing in maps
      virtual int operator()(const std::pair<Condition::key_type,Condition>& e)  const
      {  return this->process(e.second);            }
    };

    /// Default constructor
    Condition() = default;
    /// Copy constructor
    Condition(const Condition& c) = default;
    /// Initializing constructor
    Condition(Object* p);
    /// Constructor to be used when reading the already parsed object
    template <typename Q> Condition(const Handle<Q>& e);
    /// Initializing constructor for a pure, undecorated conditions object
    Condition(const std::string& name, const std::string& type);
    /// Initializing constructor for a pure, undecorated conditions object with payload buffer
    Condition(const std::string& name, const std::string& type, size_t memory);
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
    const IOV& iov()  const;

    /** Direct data items in string form */
    /// Access the type field of the condition
    const std::string& type()  const;
    /// Access the comment field of the condition
    const std::string& comment()  const;
    /// Access the value field of the condition as a string
    const std::string& value()  const;
    /// Access the address string [e.g. database identifier]
    const std::string& address()  const;

    /// Flag operations: Set a conditons flag
    void setFlag(mask_type option);
    /// Flag operations: UN-Set a conditons flag
    void unFlag(mask_type option);
    /// Flag operations: Test for a given a conditons flag
    bool testFlag(mask_type option) const;

    /** Conditions meta-data   */
    /// Access to the type information
    const std::type_info& typeInfo() const;
    /// Access to the grammar type
    const BasicGrammar& descriptor() const;
    /// Hash identifier
    key_type key()  const;
    /// DetElement part of the identifier
    detkey_type  detector_key()  const;
    /// Item part of the identifier
    itemkey_type item_key()  const;
      
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
  inline Condition::Condition(Condition::Object* p)
    : Handle<Condition::Object>(p)  {}

  /// Constructor to be used when reading the already parsed object
  template <typename Q> inline Condition::Condition(const Handle<Q>& e)
    : Handle<Condition::Object>(e) {}

    
  /// Key definition to optimize ans simplyfy the access to conditions entities
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionKey  {
  public:
#ifdef DD4HEP_CONDITIONKEY_HAVE_NAME
    /// Optional string identifier. Helps debugging a lot!
    std::string  name;
#endif
    /// Hashed key representation
    Condition::key_type     hash = 0;

    union KeyMaker  {
      Condition::key_type  hash;
      /** Note: The memory layout is important here to have properly
       *        ordered maps. The detector key MUST be on the high end 
       *        of the resulting int64 'hash'.
       */
      struct {
        Condition::itemkey_type item_key;
        Condition::detkey_type  det_key;
      } values;
      KeyMaker()  {
        this->hash = 0;
      }
      KeyMaker(Condition::key_type k)  {
        this->hash = k;
      }
      KeyMaker(Condition::detkey_type det, Condition::itemkey_type item)  {
        this->values.det_key  = det;
        this->values.item_key = item;
      }
    };
    /// Compare keys by the hash value
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    struct HashCompare {
      Condition::key_type key;
      HashCompare(Condition::key_type k) : key(k) {}
      bool operator==(const ConditionKey& k) const { return key==k.hash; }
    };
  public:
    /// Default constructor
    ConditionKey() = default;
    /// Constructor from fully qualified key
    ConditionKey(Condition::key_type key) : hash(key) {}
    /// Constructor from string
    ConditionKey(DetElement detector, const std::string& identifier);
    /// Constructor from detector element key and item sub-key
    ConditionKey(Condition::detkey_type det_key, const std::string& identifier);
    /// Constructor from detector element and item sub-key
    ConditionKey(DetElement detector, Condition::itemkey_type item_key);
    /// Constructor from detector element key and item sub-key
    ConditionKey(Condition::detkey_type det_key, Condition::itemkey_type item_key);
    /// Copy constructor
    ConditionKey(const ConditionKey& c) = default;

    /// Access the detector element part of the key
    Condition::detkey_type detector_key()  const   {
      return KeyMaker(hash).values.det_key;
    }
    /// Access the detector element part of the key
    Condition::itemkey_type item_key()  const   {
      return KeyMaker(hash).values.item_key;
    }
    /// Hash code generation from input string
    static Condition::key_type hashCode(DetElement detector, const char* value);
    /// Hash code generation from input string
    static Condition::key_type hashCode(DetElement detector, const std::string& value);
    /// 32 bit hashcode of the item
    static Condition::itemkey_type itemCode(const char* value);
    /// 32 bit hashcode of the item
    static Condition::itemkey_type itemCode(const std::string& value);
       
    /// Assignment operator from object copy
    ConditionKey& operator=(const ConditionKey& key) = default;
    /// Equality operator using key object
    bool operator==(const ConditionKey& compare)  const;
    /// Equality operator using hash value
    bool operator==(const Condition::key_type compare)  const;
    /// Equality operator using the string representation
    //bool operator==(const std::string& compare)  const;

    /// Operator less (for map insertions) using key object
    bool operator<(const ConditionKey& compare)  const;
    /// Operator less (for map insertions) using hash value
    bool operator<(const Condition::key_type compare)  const;
    /// Automatic conversion to the hashed representation of the key object
    operator Condition::key_type () const             {  return hash;     }
  };

  /// Constructor from detector element key and item sub-key
  inline ConditionKey::ConditionKey(Condition::detkey_type det_key, Condition::itemkey_type item_key)  {
    hash = KeyMaker(det_key,item_key).hash;
  }
    
  /// Equality operator using key object
  inline bool ConditionKey::operator==(const ConditionKey& compare)  const
  {  return hash == compare.hash;                            }

  /// Equality operator using hash value
  inline bool ConditionKey::operator==(const Condition::key_type compare)  const
  {  return hash == compare;                                 }

  /// Operator less (for map insertions) using key object
  inline bool ConditionKey::operator<(const ConditionKey& compare)  const
  {  return hash < compare.hash;                             }

  /// Operator less (for map insertions) using hash value
  inline bool ConditionKey::operator<(const Condition::key_type compare)  const
  {  return hash < compare;                                  }


  /// Conditions selector functor. Default implementation selects everything evaluated.
  /**
   *  Please note:
   *  This class should never be directly instantiated by the user.
   *  A typical use-case is to do so in a wrapper class, which contains a refernce
   *  to a counter object, which in turn allows to deduce information from the
   *  processed objects.
   *
   *  See class ConditionsSelectWrapper below
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionsSelect   {
  protected:
    /// Default constructor
    ConditionsSelect() = default;
    /// Copy constructor
    ConditionsSelect(const ConditionsSelect& copy) = default;
    /// Default destructor. 
    virtual ~ConditionsSelect();
    /// Default assignment operator
    ConditionsSelect& operator=(const ConditionsSelect& copy) = default;

  public:
    /// Selection callback: return true if the condition should be selected
    bool operator()(Condition cond)  const  { return (*this)(cond.ptr()); }
    /// Selection callback: return true if the condition should be selected
    bool operator()(std::pair<Condition::key_type,Condition::Object*> cond) const
    /** Arg is 2 longwords. No need to pass by reference.                      */
    { return (*this)(cond.second);            }
    /// Selection callback: return true if the condition should be selected
    /** Arg is 2 longwords. No need to pass by reference.                      */
    bool operator()(std::pair<Condition::key_type,Condition> cond) const
    { return (*this)(cond.second.ptr());      }

    /// Overloadable entry: Return number of conditions selected. Default does nothing....
    virtual size_t size()  const  { return 0; }
    /// Overloadable entry: Selection callback: return true if the condition should be selected
    virtual bool operator()(Condition::Object* cond) const = 0;
  };

  /// Conditions selector functor. Wraps a user defined object by reference
  /**
   *  Example usage for the slow ones:
   *
   *  class MyCounter : public ConditionsSelectWrapper<long> {
   *    MyCounter(long& cnt) : ConditionsSelectWrapper<long>(cnt) {}
   *    virtual bool operator()(Condition::Object* cond) const { if ( cond != 0 ) ++object; }
   *    // Optionally overload: virtual size_t size()  const  { return object; }
   *  };
   *  
   *  long counter = 0;
   *  for_each(std::begin(conditons), std::end(conditions), MyCounter(counter));
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  template <typename OBJECT> class ConditionsSelectWrapper : public ConditionsSelect {
  private:
    /// Default constructor
    ConditionsSelectWrapper() = delete;
    /// Default assignment operator
    bool operator==(const ConditionsSelectWrapper& compare) = delete;

  public:
    /// Reference to the infomation collector
    OBJECT& object;

  public:
    /// Default constructor
    ConditionsSelectWrapper(OBJECT& o) : ConditionsSelect(), object(o) {}
    /// Copy constructor
    ConditionsSelectWrapper(const ConditionsSelectWrapper& copy) = default;
    /// Default destructor. 
    virtual ~ConditionsSelectWrapper() = default;
    /// Default assignment operator
    ConditionsSelectWrapper& operator=(const ConditionsSelectWrapper& copy) = default;
  };

  // Utility type definitions
  typedef std::vector<Condition>          RangeConditions;
  typedef std::pair<RangeConditions,bool> RangeStatus;

}          /* End namespace dd4hep                   */
#endif     /* DD4HEP_CONDITIONS_CONDITIONS_H         */
