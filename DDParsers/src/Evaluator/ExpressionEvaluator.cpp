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

/// Framework include files
#include "Parsers/config.h"
#include "Evaluator/Evaluator.h"
#include "Evaluator/detail/Evaluator.h"
#include "Evaluator/DD4hepUnits.h"

/// C/C++ include files

namespace units = dd4hep;

namespace {

  dd4hep::tools::Evaluator _cgsUnits() {
    // ===================================================================================
    // CGS units
    return dd4hep::tools::Evaluator(100., 1000., 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
  }
  
  dd4hep::tools::Evaluator _tgeoUnits() {
    // ===================================================================================
    // DDG4 units (TGeo) 1 sec = 10^9 [nsec]
    //                   1 Coulomb = 1/e As
    // Ampere = C/s = 1/e * As / s = 1. / 1.60217733e-19
    // kilogram = joule*s*s/(m*m)          1/e_SI * 1 *1 / 1e2 / 1e2

    //    e.setSystemOfUnits(1.e+2, 1./1.60217733e-6, 1.0, 1./1.60217733e-19, 1.0, 1.0, 1.0);
    // use the units as defined in DD4hepUnits.h:
    return dd4hep::tools::Evaluator( units::meter,
                                     units::kilogram,
                                     units::second,
                                     units::ampere,
                                     units::kelvin,
                                     units::mole,
                                     units::candela,
                                     units::rad );
  }
  
  dd4hep::tools::Evaluator _g4Units() {
    // ===================================================================================
    // Geant4 units
    // Geant4:  kilogram = joule*s*s/(m*m) 1/e_SI * 1e-6 * 1e9 1e9 / 1e3 / 1e3 = 1. / 1.60217733e-25
    return dd4hep::tools::Evaluator(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10, 1.0, 1.0, 1.0, 1.0);
  }
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  const tools::Evaluator& evaluator() {
    static const tools::Evaluator e = _tgeoUnits();
    return e;
  }

  /// Access to G4 evaluator. Note: Uses Geant4 units!
  const tools::Evaluator& g4Evaluator()   {
    static const tools::Evaluator e = _g4Units();
    return e;
  }

  /// Access to G4 evaluator. Note: Uses cgs units!
  const tools::Evaluator& cgsEvaluator()   {
    static const tools::Evaluator e = _cgsUnits();
    return e;
  }
}

