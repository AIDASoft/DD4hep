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
#ifndef DD4HEP_CONFIG_H
#define DD4HEP_CONFIG_H

#define DD4HEP_INSTANCE_COUNTS 1
#define DD4HEP_USE_SAFE_CAST   1

/// Enable to have more debugging information for conditions and keys
/// If enabled it overrides DD4HEP_MINIMAL_CONDITIONS and sets it to true
/// If enabled it overrides DD4HEP_CONDITIONS_HAVE_NAME and sets it to true
#if defined(DD4HEP_DEBUG)
#define DD4HEP_CONDITIONS_DEBUG     1
#endif

#if !defined(DD4HEP_CONDITIONS_DEBUG)
/// Enable this if you want to minimize the footprint of conditions
#define DD4HEP_MINIMAL_CONDITIONS   1
#endif

/// Enable flag to store conditions names to keys (needs some support from user code!)
#define DD4HEP_CONDITIONS_HAVE_NAME 1

/// Valid implementations of the Gaudi plugin service are 1 and 2
#define DD4HEP_PLUGINSVC_VERSION    2

#ifdef DD4HEP_INSTANCE_COUNTS
#define INCREMENT_COUNTER InstanceCount::increment(this)
#define DECREMENT_COUNTER InstanceCount::decrement(this)
#else
#define INCREMENT_COUNTER
#define DECREMENT_COUNTER
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace cond   {
  }       /* End namespace cond      */

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {
    /// Namespace for the AIDA detector description matrix helpers
    namespace matrix {
    }     /* End namespace matrix    */
  }       /* End namespace detail    */
}         /* End namespace dd4hep    */
#endif // DD4HEP_CONFIG_H
