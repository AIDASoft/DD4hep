// $Id$
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
#include "ParsersStandardListCommon.h"

namespace ROOT {  namespace Math {
    bool operator<(const XYZPoint& a, const XYZPoint& b);
  }}
// ============================================================================
namespace DD4hep {
  namespace Parsers {

    // ==========================================================================
    int parse(ROOT::Math::XYZPoint& result,const std::string& input) {
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
    IMPLEMENT_STL_PARSERS(ROOT::Math::XYZPoint)
    IMPLEMENT_MAPPED_PARSERS(pair,ROOT::Math::XYZPoint)
  }
}


