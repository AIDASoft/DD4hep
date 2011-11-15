//
//  ILDExTPC.cc
//  ILDExample
//

#include "ILDExTPC.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


ILDExTPC::ILDExTPC( G4LogicalVolume* motherlog ) : _motherlog(motherlog){
  
  this->set_geom_values();
  this->build();
  
}

void ILDExTPC::set_geom_values(){
  
  _halfz = 2500 * mm;
  _rmax  = 1800 * mm;
  _rmin  =  350 * mm;
  
  _inner_wall_thickness = 2.0 * mm;
  _outer_wall_thickness = 3.0 * mm;
  
  _endcap_thickness = 1.0 * mm;
  
  _sensitive_gas_material = G4Material::GetMaterial("Argon");
  _wall_material = G4Material::GetMaterial("Carbon");
  _endcap_material = G4Material::GetMaterial("Silicon");
  
}

void ILDExTPC::build() {

  G4PVPlacement *Phys;
  G4VisAttributes *VisAtt;
  
  G4Colour bounding_colour ( 1.0, 1.0, 1.0 );
  G4Colour sensitive_colour( 0.7, 0.2, 0.1 );
  G4Colour support_colour  ( 0.1, 0.2, 7.0 );

 
  std::cout << "TPC_Simple:" 
  << "\t half length = " << _halfz
  << "\t r max =  " << _rmax
  << "\t r min = " << _rmin
  << "\t sensitive mat = " << _sensitive_gas_material->GetName()
  << "\t support mat = " << _endcap_material->GetName()
  << std::endl;
  
  std::stringstream name_base;
  
  name_base << "TPC";
  
  // create a bounding volume 
  
  G4Tubs* tpcBoundingSolid = new G4Tubs(name_base.str()+"_BoundingSolid", _rmin, _rmax, _halfz, 0.0*rad, twopi*rad);
  
  G4LogicalVolume* tpcBoundingLogical = new G4LogicalVolume(tpcBoundingSolid, _sensitive_gas_material, name_base.str()+"_BoundingLogical");
  
  VisAtt = new G4VisAttributes(bounding_colour);

  
  tpcBoundingLogical->SetVisAttributes(VisAtt);
  
  Phys = new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,0.), 
                    tpcBoundingLogical,
                    name_base.str()+"_BoundPhys",
                    _motherlog, 
                    false, 
                    0, 
                    false);
  
  
  // inner wall 
  
  G4Tubs* tpcInnerWallSolid = new G4Tubs(name_base.str()+"_InnerWallSolid", _rmin, _rmin+_inner_wall_thickness, _halfz, 0.0*rad, twopi*rad);
  
  G4LogicalVolume* tpcInnerWallLogical = new G4LogicalVolume(tpcInnerWallSolid, _wall_material, name_base.str()+"_InnerWallLogical");
  
  VisAtt = new G4VisAttributes(support_colour);


  tpcInnerWallLogical->SetVisAttributes(VisAtt);
  
  Phys = new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,0.), 
                           tpcInnerWallLogical,
                           name_base.str()+"_InnerWallPhys",
                           tpcBoundingLogical, 
                           false, 
                           0, 
                           false);
  
  // outer wall 
  
  G4Tubs* tpcOuterWallSolid = new G4Tubs(name_base.str()+"_OuterWallSolid", _rmax-_outer_wall_thickness, _rmax, _halfz, 0.0*rad, twopi*rad);
  
  G4LogicalVolume* tpcOuterWallLogical = new G4LogicalVolume(tpcOuterWallSolid, _wall_material, name_base.str()+"_OuterWallLogical");
  
  VisAtt = new G4VisAttributes(support_colour);

  
  tpcOuterWallLogical->SetVisAttributes(VisAtt);  
  
  Phys = new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,0.), 
                           tpcOuterWallLogical,
                           name_base.str()+"_OuterWallPhys",
                           tpcBoundingLogical, 
                           false, 
                           0, 
                           false);

  // endcap 
  
  G4Tubs* tpcEndCapSolid = new G4Tubs(name_base.str()+"_EndCapSolid", _rmin+_inner_wall_thickness, _rmax-_outer_wall_thickness, 0.5*_endcap_thickness, 0.0*rad, twopi*rad);
  
  G4LogicalVolume* tpcEndCapLogical = new G4LogicalVolume(tpcEndCapSolid, _wall_material, name_base.str()+"_EndCapLogical");
  
  VisAtt = new G4VisAttributes(support_colour);

  
  tpcEndCapLogical->SetVisAttributes(VisAtt); 
  
  Phys = new G4PVPlacement(0,
                           G4ThreeVector(0.0,0.0,_halfz-0.5*_endcap_thickness), 
                           tpcEndCapLogical,
                           name_base.str()+"_EndCapPhysFwd",
                           tpcBoundingLogical, 
                           false, 
                           0, 
                           false);
  
  Phys = new G4PVPlacement(0,
                           G4ThreeVector(0.0,0.0,-(_halfz-0.5*_endcap_thickness)), 
                           tpcEndCapLogical,
                           name_base.str()+"_EndCapPhysBwd",
                           tpcBoundingLogical, 
                           false, 
                           0, 
                           false);
  
  
  // sensitive gas
  
  G4Tubs* tpcSensitiveGasSolid = new G4Tubs(name_base.str()+"_SensitiveGasSolid", _rmin+_inner_wall_thickness, _rmax-_outer_wall_thickness, _halfz-_endcap_thickness, 0.0*rad, twopi*rad);
  
  G4LogicalVolume* tpcSensitiveGasLogical = new G4LogicalVolume(tpcSensitiveGasSolid, _wall_material, name_base.str()+"_SensitiveGasLogical");
  
  VisAtt = new G4VisAttributes(sensitive_colour);

  
  tpcSensitiveGasLogical->SetVisAttributes(VisAtt); 
  
  Phys = new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,0.), 
                           tpcSensitiveGasLogical,
                           name_base.str()+"_SensitiveGasPhys",
                           tpcBoundingLogical, 
                           false, 
                           0, 
                           false);

  
}
