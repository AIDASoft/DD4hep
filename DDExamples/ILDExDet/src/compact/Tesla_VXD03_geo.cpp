// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "VXDData.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static const double PIby2 = (M_PI/2.0);

struct LayerParams {
  struct Ladders {
    double len;
    double width;
    double gap;
  };
  int     id;
  int     num_ladder;
  double  gap;
  double  radius;
  Ladders ladders;
  bool isEven() const {  return (this->id%2)==0; }
};

struct VXD03Data : public DetElement::Object  {
  int id;
  std::vector<LayerParams> layers;
};

namespace {
  struct ZylinderPos : public DD4hep::Geometry::Position {
    ZylinderPos(double r, double offset, double phi, double z=0.0)
      : DD4hep::Geometry::Position(r*sin(phi)+offset*cos(phi),-r*cos(phi)+offset*sin(phi),z) {}
  };
}

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  DetElement  vxd;
  xml_det_t   x_det   = e;
  string      name    = x_det.nameStr();
  xml_comp_t  x_supp  = e.child(Unicode("support"));
  xml_comp_t  x_shell = x_supp.child(Unicode("central"));
  xml_comp_t  x_endpl = x_supp.child(Unicode("endplate"));
  xml_comp_t  x_endp1 = x_supp.child(Unicode("endplate_L1"));
  xml_comp_t  x_cryostat               = e.child(Unicode("cryostat"),false);
  xml_comp_t  x_be_ladder_block        = e.child(Unicode("beryllium_ladder_block"));
  xml_comp_t  x_side_band_electronics  = e.child(Unicode("side_band_electronics"));
  xml_comp_t  x_end_ladder_electronics = e.child(Unicode("end_ladder_electronics"));
  xml_comp_t  x_silicon                = e.child(Unicode("active_silicon"));  
  xml_comp_t  x_strip_lines            = e.child(Unicode("strip_lines"));

  double   support_thickness = x_supp.thickness();  
  Material support_mat       = lcdd.material(x_supp.materialStr());

  double   shell_zhalf       = x_shell.zhalf();
  double   shell_rmax        = x_shell.rmax();
  double   shell_rmin        = x_shell.rmin();
  double   shell_thickess    = shell_rmax-shell_rmin;

  double   be_ladder_block_length      = x_be_ladder_block.length();
  double   be_ladder_block_thickness   = x_be_ladder_block.thickness();
  Material be_ladder_block_mat         = lcdd.material(x_be_ladder_block.materialStr());
  
  double   side_band_electronics_width = x_side_band_electronics  ? x_side_band_electronics.width()/2.0 : 0.0;
  double   end_electronics_zhalf       = x_end_ladder_electronics ? x_end_ladder_electronics.zhalf() : 0.0;
  double   active_silicon_thickness    = x_silicon.thickness();
  
  double   strip_lines_final_z         = x_strip_lines.z();
  double   strip_lines_thickness       = x_strip_lines.thickness();
  double   strip_lines_final_radius    = x_strip_lines.radius();
  Material strip_lines_mat             = lcdd.material(x_strip_lines.materialStr()); 

  Material ladder_support_mat = lcdd.material(x_supp.materialStr());
  Material silicon_233 = lcdd.material("silicon_2.33gccm");

  
  Value<TNamed,VXD03Data>* vxd_data = new Value<TNamed,VXD03Data>();
  vxd.assign(vxd_data,name,x_det.typeStr());
  vxd_data->id = x_det.id();

  Assembly    assembly(name+"_vol");
  Volume motherVol = lcdd.pickMotherVolume(vxd);    

  
  for(xml_coll_t c(e,_X(layer)); c; ++c)  {
    xml_comp_t            x_layer(c);
    xml_comp_t            x_ladders(c.child(Unicode("ladder")));
    LayerParams           layer;
    layer.id            = x_layer.id();
    layer.num_ladder    = x_layer.number();
    layer.gap           = x_layer.gap();
    layer.radius        = x_ladders.radius();
    layer.ladders.len   = x_ladders.length();
    layer.ladders.width = x_ladders.width();
    layer.ladders.gap   = x_ladders.gap();
    vxd_data->layers.push_back(layer);

    string          layer_name = name+_toString(layer.id,"_layer_%d");
    DetElement      layer_elt(vxd, layer_name, layer.id);
    double ldd_len   = layer.ladders.len;
    double ldd_width = layer.ladders.width;
    double ldd_gap   = layer.ladders.gap;

    Assembly layer_vol(name);
    Box      ladder_supp_box(ldd_width+side_band_electronics_width,
			     ldd_len+(2.0*end_electronics_zhalf)+ be_ladder_block_length*2.,
			     support_thickness/2.);
    Volume ladder_supp_vol(layer_name+"_ladder",ladder_supp_box,ladder_support_mat);
    
    double delta_phi = 2.*M_PI / layer.num_ladder;
    double ladder_clothest_approch = be_ladder_block_thickness*2 +0.1;
    // calculate optimal offset, such that there is 0.1mm space between to the edge and the surface of two adjacent ladders.
    double offset_phi = (1-cos(delta_phi))/sin(delta_phi)*layer.radius
      -((ldd_width+side_band_electronics_width)
	+(ladder_clothest_approch+cos(delta_phi)*(support_thickness+active_silicon_thickness))/sin(delta_phi)); 

    double ladder_offset = layer.isEven() ? -(support_thickness/2.)+layer.gap : support_thickness/2.;

    ladder_supp_vol.setVisAttributes(lcdd.visAttributes("VXDSupportVis"));
    for (int i=0;i<layer.num_ladder;i++) {
      double phi = i*delta_phi, len = 0.0, r = 0.0, z = 0.0;
      ZylinderPos pos(layer.radius+ladder_offset,offset_phi,phi,0.0);

      layer_vol.placeVolume(ladder_supp_vol,pos,Rotation(PIby2,phi,0.));
      switch(layer.id) {
      case 1:
	len = be_ladder_block_length;
	r   = layer.radius+be_ladder_block_thickness+support_thickness;
	z   = ldd_len + end_electronics_zhalf + be_ladder_block_length*2.;
	break;
      case 2:
	len = be_ladder_block_length;
	r   = layer.radius+be_ladder_block_thickness+layer.gap;
	z   = ldd_len + end_electronics_zhalf + be_ladder_block_length*2.;
	break;
      case 3:
      case 5:
	len = (be_ladder_block_length + (shell_zhalf - end_electronics_zhalf*3.0 - ldd_len))/2.;
	r   = layer.radius+be_ladder_block_thickness+support_thickness;
	z   = shell_zhalf -(len/2.);
	break;
      case 4:
      case 6:
	len = (be_ladder_block_length + (shell_zhalf - end_electronics_zhalf*3.0 - ldd_len))/2.;
	r   = layer.radius+be_ladder_block_thickness+layer.gap;
	z   = shell_zhalf -(len/2.);
	break;
      default:
	break;
      }

      // **********************   Berylium annulus block *****************************************
      Box     box(ldd_width,len,be_ladder_block_thickness);
      Volume  vol(layer_name+"_Be", box, be_ladder_block_mat); 
      vol.setVisAttributes(lcdd.visAttributes("VXDBerilliumVis"));
      for( int i=0;i<layer.num_ladder;i++ ) {
	double phi =  i*delta_phi;
	layer_vol.placeVolume(vol,ZylinderPos(r,offset_phi,phi,z), Rotation(PIby2,phi,0.));
	layer_vol.placeVolume(vol,ZylinderPos(r,offset_phi,phi,-z),Rotation(PIby2,phi,0.));
      }
    }

    // *********************************  Electronics   ******************************************
    // ******************************  (dead Si layer ends)   ************************************

    // *********************************  Electronics at the end of the ladder  ******************
    if ( x_end_ladder_electronics )   {
      double thickness = x_end_ladder_electronics.thickness()/2.;
      Box    box(ldd_width,end_electronics_zhalf,thickness);
      Volume vol(layer_name+"_electronics",box,silicon_233);
      vol.setVisAttributes(lcdd.visAttributes("VXDElectronicsVis"));
      for(int i=0; i<layer.num_ladder; ++i) {
	double phi = delta_phi*i;
	double r   = layer.radius + (layer.isEven() ? thickness+layer.gap : -thickness);
	double z   = ldd_len + end_electronics_zhalf + ldd_gap/2.;

	layer_vol.placeVolume(vol,ZylinderPos(r,offset_phi+side_band_electronics_width,phi, z), Rotation(PIby2,phi,0.));
	layer_vol.placeVolume(vol,ZylinderPos(r,offset_phi+side_band_electronics_width,phi,-z),Rotation(PIby2,phi,0.));
      }
    }

    // *********************************  Electronics along the ladder  *************************
    if ( x_side_band_electronics )  {
      double thickness = x_side_band_electronics.thickness()/2.;
      Box    box(side_band_electronics_width,ldd_len/2.,thickness);
      Volume vol(layer_name+"_ldd_electronics",box,silicon_233);
      vol.setVisAttributes(lcdd.visAttributes("VXDElectronicsVis"));
      //if ( x_side_band_electronics.isSensitive() )  ElectronicsBandLogical->SetSensitiveDetector(sens);
      for(int i=0; i<layer.num_ladder; ++i) {
	double phi = i*delta_phi;
	double r   = layer.radius + (layer.isEven() ? thickness+layer.gap : -thickness);
	double z   = ldd_len/2. + ldd_gap/2.;

	layer_vol.placeVolume(vol,ZylinderPos(r,offset_phi-ldd_width,phi,z), Rotation(PIby2,phi,0.));
	layer_vol.placeVolume(vol,ZylinderPos(r,offset_phi-ldd_width,phi,-z),Rotation(PIby2,phi,0.));
      }      
    }


    //*******************************  Strip lines (Kapton )  ********************************
    //************ here the strip lines are still simulate by conical geometry ***************
    //************ I work on the next version to have a real band of kapton instead **********

    double strip_line_start_z = shell_zhalf + shell_thickess;
    if ( layer.id == 1 || layer.id == 2 )  {
      strip_line_start_z = ldd_len + ldd_gap/2 + end_electronics_zhalf*2 + shell_thickess + be_ladder_block_length*2; // to avoid overlaps
    }

    double strip_line_zhalf = (strip_lines_final_z - strip_line_start_z) / 2.;
    assert (strip_line_zhalf>0);
    double rmin = layer.radius + (layer.isEven() ? layer.gap : 0.0);

    ConeSegment strips_cone(strip_line_zhalf,
			    rmin,                                              // inside radius at  -fDz
			    rmin + strip_lines_thickness,                      // outside radius at -fDz
			    strip_lines_final_radius,                          // inside radius at  +fDz
			    strip_lines_final_radius + strip_lines_thickness); // outside radius at +fDz
    Volume strips_vol(layer_name+"_strips",strips_cone,strip_lines_mat);
    double z = strip_line_start_z + strip_line_zhalf;
    strips_vol.setVisAttributes(lcdd.visAttributes("VXDStripsVis"));
    layer_vol.placeVolume(strips_vol,Position(0,0, z));
    layer_vol.placeVolume(strips_vol,Position(0,0,-z),Rotation(M_PI,0,0));

    // *******************************  Si Active layer  *************************************
    Box    active_layer_box(ldd_width, ldd_len/2., active_silicon_thickness/2.);
    Volume active_layer_vol(layer_name+"_active",active_layer_box, silicon_233);
    PlacedVolume pv;
    active_layer_vol.setVisAttributes(lcdd.visAttributes("VXDActiveStripsVis"));
    for(int i=0; i<layer.num_ladder; ++i) {
      double phi =  delta_phi*i;
      double r   =  layer.radius + (layer.isEven() ? (active_silicon_thickness/2.)+layer.gap : -active_silicon_thickness/2.);
      double z   =  ldd_len/2.+ ldd_gap;

      pv = layer_vol.placeVolume(active_layer_vol,ZylinderPos(r,offset_phi+side_band_electronics_width,phi, z),Rotation(PIby2,phi,0.));
      pv.addPhysVolID("layer",layer.id);
      pv = layer_vol.placeVolume(active_layer_vol,ZylinderPos(r,offset_phi+side_band_electronics_width,phi,-z),Rotation(PIby2,phi,0.));
      pv.addPhysVolID("layer",layer.id);
    }
    assembly.placeVolume(layer_vol);
  }

  //****************************************
  // Outer support shell
  //****************************************

  // ************central tube*****************
  Tube         support_tube(shell_rmin,shell_rmin+shell_thickess,shell_zhalf);
  Volume       support_vol (name+"_support",support_tube,support_mat);
  support_vol.setVisAttributes(lcdd, x_supp.visStr());
  assembly.placeVolume(support_vol);

  // ************support endplates************
  Tube         endplate_tube(x_endpl.rmin(),x_endpl.rmax(),x_endpl.zhalf());
  Volume       endplate_vol (name+"_endcap",endplate_tube,support_mat);
  endplate_vol.setVisAttributes(lcdd, x_endpl.visStr());
  assembly.placeVolume(endplate_vol,Position(0,0, (shell_zhalf + x_endpl.zhalf())));
  assembly.placeVolume(endplate_vol,Position(0,0,-(shell_zhalf + x_endpl.zhalf())));

  // ************support endplates for the layer 1************
  Tube         endplate_support_tube(x_endp1.rmin(),x_endp1.rmax(),x_endp1.zhalf());
  Volume       endplate_support_vol (name+"_endplate_support",endplate_support_tube,support_mat);
  const LayerParams::Ladders& l1 = vxd_data->layers[0].ladders;
  double z = l1.len + 2.0*end_electronics_zhalf + shell_thickess/2. + (be_ladder_block_length*2) ;
  endplate_support_vol.setVisAttributes(lcdd, x_endp1.visStr());
  assembly.placeVolume(endplate_support_vol,Position(0,0, z));
  assembly.placeVolume(endplate_support_vol,Position(0,0,-z));

