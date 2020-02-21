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
    OpaqueData(const OpaqueData& c) = default;
    /// Assignment operator
    OpaqueData& operator=(const OpaqueData& c) = default;

  public:
    /// Data type
    const BasicGrammar* grammar = 0;  //!

  protected:
    /// Pointer to object data
    void* pointer = 0;   //! No ROOT persistency

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

  protected:
    /// Data buffer: plain data are allocated directly on this buffer
    /** This internal data buffer is sufficient to store any 
     *  STL vector, list, map, etc. and hence should be sufficient to
     *  probably store normal relatively primitive basic objects.
     */
    unsigned char data[sizeof(std::vector<void*>)];

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
    /// Copy constructor
    OpaqueDataBlock(const OpaqueDataBlock& data);
    /// Standard Destructor
    ~OpaqueDataBlock();
    /// Assignment operator
    OpaqueDataBlock& operator=(const OpaqueDataBlock& clone);
    /// Move the data content: 'from' will be reset to NULL
    bool move(OpaqueDataBlock& from);
    /// Write access to the data buffer. Is only valid after call to bind<T>()
    void* ptr()  const {  return pointer;      }
    /// Bind data value
    void* bind(const BasicGrammar* grammar);
    /// Bind data value in place
    void* bind(void* ptr, size_t len, const BasicGrammar* grammar);
    /// Bind external data value to the pointer
    void bindExtern(void* ptr, const BasicGrammar* grammar);
    /// Bind external data value to the pointer
    template <typename T> void bindExtern(T* ptr);
    /// Construct conditions object and bind the data
    template <typename T, typename... Args> T& construct(Args&&... args);
    /// Bind data value
    template <typename T> T& bind();
    /// Bind data value
    template <typename T> T& bind(void* ptr, size_t len);
    /// Bind data value
    template <typename T> T& bind(const std::string& value);
    /// Bind data value
    template <typename T> T& bind(void* ptr, size_t len, const std::string& value);
    /// Set data value
    void assign(const void* ptr,const std::type_info& typ);
  };
}      /* End namespace dd4hep */

#include "DD4hep/BasicGrammar.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  template <typename T> inline void OpaqueDataBlock::bindExtern(T* ptr)   {
    this->OpaqueDataBlock::bindExtern((void*)ptr, SimpleGrammar::instance<T>());
  }
}      /* End namespace dd4hep */
#endif /* DD4HEP_OPAQUEDATA_H  */
