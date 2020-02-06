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
#include "Parsers/spirit/ToStream.h"

#endif

#ifndef DD4HEP_PARSERS_NO_ROOT
#include "Math/Translation3D.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {
    // ============================================================================
    // print XYZ-point
    std::ostream& toStream_(const ROOT::Math::XYZPoint&  obj, std::ostream& s)  {
      s << "( ";
      toStream_(obj.X () , s );
      s << " , ";
      toStream_(obj.Y () , s );
      s << " , ";
      toStream_(obj.Z () , s );
      s << " )";
      return s;
    }
    template <> std::ostream& toStream(const ROOT::Math::XYZPoint& obj, std::ostream& s)
    {  return toStream_(obj, s); }
    // ============================================================================
    // print XYZ-vector
    std::ostream& toStream_(const ROOT::Math::XYZVector& obj, std::ostream& s)  {
      s << "( ";
      toStream_(obj.X () , s );
      s << " , ";
      toStream_(obj.Y () , s );
      s << " , ";
      toStream_(obj.Z () , s );
      s << " )";
      return s;
    }
    template <> std::ostream& toStream(const ROOT::Math::XYZVector& obj, std::ostream& s)
    {  return toStream_(obj, s); }
    // ============================================================================
    // print LorentzVector
    std::ostream& toStream_(const ROOT::Math::PxPyPzEVector& obj, std::ostream& s){
      s << "( ";
      toStream_(obj.Px () , s , 12 );
      s << " , ";
      toStream_(obj.Py () , s , 12 );
      s << " , ";
      toStream_(obj.Pz () , s , 13 );
      s << " , ";
      toStream_(obj.E  () , s , 14 );
      s << " )";
      return s;
    }
    template <> std::ostream& toStream(const ROOT::Math::PxPyPzEVector& obj, std::ostream& s)
    {  return toStream_(obj, s); }
    // ============================================================================
    // print Translation3D
    std::ostream& toStream_(const ROOT::Math::Translation3D& obj, std::ostream& s){
      double x,y,z;
      obj.GetComponents(x,y,z);
      s << "( ";
      toStream_(x , s );
      s << " , ";
      toStream_(y , s );
      s << " , ";
      toStream_(z, s );
      s << " )";
      return s;
    }
    template <> std::ostream& toStream(const ROOT::Math::Translation3D& obj, std::ostream& s)
    {  return toStream_(obj, s); }
    // ============================================================================
    // print RotationZYX
    std::ostream& toStream_(const ROOT::Math::RotationZYX& obj, std::ostream& s){
      s << "( ";
      toStream_(obj.Phi () , s , 12 );    // Z-angle
      s << " , ";
      toStream_(obj.Theta () , s , 12 );  // Y-angle
      s << " , ";
      toStream_(obj.Psi () , s , 12 );    // X-angle
      s << " )";
      return s;
    }
    template <> std::ostream& toStream(const ROOT::Math::RotationZYX& obj, std::ostream& s)
    {  return toStream_(obj, s); }
  }
}
#endif
