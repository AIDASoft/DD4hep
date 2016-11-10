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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the OpaqueData include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_OPAQUEDATA_INL_H
#define DD4HEP_OPAQUEDATA_INL_H

// Framework include files
#include "DD4hep/Primitives.h"
#include "DD4hep/OpaqueData.h"
#include "DD4hep/BasicGrammar.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  namespace   {
    template <typename T> static void opaqueCopyObject(void* t, const void* s)  {
      new(t) T(*(const T*)s);
    }
    template <typename T> static void opaqueDestructObject(void* p)  {
      T* t = (T*)p;
      t->~T();
    }
  }
  /// Generic getter. Specify the exact type, not a polymorph type
  template <typename T> T& OpaqueData::get() {
    if (!grammar || (grammar->type() != typeid(T))) { throw std::bad_cast(); }
    return *(T*)pointer;
  }
  /// Generic getter (const version). Specify the exact type, not a polymorph type
  template <typename T> const T& OpaqueData::get() const {
    if (!grammar || (grammar->type() != typeid(T))) { throw std::bad_cast(); }
    return *(T*)pointer;
  }
  /// Bind data value
  template <typename T> T& OpaqueDataBlock::bind()  {
    this->bind(&BasicGrammar::instance<T>(),opaqueCopyObject<T>,opaqueDestructObject<T>);
    return *(new(this->pointer) T());
  }
  /// Bind grammar and assign value
  template <typename T> T& OpaqueDataBlock::bind(const std::string& value)   {
    T& ret = this->bind<T>();
    if ( !value.empty() && !this->fromString(value) )  {
      throw std::runtime_error("OpaqueDataBlock::set> Failed to bind type "+
                               typeName(typeid(T))+" to condition data block.");
    }
    return ret;
  }
  /// Bind grammar and assign value
  template <typename T> T& OpaqueDataBlock::set(const std::string& value)   {
    T& ret = this->bind<T>();
    if ( !value.empty() && !this->fromString(value) )  {
      throw std::runtime_error("OpaqueDataBlock::set> Failed to bind type "+
                               typeName(typeid(T))+" to condition data block.");
    }
    return ret;
  }

} /* End namespace DD4hep               */
#endif    /* DD4HEP_OPAQUEDATA_INL_H    */
