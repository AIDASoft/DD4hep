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
// Setup XML parsing for the use of Apache Xerces-C and TiXml
//
//==========================================================================
#ifndef DD4HEP_DDCORE_CONFIG_H
#define DD4HEP_DDCORE_CONFIG_H

#include "Parsers/config.h"
#define DD4HEP_INSTANCE_COUNTS 1
#define DD4HEP_USE_SAFE_CAST   1

#ifdef DD4HEP_INSTANCE_COUNTS
#define INCREMENT_COUNTER InstanceCount::increment(this)
#define DECREMENT_COUNTER InstanceCount::decrement(this)
#else
#define INCREMENT_COUNTER
#define DECREMENT_COUNTER
#endif

#endif // DD4HEP_DDCORE_CONFIG_H
