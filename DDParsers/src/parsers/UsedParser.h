//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_PARSERS_USEDPARSERS_H
#define DD4HEP_DDCORE_PARSERS_USEDPARSERS_H


#if defined(DD4HEP_PARSER_HEADER)
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#else

// Standard dd4hep parser handling
#include "DDParsers/Parsers.h"
#include "DDParsers/ToStream.h"

#endif

#endif //  DD4HEP_DDCORE_PARSERS_USEDPARSERS_H
