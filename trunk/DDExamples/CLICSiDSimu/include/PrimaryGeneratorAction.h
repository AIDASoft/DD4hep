#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

/// Geant and framework include files
#include "G4VUserPrimaryGeneratorAction.hh"
#include "DD4hep/LCDD.h"
#include "globals.hh"

/// Forward declarations
class G4Event;
class G4ParticleGun;
class DetectorConstruction;
class PrimaryGeneratorMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction  {
public:
  PrimaryGeneratorAction(const DD4hep::Geometry::LCDD&);    
  virtual ~PrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  void SetRndmFlag(G4String val) { m_rndmFlag = val;}

private:
  G4ParticleGun*                  m_gun;	  // Pointer a to G4  class
  const DD4hep::Geometry::LCDD&   m_detector;     // Pointer to the geometry
  PrimaryGeneratorMessenger*      m_gunMessenger; // Messenger of this class
  G4String                        m_rndmFlag;	  // Flag for a rndm impact point
};
#endif


