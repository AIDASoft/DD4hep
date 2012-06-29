#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "DD4hep/LCDD.h"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class DetectorConstruction;
class PrimaryGeneratorMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction  {
public:
  PrimaryGeneratorAction(const DD4hep::Geometry::LCDD&);    
  virtual ~PrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  void SetRndmFlag(G4String val) { rndmFlag = val;}

private:
  G4ParticleGun*                particleGun;	  //pointer a to G4  class
  const DD4hep::Geometry::LCDD&       Detector;    //pointer to the geometry
  PrimaryGeneratorMessenger* gunMessenger;   //messenger of this class
  G4String                      rndmFlag;	  //flag for a rndm impact point
};
#endif


