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
#include "DD4hep/BasicGrammar.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Concrete type dependent grammar definition
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  template <typename TYPE> class Grammar : public BasicGrammar {
    friend class BasicGrammar;
    /// Cached type information name
    std::string m_typeName;
  public:
    /// Default destructor
    virtual ~Grammar();
    /// Standarsd constructor
    Grammar();

    /** Base class overrides   */
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const  override;
    /// Access to the type information name
    virtual const std::string& type_name() const  override;    
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const  override;
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const  override;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const  override;
    /// Opaque object destructor
    virtual void destruct(void* pointer) const  override;
    /// Opaque object copy construction. Memory must be allocated externally
    virtual void copy(void* to, const void* from)  const  override;

    /** Class member function   */
    /// Evaluate string value if possible before calling boost::spirit
    virtual int evaluate(void* ptr, const std::string& value) const;
  };
}
#endif  /* DD4HEP_DDCORE_DETAIL_GRAMMAR_H */
