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

#include "XML/Evaluator.h"

namespace {
  void _init(XmlTools::Evaluator& e) {
    // Initialize numerical expressions parser with the standard math functions
    // and the system of units used by Gaudi (Geant4)
    e.setStdMath();
  }
  void _g4Units(XmlTools::Evaluator& e) {
    // ===================================================================================
    // Geant4 units
    // Geant4:  kilogram = joule*s*s/(m*m) 1/e_SI * 1e-6 * 1e9 1e9 / 1e3 / 1e3 = 1. / 1.60217733e-25
    e.setSystemOfUnits(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10, 1.0, 1.0, 1.0);
  }
}

namespace dd4hep {
  XmlTools::Evaluator& evaluator() {
    static XmlTools::Evaluator* e = 0;
    if ( !e )   {
      static XmlTools::Evaluator ev;
      _init(ev);
      _g4Units(ev);
      e = &ev;
    }
    return *e;
  }
}

