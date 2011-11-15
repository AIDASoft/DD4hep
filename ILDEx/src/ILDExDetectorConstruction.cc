
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ILDExDetectorConstruction.hh"
#include "ILDExDetectorMessenger.hh"

#include "G4GDMLParser.hh"
#include "G4PVPlacement.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "ILDExVXD.hh"
#include "ILDExSIT.hh"
#include "ILDExTPC.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExDetectorConstruction::ILDExDetectorConstruction()
: solidWorld(0),logicWorld(0),physiWorld(0),
magField(0)
{
  
  WorldSizeX = 10.0 * m; WorldSizeY = 10.0 * m; WorldSizeZ = 10.0 * m;
  
  // materials
  DefineMaterials();
  
  // create commands for interactive definition of the calorimeter
  detectorMessenger = new ILDExDetectorMessenger(this);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExDetectorConstruction::~ILDExDetectorConstruction()
{ delete detectorMessenger;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* ILDExDetectorConstruction::Construct()
{
  return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExDetectorConstruction::DefineMaterials()
{ 
  //This function illustrates the possible ways to define materials
  
  G4String symbol;             //  a=mass of a mole;
  G4double a, z, density;      //  z=mean number of protons;  
  G4int iz, n;                 // iz=number of protons  in an isotope; 
                               //  n=number of nucleons in an isotope;
  
  G4int ncomponents, natoms;
  G4double abundance, fractionmass;
  
  //
  // define Elements
  //
  
  G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
  G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
  G4Element* Si = new G4Element("Silicon",symbol="Si" , z= 14., a= 28.09*g/mole);
  
  //
  // define an Element from isotopes, by relative abundance 
  //
  
  G4Isotope* U5 = new G4Isotope("U235", iz=92, n=235, a=235.01*g/mole);
  G4Isotope* U8 = new G4Isotope("U238", iz=92, n=238, a=238.03*g/mole);
  
  G4Element* U  = new G4Element("enriched Uranium",symbol="U",ncomponents=2);
  U->AddIsotope(U5, abundance= 90.*perCent);
  U->AddIsotope(U8, abundance= 10.*perCent);
  
  //
  // define simple materials
  //
  
  new G4Material("Aluminium"  , z=13., a= 26.98*g/mole,  density= 2.700*g/cm3);
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole,  density= 1.390*g/cm3);
  new G4Material("Lead"       , z=82., a= 207.19*g/mole, density= 11.35*g/cm3);
  new G4Material("Carbon"     , z=6. , a= 12.01*g/mole,  density= 2.210*g/cm3);
  new G4Material("Silicon"    , z=14., a= 28.09*g/mole,  density= 2.329*g/cm3);
  new G4Material("Gold"       , z=79., a= 196.97*g/mole, density=19.320*g/cm3);
  new G4Material("Argon"      , z=18., a= 39.948*g/mole,
      density=0.001784*g/cm3);
  //
  // define a material from elements.   case 1: chemical molecule
  //
  
  G4Material* H2O = 
  new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
  H2O->AddElement(H, natoms=2);
  H2O->AddElement(O, natoms=1);
  // overwrite computed meanExcitationEnergy with ICRU recommended value 
  H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);
  
  G4Material* Sci = 
  new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
  Sci->AddElement(C, natoms=9);
  Sci->AddElement(H, natoms=10);
  
  G4Material* Myl = 
  new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
  Myl->AddElement(C, natoms=10);
  Myl->AddElement(H, natoms= 8);
  Myl->AddElement(O, natoms= 4);
  
  G4Material* SiO2 = 
  new G4Material("quartz",density= 2.200*g/cm3, ncomponents=2);
  SiO2->AddElement(Si, natoms=1);
  SiO2->AddElement(O , natoms=2);
  
  //
  // define a material from elements.   case 2: mixture by fractional mass
  //
  
  G4Material* Air = 
  new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);
  
  //
  // define a material from elements and/or others materials (mixture of mixtures)
  //
  
  G4Material* Aerog = 
  new G4Material("Aerogel", density= 0.200*g/cm3, ncomponents=3);
  Aerog->AddMaterial(SiO2, fractionmass=62.5*perCent);
  Aerog->AddMaterial(H2O , fractionmass=37.4*perCent);
  Aerog->AddElement (C   , fractionmass= 0.1*perCent);
  
  //
  // examples of gas in non STP conditions
  //
  
  G4Material* CO2 = 
  new G4Material("CarbonicGas", density= 1.842*mg/cm3, ncomponents=2,
                 kStateGas, 325.*kelvin, 50.*atmosphere);
  CO2->AddElement(C, natoms=1);
  CO2->AddElement(O, natoms=2);
  
  G4Material* steam = 
  new G4Material("WaterSteam", density= 0.3*mg/cm3, ncomponents=1,
                 kStateGas, 500.*kelvin, 2.*atmosphere);
  steam->AddMaterial(H2O, fractionmass=1.);
  
  //
  // examples of vacuum
  //
  
  G4Material* Vacuum =
  new G4Material("Vacuum", z=1., a=1.01*g/mole,density= universe_mean_density,
                 kStateGas, 2.73*kelvin, 3.e-18*pascal);
  
  G4Material* beam = 
  new G4Material("Beam", density= 1.e-5*g/cm3, ncomponents=1,
                 kStateGas, STP_Temperature, 2.e-2*bar);
  beam->AddMaterial(Air, fractionmass=1.);
  
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  
  //default materials of the World
  defaultMaterial  = Vacuum;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* ILDExDetectorConstruction::ConstructDetector()
{
  
  // Clean old geometry, if any
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  
  G4VisAttributes* VisAtt ;
  
  VisAtt = new G4VisAttributes(G4Colour(1.,1.,1.)); 
  VisAtt->SetVisibility(true);
    
  //     
  // World
  //
  solidWorld = new G4Box("World",				//its name
                         WorldSizeX/2,WorldSizeY/2,WorldSizeZ/2);	//its size
  
  logicWorld = new G4LogicalVolume(solidWorld,		//its solid
                                   defaultMaterial,	//its material
                                   "World");		//its name
  
  physiWorld = new G4PVPlacement(0,			//no rotation
                                 G4ThreeVector(),	//at (0,0,0)
                                 logicWorld,		//its logical volume				 
                                 "World",		//its name
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number

  
  logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
  
  ILDExVXD vxd(logicWorld);

  ILDExSIT sit(logicWorld);

  ILDExTPC tpc(logicWorld);

  std::ifstream gdml_file("World.gdml");
  if (gdml_file.good()) {
    G4cout << G4endl;
    G4cout << "#############################################" << G4endl;
    G4cout << "#     Please remove file World.gdml         #" << G4endl;
    G4cout << "#############################################" << G4endl;
    G4cout << G4endl;
    exit(1);
  }

  G4GDMLParser parser;
  parser.Write("World.gdml", physiWorld) ;                      
  
  //
  //always return the physical World
  //
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

void ILDExDetectorConstruction::SetMagField(G4double fieldValue)
{
  //apply a global uniform magnetic field along Z axis
  G4FieldManager* fieldMgr
  = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  
  if(magField) delete magField;		//delete the existing magn field
  
  if(fieldValue!=0.)			// create a new one if non nul
    { magField = new G4UniformMagField(G4ThreeVector(0.,0.,fieldValue));
      fieldMgr->SetDetectorField(magField);
      fieldMgr->CreateChordFinder(magField);
    } else {
      magField = 0;
      fieldMgr->SetDetectorField(magField);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"

void ILDExDetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


