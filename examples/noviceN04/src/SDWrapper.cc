#include "DDG4/Factories.h"
#include "DDG4/G4SDFactory.h"
#include "DD4hep/LCDD.h"

#include "ExN04CalorimeterSD.hh"
#include "ExN04CalorimeterROGeometry.hh"
#include "ExN04MuonSD.hh"
#include "ExN04TrackerSD.hh"


namespace DD4hep { namespace Simulation {
    
    
    /** Factory class to create an instance of ExN04CalorimeterSD
     */
    class DD4hepExN04CalorimeterSD : public DD4hep::Simulation::G4SDFactory {
    public:
      virtual G4VSensitiveDetector* createSD(const std::string& name,  Geometry::LCDD& lcdd){
	
	G4String calorimeterSDname = "/mydet/calorimeter";
	ExN04CalorimeterSD * calorimeterSD = new ExN04CalorimeterSD(calorimeterSDname);
	G4String ROgeometryName = "CalorimeterROGeom";
	G4VReadOutGeometry* calRO = new ExN04CalorimeterROGeometry(ROgeometryName);
	calRO->BuildROGeometry();
	calRO->SetName(ROgeometryName);
	calorimeterSD->SetROgeometry(calRO);
	return calorimeterSD ;
      }
    } ;
    /** Factory class to create an instance of ExN04TrackerSD
     */
    class DD4hepExN04TrackerSD : public DD4hep::Simulation::G4SDFactory{
    public:
      virtual G4VSensitiveDetector* createSD(const std::string& name,  Geometry::LCDD& lcdd){
	G4String trackerSDname = "/mydet/tracker";
	return new ExN04TrackerSD(trackerSDname);
      }
    } ;

    /** Factory class to create an instance of ExN04MuonSD
     */
    class DD4hepExN04MuonSD : public DD4hep::Simulation::G4SDFactory{
    public:
      virtual G4VSensitiveDetector* createSD(const std::string& name,  Geometry::LCDD& lcdd){
	G4String muonSDname = "/mydet/muon";
	return new ExN04MuonSD(muonSDname);
      }
    } ;
    
    
  } 
}    // End namespace DD4hep::Simulation

DECLARE_G4SDFACTORY(DD4hepExN04CalorimeterSD)
DECLARE_G4SDFACTORY(DD4hepExN04MuonSD)
DECLARE_G4SDFACTORY(DD4hepExN04TrackerSD)


