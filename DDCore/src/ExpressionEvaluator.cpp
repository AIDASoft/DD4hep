#include "XML/Evaluator.h"

namespace DD4hep {
  XmlTools::Evaluator& evaluator() {
    static XmlTools::Evaluator e;
    return e;
  }
}

namespace {
  struct _Init {
    _Init() {
      // Initialize numerical expressions parser with the standard math functions
      // and the system of units used by Gaudi (Geant4)
      DD4hep::evaluator().setStdMath();
      // ======================================================================================
      // Geant4 units
      // Geant4:  kilogram = joule*s*s/(m*m) 1/e_SI * 1e-6 * 1e9 1e9 / 1e3 / 1e3 = 1. / 1.60217733e-25
      //DD4hep::evaluator().setSystemOfUnits(1.e+3, 1. / 1.60217733e-25, 1.e+9, 1. / 1.60217733e-10, 1.0, 1.0, 1.0);
      // ======================================================================================
      // CGS units
      //DD4hep::evaluator().setSystemOfUnits(100., 1000., 1.0, 1.0, 1.0, 1.0, 1.0);
      // ======================================================================================
      // DDG4 units (TGeo) 1 sec = 10^9 [nsec]
      //                   1 Coulomb = 1/e As
      // Ampere = C/s = 1/e * As / s = 1. / 1.60217733e-19
      // kilogram = joule*s*s/(m*m)          1/e_SI * 1 *1 / 1e2 / 1e2
      DD4hep::evaluator().setSystemOfUnits(1.e+2, 1./1.60217733e-6, 1.0, 1./1.60217733e-19, 1.0, 1.0, 1.0);
    }

  };
  _Init s_init;
}
