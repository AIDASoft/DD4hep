// $Id: ParsersObjects_PxPyPzEVector.cpp 2190 2016-04-08 08:31:42Z /DC=ch/DC=cern/OU=Organic Units/OU=Users/CN=sailer/CN=683529/CN=Andre Sailer $
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
    bool operator<(const PxPyPzEVector& a, const PxPyPzEVector& b);
  }}

// ============================================================================
namespace DD4hep {
  namespace Parsers {

    int parse(ROOT::Math::PxPyPzEVector& result, const std::string& input) {
      return parse_(result, input);
    }

    // ==========================================================================
    /*  parse the vector of vectors
     *  @param result (OUTPUT) the parser vector
     *  @param input (INPUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    IMPLEMENT_STL_PARSERS(ROOT::Math::PxPyPzEVector)
    typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > _L;
    IMPLEMENT_MAPPED_PARSERS(pair,_L)
  }
}
#endif

