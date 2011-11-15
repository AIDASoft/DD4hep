
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExDetectorConstruction_h
#define ILDExDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UniformMagField;
class ILDExDetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  ILDExDetectorConstruction();
  ~ILDExDetectorConstruction();
  
public:
  
  
  void SetMagField(G4double);
  
  G4VPhysicalVolume* Construct();
  
  void UpdateGeometry();
  
public:
  
  void PrintDetectorParameters(); 
  
  G4double GetWorldSizeX()           {return WorldSizeX;} 
  G4double GetWorldSizeY()           {return WorldSizeY;}
  G4double GetWorldSizeZ()           {return WorldSizeZ;}
    
  const G4VPhysicalVolume* GetphysiWorld() {return physiWorld;}           
  
private:
    
  G4Material*        defaultMaterial;

  G4double           WorldSizeX;
  G4double           WorldSizeY;
  G4double           WorldSizeZ;
  
  G4Box*             solidWorld;    //pointer to the solid World 
  G4LogicalVolume*   logicWorld;    //pointer to the logical World
  G4VPhysicalVolume* physiWorld;    //pointer to the physical World
  
  
  G4UniformMagField* magField;      //pointer to the magnetic field
  
  ILDExDetectorMessenger* detectorMessenger;  //pointer to the Messenger
  
private:
  
  void DefineMaterials();
  void ComputeDetectorParameters();
  G4VPhysicalVolume* ConstructDetector();     
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

