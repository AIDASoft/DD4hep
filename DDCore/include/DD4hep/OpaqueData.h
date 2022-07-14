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
#ifndef DD4HEP_OPAQUEDATA_H
#define DD4HEP_OPAQUEDATA_H

// Framework include files
#include "DD4hep/Grammar.h"

// C/C++ include files
#include <typeinfo>
#include <vector>
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class BasicGrammar;

  /// Class describing an opaque data block
  /**
   *  Access methods are templated. Once the access is fixed
   *  on the first call, the data type may not be changed anymore.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class OpaqueData   {
  private:
  protected:
    /// Standard initializing constructor
    OpaqueData() = default;
    /// Standard Destructor
    virtual ~OpaqueData() = default;
    /// Copy constructor
    OpaqueData(const OpaqueData& copy) = default;
    /// Assignment operator
    OpaqueData& operator=(const OpaqueData& copy) = default;

  public:
    /// Data type
    const BasicGrammar* grammar = 0;  //! No ROOT persistency

  protected:
    /// Pointer to object data
    void* pointer = 0;                //! No ROOT persistency

  public:
    /// Create data block from string representation
    bool fromString(const std::string& rep);
    /// Create string representation of the data block
    std::string str()  const;
    /// Access type id of the condition
    const std::type_info& typeInfo() const;
    /// Access type name of the condition data block
    const std::string& dataType() const;
    /// Access to the data buffer (read only!). Is only valid after call to bind<T>()
    const void* ptr()  const {  return pointer;      }
    /// Check if object is already bound....
    bool is_bound()  const   {  return 0 != pointer; }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& get();
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& get() const;
    /// Generic getter. Resolves polymorph types. It is mandatory that the datatype is polymorph!
    template <typename T> T& as();
    /// Generic getter (const version). Resolves polymorph types. It is mandatory that the datatype is polymorph!
    template <typename T> const T& as() const;
  };

  
  /// Class describing an opaque conditions data block
  /**
   *  This class is used to handle the actual data IO.
   *  Hence, we have write access to the data in abstract form.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class OpaqueDataBlock : public OpaqueData   {

  public:
    /// Buffer size of the in-place data buffer
    constexpr static const size_t BUFFER_SIZE = 40;

  protected:
    /// Data buffer: plain data are allocated directly on this buffer
    /** This internal data buffer is sufficient to store any 
     *  STL vector, list, map, etc. and hence should be sufficient to
     *  probably store normal relatively primitive basic objects.
     *
     *  It appears that on clang the size of std::any is 32 bytes (16 bytes on g++_,
     *  whereas the size of std::vector is 24. Lets take 40 bytes and check it in the 
     *  Conditions_any_basic example...
     */
    unsigned char data[BUFFER_SIZE];

  public:
    enum _DataTypes  {
      PLAIN_DATA  =  1<<0,
      ALLOC_DATA  =  1<<1,
      STACK_DATA  =  1<<2,
      BOUND_DATA  =  1<<3,
      EXTERN_DATA =  1<<4
    };
    /// Data buffer type: Must be a bitmap of enum _DataTypes!
    unsigned int type;

  public:
    /// Standard initializing constructor
    OpaqueDataBlock();
    /// Initializing constructor binding data to buffer with move
    template <typename OBJECT> OpaqueDataBlock(OBJECT&& data);
    /// Copy constructor (Required by ROOT dictionaries)
    OpaqueDataBlock(const OpaqueDataBlock& copy);
    /// Standard Destructor
    ~OpaqueDataBlock();
    /// Assignment operator (Required by ROOT dictionaries)
    OpaqueDataBlock& operator=(const OpaqueDataBlock& copy);
    /// Write access to the data buffer. Is only valid after call to bind<T>()
    void* ptr()  const {  return pointer;      }
    /// Bind data value
    void* bind(const BasicGrammar* grammar);
    /// Bind data value in place
    void* bind(void* ptr, size_t len, const BasicGrammar* grammar);
    /// Bind external data value to the pointer
    void bindExtern(void* ptr, const BasicGrammar* grammar);
    /// Construct conditions object and bind the data
    template <typename T, typename... Args> T& construct(Args... args);
    /// Bind data value
    template <typename T> T& bind();
    /// Bind data value
    template <typename T> T& bind(void* ptr, size_t len);
    /// Bind data value
    template <typename T> T& bind(const std::string& value);
    /// Bind data value
    template <typename T> T& bind(void* ptr, size_t len, const std::string& value);
    /// Bind data value
    template <typename T> T& bind(T&& data);
    /// Bind external data value to the pointer
    template <typename T> void bindExtern(T* ptr);
  };

  /// Generic getter. Specify the exact type, not a polymorph type
  template <typename T> inline T& OpaqueData::get() {
    if (!grammar || !grammar->equals(typeid(T))) { throw std::bad_cast(); }
    return *(T*)pointer;
  }

  /// Generic getter (const version). Specify the exact type, not a polymorph type
  template <typename T> inline const T& OpaqueData::get() const {
    if (!grammar || !grammar->equals(typeid(T))) { throw std::bad_cast(); }
    return *(T*)pointer;
  }

  /// Generic getter. Specify the exact type, not a polymorph type
  template <typename T> inline T& OpaqueData::as() {
    if ( grammar )   {
      T* obj = (T*)(grammar->cast().apply_dynCast(Cast::instance<T>(), this->pointer));
      if ( obj ) return *obj;
    }
    throw std::bad_cast();
  }

  /// Generic getter (const version). Specify the exact type, not a polymorph type
  template <typename T> inline const T& OpaqueData::as() const {
    if ( grammar )   {
      const T* obj = (const T*)(grammar->cast().apply_dynCast(Cast::instance<T>(), this->pointer));
      if ( obj ) return *obj;
    }
    throw std::bad_cast();
  }

  /// Initializing constructor binding data to buffer with move
  template <typename OBJECT> OpaqueDataBlock::OpaqueDataBlock(OBJECT&& obj)    {
    this->bind(&BasicGrammar::instance<OBJECT>());
    new(this->pointer) OBJECT(std::move(obj));
  }

  /// Construct conditions object and bind the data
  template <typename T, typename... Args> inline T& OpaqueDataBlock::construct(Args... args)   {
    this->bind(&BasicGrammar::instance<T>());
    return *(new(this->pointer) T(std::forward<Args>(args)...));
  }

  /// Bind data value
  template <typename T> inline T& OpaqueDataBlock::bind()  {
    this->bind(&BasicGrammar::instance<T>());
    return *(new(this->pointer) T());
  }

  /// Bind data value
  template <typename T> inline T& OpaqueDataBlock::bind(T&& obj)   {
    this->bind(&BasicGrammar::instance<T>());
    new(this->pointer) T(std::move(obj));
  }

  /// Bind data value
  template <typename T> inline T& OpaqueDataBlock::bind(void* ptr, size_t len)  {
    this->bind(ptr,len,&BasicGrammar::instance<T>());
    return *(new(this->pointer) T());
  }

  /// Bind grammar and assign value
  template <typename T> inline T& OpaqueDataBlock::bind(const std::string& value)   {
    T& ret = this->bind<T>();
    if ( !value.empty() && !this->fromString(value) )  {
      throw std::runtime_error("OpaqueDataBlock::set> Failed to bind type "+
                               typeName(typeid(T))+" to condition data block.");
    }
    return ret;
  }

  /// Bind grammar and assign value
  template <typename T> inline T& OpaqueDataBlock::bind(void* ptr, size_t len, const std::string& value)   {
    T& ret = this->bind<T>(ptr, len);
    if ( !value.empty() && !this->fromString(value) )  {
      throw std::runtime_error("OpaqueDataBlock::set> Failed to bind type "+
                               typeName(typeid(T))+" to condition data block.");
    }
    return ret;
  }
  /// Bind external data value to the pointer
  template <typename T> inline void OpaqueDataBlock::bindExtern(T* ptr)    {
    bindExtern(ptr, &BasicGrammar::instance<T>());
  }

}      /* End namespace dd4hep */
#endif // DD4HEP_OPAQUEDATA_H
