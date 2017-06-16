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
#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the dd4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#else

// Standard dd4hep parser handling
#include "DD4hep/ToStream.h"

#endif

#ifndef DD4HEP_PARSERS_NO_ROOT

// ============================================================================
// print XYZ-point
std::ostream& dd4hep::Utils::toStream(const ROOT::Math::XYZPoint&  obj, std::ostream& s)  {
  s << "( ";
  toStream(obj.X () , s );
  s << " , ";
  toStream(obj.Y () , s );
  s << " , ";
  toStream(obj.Z () , s );
  s << " )";
  return s;
}
// ============================================================================
// print XYZ-vector
std::ostream& dd4hep::Utils::toStream(const ROOT::Math::XYZVector& obj, std::ostream& s)  {
  s << "( ";
  toStream(obj.X () , s );
  s << " , ";
  toStream(obj.Y () , s );
  s << " , ";
  toStream(obj.Z () , s );
  s << " )";
  return s;
}
// ============================================================================
// print LorentzVector
std::ostream& dd4hep::Utils::toStream(const ROOT::Math::PxPyPzEVector& obj, std::ostream& s){
  s << "( ";
  toStream(obj.Px () , s , 12 );
  s << " , ";
  toStream(obj.Py () , s , 12 );
  s << " , ";
  toStream(obj.Pz () , s , 13 );
  s << " , ";
  toStream(obj.E  () , s , 14 );
  s << " )";
  return s;
}
#endif
