// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoBBox.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace {
  struct ECAL : public DetElement {
    LCDD&     lcdd;
    SensitiveDetector& sens_det;

    xml_det_t  x_det;
    xml_comp_t m_barrel;
    xml_comp_t m_endcap;
    xml_comp_t m_support;
    xml_comp_t m_front;

    xml_comp_t m_shield;
    struct XMLSlab : public xml_comp_t {
      xml_comp_t shield;
      xml_comp_t copper;
      xml_comp_t sensitive;
      xml_comp_t ground;
      xml_comp_t glue;
      XMLSlab() : xml_comp_t(0), shield(0), copper(0), sensitive(0), ground(0), glue(0) {}
      XMLSlab& operator=(const xml_h& e) { this->Element::operator=(e); return *this; }
    } m_xml_slab;

    struct Slab  {
      double     h_fiber_thickness;
      double     shield_thickness;
      VisAttr    vis;
      double     ground_thickness;
      Material   ground_mat;
      VisAttr    ground_vis;
      double     sensitive_thickness;
      Material   sensitive_mat;
      VisAttr    sensitive_vis;
      double     copper_thickness;
      Material   copper_mat;
      VisAttr    wafer_vis;
      double     glue_thickness;
      double     total_thickness;
      Slab() : h_fiber_thickness(0), total_thickness(0) {}
    } m_slab;

    struct Layer {
      int      nLayer;
      double   thickness;
      Material rad_mat;
    } m_layers[5];

    Volume   m_center_tube;
    Position m_alveolus;

    double   m_module_thickness;
    double   m_lateral_face_thickness;
    double   m_fiber_thickness;
    double   m_guard_ring_size;
    double   m_cell_size;

    double   m_endcap_center_box_size;

    vector<Volume>    EC_TowerSlabs;
    vector<Volume>    EC_Towers[3];
    vector<Position>  EC_TowerXYCenters;

    typedef Position Dimension;

    /// Standard constructor
    ECAL(LCDD& l, const xml_det_t& e, SensitiveDetector& s);

    /// Build Endcap Standard Module 
    Volume buildModule(bool Zminus, double rmax, const Solid& siSolid, const Volume& siVol, const Rotation& rot);

    /// Build slab 
    Volume buildSlab(const Dimension dim);

    // Build radiator solid
    Volume buildRadiator(const Dimension& dim, const Material& mat);
  };
}

#define N_FIBERS_ALVOULUS 3
#define N_FIBERS_W_STRUCTURE 2

