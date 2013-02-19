#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4Converter.h"

#include "SteppingAction.h"
#include "EventAction.h"
#include "G4Step.hh"
#include "G4PVPlacement.hh"
#include "G4VSensitiveDetector.hh"


using namespace DD4hep;
using namespace DD4hep::Simulation;

SteppingAction::SteppingAction(EventAction* evt) : eventaction(evt)					 
{ 
}

SteppingAction::~SteppingAction()
{ }



void SteppingAction::UserSteppingAction(const G4Step* aStep)   {  
  Geant4StepHandler step(aStep);
  Geant4Converter& cnv = Geant4Converter::instance();
  Geant4Converter::G4GeometryInfo& data = cnv.data();
  SiMaterial     = G4Material::GetMaterial("Silicon");
  TPCGasMaterial = G4Material::GetMaterial("Argon");  

  // get volume of the current step
  G4VPhysicalVolume* volume = step.preVolume();
  G4Material* material = volume->GetLogicalVolume()->GetMaterial();
  
  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  G4double stepl = 0.;
  if (step.trackDef()->GetPDGCharge() != 0.) stepl = aStep->GetStepLength();

  if (material == SiMaterial || material == TPCGasMaterial) { 
    eventaction->SumSensitive(edep, stepl, 0.);
  }
  else {
    eventaction->SumSupport(edep, stepl, 0.);
  }
    
  //example of saving random number seed of this event, under condition
  //// if (condition) G4RunManager::GetRunManager()->rndmSaveThisEvent(); 

  Position pos1 = step.prePos();
  Position pos2 = step.postPos();
  Momentum mom  = step.postMom();
  
  if ( step.sd(step.pre) ) {
    double len = (pos2-pos1).R();
    ::printf("  Track:%08ld pos:%.0f Len:%.1f  SD:%s [%s] Deposit:%.0f keV\n",
	     long(step.track),pos2.R()/cm,len/cm,step.sdName(step.pre,"----"), step.preStepStatus(),
	     edep/keV);
  }
#if 0
  ::printf("  Track:%08ld Pos:(%8f %8f %8f) -> (%f %f %f)  Mom:%7.0f %7.0f %7.0f \n",
	   long(step.track), pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z(), mom.X(), mom.Y(), mom.Z());
  ::printf("                pre-Vol: %s  Status:%s  SD:%s\n",
	   step.volName(step.pre,"----"), step.preStepStatus(), step.sdName(step.pre,"----"));
  ::printf("                post-Vol:%s  Status:%s  SD:%s\n",
	   step.volName(step.post,"----"), step.postStepStatus(), step.sdName(step.post,"----"));
#endif
  const G4VPhysicalVolume* pv = step.volume(step.post);
  typedef Geant4Converter::PlacementMap Places;
  const Places& places = cnv.data().g4Placements;
  for(Places::const_iterator i=places.begin(); i!=places.end();++i) {
    const G4PVPlacement* pl = (*i).second;
    const G4VPhysicalVolume* qv = pl;
    if ( qv == pv ) {
      const TGeoNode* tpv = (*i).first;
      //printf("           Found TGeoNode:%s!\n",tpv->GetName());
    }    
  }
  
}
