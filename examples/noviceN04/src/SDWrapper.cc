#include "DDG4/Factories.h"
#include "DDG4/G4SDFactory.h"
#include "DD4hep/LCDD.h"

#include "ExN04CalorimeterSD.hh"
#include "ExN04CalorimeterROGeometry.hh"
#include "ExN04MuonSD.hh"
#include "ExN04TrackerSD.hh"


namespace DD4hep { namespace Simulation {
    
    
    /** Factory method to create an instance of ExN04CalorimeterSD
     */
    static G4VSensitiveDetector* create_calo_sd(const std::string& name,  DD4hep::Geometry::LCDD& lcdd)  {	
      G4String calorimeterSDname = "/mydet/calorimeter";
      ExN04CalorimeterSD * calorimeterSD = new ExN04CalorimeterSD(calorimeterSDname);
      G4String ROgeometryName = "CalorimeterROGeom";
      G4VReadOutGeometry* calRO = new ExN04CalorimeterROGeometry(ROgeometryName);
      calRO->BuildROGeometry();
      calRO->SetName(ROgeometryName);
      calorimeterSD->SetROgeometry(calRO);
      
      std::cout << "############ create_calo_sd : " << name  << std::endl ;
      return calorimeterSD ;
    }
    
    /** Factory method to create an instance of ExN04TrackerSD
     */
    static G4VSensitiveDetector* create_tracker_sd(const std::string& name,  DD4hep::Geometry::LCDD& lcdd)  {	
      G4String trackerSDname = "/mydet/tracker";

      std::cout << "############ create_tracker_sd : " << name  << std::endl ;
      return new ExN04TrackerSD(trackerSDname);
    }
    

    /** Factory method to create an instance of ExN04MuonSD
     */
    static G4VSensitiveDetector* create_muon_sd(const std::string& name,  DD4hep::Geometry::LCDD& lcdd)  {	
      G4String muonSDname = "/mydet/muon";

      std::cout << "############ create_muon_sd : " << name  << std::endl ;
      return new ExN04MuonSD(muonSDname);
    }
    
    // /** Factory class to create an instance of ExN04CalorimeterSD
    //  */
    // class DD4hepExN04CalorimeterSD : public DD4hep::Simulation::G4SDFactory {
    // public:
    //   virtual G4VSensitiveDetector* createSD(const std::string& name,  Geometry::LCDD& lcdd){
	
    // 	G4String calorimeterSDname = "/mydet/calorimeter";
    // 	ExN04CalorimeterSD * calorimeterSD = new ExN04CalorimeterSD(calorimeterSDname);
    // 	G4String ROgeometryName = "CalorimeterROGeom";
    // 	G4VReadOutGeometry* calRO = new ExN04CalorimeterROGeometry(ROgeometryName);
    // 	calRO->BuildROGeometry();
    // 	calRO->SetName(ROgeometryName);
    // 	calorimeterSD->SetROgeometry(calRO);
    // 	return calorimeterSD ;
    //   }
    // } ;
    // /** Factory class to create an instance of ExN04TrackerSD
    //  */
    // class DD4hepExN04TrackerSD : public DD4hep::Simulation::G4SDFactory{
    // public:
    //   virtual G4VSensitiveDetector* createSD(const std::string& name,  Geometry::LCDD& lcdd){
    // 	G4String trackerSDname = "/mydet/tracker";
    // 	return new ExN04TrackerSD(trackerSDname);
    //   }
    // } ;

    // /** Factory class to create an instance of ExN04MuonSD
    //  */
    // class DD4hepExN04MuonSD : public DD4hep::Simulation::G4SDFactory{
    // public:
    //   virtual G4VSensitiveDetector* createSD(const std::string& name,  Geometry::LCDD& lcdd){
    // 	G4String muonSDname = "/mydet/muon";
    // 	return new ExN04MuonSD(muonSDname);
    //   }
    // } ;
    
    
  } 
}    // End namespace DD4hep::Simulation

// DECLARE_G4SDFACTORY(DD4hepExN04CalorimeterSD)
// DECLARE_G4SDFACTORY(DD4hepExN04MuonSD)
// DECLARE_G4SDFACTORY(DD4hepExN04TrackerSD)     

DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(DD4hepExN04CalorimeterSD,create_calo_sd) 
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(DD4hepExN04TrackerSD,create_tracker_sd)     
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(DD4hepExN04MuonSD,create_muon_sd)	      
