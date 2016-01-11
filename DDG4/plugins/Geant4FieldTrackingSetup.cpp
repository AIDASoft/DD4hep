// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4HEP_DDG4_GEANT4FIELDTRACKINGSETUP_H
#define DD4HEP_DDG4_GEANT4FIELDTRACKINGSETUP_H 1

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDG4/Geant4ActionPhase.h"
#include "DDG4/Geant4DetectorConstruction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Generic Setup component to perform the magnetic field tracking in Geant4
    /** Geant4FieldTrackingSetup.
     *
     *  This base class is use jointly between the XML setup and the 
     *  phase action used by the python setup.
     *
     *  Note:
     *  Negative parameters are not passed to Geant4 objects, but ignored -- if possible.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4FieldTrackingSetup  {
    protected:
      /** Variables to be filled before calling execute */
      /// Name of the G4Mag_EqRhs class
      std::string eq_typ;
      /// Name of the G4MagIntegratorStepper class
      std::string stepper_typ;
      /// G4ChordFinder parameter: min_chord_step
      double      min_chord_step;
      /// G4ChordFinder parameter: delta
      double      delta_chord;
      /// G4FieldManager parameter: delta_one_step
      double      delta_one_step;
      /// G4FieldManager parameter: delta_intersection
      double      delta_intersection;
      /// G4PropagatorInField parameter: eps_min
      double      eps_min;
      /// G4PropagatorInField parameter: eps_min
      double      eps_max;
      /// G4PropagatorInField parameter: LargestAcceptableStep
      double      largest_step;

    public:
      /// Default constructor
      Geant4FieldTrackingSetup();
      /// Default destructor
      virtual ~Geant4FieldTrackingSetup();
      /// Perform the setup of the magnetic field tracking in Geant4
      virtual int execute(Geometry::LCDD& lcdd);
    };

    /// Phase action to perform the setup of the Geant4 tracking in magnetic fields
    /** Geant4FieldTrackingSetupAction.
     *
     *  The phase action configures the Geant4FieldTrackingSetup base class using properties
     *  and then configures the Geant4 tracking in magnetic fields.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4FieldTrackingSetupAction :
      public Geant4PhaseAction,
      public Geant4FieldTrackingSetup
    {
    protected:
    public:
      /// Standard constructor
      Geant4FieldTrackingSetupAction(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4FieldTrackingSetupAction() {}

      /// Phase action callback
      void operator()();

    };

    /// Detector construction action to perform the setup of the Geant4 tracking in magnetic fields
    /** Geant4FieldTrackingSetupAction.
     *
     *  The phase action configures the Geant4FieldTrackingSetup base class using properties
     *  and then configures the Geant4 tracking in magnetic fields.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4FieldTrackingConstruction : 
      public Geant4DetectorConstruction,
      public Geant4FieldTrackingSetup
    {
    protected:
    public:
      /// Standard constructor
      Geant4FieldTrackingConstruction(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4FieldTrackingConstruction() {}

      /// Phase action callback
      void operator()();

    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4FIELDTRACKINGSETUP_H


// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Handle.h"
#include "DD4hep/Fields.h"
#include "DDG4/Factories.h"
#include "DDG4/Geant4Field.h"
#include "DDG4/Geant4Converter.h"

#include "G4TransportationManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ChordFinder.hh"
#include "G4PropagatorInField.hh"
#include <limits>

using namespace std;
using namespace CLHEP;
using namespace DD4hep;
using namespace DD4hep::Simulation;
typedef DD4hep::Geometry::LCDD lcdd_t;

/// Local declaration in anonymous namespace
namespace {

  struct Geant4SetupPropertyMap {
    const map<string,string>& vals;
    Geant4SetupPropertyMap(const map<string,string>& v) : vals(v) {}
    string value(const string& key) const;
    double toDouble(const string& key) const;
    bool operator[](const string& key) const  { return vals.find(key) != vals.end(); }
  };
  
  string Geant4SetupPropertyMap::value(const string& key) const {
    lcdd_t::PropertyValues::const_iterator iV = vals.find(key);
    return iV == vals.end() ? "" : (*iV).second;
  }

  double Geant4SetupPropertyMap::toDouble(const string& key) const {
    return Geometry::_toDouble(this->value(key));
  }

}

/// Default constructor
Geant4FieldTrackingSetup::Geant4FieldTrackingSetup() : eq_typ(), stepper_typ() {
  eps_min            = -1.0;
  eps_max            = -1.0;
  min_chord_step     =  1.0e-2 *CLHEP::mm;
  delta_chord        = -1.0;
  delta_one_step     = -1.0;
  delta_intersection = -1.0;
  largest_step       = -1.0;
}

/// Default destructor
Geant4FieldTrackingSetup::~Geant4FieldTrackingSetup()   {
}

/// Perform the setup of the magnetic field tracking in Geant4
int Geant4FieldTrackingSetup::execute(Geometry::LCDD& lcdd)   {
  Geometry::OverlayedField fld  = lcdd.field();
  G4MagneticField*         mag_field    = new Simulation::Geant4Field(fld);
  G4Mag_EqRhs*             mag_equation = PluginService::Create<G4Mag_EqRhs*>(eq_typ,mag_field);
  G4MagIntegratorStepper*  fld_stepper  = PluginService::Create<G4MagIntegratorStepper*>(stepper_typ,mag_equation);
  G4ChordFinder*           chordFinder  = new G4ChordFinder(mag_field,min_chord_step,fld_stepper);
  G4TransportationManager* transportMgr = G4TransportationManager::GetTransportationManager();
  G4PropagatorInField*     propagator   = transportMgr->GetPropagatorInField();
  G4FieldManager*          fieldManager = transportMgr->GetFieldManager();

  fieldManager->SetFieldChangesEnergy(fld.changesEnergy());
  fieldManager->SetDetectorField(mag_field);
  fieldManager->SetChordFinder(chordFinder);

  if ( delta_chord >= 0e0 )
    chordFinder->SetDeltaChord(delta_chord);
  if ( delta_one_step >= 0e0 )
    fieldManager->SetAccuraciesWithDeltaOneStep(delta_one_step);
  if ( delta_intersection >= 0e0 )
    fieldManager->SetDeltaIntersection(delta_intersection);
  if ( eps_min >= 0e0 )
    propagator->SetMinimumEpsilonStep(eps_min);
  if ( eps_max >= 0e0 )
    propagator->SetMaximumEpsilonStep(eps_max);
  if ( largest_step >= 0e0 ) {
    propagator->SetLargestAcceptableStep(largest_step);
  } else {
    largest_step = propagator->GetLargestAcceptableStep();
  }
  return 1;
}

static long setup_fields(lcdd_t& lcdd, const DD4hep::Geometry::GeoHandler& /* cnv */, const map<string,string>& vals) {
  struct XMLFieldTrackingSetup : public Geant4FieldTrackingSetup {
    XMLFieldTrackingSetup(const map<string,string>& values) : Geant4FieldTrackingSetup() {
      Geant4SetupPropertyMap pm(values);
      lcdd_t::PropertyValues::const_iterator iV = values.find("min_chord_step");
      eq_typ      = pm.value("equation");
      stepper_typ = pm.value("stepper");
      min_chord_step = Geometry::_toDouble((iV==values.end()) ? string("1e-2 * mm") : (*iV).second);
      if ( pm["eps_min"] ) eps_min = pm.toDouble("eps_min");
      if ( pm["eps_max"] ) eps_max = pm.toDouble("eps_max");
      if ( pm["delta_chord"] ) delta_chord = pm.toDouble("delta_chord");
      if ( pm["delta_one_step"] ) delta_one_step = pm.toDouble("delta_one_step");
      if ( pm["delta_intersection"] ) delta_intersection = pm.toDouble("delta_intersection");
      if ( pm["largest_step"] ) largest_step = pm.toDouble("largest_step");
    }
    virtual ~XMLFieldTrackingSetup() {}
  } setup(vals);
  return setup.execute(lcdd);
}

