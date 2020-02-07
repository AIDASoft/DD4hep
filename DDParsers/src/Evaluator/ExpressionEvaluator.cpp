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
#include "Evaluator/Evaluator.h"
#include "Evaluator/DD4hepUnits.h"
namespace units = dd4hep;

namespace {
  void _init(dd4hep::tools::Evaluator& e) {
    // Initialize numerical expressions parser with the standard math functions
    // and the system of units used by Gaudi (Geant4)
    e.setStdMath();
  }
  void _cgsUnits(dd4hep::tools::Evaluator& e) {
    // ===================================================================================
    // CGS units
    e.setSystemOfUnits(100., 1000., 1.0, 1.0, 1.0, 1.0, 1.0);
  }
  void _tgeoUnits(dd4hep::tools::Evaluator& e) {
    // ===================================================================================
    // DDG4 units (TGeo) 1 sec = 10^9 [nsec]
    //                   1 Coulomb = 1/e As
    // Ampere = C/s = 1/e * As / s = 1. / 1.60217733e-19
    // kilogram = joule*s*s/(m*m)          1/e_SI * 1 *1 / 1e2 / 1e2

    //    e.setSystemOfUnits(1.e+2, 1./1.60217733e-6, 1.0, 1./1.60217733e-19, 1.0, 1.0, 1.0);
    // use the units as defined in DD4hepUnits.h:
    e.setSystemOfUnits( units::meter,
                        units::kilogram,
                        units::second,
                        units::ampere,
                        units::kelvin,
                        units::mole,
                        units::candela,
                        units::rad );
  }
  void _g4Units(dd4hep::tools::Evaluator& e) {
    // ===================================================================================
    // Geant4 units
    // Geant4:  kilogram = joule*s*s/(m*m) 1/e_SI * 1e-6 * 1e9 1e9 / 1e3 / 1e3 = 1. / 1.60217733e-25
    e.setSystemOfUnits(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10, 1.0, 1.0, 1.0);
  }
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  tools::Evaluator& evaluator() {
    static tools::Evaluator* e = 0;
    if ( !e )   {
      static tools::Evaluator ev;
      _init(ev);
      _tgeoUnits(ev);
      e = &ev;
    }
    return *e;
  }

  /// Access to G4 evaluator. Note: Uses Geant4 units!
  tools::Evaluator& g4Evaluator()   {
    static tools::Evaluator* e = 0;
    if ( !e )   {
      static tools::Evaluator ev;
      _init(ev);
      _g4Units(ev);
      e = &ev;
    }
    return *e;
  }

  /// Access to G4 evaluator. Note: Uses cgs units!
  tools::Evaluator& cgsEvaluator()   {
    static tools::Evaluator* e = 0;
    if ( !e )   {
      static tools::Evaluator ev;
      _init(ev);
      _cgsUnits(ev);
      e = &ev;
    }
    return *e;
  }
}

