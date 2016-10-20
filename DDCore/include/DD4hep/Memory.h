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

#ifndef DD4HEP_MEMORY_H
#define DD4HEP_MEMORY_H

// Framework include files
#include "RVersion.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" // Code that causes warning goes here
#endif

// C/C++ include files
#include <memory>

// Use std::auto_ptr<T> instead of std::unique_ptr<T>
#define DD4HEP_DD4HEP_PTR_AUTO

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Out version of the std auto_ptr implementation base either on auto_ptr or unique_ptr.
  /**
   *  Only need to overload constructor. Everything else 
   *  like operator->, operator*, get, release etc. comes from base.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_GEOMETRY
   */
  template <typename T> class dd4hep_ptr
  //#if defined(DD4HEP_NEVER) && __cplusplus >= 201103L && ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
#if !defined(DD4HEP_DD4HEP_PTR_AUTO) && __cplusplus >= 201103L && ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
    : public std::unique_ptr<T>  {
  public:
    typedef std::unique_ptr<T> base_t;
#else
      : public std::auto_ptr<T>  {
    public:
        typedef std::auto_ptr<T> base_t;
        void swap(base_t& c) {
          this->base_t::operator=(base_t(c.release()));
        }
        /// Constructor from copy
        dd4hep_ptr(dd4hep_ptr<T>& c) : base_t(c) {}
#endif
    public:
        /// Default Constructor.
        dd4hep_ptr() : base_t() {}
        /// Constructor from pointer
        dd4hep_ptr(T* p) : base_t(p) {}
        /// Constructor from copy
        dd4hep_ptr(base_t& c) : base_t(c) {}
        /// Assignment operator
        dd4hep_ptr& operator=(base_t& c) {
          if ( this != &c )  {
            this->swap(c);
          }
          return *this;
        }
        /// Assignment operator
        dd4hep_ptr& adopt(T* ptr) {
          base_t smart_ptr(ptr);
          this->swap(smart_ptr);
          return *this;
        }
      };
  }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif  // DD4HEP_MEMORY_H
