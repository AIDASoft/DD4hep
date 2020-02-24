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
//==========================================================================
// Include files
//==========================================================================
#ifndef DD4HEP_PARSERS_NO_ROOT
#include "Parsers/spirit/ParsersStandardListCommon.h"

namespace ROOT {  namespace Math {
    bool operator<(const RotationZYX& a, const RotationZYX& b)  {
      if ( a.Theta() < b.Theta() ) return true;
      if ( a.Phi() < b.Phi() ) return true;
      if ( a.Psi() < b.Psi() ) return true;
      return false;
    }
  }}

template struct std::less<ROOT::Math::RotationZYX>;

// ============================================================================
namespace dd4hep {
  namespace Parsers {

    // ==========================================================================
    template <> int parse(ROOT::Math::RotationZYX& result,const std::string& input) {
      return parse_(result, input);
    }
    // ==========================================================================
    /*  parse the vector of points
     *  @param resut (OUTPUT) the parser vector
     *  @param input (INPIUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    IMPLEMENT_STL_PARSERS(ROOT::Math::RotationZYX)
    IMPLEMENT_MAPPED_PARSERS(pair,ROOT::Math::RotationZYX)
  }
}
#endif

