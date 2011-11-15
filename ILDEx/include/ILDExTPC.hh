//
//  ILDExTPC.h
//  ILDExample
//

#ifndef ILDExTPC_h
#define ILDExTPC_h 1

#include "globals.hh"

class G4LogicalVolume;
class G4Material;

class ILDExTPC {
  
  G4LogicalVolume* _motherlog;
  
  G4double _rmin;
  G4double _rmax;
  G4double _halfz;
  
  G4double _inner_wall_thickness;
  G4double _outer_wall_thickness;
  
  G4double _endcap_thickness;
  
  G4Material* _sensitive_gas_material;
  G4Material* _wall_material;  
  G4Material* _endcap_material;  
  
  void set_geom_values();
  
  void build();
  
public:
  
  ILDExTPC( G4LogicalVolume* motherlog );
  
  ~ILDExTPC() {/* no-op */;}
  
};



#endif
