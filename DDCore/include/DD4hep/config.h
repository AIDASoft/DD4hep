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

#ifdef DD4HEP_INSTANCE_COUNTS
#define INCREMENT_COUNTER InstanceCount::increment(this)
#define DECREMENT_COUNTER InstanceCount::decrement(this)
#else
#define INCREMENT_COUNTER
#define DECREMENT_COUNTER
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

  } /* End namespace detail  */
} /*   End namespace dd4hep    */
#endif    /* DD4HEP_CONFIG_H         */
