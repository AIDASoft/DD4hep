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
#ifndef DD4HEP_DDCORE_BASICGRAMMAR_INL_H
#define DD4HEP_DDCORE_BASICGRAMMAR_INL_H

// Framework include files
#include "DD4hep/config.h"
#include "DD4hep/Primitives.h"
#include "Parsers/spirit/Parsers.h"
#include "Parsers/spirit/ToStream.h"

#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#endif

#endif  /* DD4HEP_DDCORE_BASICGRAMMAR_INL_H */
