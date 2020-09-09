#ifndef DD4HEP_CONDITIONTYPES_H
#define DD4HEP_CONDITIONTYPES_H

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

#define DD4HEP_INSTANTIATE_GRAMMAR_TYPE(x)  \
namespace dd4hep {template<> const BasicGrammar& BasicGrammar::instance<x>()  { static Grammar<x> s; return s;}}

#endif
