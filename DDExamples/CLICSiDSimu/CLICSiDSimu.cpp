#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "Randomize.hh"

#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "EventAction.h"
#include "SteppingAction.h"
#include "SteppingVerbose.h"

#include "G4UIExecutive.hh"

#include "G4VUserDetectorConstruction.hh"
namespace DD4hep {
  namespace Geometry {
    class LCDD;
  }
  class G4DetectorConstruction : public G4VUserDetectorConstruction
  {
  public:
    
    G4DetectorConstruction(const std::string&);
    ~G4DetectorConstruction();
    G4VPhysicalVolume* Construct();
    Geometry::LCDD& GetLCDD() { return m_lcdd;}
  private:
    Geometry::LCDD& m_lcdd;
    std::string m_compactfile;
  }; 
}
#include "DD4hep/LCDD.h"
#include "TGeoManager.h"
#include "G4Material.hh"
using namespace DD4hep;
using namespace DD4hep::Geometry;

G4DetectorConstruction::G4DetectorConstruction(const std::string& xmlfile) : m_lcdd(LCDD::getInstance()), m_compactfile(xmlfile) {
  // We need to construct the geometry at this level already
  m_lcdd.fromCompact(m_compactfile);
}

G4DetectorConstruction::~G4DetectorConstruction() {}

G4VPhysicalVolume* G4DetectorConstruction::Construct() {

#if 0  
  // Import geometry from Root to VGM
  RootGM::Factory rtFactory;
  rtFactory.SetDebug(0);
  rtFactory.Import(gGeoManager->GetTopNode());
  
  // Export VGM geometry to Geant4
  //
  Geant4GM::Factory g4Factory;
  g4Factory.SetDebug(0);
  rtFactory.Export(&g4Factory);
  G4VPhysicalVolume* world = g4Factory.World();
#endif
  G4VPhysicalVolume* world = 0;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  return world;
}

int main(int argc,char** argv)   {
  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  // User Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
  
  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // Get the detector constructed
  DD4hep::G4DetectorConstruction* detector = new DD4hep::G4DetectorConstruction(argv[1]);
  runManager->SetUserInitialization(detector);
  
  //
  G4VUserPhysicsList* physics = new PhysicsList;
  runManager->SetUserInitialization(physics);
  
  // Set user action classes
  G4VUserPrimaryGeneratorAction* gen_action = 
    new PrimaryGeneratorAction(detector->GetLCDD());
  runManager->SetUserAction(gen_action);
  //
  RunAction* run_action = new RunAction;  
  runManager->SetUserAction(run_action);
  //
  EventAction* event_action = new EventAction(run_action);
  runManager->SetUserAction(event_action);
  //
  G4UserSteppingAction* stepping_action =
    new SteppingAction(event_action);
  runManager->SetUserAction(stepping_action);
  
  // Initialize G4 kernel
  //
  runManager->Initialize();
  
  // Initialize visualization
  //
  //G4VisManager* visManager = new G4VisExecutive;
  //visManager->Initialize();
    
  // Get the pointer to the User Interface manager  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if (argc!=1) {   // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[2];
    UImanager->ApplyCommand(command+fileName);    
  }
  else {  // interactive mode : define UI session    
    //G4UIsession* ui = new G4UIQt(argc, argv);
    //ui->SessionStart();
    //delete ui;
  }
  
  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
  
  //delete visManager;
  delete runManager;  
  return 0;
}
