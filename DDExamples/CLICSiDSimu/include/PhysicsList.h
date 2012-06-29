#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

class PhysicsList: public G4VUserPhysicsList  {
public:
  PhysicsList();
  virtual ~PhysicsList();

  // Construct particle and physics
  void ConstructParticle();
  void ConstructProcess();
 
  void SetCuts();
   
private:

  // these methods Construct physics processes and register them
  void ConstructDecay();
  void ConstructEM();
};
#endif



