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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the BasicGrammar include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_DETAIL_GRAMMAR_H
#define DD4HEP_DDCORE_DETAIL_GRAMMAR_H

// Framework include files
#include "DD4hep/config.h"
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <string>
#include <typeinfo>

// Forward declarations
class TClass;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declarations
  class GrammarRegistry;
  
  /// Base class describing string evaluation to C++ objects using boost::spirit
  /**
   *   Grammar object handle the boost::spirit conversion between strings and numeric
   *   values/objects. Numeric objects could be atomic (int, long, float, double, etc)
   *   or complex (vector<int>, vector<float>...). This way e.g. a vector<int> may
   *   be converted into a list<double>. The conversion though requires an intermediate
   *   string representation. For this reason the conversion mechanism is relatively
   *   slow and hence should not be used inside number-crunching algorithms.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  class BasicGrammar {
    friend class GrammarRegistry;
  public:
    typedef unsigned long long int key_type;
    /// Instance type name
    const std::string  name;
    /// Instance hash code
    const key_type     hash_value     = 0;
    /// Cached TClass reference for speed improvements
    mutable TClass*    root_class     = 0;
    /// Cached TDataType information for fundamental types
    mutable int        root_data_type = -1;
    /// Initialization flag
    mutable bool       inited         = false;
    
  protected:
    /// Default constructor
    BasicGrammar(const std::string& typ);

    /// Default destructor
    virtual ~BasicGrammar();

    /// Second step initialization after the virtual table is fixed
    void    initialize()   const;
    int     initialized_data_type()  const;
    TClass* initialized_clazz()  const;

    
    /// Instance factory
    static void pre_note(const std::type_info& info, const BasicGrammar& (*fcn)());
    static const BasicGrammar& get(const std::type_info& info);
    
  public:
    /// Instance factory
    template <typename TYPE> static const BasicGrammar& instance();
    /// Lookup existing grammar using the grammar's hash code/hash64(type-name) (used for reading objects)
    static const BasicGrammar& get(unsigned long long int hash_code);
    /// Error callback on invalid conversion
    static void invalidConversion(const std::type_info& from, const std::type_info& to);
    /// Error callback on invalid conversion
    static void invalidConversion(const std::string& value, const std::type_info& to);
    /// Access the hash value for this grammar type
    key_type hash() const                 {  return hash_value;   }
    /// Access to the type information name
    const std::string& type_name() const  {  return name;         }
    /// Access ROOT data type for fundamentals
    int data_type()  const  {
      if ( inited ) return root_data_type;
      return initialized_data_type();
    }
    /// Access the ROOT class for complex objects
    TClass* clazz()  const   {
      if ( inited ) return root_class;
      return initialized_clazz();
    }
    /// Access to the type information
    virtual bool equals(const std::type_info& other_type) const = 0;
    /// Access to the type information
    virtual const std::type_info& type() const = 0;
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const = 0;
    /// Opaque copy constructor
    virtual void copy(void* to, const void* from) const = 0;
    /// Opaque object destructor
    virtual void destruct(void* pointer) const = 0;

    /// Bind opaque address to object
    virtual void bind(void* pointer)  const = 0;
    /// Serialize an opaque value to a string
    virtual std::string str(const void* ptr) const = 0;
    /// Set value from serialized string. On successful data conversion TRUE is returned.
    virtual bool fromString(void* ptr, const std::string& value) const = 0;
  };

  /// Concrete type dependent grammar definition
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  template <typename TYPE> class Grammar : public BasicGrammar {
  private:
    
    friend class BasicGrammar;
    /// Default destructor
    virtual ~Grammar();
    /// Standard constructor
    Grammar();

  public:

    /** Base class overrides   */
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const  override;
    /// Access to the type information
    virtual bool equals(const std::type_info& other_type) const  override;
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const  override;
    /// Opaque copy constructor
    virtual void copy(void* to, const void* from) const  override;
    /// Opaque object destructor
    virtual void destruct(void* pointer) const  override;
    /// Bind opaque address to object
    virtual void bind(void* pointer)  const override;
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const  override;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const  override;

    /** Class member function   */
    /// Evaluate string value if possible before calling boost::spirit
    virtual int evaluate(void* ptr, const std::string& value) const;
  };

  /// Standarsd constructor
  template <typename TYPE> Grammar<TYPE>::Grammar() : BasicGrammar(typeName(typeid(TYPE)))  {
  }

  /// Default destructor
  template <typename TYPE> Grammar<TYPE>::~Grammar() {
  }

  /// PropertyGrammar overload: Access to the type information
  template <typename TYPE> const std::type_info& Grammar<TYPE>::type() const {
    return typeid(TYPE);
  }

  /// Access to the type information
  template <typename TYPE> bool Grammar<TYPE>::equals(const std::type_info& other_type) const  {
    return other_type == typeid(TYPE);
  }
  
  /// Access the object size (sizeof operator)
  template <typename TYPE> size_t Grammar<TYPE>::sizeOf() const   {
    return sizeof(TYPE);
  }

  /// Opaque object destructor
  template <typename TYPE> void Grammar<TYPE>::destruct(void* pointer) const   {
    TYPE* obj = (TYPE*)pointer;
    obj->~TYPE();
  }

  /// Opaque copy constructor
  template <typename TYPE> void Grammar<TYPE>::copy(void* to, const void* from) const   {
    const TYPE* from_obj = (const TYPE*)from;
    new (to) TYPE(*from_obj);
  }
  
  /// Bind opaque address to object
  template <typename TYPE> void Grammar<TYPE>::bind(void* pointer)  const  {
    new(pointer) TYPE();
  }

  /// Standarsd constructor
  template <typename TYPE> const BasicGrammar& BasicGrammar::instance()  {
    static Grammar<TYPE> s_gr;
    return s_gr;
  }

  /// Grammar registry interface
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  class GrammarRegistry {
    /// Default constructor
    GrammarRegistry() = default;
  public:
    /// Registry instance singleton
    static const GrammarRegistry& instance();
    template <typename T> static const GrammarRegistry& pre_note()   {
      BasicGrammar::pre_note(typeid(T),BasicGrammar::instance<T>);
      // Apple (or clang)  wants this....
      std::string (Grammar<T>::*str)(const void*) const = &Grammar<T>::str;
      bool        (Grammar<T>::*fromString)(void*, const std::string&) const = &Grammar<T>::fromString;
      int         (Grammar<T>::*evaluate)(void*, const std::string&) const = &Grammar<T>::evaluate;
      if (fromString && str && evaluate) {
      }
      return instance();
    }
  };

}
#endif  /* DD4HEP_DDCORE_DETAIL_GRAMMAR_H */
