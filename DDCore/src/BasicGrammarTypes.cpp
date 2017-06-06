//==========================================================================
//  AIDA Detector description implementation for LCD
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

// Framework include files
#include "DD4hep/detail/BasicGrammar_inl.h"

#ifndef DD4HEP_PARSERS_NO_ROOT
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(ROOT::Math::XYZPoint,eval_obj)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(ROOT::Math::XYZVector,eval_obj)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(ROOT::Math::PxPyPzEVector,eval_obj)
#endif

#if defined(DD4HEP_HAVE_ALL_PARSERS)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(char)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(short)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(long long)
#endif   //  DD4HEP_HAVE_ALL_PARSERS

DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(int)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(long)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(bool,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(float,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(double,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(std::string,eval_item)
