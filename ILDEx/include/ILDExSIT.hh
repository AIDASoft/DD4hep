//
//  ILDExSIT.h
//  ILDExample
//
#ifndef ILDExSIT_h
#define ILDExSIT_h 1

#include "globals.hh"
#include <vector>

class G4LogicalVolume;
class G4Material;

class ILDExSIT {
  
  G4LogicalVolume* _motherlog;
  
  struct SIT_Layer {
    
    
    G4double nladders;
    
    G4double half_z;
    
    G4double sensitive_radius;
    G4double sensitive_thickness;
    
    G4double support_thickness;
    
    G4Material* sensitive_material;
    G4Material* support_material;
    
  } ;
  
  std::vector<SIT_Layer> _layers;
  
  void set_geom_values();
  
  void build();
  
public:
  
  ILDExSIT( G4LogicalVolume* motherlog );
  
  ~ILDExSIT() {/* no-op */;}
  
  
};

#endif
