// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det  = e;
  xml_comp_t  x_layer= e.child(_X(layer));
  string      name   = x_det.nameStr();
  Material    air    = lcdd.air();
  DetElement  side("pos",x_det.id());

  // LumiCal dimensions
  int         n_layers        = x_layer.attr<int>(_A(nmodules));
  double      bx_angle        = x_layer.crossing_angle();
  double      cal_innerradius = x_layer.inner_r();
  double      cal_outerradius = x_layer.outer_r();
  double      ncell_theta     = x_layer.attr<double>(_A(thetaBins));
  double      ncell_phi       = x_layer.attr<double>(_A(phiBins));
  double      z_begin         = x_layer.inner_z();
  double      layer_gap       = x_layer.gap();

  // Materials + thicknesses
  double supp_thick  = 0.0;
  double abs_thick   = 0.0;
  double cell_thick  = 0.0;
  Material supp_mat, abs_mat, cell_mat;
  VisAttr  supp_vis, abs_vis, cell_vis;
  for(xml_coll_t c(x_layer,_X(slice)); c; ++c)  {
    xml_comp_t slice = c;
    string nam = slice.nameStr();
    if ( nam == "support"  )   {
      supp_thick = slice.thickness();
      supp_mat   = lcdd.material(slice.materialStr());
      supp_vis   = lcdd.visAttributes(slice.visStr());
    }
    else if ( nam == "absorber" ) {
      abs_thick  = slice.thickness();
      abs_mat    = lcdd.material(slice.materialStr());
      abs_vis    = lcdd.visAttributes(slice.visStr());
    }
    else if ( nam == "active"  ) {
      cell_thick = slice.thickness();
      cell_mat   = lcdd.material(slice.materialStr());
      cell_vis   = lcdd.visAttributes(slice.visStr());
    }
  }
  // Calculate basic dimension   
  double layer_hz      = abs_thick+supp_thick+cell_thick;
  double cal_hz        = double(n_layers) * (layer_hz+layer_gap/2.);
  double thetastrip_dr = (cal_outerradius-cal_innerradius)/ncell_theta;
  double phistrip_dphi = 2.0*M_PI/ncell_phi;
  
  // Air tube which will hold all layers whole LumiCal  
  Tube        env_tube(cal_innerradius, cal_outerradius, cal_hz);
  Volume      env_vol (name+"_envelope",env_tube, air);
  // Air box which will hold the layer components whole theta layer
  Tube        lay_tube(cal_innerradius, cal_outerradius, layer_hz);
  Volume      lay_vol (name+"_layers",lay_tube,air);

  // Volumes for the absorber and the scintillator to be filled into layer
  Tube        abs_tube(cal_innerradius, cal_outerradius,abs_thick);
  Volume      abs_vol (name+"_absorber", abs_tube, abs_mat);
  abs_vol.setVisAttributes(abs_vis);

  // ceramic support plate for the sillicon detector
  Tube        supp_tube(cal_innerradius, cal_outerradius, supp_thick);
  Volume      supp_vol (name+"_support", supp_tube, supp_mat);
  supp_vol.setVisAttributes(supp_vis);

  // create a cell dr-dphi
  Tube        cell_tube(cal_innerradius,(cal_innerradius+thetastrip_dr), cell_thick);//, phistrip_dphi);
  Volume      cell_vol (name+"_cell",cell_tube,cell_mat);
  cell_vol.setVisAttributes(cell_vis);
  cell_vol.setSensitiveDetector(sens);

  //  here PHI silicon sector
  Tube        sector_tube(cal_innerradius,cal_outerradius,cell_thick);//,phistrip_dphi);
  Volume      sector_vol(name+"_sector",sector_tube,cell_mat);
  sector_vol.setVisAttributes(cell_vis);

  // whole silicon (helper) plane
  Tube        sensor_tube(cal_innerradius,cal_outerradius,cell_thick); 
  Volume      sensor_vol (name+"_sensor",sensor_tube, cell_mat);
  sensor_vol.setVisAttributes(cell_vis);


  // Note: Problem with divisions still - hence no special attributes
  // Replicate strips within a theta and phi plane padded version
  Volume sens_theta_vol = Ref_t(sector_tube->Divide(sector_vol,(name+"_sector_theta").c_str(),1,ncell_theta,0,thetastrip_dr));
  //sens_theta_vol.setVisAttributes(cell_vis);
  
  Volume sens_phi_vol = Ref_t(sensor_tube->Divide(sensor_vol,(name+"_sensor_phi").c_str(),2,ncell_phi,0,phistrip_dphi*RAD_2_DEGREE));
  //sens_phi_vol.setVisAttributes(cell_vis);

  //Declare the silcon strip  to be sensitive 
  //CellLogical->SetSensitiveDetector(theLumiCalSD); 

  // Layer assembly:
  // 1) Absorber plate 
  Position pos(0,0, -(supp_thick+cell_thick));
  lay_vol.placeVolume(abs_vol,pos);
  // 2) Support
  pos.SetZ(pos.Z()+abs_thick + supp_thick); 
  lay_vol.placeVolume(supp_vol,pos);
  // 3) silicon detector
  pos.SetZ(pos.Z()+supp_thick + cell_thick); 
  lay_vol.placeVolume(sensor_vol,pos);

  // position of first layer
  double lay_z = -cal_hz + layer_hz;

  // Put the layers into the LumiCal sub-module
  PlacedVolume pv;
  for (int nLay = 1; nLay < n_layers+1; nLay++)  {
    DetElement layer_det(side,_toString(nLay,"layer%d"),nLay);
    pv = env_vol.placeVolume(lay_vol,Position(0,0,lay_z));
    layer_det.setPlacement(pv);
    lay_z += (layer_hz*2.0+layer_gap);
  }
  // Place two LumiCal sub-modules into the world
  double z_center = z_begin+cal_hz;

  //Debug: one side centered only: mother.placeVolume(env_vol,Position(0,0,0),Rotation(bx_angle,0,0));
  DetElement  lcal(name,x_det.id());
  Volume      mother = lcdd.pickMotherVolume(lcal);
  Assembly    assembly(name);

  pv = assembly.placeVolume(env_vol,Rotation(0,0,bx_angle),RotateY(Position(0,0,z_center),bx_angle));
  side.setPlacement(pv);
  lcal.add(side);

  DetElement other_side = side.clone("neg",-x_det.id());
  pv = assembly.placeVolume(env_vol,Rotation(0,0,M_PI-bx_angle),RotateY(Position(0,0,z_center),M_PI-bx_angle));
  other_side.setPlacement(pv);
  lcal.add(other_side);

  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  pv = mother.placeVolume(assembly);
  lcal.setPlacement(pv);
  return lcal;
}

DECLARE_DETELEMENT(Tesla_LumiCalX,create_element);
