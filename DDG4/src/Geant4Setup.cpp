// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDG4/Factories.h"
#include "DDG4/Geant4Field.h"
#include "DDG4/Geant4Converter.h"
#include "DD4hep/Fields.h"


#include "G4TransportationManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ChordFinder.hh"
#include "G4PropagatorInField.hh"

using namespace DD4hep::Simulation;
using namespace DD4hep::Geometry;
using namespace std;
typedef DD4hep::Geometry::LCDD lcdd_t;

struct PropertyMap {
  const map<string,string>& vals;
  PropertyMap(const map<string,string>& v) : vals(v) {}
  string value(const string& key) const;
  double toDouble(const string& key) const;
  bool operator[](const string& key) const  { return vals.find(key) != vals.end(); }
};

string PropertyMap::value(const string& key) const {
  lcdd_t::PropertyValues::const_iterator iV = vals.find(key);
  return iV == vals.end() ? "" : (*iV).second;
}

double PropertyMap::toDouble(const string& key) const {
  return _toDouble(value(key));
}

static long setup_fields(lcdd_t& lcdd, const Geant4Converter& cnv, const map<string,string>& vals) {
  PropertyMap pm(vals);
  DD4hep::Geometry::OverlayedField fld  = lcdd.field();
  G4MagIntegratorStepper*  stepper = 0;
  G4MagneticField*         field = 0;
  G4FieldManager*          fieldMgr = 0;
  G4TransportationManager* tr = 0;
  G4PropagatorInField*     propagator = 0;
  G4ChordFinder*           chordFinder = 0;  

  lcdd_t::PropertyValues::const_iterator iV;

  string eq_typ      = pm.value("equation");
  string stepper_typ = pm.value("stepper");
  double value;

  field    = new Geant4Field(fld);
  tr       = G4TransportationManager::GetTransportationManager();
  fieldMgr = tr->GetFieldManager();

  fieldMgr->SetFieldChangesEnergy(fld.changesEnergy());
  fieldMgr->SetDetectorField(field);

  G4Mag_EqRhs* equation = ROOT::Reflex::PluginService::Create<G4Mag_EqRhs*>(eq_typ,field);
  stepper = ROOT::Reflex::PluginService::Create<G4MagIntegratorStepper*>(stepper_typ,equation);

  iV = vals.find("min_chord_step");
  value = _toDouble((iV == vals.end()) ? string("1.0e-2 * mm") : (*iV).second);
  chordFinder  = new G4ChordFinder(field,value,stepper);
  propagator   = tr->GetPropagatorInField();
  fieldMgr->SetChordFinder(chordFinder);

  if ( pm["delta_chord"] ) {
    chordFinder->SetDeltaChord(pm.toDouble("delta_chord"));
  }
  if ( pm["delta_one_step"] ) {
    fieldMgr->SetAccuraciesWithDeltaOneStep(pm.toDouble("delta_one_step"));
  }
  if ( pm["delta_intersection"] ) {
    fieldMgr->SetDeltaIntersection(pm.toDouble("delta_intersection"));
  }
  if ( pm["eps_min"] ) {
    propagator->SetMinimumEpsilonStep(pm.toDouble("eps_min"));
  }
  if ( pm["eps_max"] ) {
    propagator->SetMaximumEpsilonStep(pm.toDouble("eps_max"));
  }
  return 1;
}
DECLARE_GEANT4_SETUP(Geant4FieldSetup,setup_fields);
