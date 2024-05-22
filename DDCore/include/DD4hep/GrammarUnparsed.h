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
#ifndef DD4HEP_GRAMMARUNPARSED_H
#define DD4HEP_GRAMMARUNPARSED_H

#if defined(DD4HEP_GRAMMARPARSED_H)
#error "The header files GrammarParsed.h and GrammarUnparsed.h may not be included in the same compilation unit!"
#endif

// Framework include files
#include <DD4hep/Grammar.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Standarsd constructor
  template <typename TYPE> const BasicGrammar& BasicGrammar::instance()  {
    static Grammar<TYPE> s_gr;
    return s_gr;
  }
}
#endif // DD4HEP_GRAMMARUNPARSED_H
