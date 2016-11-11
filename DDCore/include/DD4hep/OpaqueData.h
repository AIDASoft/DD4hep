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
#ifndef DD4HEP_OPAQUEDATA_H
#define DD4HEP_OPAQUEDATA_H

// C/C++ include files
#include <typeinfo>
#include <vector>
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

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
    OpaqueData();
    /// Standard Destructor
    virtual ~OpaqueData();
    /// Copy constructor
    OpaqueData(const OpaqueData& c);
    /// Assignment operator
    OpaqueData& operator=(const OpaqueData& c);

  public:
    /// Data type
    const BasicGrammar* grammar;

  protected:
    /// Pointer to object data
    void* pointer;

  public:
    /// Create data block from string representation
    bool fromString(const std::string& rep);
    /// Create string representation of the data block
    std::string str()  const;
    /// Access type id of the condition
    const std::type_info& typeInfo() const;
    /// Access type name of the condition data block
    const std::string& dataType() const;
    /// Check if object is already bound....
    bool is_bound()  const  {  return 0 != pointer; }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& get();
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& get() const;
  };


  /// Class describing an opaque conditions data block
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class OpaqueDataBlock : public OpaqueData   {
  private:
    enum {
      PLAIN_DATA = 1<<0,
      ALLOC_DATA = 1<<1,
      BOUND_DATA = 1<<2
    } _DataTypes;
    /// Data buffer: plain data are allocated directly on this buffer
    /** Internal data buffer is sufficient to store any vector  */
    unsigned char data[sizeof(std::vector<void*>)];
    /// Destructor function -- only set if the object is valid
    void (*destruct)(void*);
    /// Constructor function -- only set if the object is valid
    void (*copy)(void*,const void*);

  public:
    /// Data buffer type: Must be a bitmap!
    unsigned int type;
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
    /// Bind data value
    bool bind(const BasicGrammar* grammar,
	      void (*ctor)(void*,const void*),
	      void (*dtor)(void*));
    /// Bind data value
    template <typename T> T& bind();
    /// Bind data value [Equivalent to set(value)]
    template <typename T> T& bind(const std::string& value);
    /// Bind data value. Ugly, but some compilers otherwise do not recognize the function in templates
    template <typename T> void bind(const T*);

    /// Set data value
    void assign(const void* ptr,const std::type_info& typ);
    /// Bind grammar and assign value
    template<typename T> T& set(const std::string& value);
  };

}      /* End namespace DD4hep */
#endif /* DD4HEP_OPAQUEDATA_H  */