ECAL::ECAL(LCDD& l, const xml_det_t& e, SensitiveDetector& s)
  : DetElement(e.nameStr(), e.id()), lcdd(l), x_det(e), m_barrel(0), m_endcap(0), m_support(0), m_front(0), m_shield(0), sens_det(s)
{

  // EC_Initialize() builds the Slabs and the 
  // radiator plates for the several towers 
  // to be placed latter into each module
  Volume motherVol = lcdd.pickMotherVolume(*this);
  string det_nam = name();
  /*
    <detector id="<id>" name="<name>" type="<type>"
      inner_r="TPC_outer_radius+Ecal_Tpc_gap"
      lateral_face_thickness="Ecal_lateral_face_thickness"
      fiber_thickness="Ecal_fiber_thickness"
      guard_ring_size="Ecal_guard_ring_size"
      cell_size="Ecal_cell_size"
      alveolus_gap="Ecal_Alveolus_Air_Gap"
    >
    <support   thickness="Ecal_support_thickness"/>
    <front     thickness="Ecal_front_face_thickness"/>
    <pcbshield thickness="Ecal_Slab_PCB_thickness + Ecal_Slab_copper_thickness + Ecal_Slab_shielding"/>

    <layer id="1" repeat="Ecal_nlayers1" thickness="Ecal_radiator_thickness1" material="Ecal_radiator_material"/>
    <layer id="2" repeat="Ecal_nlayers2" thickness="Ecal_radiator_thickness2" material="Ecal_radiator_material"/>
    <layer id="3" repeat="Ecal_nlayers3" thickness="Ecal_radiator_thickness3" material="Ecal_radiator_material"/>
    <slab h_fiber_thickness="Ecal_H_fiber_thickness" sensitive="Ecal_sensitive_material">
      <shielding thickness="Ecal_Slab_shielding"/>
      <copper    thickness="Ecal_Slab_copper_thickness"/>
      <ground>   thickness="Ecal_Slab_ground_thickness"/>
      <glue      thickness="Ecal_Slab_glue_gap"/>
      <sensitive thickness="Ecal_Si_thickness"/>
    </slab>
    <barrel zhalf="Ecal_Barrel_halfZ" towers="Ecal_barrel_number_of_towers"/>
    <endcap extra_size="Ecal_endcap_extra_size" center_box_size="Ecal_endcap_center_box_size"/>
    </detector>
  */
  m_support      = x_det.child(Unicode("support"));
  m_barrel       = x_det.child(_X(barrel));
  m_endcap       = x_det.child(_X(endcap));
  m_front        = x_det.child(Unicode("front"));
  m_shield       = x_det.child(Unicode("pcbshield"));

  m_xml_slab           = x_det.child(Unicode("slab"));
  m_xml_slab.shield    = m_xml_slab.child(Unicode("shielding"));
  m_xml_slab.copper    = m_xml_slab.child(Unicode("copper"));
  m_xml_slab.sensitive = m_xml_slab.child(Unicode("sensitive"));
  m_xml_slab.ground    = m_xml_slab.child(Unicode("ground"));
  m_xml_slab.glue      = m_xml_slab.child(Unicode("glue"));

  double ec_inner_r              = m_endcap.inner_r();
  double barrel_module_dim_z     = 2 * m_barrel.zhalf() / 5.;
  double endcap_extra_size       = m_endcap.attr<double>(Unicode("extra_size"));
  double barrel_number_of_towers = m_barrel.attr<int>(Unicode("towers"));

  m_lateral_face_thickness       = x_det.attr<double>(Unicode("lateral_face_thickness"));
  m_fiber_thickness              = x_det.attr<double>(Unicode("fiber_thickness"));
  m_cell_size                    = x_det.attr<double>(Unicode("cell_size"));
  m_guard_ring_size              = x_det.attr<double>(Unicode("guard_ring_size"));

  m_slab.vis                     = lcdd.visAttributes("EcalSlabVis");
  m_slab.h_fiber_thickness       = m_xml_slab.attr<double>(Unicode("h_fiber_thickness"));
  m_slab.shield_thickness        = m_xml_slab.shield.thickness();
  m_slab.ground_thickness        = m_xml_slab.ground.thickness();
  m_slab.ground_mat              = lcdd.material(m_xml_slab.ground.materialStr());
  m_slab.ground_vis              = lcdd.visAttributes("EcalSlabGroundVis");
  m_slab.sensitive_thickness     = m_xml_slab.sensitive.thickness();
  m_slab.sensitive_mat           = lcdd.material(m_xml_slab.sensitive.materialStr());
  m_slab.sensitive_vis           = lcdd.visAttributes("EcalSlabSensitiveVis");
  m_slab.copper_thickness        = m_xml_slab.copper.thickness();
  m_slab.copper_mat              = lcdd.material(m_xml_slab.copper.materialStr());
  m_slab.glue_thickness          = m_xml_slab.glue.thickness();
  m_slab.wafer_vis               = lcdd.visAttributes("EcalWaferVis");

  size_t i=0;
  for(xml_coll_t c(x_det,_X(layer)); c; ++c, ++i) {
    xml_comp_t layer(c);
    m_layers[i].nLayer    = layer.repeat();
    m_layers[i].thickness = layer.thickness();
    m_layers[i].rad_mat   = lcdd.material(layer.materialStr());
  }
  m_slab.total_thickness =    m_slab.shield_thickness  + m_slab.copper_thickness + 
    m_shield.thickness()    + m_slab.sensitive_thickness + m_slab.glue_thickness +
    m_slab.ground_thickness + x_det.attr<double>(Unicode("alveolus_gap")) / 2;

  int n_total_layers = m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer;
  m_module_thickness = 
    m_layers[0].nLayer * m_layers[0].thickness +
    m_layers[1].nLayer * m_layers[1].thickness +
    m_layers[2].nLayer * m_layers[2].thickness +
    int(n_total_layers/2) * (N_FIBERS_W_STRUCTURE * 2 *  m_fiber_thickness) + // fiber around W struct layers
    (n_total_layers + 1) * (m_slab.total_thickness + (N_FIBERS_ALVOULUS + 1 ) * m_fiber_thickness) + // slabs plus fiber around and inside
    m_support.thickness() + m_front.thickness();
  
  m_alveolus.z =
    (barrel_module_dim_z - 2. * m_lateral_face_thickness) /
    barrel_number_of_towers - 
    2 * N_FIBERS_ALVOULUS  * m_fiber_thickness  - 
    2 * m_slab.h_fiber_thickness -
    2 * m_slab.shield_thickness;

  double siPlateSize = m_endcap.attr<double>(Unicode("center_box_size")) -
    2.0 * m_lateral_face_thickness -
    2.0 * m_endcap.attr<double>(Unicode("ring_gap"));
  m_center_tube = Tube(ec_inner_r,ec_inner_r,0.);
  Box  ec_ringSiBox(siPlateSize/2,siPlateSize/2,m_slab.sensitive_thickness/2);
  //SubtractionSolid ec_ringSiSolid(m_center_tube,ec_ringSiBox,Position(),Rotation());
  SubtractionSolid ec_ringSiSolid(ec_ringSiBox,m_center_tube,Position(),Rotation());
  Volume ec_ringSiVol(det_nam+"_ec_ring_vol",ec_ringSiSolid,m_slab.sensitive_mat);

  //=================================================
  //
  // Take care:
  //
  // To turn the direction of slabs in the end caps
  // we interchanged X<->Y. It's not clean, but it
  // works...
  //=================================================

  // Main parameters for the virtual EC module
  double endcap_rmax = x_det.inner_r() + m_module_thickness + endcap_extra_size;
  double y_top   =  endcap_rmax - m_lateral_face_thickness;
  double y_mid   =  endcap_rmax * std::tan(M_PI/8) - m_lateral_face_thickness;
  double y_bot   =  m_endcap_center_box_size / 2 + m_lateral_face_thickness;  
  double x_left  = -endcap_rmax + m_lateral_face_thickness;
  double x_right =  m_endcap_center_box_size / 2 - m_lateral_face_thickness;
  double x_mid   = -y_mid;

  double fiber_inter_alveolus =
    2 * (m_slab.h_fiber_thickness + m_slab.shield_thickness + N_FIBERS_ALVOULUS * m_fiber_thickness);
  
  // The alveolus size takes in account the module Z size
  // but also 4 fiber layers for the alveoulus wall, the all
  // divided by the number of towers
  m_alveolus.y = m_alveolus.z;

  double inc_x = (x_mid-x_left ) / (y_top-y_mid);
  double inc_y = m_alveolus.y + fiber_inter_alveolus;
  for(double last_dim_x=0., y_curr=y_bot+m_alveolus.y; y_curr < y_top; y_curr += inc_y )    {
    m_alveolus.x = x_right - x_left - ((y_curr <= y_mid) ? 0 : (y_curr - y_mid) * inc_x);
    // While the towers have the same shape use the same logical volumes and parameters.
    if(last_dim_x != m_alveolus.x)      {
      EC_TowerSlabs.push_back(buildSlab(Dimension(m_alveolus.y,m_slab.total_thickness,m_alveolus.x)));
      if( m_layers[0].nLayer > 0 )
	EC_Towers[0].push_back(buildRadiator(Dimension(m_alveolus.y,m_layers[0].thickness,m_alveolus.x),m_layers[0].rad_mat));
      if( m_layers[1].nLayer > 0 )
	EC_Towers[1].push_back(buildRadiator(Dimension(m_alveolus.y,m_layers[1].thickness,m_alveolus.x),m_layers[1].rad_mat));
      if( m_layers[2].nLayer > 0 )
	EC_Towers[2].push_back(buildRadiator(Dimension(m_alveolus.y,m_layers[2].thickness,m_alveolus.x),m_layers[2].rad_mat));
      last_dim_x = m_alveolus.x;
    }
    else   {
      EC_TowerSlabs.push_back(EC_TowerSlabs.back());
      if( m_layers[0].nLayer > 0 ) EC_Towers[0].push_back(EC_Towers[0].back());
      if( m_layers[1].nLayer > 0 ) EC_Towers[1].push_back(EC_Towers[1].back());
      if( m_layers[2].nLayer > 0 ) EC_Towers[2].push_back(EC_Towers[2].back());
    }
    EC_TowerXYCenters.push_back(Position(-(y_curr - m_alveolus.y/2),-(-m_alveolus.x/2 + x_right),0));
  }
  
  Volume ec1 = buildModule(false,endcap_rmax, ec_ringSiSolid, ec_ringSiVol,Rotation());
  motherVol.placeVolume(ec1,Position());
  //Volume ECAL::buildModule(bool Zminus, double endcap_rmax, const Solid& siBox, const Volume& siVol, const Rotation& rot);
}