#if 0
  //*** Cryostat ***************************************************************
  double useCryo = x_cryostat.isValid();
  if ( useCryo ) {
    double rAlu  = x_cryostat.attr<double>(Unicode("VXD_cryo_alu_skin_inner_radius"));
    double drAlu = x_cryostat.attr<double>(Unicode("VXD_cryo_alu_skin_tickness"));
    double rSty   = x_cryostat.attr<double>(Unicode("VXD_cryo_foam_inner_radius"));
    double drSty  = x_cryostat.attr<double>(Unicode("VXD_cryo_foam_tickness"));
    double dzSty  = x_cryostat.attr<double>(Unicode("VXD_cryo_foam_zhalf"));
    double rInner = x_endpl.rmin();


    double aluEndcapZ = dzSty + drSty + drAlu / 2;
    double styEndcapZ = dzSty + drSty / 2;

    double aluHalfZ = dzSty + drSty;

    Material aluMaterial = lcdd.material("Aluminium");
    G4VisAttributes *aluVisAttributes = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
      //SJA: aluVisAttributes->SetForceWireframe(true);

      G4Material *styMaterial = CGAGeometryManager::GetMaterial("styropor");
      G4VisAttributes *styVisAttributes = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
      //SJA: styVisAttributes->SetForceWireframe(true);

      G4Tubs *aluBarrelSolid = new G4Tubs("CryostatAluSkinBarrel", rAlu, rAlu + drAlu,aluHalfZ, sPhi, dPhi);
      G4LogicalVolume *aluBarrelLog = new G4LogicalVolume(aluBarrelSolid, aluMaterial, "CryostatAluSkinBarrel", 0, 0, 0);
      aluBarrelLog->SetVisAttributes(aluVisAttributes);
      new G4PVPlacement(0, G4ThreeVector(), aluBarrelLog, "CryostatAluSkinBarrel", worldLog, false, 0);

      G4Tubs *styBarrelSolid = new G4Tubs("CryostatFoamBarrel", rSty, rSty + drSty, dzSty, sPhi, dPhi);
      G4LogicalVolume *styBarrelLog = new G4LogicalVolume(styBarrelSolid, styMaterial, "CryostatFoamBarrel", 0, 0, 0);
      styBarrelLog->SetVisAttributes(styVisAttributes);
      new G4PVPlacement(0, G4ThreeVector(), styBarrelLog, "CryostatFoamBarrel", worldLog, false, 0);

      G4Tubs *aluEndcapSolid = new G4Tubs("CryostatAluSkinEndPlate", rInner, rAlu + drAlu, drAlu / 2, sPhi, dPhi);
      G4LogicalVolume *aluEndcapLog = new G4LogicalVolume(aluEndcapSolid, aluMaterial, "CryostatAluSkinEndPlate", 0, 0, 0);
      aluEndcapLog->SetVisAttributes(aluVisAttributes);
      new G4PVPlacement(0, G4ThreeVector(0, 0, +aluEndcapZ), aluEndcapLog, "CryostatAluSkinEndPlate", worldLog, false, +1);
      new G4PVPlacement(0, G4ThreeVector(0, 0, -aluEndcapZ), aluEndcapLog, "CryostatAluSkinEndPlate", worldLog, false, -1);

      G4Tubs *styEndcapSolid = new G4Tubs("CryostatFoamEndPlate", rInner, rSty + drSty, drSty / 2, sPhi, dPhi);
      G4LogicalVolume *styEndcapLog = new G4LogicalVolume(styEndcapSolid, styMaterial, "CryostatFoamEndPlate", 0, 0, 0);
      styEndcapLog->SetVisAttributes(styVisAttributes);
      new G4PVPlacement(0, G4ThreeVector(0, 0, +styEndcapZ), styEndcapLog, "CryostatFoamEndPlate", worldLog, false, +1);
      new G4PVPlacement(0, G4ThreeVector(0, 0, -styEndcapZ), styEndcapLog, "CryostatFoamEndPlate", worldLog, false, -1);
    }


  return true;
#endif

  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  PlacedVolume lpv = motherVol.placeVolume(assembly);

  return vxd;
}

DECLARE_DETELEMENT(Tesla_VXD03,create_element);
