// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDG4/Geant4Field.h"

using namespace DD4hep::Simulation;
using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

G4bool Geant4Field::DoesFieldChangeEnergy() const {
  return m_field.changesEnergy();
}

void Geant4Field::GetFieldValue(const double pos[4], double *field) const {
  return m_field.magneticField(pos,field);
}

