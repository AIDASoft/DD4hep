//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
//
//  For the licensing terms see OnlineSys/LICENSE.
//
//--------------------------------------------------------------------------
//  ExtraProperties.cpp
//--------------------------------------------------------------------------
//
//  Package    : Dataflow
//
//  Author     : Markus Frank
//==========================================================================
// Note:
//
// This is an example file, which shows how to instantiate extera parsers,
// which are not part of dd4hep, but may be interesting for a
// client application.
//
//==========================================================================

/// Framework includes
#include "ParsersFactory.h"

PARSERS_DEF_FOR_SINGLE(unsigned int)
PARSERS_DEF_FOR_SINGLE(unsigned long)
PARSERS_DEF_FOR_SINGLE(long long)
PARSERS_DEF_FOR_SINGLE(unsigned long long)
