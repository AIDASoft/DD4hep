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
#ifndef DD4HEP_DDG4_BASICGRAMMAR_H
#define DD4HEP_DDG4_BASICGRAMMAR_H

// C/C++ include files
#include <string>
#include <typeinfo>

// Forward declarations
class TClass;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

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
    void initialize()   const;
    int initialized_data_type()  const;
    TClass* initialized_clazz()  const;

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
    virtual const std::type_info& type() const = 0;
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const = 0;
    /// Serialize an opaque value to a string
    virtual std::string str(const void* ptr) const = 0;
    /// Set value from serialized string. On successful data conversion TRUE is returned.
    virtual bool fromString(void* ptr, const std::string& value) const = 0;
    /// Opaque object destructor
    virtual void destruct(void* pointer) const = 0;
    /// Opaque object copy construction. Memory must be allocated externally
    virtual void copy(void* to, const void* from)  const = 0;
    /// Bind opaque address to object
    virtual void bind(void* pointer)  const = 0;
  };
}      // End namespace dd4hep

#endif  /* DD4HEP_DDG4_BASICGRAMMAR_H */
