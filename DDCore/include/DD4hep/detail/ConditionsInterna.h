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
// instantiate code. Otherwise the Conditions include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_CONDITIONINTERNA_H
#define DD4HEP_DDCORE_CONDITIONINTERNA_H

// Framework include files
#include "DD4hep/DetElement.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Grammar.h"
#include "DD4hep/NamedObject.h"

// C/C++ include files
#include <map>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class IOVType;

  /// DD4hep internal namespace declaration for utilities and implementation details
  /** Internaly defined datastructures are not presented to the
   *  user directly, but are used by dedicated views.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  namespace detail {

    /// The data class behind a conditions handle.
    /**
     *  See ConditionsInterna.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionObject
#if defined(DD4HEP_CONDITIONS_DEBUG) || !defined(DD4HEP_MINIMAL_CONDITIONS)
      : public NamedObject
#endif
    {
    public:
      /// Condition value (in string form)
      std::string          value;
#if defined(DD4HEP_CONDITIONS_DEBUG) || !defined(DD4HEP_MINIMAL_CONDITIONS)
      /// Condition validity (in string form)
      std::string          validity;
      /// Condition address
      std::string          address;
      /// Comment string
      std::string          comment;
#endif
      /// Data block
      OpaqueDataBlock      data;
      /// Interval of validity
      const IOV*           iov   = 0;     //! No ROOT persistency
      /// Hash value of the name
      Condition::key_type  hash  = 0;
      /// Flags
      Condition::mask_type flags = 0;
      /// Reference count
      int                  refCount = 1;
      /// Default constructor
      ConditionObject();
      /// No copy constructor
      ConditionObject(const ConditionObject&) = delete;
      /// Standard constructor
      ConditionObject(const std::string& nam,const std::string& tit="");
      /// Standard Destructor
      virtual ~ConditionObject();
      /// No assignment operation
      ConditionObject& operator=(const ConditionObject&) = delete;
      /// Increase reference counter (Used by persistency mechanism)
      ConditionObject* addRef()  {  ++refCount; return this;         }
      /// Release object (Used by persistency mechanism)
      void release();
      /// Data offset from the opaque data block pointer to the condition
      static size_t offset();
      /// Move data content: 'from' will be reset to NULL
      ConditionObject& move(ConditionObject& from);
      /// Access safely the IOV
      const IOV* iovData() const;
      /// Access safely the IOV-type
      const IOVType* iovType() const;
      /// Access the bound data payload. Exception id object is unbound
      void* payload() const;
      /// Check if object is already bound....
      bool is_bound()  const                           {  return data.is_bound();         }
      bool is_traced()  const                          {  return true;                    }
      /// Flag operations: Set a conditons flag
      void setFlag(Condition::mask_type option)        {  flags |= option;                }
      /// Flag operations: UN-Set a conditons flag
      void unFlag(Condition::mask_type option)         {  flags &= ~option;               }
      /// Flag operations: Test for a given a conditons flag
      bool testFlag(Condition::mask_type option) const {  return option == (flags&option);}
    };

  }       /* End namespace detail                   */
}         /* End namespace dd4hep                   */
#endif    /* DD4HEP_DDCORE_CONDITIONINTERNA_H       */
