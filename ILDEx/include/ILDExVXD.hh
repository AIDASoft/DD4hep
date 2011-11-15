//
//  ILDExVXD.h
//  ILDExample
//

#ifndef ILDExVXD_h
#define ILDExVXD_h 1

#include "globals.hh"
#include "vector"

class G4LogicalVolume;
class G4Material;

class ILDExVXD {
  
  G4LogicalVolume* _motherlog;

  struct VXD_Layer {
        
    G4double nladders;
    
    G4double half_z;
    
    G4double ladder_transverse_offset;
    
    G4double sensitive_radius;
    G4double sensitive_thickness;
    
    G4double support_thickness;
    
    G4Material* sensitive_material;
    G4Material* support_material;
    
    
  } ;
  
  std::vector<VXD_Layer> _layers;
  
  void set_geom_values();
  
  void build();

  
public:
  
  ILDExVXD( G4LogicalVolume* motherlog );
  
  ~ILDExVXD() {/* no-op */;}
  
};

#endif
