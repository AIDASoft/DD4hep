// $Id: ParsersObjects_XYZVector.cpp 2119 2016-03-03 14:10:32Z markus.frank@cern.ch $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "ParsersStandardListCommon.h"

namespace ROOT {  namespace Math {
    bool operator<(const XYZVector& a, const XYZVector& b)  {
      if ( a.X() < b.X() ) return true;
      if ( a.Y() < b.Y() ) return true;
      if ( a.Z() < b.Z() ) return true;
      return false;
    }
  }}

template struct std::less<ROOT::Math::XYZPoint>;
template struct std::less<ROOT::Math::XYZVector>;
template struct std::less<ROOT::Math::PxPyPzEVector>;

// ============================================================================
namespace DD4hep {
  namespace Parsers {

    // ==========================================================================
    /*  parse 3D-vector
     *  @param result (output) the parsed vector
     *  @param input  (input)  the input string
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    int parse(ROOT::Math::XYZVector& result,const std::string& input) {
      ROOT::Math::XYZPoint point;
      int sc = parse(point,input);
      if ( 0 == sc ){ return sc; }                               // RETURN
      result = point;
      return 1;
    }

    // ==========================================================================
    /*  parse the vector of vectors
     *  @param resut (OUTPUT) the parser vector
     *  @param input (INPIUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    IMPLEMENT_STL_PARSERS(ROOT::Math::XYZVector)
    IMPLEMENT_MAPPED_PARSERS(pair,ROOT::Math::XYZVector)
  }
}
#endif