/// Standard constructor
Geant4FieldTrackingSetupAction::Geant4FieldTrackingSetupAction(Geant4Context* ctxt, const std::string& nam)
  : Geant4PhaseAction(ctxt,nam), Geant4FieldTrackingSetup()
{
  declareProperty("equation",           eq_typ);
  declareProperty("stepper",            stepper_typ);
  declareProperty("min_chord_step",     min_chord_step = 1.0e-2);
  declareProperty("delta_chord",        delta_chord = -1.0);
  declareProperty("delta_one_step",     delta_one_step = -1.0);
  declareProperty("delta_intersection", delta_intersection = -1.0);
  declareProperty("eps_min",            eps_min = -1.0);
  declareProperty("eps_max",            eps_max = -1.0);
  declareProperty("largest_step",       largest_step = -1.0);
}

/// Post-track action callback
void Geant4FieldTrackingSetupAction::operator()()   {
  execute(context()->lcdd());
  printout( INFO, "FieldSetup", "Geant4 magnetic field tracking configured.");
  printout( INFO, "FieldSetup", "G4MagIntegratorStepper:%s G4Mag_EqRhs:%s",
	    stepper_typ.c_str(), eq_typ.c_str());
  printout( INFO, "FieldSetup", "Epsilon:[min:%f mm max:%f mm]", eps_min, eps_max);
  printout( INFO, "FieldSetup", "Delta:[chord:%f 1-step:%f intersect:%f] LargestStep %f mm",
	    delta_chord, delta_one_step, delta_intersection, largest_step);
}


/// Standard constructor
Geant4FieldTrackingConstruction::Geant4FieldTrackingConstruction(Geant4Context* ctxt, const std::string& nam)
  : Geant4DetectorConstruction(ctxt,nam), Geant4FieldTrackingSetup()
{
  declareProperty("equation",           eq_typ);
  declareProperty("stepper",            stepper_typ);
  declareProperty("min_chord_step",     min_chord_step = 1.0e-2);
  declareProperty("delta_chord",        delta_chord = -1.0);
  declareProperty("delta_one_step",     delta_one_step = -1.0);
  declareProperty("delta_intersection", delta_intersection = -1.0);
  declareProperty("eps_min",            eps_min = -1.0);
  declareProperty("eps_max",            eps_max = -1.0);
  declareProperty("largest_step",       largest_step = -1.0);
}

/// Post-track action callback
void Geant4FieldTrackingConstruction::operator()()   {
  execute(context()->lcdd());
  printout( INFO, "FieldSetup", "Geant4 magnetic field tracking configured.");
  printout( INFO, "FieldSetup", "G4MagIntegratorStepper:%s G4Mag_EqRhs:%s",
	    stepper_typ.c_str(), eq_typ.c_str());
  printout( INFO, "FieldSetup", "Epsilon:[min:%f mm max:%f mm]", eps_min, eps_max);
  printout( INFO, "FieldSetup", "Delta:[chord:%f 1-step:%f intersect:%f] LargestStep %f mm",
	    delta_chord, delta_one_step, delta_intersection, largest_step);
}

DECLARE_GEANT4_SETUP(Geant4FieldSetup,setup_fields)
DECLARE_GEANT4ACTION(Geant4FieldTrackingSetupAction)
DECLARE_GEANT4ACTION(Geant4FieldTrackingConstruction)
