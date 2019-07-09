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

#ifndef DD4HEP_MEMORY_H
#define DD4HEP_MEMORY_H

// Framework include files
#include "RVersion.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" // Code that causes warning goes here
#elif defined(__llvm__) || defined(__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations" // Code that causes warning goes here
#endif


// C/C++ include files
#include <memory>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Out version of the std auto_ptr implementation base either on auto_ptr or unique_ptr.
  /**
   *  Only need to overload constructor. Everything else 
   *  like operator->, operator*, get, release etc. comes from base.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  template <typename T> class dd4hep_ptr
    : public std::unique_ptr<T>  {
    public:
        typedef std::unique_ptr<T> base_t;
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
#elif defined(__llvm__) || defined(__APPLE__)
#pragma clang diagnostic pop
#endif

#endif  // DD4HEP_MEMORY_H