// Build radiator solid
Volume ECAL::buildRadiator(const Dimension& dim, const Material& mat)   {
  Box    box(dim.x/2,dim.z/2,dim.y/2);
  ::printf("Radiator: %f %f %f\n",dim.x/2,dim.z/2,dim.y/2);
  Volume vol("_radiator",box,mat);
  vol.setVisAttributes(lcdd.visAttributes("EcalTowerVis"));
  return vol;
}

/// Build slab 
Volume ECAL::buildSlab(Dimension dim) {
  string nam = name();
  double ground_thickness = m_slab.ground_thickness;

  // Slab solid: hx, hz, hy
  Box slabBox(dim.x/2,dim.z/2,dim.y/2);
  Volume slabVol(nam+"_slab",slabBox,lcdd.air());
  slabVol.setVisAttributes(m_slab.vis);
  double y_slab_floor  = -dim.y/2;

  // Ground plate
  Box    groundBox(dim.x/2, dim.z/2, ground_thickness/2);
  Volume groundVol(nam+"_ground",groundBox,m_slab.ground_mat);
  groundVol.setVisAttributes(m_slab.ground_vis);
  slabVol.placeVolume(groundVol,Position(0,0,y_slab_floor+ground_thickness/2));
  
  y_slab_floor += ground_thickness;

  // Si layer
  // we place a big plane of Si and inside it the Si wafers, to simplify the gard ring placements
  Box     siBox(dim.x/2,dim.z/2,m_slab.sensitive_thickness/2);
  Volume  siVol(nam+"_sensitive",siBox,m_slab.sensitive_mat);
  siVol.setVisAttributes(m_slab.sensitive_vis);

  double y_slab_si_offset = -dim.y/2 + ground_thickness + m_slab.sensitive_thickness/2;
  slabVol.placeVolume(siVol,Position(0,0,y_slab_si_offset));

  // Then we place the Si wafers inside the big Si plane
  //
  // User limits to limit the step to stay inside the cell
  //G4UserLimits* pULimits = new G4UserLimits(theMaxStepAllowed);

  // Normal squared wafers
  double cell_dim_x = m_cell_size;
  double total_Si_dim_z = m_alveolus.z;
  double util_SI_wafer_dim_z = total_Si_dim_z/2 -  2 * m_guard_ring_size;
  double cell_dim_z =  util_SI_wafer_dim_z / std::floor(util_SI_wafer_dim_z/cell_dim_x);
  int N_cells_in_Z = int(util_SI_wafer_dim_z/cell_dim_z);
  int N_cells_in_X = N_cells_in_Z;
  
  cell_dim_x = cell_dim_z;

  double wafer_dim_x = N_cells_in_X * cell_dim_x;
  double wafer_dim_z = N_cells_in_Z * cell_dim_z;

  Box     waferBox(wafer_dim_x/2, wafer_dim_z/2, m_slab.sensitive_thickness/2);
  Volume  waferVol(nam+"_wafer",waferBox,m_slab.sensitive_mat);
  waferVol.setVisAttributes(m_slab.wafer_vis);
  // waferVol.setLimits(pULimits);
  // waferVol->SetSensitiveDetector(theSD);

  // As the same method builds both barrel and end cap
  // slabs, place the wafers along the biggest axe
  if (dim.z < dim.x)     {
    ::printf("...Building slab for barrel....\n");
    // Barrel
    double real_wafer_size_x = wafer_dim_x + 2 * m_guard_ring_size;      
    int    num_wafer         = int(floor(dim.x / real_wafer_size_x));
    double wafer_pos_x       = -dim.x/2 + m_guard_ring_size +	wafer_dim_x /2 ;
    for (int iwaf = 1; iwaf < num_wafer + 1; iwaf++)      {
      double wafer_pos_z = -dim.z/2 + m_guard_ring_size + wafer_dim_z /2;
      for (int n_wafer_z = 1;	       n_wafer_z < 3;	       n_wafer_z++)	    {
	PlacedVolume pv = siVol.placeVolume(waferVol,Position(wafer_pos_x,wafer_pos_z,0));
	pv.addPhysVolID("wafer",iwaf*1000 + n_wafer_z);
	wafer_pos_z += wafer_dim_z + 2 * m_guard_ring_size;
      }
      wafer_pos_x += wafer_dim_x + 2 * m_guard_ring_size;
    }
      
    // Magic wafers to complete the slab...
    // (wafers with variable number of cells just
    // to complete the slab. in reality we think that
    // we'll have just a few models of special wafers
    // for that.
    double resting_dim_x = dim.x - (wafer_dim_x + 2 * m_guard_ring_size) * num_wafer;
      
    if(resting_dim_x > (cell_dim_x + 2 * m_guard_ring_size))	{
      int N_cells_x_remaining =	int(floor((resting_dim_x - 2 * m_guard_ring_size)/cell_dim_x));
      wafer_dim_x = N_cells_x_remaining * cell_dim_x;
      Box    waf_box(wafer_dim_x/2, wafer_dim_z/2, m_slab.sensitive_thickness/2);
      Volume waf_vol(nam+"_wafer",waf_box,m_slab.sensitive_mat);
      //waf_vol.setLimits(pULimits);
      waf_vol.setVisAttributes(m_slab.wafer_vis);
      //waf_vol->SetSensitiveDetector(theSD);
	  
      real_wafer_size_x  =  wafer_dim_x + 2 * m_guard_ring_size;
      wafer_pos_x        = -dim.x/2 + num_wafer * real_wafer_size_x + real_wafer_size_x/2;
      double wafer_pos_z = -dim.z/2 + m_guard_ring_size + wafer_dim_z /2;
      for (int n_wafer_z = 1; n_wafer_z < 3; n_wafer_z++)  {
	PlacedVolume pv = siVol.placeVolume(waf_vol,Position(wafer_pos_x,wafer_pos_z,0));
	wafer_pos_z += wafer_dim_z + 2 * m_guard_ring_size;
      }
    }
  }
  else    {
    ::printf("...Building slab for end-cap....\n");
    // End caps
    double real_wafer_size_x = wafer_dim_z + 2 * m_guard_ring_size;
    double wafer_pos_x       = -dim.z/2 + m_guard_ring_size +	wafer_dim_z /2;
    int num_wafer            = int(floor(dim.z / real_wafer_size_x));      
    for (int iwaf=1; iwaf < num_wafer + 1; ++iwaf)	{
      double wafer_pos_z =  -dim.x/2 + m_guard_ring_size + wafer_dim_x /2;
      for (int n_wafer_z = 1; n_wafer_z < 3; ++n_wafer_z)    {
	PlacedVolume pv = siVol.placeVolume(siVol,Position(wafer_pos_z,wafer_pos_x,0));
	pv.addPhysVolID("wafer",iwaf*1000 + n_wafer_z);
	wafer_pos_x += wafer_dim_z + 2 * m_guard_ring_size;
      }
      
      // Magic wafers to complete the slab...
      // (wafers with variable number of cells just to complete the slab. in reality we think that
      // we'll have just a few models of special wafers for that.
      double resting_dim_x = dim.z - (wafer_dim_z + 2 * m_guard_ring_size) * num_wafer;
      
      if(resting_dim_x > (cell_dim_z + 2 * m_guard_ring_size))   {
	int N_cells_x_remaining = int(floor((resting_dim_x - 2 * m_guard_ring_size)/cell_dim_z));
	wafer_dim_x = N_cells_x_remaining * cell_dim_z;
	Box    waf_box(wafer_dim_z/2, wafer_dim_x/2, m_slab.sensitive_thickness/2);
	Volume waf_vol(nam+"_wafer", waf_box, m_slab.sensitive_mat);
	//waf_vol.setLimits(pULimits);
	waf_vol.setVisAttributes(m_slab.sensitive_vis);
	//waf_vol->SetSensitiveDetector(theSD);
	wafer_pos_x        =  -dim.z/2 + num_wafer * real_wafer_size_x + (wafer_dim_x + 2 * m_guard_ring_size)/2;
	real_wafer_size_x  = wafer_dim_x + 2 * m_guard_ring_size;
	double wafer_pos_z = -dim.x/2 + m_guard_ring_size + wafer_dim_z /2;
	for (int n_wafer_z = 1; n_wafer_z < 3; n_wafer_z++)    {
	  PlacedVolume pv = siVol.placeVolume(siVol,Position(wafer_pos_z,wafer_pos_x,0));
	  pv.addPhysVolID("wafer",iwaf*1000 + n_wafer_z);
	  wafer_pos_z += wafer_dim_z + 2 * m_guard_ring_size;
	}
      }
    }
  }

  // Glue space as just a air gap, we don't care about a few points of glue...
  y_slab_floor += (m_slab.sensitive_thickness+m_slab.glue_thickness);

  // The PCB layer, the copper and the shielding are
  // placed as a big G10 layer, as the copper and the
  // shielding ones are very tiny.
  Box    pcbShieldBox(dim.x/2,dim.z/2,m_shield.thickness()/2);
  Volume pcbShieldVol(nam+"_pcbShield",pcbShieldBox,lcdd.material(m_shield.materialStr()));
  pcbShieldVol.setVisAttributes(lcdd.visAttributes(m_shield.visStr()));
  slabVol.placeVolume(pcbShieldVol,Position(0,0,y_slab_floor+m_shield.thickness()/2));
  return slabVol;
}

