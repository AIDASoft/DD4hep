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

// Framework include files
#include <DD4hep/GrammarParsed.h>

#include "Math/Point3D.h"
#include "Math/Vector4D.h"
#include "Math/Vector3D.h"

#ifndef DD4HEP_PARSERS_NO_ROOT

/// ROOT include files
#include <Math/Point3D.h>
#include <Math/Vector4D.h>
#include <Math/Vector3D.h>

namespace ROOT {
  namespace Math {
    /// Allow point insertion of a point in maps
    bool operator<(const XYZPoint& a, const XYZPoint& b);
    /// Allow 3-vector insertion of a  in maps
    bool operator<(const XYZVector& a, const XYZVector& b);
    /// Allow 4-vector insertion of a  in maps
    bool operator<(const PxPyPzEVector& a, const PxPyPzEVector& b);
  }
}
DD4HEP_DEFINE_PARSER_GRAMMAR_ROOTMATH(ROOT::Math::XYZPoint,eval_obj)
DD4HEP_DEFINE_PARSER_GRAMMAR_ROOTMATH(ROOT::Math::XYZVector,eval_obj)
DD4HEP_DEFINE_PARSER_GRAMMAR_ROOTMATH(ROOT::Math::PxPyPzEVector,eval_obj)
#endif

#if defined(DD4HEP_HAVE_ALL_PARSERS)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(char)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(short)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(long long)
#else
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(unsigned long,eval_item)
#ifdef __APPLE__
DD4HEP_DEFINE_PARSER_GRAMMAR(long long,eval_item)
#endif
#endif   //  DD4HEP_HAVE_ALL_PARSERS

DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(int)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(long)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(bool,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(float,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(double,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(std::string,eval_item)
