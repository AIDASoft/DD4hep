//
//  ILDExSIT.cc
//  ILDExample
//

#include "ILDExSIT.hh"

#include "G4Material.hh"
#include "G4Box.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


ILDExSIT::ILDExSIT( G4LogicalVolume* motherlog ) : _motherlog(motherlog) {
  
  this->set_geom_values();
  this->build();
  
}

void ILDExSIT::set_geom_values(){
  
  SIT_Layer layer;
  
  layer.nladders = 10;
  layer.half_z = 370.0 * mm;
  
  layer.sensitive_thickness = 0.01 * mm;
  layer.sensitive_material = G4Material::GetMaterial("Silicon");
  
  layer.support_thickness = 0.1 * mm;
  layer.support_material = G4Material::GetMaterial("Carbon");
  
  layer.sensitive_radius = 155.0 * mm;
  
  _layers.push_back(layer);
  
  layer.nladders = 19;
  
  layer.sensitive_radius = 300.0 * mm;
  layer.half_z = 650.0 * mm;
  
  _layers.push_back(layer);
  
  
}


void ILDExSIT::build(){
  
  G4PVPlacement *Phys;
  G4VisAttributes *VisAtt;

  G4Colour bounding_colour ( 1.0, 1.0, 1.0 );
  G4Colour sensitive_colour( 0.0, 0.7, 0.3 );
  G4Colour support_colour  ( 0.0, 0.3, 0.7 );
  
  for (unsigned int i=0; i<_layers.size(); ++i) {
    

    const G4double ladder_dphi = ( twopi / _layers[i].nladders ) * rad;
    

    G4double support_radius = _layers[i].sensitive_radius + 0.5 * _layers[i].sensitive_thickness + 0.5 * _layers[i].support_thickness;
    
    G4double ladder_width = 2.0 * tan(ladder_dphi*0.5) * (_layers[i].sensitive_radius - 0.5 * _layers[i].sensitive_thickness);
    
        
    std::cout << "SIT_Simple_Planar: Layer:" << i
    << "\t half length = " << _layers[i].half_z
    << "\t min r sensitive = " << _layers[i].sensitive_radius
    << "\t min r support = " << support_radius
    << "\t n ladders = " << _layers[i].nladders
    << "\t ladder width = " << ladder_width
    << "\t ladder dphi = " << ladder_dphi / degree
    << "\t sensitive mat = " << _layers[i].sensitive_material->GetName()
    << "\t support mat = " << _layers[i].sensitive_material->GetName()
    << std::endl;
    
    
    std::stringstream name_base;
    
    name_base << "SIT";
    
    std::stringstream name_enum;
    name_enum << i;
    
    // create a bounding volume e.g. a single ladder  
    
    G4Box *sitLadderSolid
    = new G4Box(name_base.str()+"_LadderSolid_"+name_enum.str(),
                ( _layers[i].sensitive_thickness + _layers[i].support_thickness ) / 2.0 ,
                ladder_width / 2.0,
                _layers[i].half_z);
    
    
    G4LogicalVolume *sitLadderLogical 
    = new G4LogicalVolume(sitLadderSolid,
                          _layers[i].sensitive_material, 
                          name_base.str()+"_LadderLogical_"+name_enum.str() );
    
    
    VisAtt = new G4VisAttributes( bounding_colour ); 
    // VisAtt->SetForceWireframe(true);
    
    sitLadderLogical->SetVisAttributes(VisAtt);
    
    // create the sensitive layer and place it in the bounding volume 
    
    G4Box *sitSenSolid
    = new G4Box(name_base.str()+"_SenSolid_"+name_enum.str(),
                ( _layers[i].sensitive_thickness ) / 2.0 ,
                ladder_width / 2.0,
                _layers[i].half_z);
    
    G4LogicalVolume *sitSenLogical 
    = new G4LogicalVolume(sitSenSolid,
                          _layers[i].sensitive_material, 
                          name_base.str()+"_SenLogical_"+name_enum.str());
    
    
    VisAtt = new G4VisAttributes( sensitive_colour ); 
    // VisAtt->SetForceWireframe(true);
    
    sitSenLogical->SetVisAttributes(VisAtt);

    
    // create the support layer and place it in the bounding volume 
    
    G4Box *sitSupSolid
    = new G4Box(name_base.str()+"_SupSolid_"+name_enum.str(),
                ( _layers[i].support_thickness ) / 2.0 ,
                ladder_width / 2.0,
                _layers[i].half_z);
    
    G4LogicalVolume *sitSupLogical 
    = new G4LogicalVolume(sitSupSolid,
                          _layers[i].support_material, 
                          name_base.str()+"_SupLogical_"+name_enum.str());
    
    
    VisAtt = new G4VisAttributes( support_colour ); 
    // VisAtt->SetForceWireframe(true);
    
    sitSupLogical->SetVisAttributes(VisAtt);
    
    G4int cellID0 = i;
    
    Phys =
    new G4PVPlacement(0,
                      G4ThreeVector( (-( _layers[i].sensitive_thickness + _layers[i].support_thickness ) / 2.0 + (_layers[i].sensitive_thickness / 2.0) ), 
                                    0., 
                                    0.),
                      sitSenLogical,
                      name_base.str()+"_SenPhys_"+name_enum.str(),
                      sitLadderLogical,
                      false,
                      cellID0,
                      false);
    
    Phys =
    new G4PVPlacement(0,
                      G4ThreeVector( (-( _layers[i].sensitive_thickness + _layers[i].support_thickness ) / 2.0 + _layers[i].sensitive_thickness + (_layers[i].support_thickness / 2.0)   ), 
                                    0., 
                                    0.),
                      sitSupLogical,
                      name_base.str()+"_SupPhys_"+name_enum.str(),
                      sitLadderLogical,
                      false,
                      0,
                      false);
    
    
    for( int j = 0 ; j < _layers[i].nladders ; ++j ){
      
      std::stringstream ladder_enum;
      
      ladder_enum << j;
      
      G4RotationMatrix *rot = new G4RotationMatrix();
      rot->rotateZ( j * -ladder_dphi );
      
      cellID0 = i+1 * j ;  
      
      G4float dr = ( ( _layers[i].sensitive_thickness + _layers[i].support_thickness ) / 2.0 ) - ( _layers[i].sensitive_thickness / 2.0 ) ;
      
      Phys =
      new G4PVPlacement(rot,
                        G4ThreeVector( (_layers[i].sensitive_radius+dr) * cos(j * ladder_dphi), 
                                      (_layers[i].sensitive_radius+dr) * sin(j * ladder_dphi), 
                                      0.),
                        sitLadderLogical,
                        name_base.str()+"_LadderPhys_"+name_enum.str()+"_"+ladder_enum.str(),
                        _motherlog,
                        false,
                        cellID0,
                        false);
      
    }
  } 
}