/// Build Endcap Standard Module 
Volume ECAL::buildModule(bool Zminus, double endcap_rmax, const Solid& siSolid, const Volume& siVol, const Rotation& rot)   {
  // While waiting for more geometric details,
  // build a simple Endcap using a fiber polyhedra
  // and substract the center box
  string  nam = name();
  PolyhedraRegular hedra(8, 0, endcap_rmax, m_module_thickness);
  SubtractionSolid solid(hedra, m_center_tube, Position(), rot);
  Volume           endcap_vol(nam+"_endcap", solid,lcdd.material(m_shield.materialStr()));
  VisAttr          vis = lcdd.visAttributes("EcalEndcapVis");
  endcap_vol.setVisAttributes(vis);

  //----------------------------------------------------
  // Radiator plates in the EndCap structure also as polyhedra, 
  // and radiator plates in the slab of EndCap Rings as box less Tub
  //-------------------------------------------------------
  Volume vol_ringL1, vol_ringL2, vol_ringL3;
  Volume vol_radL1,  vol_radL2,  vol_radL3;

  double r_inner = m_lateral_face_thickness;
  double r_outer = endcap_rmax - m_lateral_face_thickness;
  double box_dim = (m_endcap_center_box_size - m_lateral_face_thickness)/ 2.;
  VisAttr ring_vis     = lcdd.visAttributes("EcalRingVis");
  VisAttr radiator_vis = lcdd.visAttributes("EcalRadiatorVis");

  if(m_layers[0].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, r_inner, r_outer, m_layers[0].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, Position(), rot);
    vol_radL1 = Volume(nam+"_L1_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL1.setVisAttributes(radiator_vis);
    // plate for slab in ring
    Box              box_ring(box_dim,box_dim, m_layers[0].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,Position(), rot);
    vol_ringL1 = Volume(nam+"_L1_ring",sol_ring,m_layers[0].rad_mat);
    vol_ringL1.setVisAttributes(ring_vis);
  }
  if(m_layers[1].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, r_inner, r_outer, m_layers[1].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, Position(), rot);
    vol_radL2 = Volume(nam+"_L2_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL2.setVisAttributes(radiator_vis);
    // plate for slab in ring
    Box              box_ring(box_dim,box_dim, m_layers[1].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,Position(), rot);
    vol_ringL2 = Volume(nam+"_L2_ring",sol_ring,m_layers[0].rad_mat);
    vol_ringL2.setVisAttributes(ring_vis);
  }
  if(m_layers[2].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, r_inner, r_outer, m_layers[2].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, Position(), rot);
    vol_radL1 = Volume(nam+"_L1_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL1.setVisAttributes(radiator_vis);
    // plate for slab in ring
    Box box_ring(box_dim,box_dim, m_layers[2].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,Position(), rot);
    vol_ringL3 = Volume(nam+"_L3_ring",sol_ring,m_layers[2].rad_mat);
    vol_ringL3.setVisAttributes(ring_vis);
  }

  //-------------------------------------------------------
  // Radiator and towers placements inside the Endcap module
  //-------------------------------------------------------

  // We count the layers starting from IP and from 1, so odd layers should be 
  // inside slabs and even ones on the structure.
  double z_floor = -m_module_thickness/2 + m_front.thickness() + N_FIBERS_ALVOULUS * m_fiber_thickness;
  //
  // ATTENTION, TWO LAYERS PER LOOP AS THERE IS ONE INSIDE THE ALVEOLUS.
  //
  double y_slab_si_offset = /*-slab_dim_y/2*/ -m_slab.total_thickness/2 + m_slab.ground_thickness + m_slab.sensitive_thickness/2;
  int num_layers = m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer;
  Rotation rotOdd(M_PI,0,M_PI);
  Rotation rotEven(0,0,M_PI);
  Volume vol_rad, vol_ring;
  vector<Volume>* tower_vols = 0;

  for(int layer_id = 1; layer_id <= num_layers; layer_id+=2)   {
    // place the tower layer for the four modules
    double rad_thickness = m_layers[0].thickness;
    ::printf("%s> installing layer %d....\n",name(),layer_id);
    if(layer_id <= m_layers[0].nLayer) {
      rad_thickness = m_layers[0].thickness;
      vol_ring      = vol_ringL1;
      vol_rad       = vol_radL1;
      tower_vols    = &EC_Towers[0];
    }
    else if(layer_id <= m_layers[0].nLayer + m_layers[1].nLayer) {
      rad_thickness = m_layers[1].thickness;
      vol_ring      = vol_ringL2;
      vol_rad       = vol_radL2;
      tower_vols    = &EC_Towers[1];
    }
    else if(layer_id <= m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer) {
      rad_thickness = m_layers[2].thickness;
      vol_ring      = vol_ringL3;
      vol_rad       = vol_radL3;
      tower_vols    = &EC_Towers[2];
    }

    int num_staves = 4;
    double AlveolusThickness = 2 * m_slab.total_thickness + rad_thickness + 2 * m_fiber_thickness;
    for (int istave = 1; istave <= num_staves; ++istave)  {
      double angle_module = M_PI/2 * ( istave - 1 );
      //if(layer_id==1)
      //	theEndCapSD->SetStaveRotationMatrix(istave,angle_module);

      Rotation rot_o(rotOdd), rot_e(rotEven.theta,rotEven.psi,rotEven.phi+angle_module);
      rot_o.rotateZ(angle_module);
      //rot_e.rotateZ(angle_module);
      size_t ntower = 1;//EC_TowerSlabs.size();
      for (unsigned int itow=0; itow < ntower; itow++)	{
	// first slab
	Volume   tower_slab= EC_TowerSlabs[itow];
	Volume   tower_vol = (*tower_vols)[itow];
	Position tower_pos = EC_TowerXYCenters[itow];
	Box      tower_sol = tower_vol.solid();
	Box      slab_sol  = tower_slab.solid();
	Position pos(tower_pos);
	Rotation rot_o2(rotOdd);
#if 0
	::printf("Tower:%d  Pos: %f %f %f bbox:%f %f %f     solid:%f %f %f\n",	       
		 itow, tower_pos.x,tower_pos.y,tower_pos.z,
		 slab_sol.x(),slab_sol.y(),slab_sol.z(),
		 tower_sol.x(),tower_sol.y(),tower_sol.z());
#endif
	pos.z = z_floor + m_slab.total_thickness/2.0;
	pos.rotateZ(angle_module);
#if 0
	PlacedVolume pv_slab = endcap_vol.placeVolume(tower_slab,pos,rot_o);
	pv_slab.addPhysVolID("Layer",istave*100000 + (itow+1) * 1000 + layer_id);
	if (istave == 1 && itow == 0 && !Zminus) {
	  //  theEndCapSD->AddLayer(layer_id,-pos.x-tower_sol.x(),pos.z-Si_Slab_Y_offset,pos.y-tower_sol.y());
	}
#endif

	// reinitialise pos as it's now rotated for the slab.
	pos   = tower_pos;
	pos.z = z_floor + m_slab.total_thickness /2;
	// update pos to take in account slab + fiber dim
	pos.z += m_slab.total_thickness / 2 + m_fiber_thickness + rad_thickness/2;
	pos.rotateZ(angle_module);

	// radiator inside alveolus
	endcap_vol.placeVolume(tower_vol,pos,rot_e);
	// second slab
	pos.z +=  rad_thickness/2 + m_fiber_thickness + m_slab.total_thickness /2;

	Rotation rot(rotOdd);
	rot.rotateZ(angle_module);
	if(istave % 2 == 1 ) rot.rotateX(M_PI);
	//PlacedVolume pv_tower = endcap_vol.placeVolume(tower_vol,pos,rot);
	//pv_tower.addPhysVolID("Layer",istave*100000 + (itow+1) * 1000 + layer_id+1);

	if (istave == 1 && itow == 0 && !Zminus) {
	  //theEndCapSD->AddLayer(layer_id+1,-pos.x-tower_sol.x(),pos.z-Si_Slab_Y_offset,pos.y-tower_sol.y());
	}
      }
    }

    // ====> BuildECRingAlveolus 
    // Place Si 1 (in z_floor + m_slab.total_thickness / 2)
    //    PlacedVolume pv = endcap_vol.placeVolume(siVol,Position(0,0,z_floor+m_slab.total_thickness/2));
    //    pv.addPhysVolID("Layer",layer_id);
    // set layer in SD
    if(!Zminus) {
      //theEndCapRingSD->AddLayer(layer_id,-siBox.x(),-siBox.y(),z_floor+m_slab.total_thickness/2);
    }
    z_floor += m_slab.total_thickness + m_fiber_thickness;  
    // Place Radiator ring
    //endcap_vol.placeVolume(vol_rad,Position(0,0,z_floor+rad_thickness/2));
    z_floor += rad_thickness + m_fiber_thickness;
  
    // Place Si 2
    //    pv = endcap_vol.placeVolume(siVol,Position(0,0,z_floor+m_slab.total_thickness/2));
    //    pv.addPhysVolID("Layer",layer_id + 1);
    if(!Zminus) {
      //theEndCapRingSD->AddLayer(layer_id+1,-siBox.x(),-siBox.y(),z_floor + m_slab.total_thickness/2);
    }

    z_floor += AlveolusThickness + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness;
    // Place a radiator layer if the number of layers is not complete
    if( layer_id == num_layers) break;
    //    endcap_vol.placeVolume(vol_rad,Position(0,0,z_floor+rad_thickness/ 2.));
    // update the z_floor
    z_floor += rad_thickness + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness;
  }

  return endcap_vol;
}


static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  ECAL ecal(lcdd,e,sens);
  return ecal;
}

DECLARE_DETELEMENT(Tesla_SEcal03,create_element);
