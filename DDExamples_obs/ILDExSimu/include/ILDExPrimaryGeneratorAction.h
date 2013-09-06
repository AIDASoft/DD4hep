
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExPrimaryGeneratorAction_h
#define ILDExPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "DD4hep/LCDD.h"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class ILDExDetectorConstruction;
class ILDExPrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  ILDExPrimaryGeneratorAction(const DD4hep::Geometry::LCDD&);    
  virtual ~ILDExPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  void SetRndmFlag(G4String val) { rndmFlag = val;}

private:
  G4ParticleGun*                particleGun;	  //pointer a to G4  class
  const DD4hep::Geometry::LCDD&       ILDExDetector;    //pointer to the geometry
  ILDExPrimaryGeneratorMessenger* gunMessenger;   //messenger of this class
  G4String                      rndmFlag;	  //flag for a rndm impact point
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


