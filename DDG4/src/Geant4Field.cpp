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
#include "DDG4/Geant4Field.h"
#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"
namespace units = dd4hep;

using namespace dd4hep::sim;

G4bool Geant4Field::DoesFieldChangeEnergy() const {
  return m_field.changesEnergy();
}

void Geant4Field::GetFieldValue(const double pos[4], double *field) const {
  static const double fac1 = units::mm/CLHEP::mm;
  static const double fac2 = CLHEP::tesla/units::tesla;
  double p[3] = {pos[0]*fac1, pos[1]*fac1, pos[2]*fac1}; // Convert from CLHEP units to tgeo units
  field[0] = field[1] = field[2] = 0.0;                  // Reset field vector
  m_field.magneticField(p, field);
  field[0] *= fac2;                                      // Convert from tgeo units to CLHEP units
  field[1] *= fac2;
  field[2] *= fac2;
  //::printf("Pos: %7.4f %7.4f %7.4f --> %9g %9g %9g\n",p[0],p[1],p[2],field[0],field[1],field[2]);
}
