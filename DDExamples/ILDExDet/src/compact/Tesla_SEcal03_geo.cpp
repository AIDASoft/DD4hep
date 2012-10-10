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
    typedef Position Dimension;
    enum { ECALBARREL };
    LCDD&              lcdd;
    SensitiveDetector& sens_det;

    xml_det_t  x_det;
    xml_comp_t x_barrel;
    xml_comp_t x_endcap;
    xml_comp_t x_support;
    xml_comp_t x_front;

    xml_comp_t x_shield;
    struct XMLSlab : public xml_comp_t {
      xml_comp_t shield;
      xml_comp_t copper;
      xml_comp_t sensitive;
      xml_comp_t ground;
      xml_comp_t glue;
      XMLSlab() : xml_comp_t(0), shield(0), copper(0), sensitive(0), ground(0), glue(0) {}
      XMLSlab& operator=(const xml_h& e) { this->Element::operator=(e); return *this; }
    } m_xml_slab;

    struct Slab_t {
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
      Slab_t() : h_fiber_thickness(0), shield_thickness(0), ground_thickness(0),
		 sensitive_thickness(0), copper_thickness(0), glue_thickness(0),
		 total_thickness(0) {}
    } m_slab;

    struct Layer {
      int      nLayer;
      double   thickness;
      Material rad_mat;
    } m_layers[5];


    Volume    m_center_tube;
    Position  m_alveolus;
    struct Barrel_t : public Dimension   {
      typedef std::vector<Layer*> Layers;
      
      int      numTowers;
      double   bottom;
      double   top;
      double   thickness;
      double   inner_r;
      Material radiatorMaterial;
      Layers   layers;
      Volume   stave;
      int      numStaves, numModules;
      PlacedVolume module[9][6];
    } m_barrel;

    struct Endcap_t {
      typedef std::vector<Layer*> Layers;
      
      double rmin;
      double rmax;
      double z;
      double thickness;
      Volume sideA;
      Volume sideB;
      PlacedVolume pvSideA;
      PlacedVolume pvSideB;
      Layers layers;
    } m_endcap;

    int       m_numLayer;
    double    m_lateral_face_thickness;
    double    m_fiber_thickness;
    double    m_guard_ring_size;
    double    m_cell_size;
    double    m_cables_gap;
    double    m_endcap_center_box_size;
    double    m_centerTubDisplacement;
    VisAttr   m_radiatorVis;

    vector<Volume>    EC_TowerSlabs;
    vector<Volume>    EC_Towers[3];
    vector<Position>  EC_TowerXYCenters;

    /// Standard constructor
    ECAL(LCDD& l, const xml_det_t& e, SensitiveDetector& s);

    /// Simple access to the radiator thickness depending on the layer
    double radiatorThickness(int layer_id)   const;

    /// Build Endcap Standard Module 
    Volume buildEndcap(bool Zminus, const Endcap_t& endcap, const Solid& siSolid, const Volume& siVol, const Rotation& rot);

    /// Build barrel volume
    Volume buildBarrelStave(const Barrel_t& barrel);

    /// Build slab 
    Volume buildSlab(bool barrel, const Dimension dim);

    // Build radiator solid
    Volume buildRadiator(const string& name, const Dimension& dim, const Material& mat);
  };
}

#define N_FIBERS_ALVOULUS 3
#define N_FIBERS_W_STRUCTURE 2

ECAL::ECAL(LCDD& l, const xml_det_t& e, SensitiveDetector& s)
  : DetElement(e.nameStr(), e.id()), lcdd(l), x_det(e), x_barrel(0), x_endcap(0), x_support(0), x_front(0), x_shield(0), sens_det(s)
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
  x_support      = x_det.child(Unicode("support"));
  x_barrel       = x_det.child(_X(barrel));
  x_endcap       = x_det.child(_X(endcap));
  x_front        = x_det.child(Unicode("front"));
  x_shield       = x_det.child(Unicode("pcbshield"));

  m_xml_slab           = x_det.child(Unicode("slab"));
  m_xml_slab.shield    = m_xml_slab.child(Unicode("shielding"));
  m_xml_slab.copper    = m_xml_slab.child(Unicode("copper"));
  m_xml_slab.sensitive = m_xml_slab.child(Unicode("sensitive"));
  m_xml_slab.ground    = m_xml_slab.child(Unicode("ground"));
  m_xml_slab.glue      = m_xml_slab.child(Unicode("glue"));

  double endcap_extra_size       = x_endcap.attr<double>(Unicode("extra_size"));
  double crossing_angle          = x_det.attr<double>(Unicode("crossing_angle"));
  m_cables_gap                   = x_det.attr<double>(Unicode("cables_gap"));
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
  m_radiatorVis                  = lcdd.visAttributes("EcalRadiatorVis");

  size_t i=0;
  for(xml_coll_t c(x_det,_X(layer)); c; ++c, ++i) {
    xml_comp_t layer(c);
    m_layers[i].nLayer    = layer.repeat();
    m_layers[i].thickness = layer.thickness();
    m_layers[i].rad_mat   = lcdd.material(layer.materialStr());
    m_barrel.layers.push_back(&m_layers[i]);
    m_endcap.layers.push_back(&m_layers[i]);
  }
  m_numLayer = m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer;
  m_slab.total_thickness  =   m_slab.shield_thickness +    m_slab.copper_thickness + 
    x_shield.thickness()    + m_slab.sensitive_thickness + m_slab.glue_thickness +
    m_slab.ground_thickness + x_det.attr<double>(Unicode("alveolus_gap")) / 2;
  double total_thickness =
    m_layers[0].nLayer * m_layers[0].thickness +
    m_layers[1].nLayer * m_layers[1].thickness +
    m_layers[2].nLayer * m_layers[2].thickness +
    int(m_numLayer/2) * (N_FIBERS_W_STRUCTURE * 2 *  m_fiber_thickness) + // fiber around W struct layers
    (m_numLayer + 1) * (m_slab.total_thickness + (N_FIBERS_ALVOULUS + 1 ) * m_fiber_thickness) + // slabs plus fiber around and inside
    x_support.thickness() + x_front.thickness();
  
  m_barrel.numTowers        = x_barrel.attr<int>(Unicode("towers"));
  m_barrel.thickness        = total_thickness;
  m_barrel.inner_r          = x_barrel.inner_r();
  m_barrel.z                = 2 * x_barrel.zhalf() / 5.;
  m_barrel.bottom           = 2. * std::tan(M_PI/8.) * m_barrel.inner_r + 2.* std::tan(M_PI/8.) * m_barrel.thickness;
  m_barrel.top              = 2. * std::tan(M_PI/8.) * m_barrel.inner_r; //m_barrel.bottom -  2.0 * m_barrel.thickness;
  m_barrel.radiatorMaterial = lcdd.material(x_barrel.attr<string>(Unicode("radiatorMaterial")));

  double module_z_offset    = m_barrel.z*2.5 + m_cables_gap + m_barrel.thickness/2.;
  m_endcap.thickness        = total_thickness;
  m_endcap.rmin             = x_barrel.inner_r();
  m_endcap.rmax             = x_barrel.inner_r() + m_endcap.thickness + endcap_extra_size;
  m_endcap.z                = module_z_offset;

  m_alveolus.z = (m_barrel.z - 2. * m_lateral_face_thickness) / m_barrel.numTowers - 
    2 * N_FIBERS_ALVOULUS  * m_fiber_thickness  - 
    2 * m_slab.h_fiber_thickness -
    2 * m_slab.shield_thickness;

  double siPlateSize = x_endcap.attr<double>(Unicode("center_box_size")) -
    2.0 * m_lateral_face_thickness -
    2.0 * x_endcap.attr<double>(Unicode("ring_gap"));

  m_centerTubDisplacement = m_endcap.z * std::tan(crossing_angle/2000);
  m_center_tube   = Tube(m_endcap.rmin,m_endcap.rmin,0.);
  Box              ec_ringSiBox   (siPlateSize/2.,siPlateSize/2.,m_slab.sensitive_thickness/2.);
  SubtractionSolid ec_ringSiSolid1(ec_ringSiBox,m_center_tube,Position(m_centerTubDisplacement,0,0),Rotation());
  Volume           ec_ringSiVol1  (det_nam+"_ec_ring_volPlus",ec_ringSiSolid1,m_slab.sensitive_mat);
  SubtractionSolid ec_ringSiSolid2(ec_ringSiBox,m_center_tube,Position(-m_centerTubDisplacement,0,0),Rotation());
  Volume           ec_ringSiVol2  (det_nam+"_ec_ring_volNeg",ec_ringSiSolid2,m_slab.sensitive_mat);

  //=================================================
  //
  // Take care:
  //
  // To turn the direction of slabs in the end caps
  // we interchanged X<->Y. It's not clean, but it
  // works...
  //=================================================

  // Main parameters for the virtual EC module
  double y_top   =  m_endcap.rmax - m_lateral_face_thickness;
  double y_mid   =  m_endcap.rmax * std::tan(M_PI/8.) - m_lateral_face_thickness;
  double y_bot   =  m_endcap_center_box_size / 2. + m_lateral_face_thickness;  
  double x_left  = -m_endcap.rmax + m_lateral_face_thickness;
  double x_right =  m_endcap_center_box_size / 2. - m_lateral_face_thickness;
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
      //printf("%s> Build slab x=%f y=%f\n",name().c_str(),m_alveolus.x,m_alveolus.y);
      EC_TowerSlabs.push_back(buildSlab(false,Dimension(m_alveolus.y,m_slab.total_thickness,m_alveolus.x)));
      if( m_layers[0].nLayer > 0 )
	EC_Towers[0].push_back(buildRadiator(det_nam+"_T1",Dimension(m_alveolus.y,m_layers[0].thickness,m_alveolus.x),m_layers[0].rad_mat));
      if( m_layers[1].nLayer > 0 )
	EC_Towers[1].push_back(buildRadiator(det_nam+"_T2",Dimension(m_alveolus.y,m_layers[1].thickness,m_alveolus.x),m_layers[1].rad_mat));
      if( m_layers[2].nLayer > 0 )
	EC_Towers[2].push_back(buildRadiator(det_nam+"_T3",Dimension(m_alveolus.y,m_layers[2].thickness,m_alveolus.x),m_layers[2].rad_mat));
      last_dim_x = m_alveolus.x;
    }
    else   {
      EC_TowerSlabs.push_back(EC_TowerSlabs.back());
      if( m_layers[0].nLayer > 0 ) EC_Towers[0].push_back(EC_Towers[0].back());
      if( m_layers[1].nLayer > 0 ) EC_Towers[1].push_back(EC_Towers[1].back());
      if( m_layers[2].nLayer > 0 ) EC_Towers[2].push_back(EC_Towers[2].back());
    }
    EC_TowerXYCenters.push_back(Position(-(y_curr + m_alveolus.y/2.),-(-m_alveolus.x/2. + x_right),0));
  }

  m_endcap.sideA   = buildEndcap(false,m_endcap,ec_ringSiSolid1,ec_ringSiVol1,Rotation());
  m_endcap.pvSideA = motherVol.placeVolume(m_endcap.sideA,Position(0,0,module_z_offset));
  m_endcap.sideB   = buildEndcap(true,m_endcap,ec_ringSiSolid2,ec_ringSiVol2,Rotation());
  m_endcap.pvSideA = motherVol.placeVolume(m_endcap.sideB,Position(0,0,-module_z_offset),Rotation(M_PI,0,0));

  m_barrel.stave = buildBarrelStave(m_barrel);  
  // BarrelStandardModule placements
  double X = 0;//m_barrel.thickness * std::tan(M_PI/4.);
  double Y = m_barrel.inner_r + m_barrel.thickness/2;
  // theBarrelSD->SetStandardXOffset(X);
  ::memset(&m_barrel.module[0][0],0,sizeof(m_barrel.module));
  m_barrel.numStaves = 8;
  m_barrel.numModules = 5;
  for(int stav_id=1; stav_id < 9; ++stav_id) {
    for(int mod_id=1; mod_id < 6; ++mod_id)   {
      double phi = (stav_id-1) * M_PI/4.;
      double z_offset =  (2.*mod_id-6.)*m_barrel.z/2.;
      Rotation rot(M_PI/2,phi,0);
      Position pos(X,Y,z_offset);
      PlacedVolume pv = motherVol.placeVolume(m_barrel.stave,pos.rotateZ(phi),rot);
      pv.addPhysVolID("barrel",ECALBARREL*100+stav_id*10+mod_id);
      m_barrel.module[stav_id-1][mod_id-1] = pv;
      ::printf("Place Barrel stave:%d,%d Phi:%.4f Pos: %.2f %.2f %.2f \n",stav_id,mod_id,phi,
	       pos.x,pos.y,pos.z);
      //theBarrelSD->SetStaveRotationMatrix(stav_id,phi);
      //theBarrelSD->SetModuleZOffset(mod_id,z_offset);
    }
  }
}

/// Simple access to the radiator thickness depending on the layer
double ECAL::radiatorThickness(int layer_id)   const  {
  if(layer_id <= m_layers[0].nLayer)
    return m_layers[0].thickness;
  else if(layer_id <= (m_layers[0].nLayer + m_layers[1].nLayer))
    return m_layers[1].thickness;
  return m_layers[2].thickness;
}

// Build radiator solid
Volume ECAL::buildRadiator(const string& name, const Dimension& dim, const Material& mat)   {
  Box    box(dim.x/2,dim.z/2,dim.y/2);
  //::printf("%s> Radiator: %.2f %.2f %.2f\n",name.c_str(),dim.x/2,dim.z/2,dim.y/2);
  Volume vol(name+"_radiator",box,mat);
  vol.setVisAttributes(m_radiatorVis);
  return vol;
}

/// Build slab 
Volume ECAL::buildSlab(bool barrel, Dimension dim) {
  string nam = name();

  // Slab solid: hx, hz, hy
  Box slabBox(dim.x/2,dim.z/2,dim.y/2);
  Volume slabVol(nam+"_slab",slabBox,lcdd.air());
  slabVol.setVisAttributes(m_slab.vis);
  double y_slab_floor  = -dim.y/2;

  // Ground plate
  Box    groundBox(dim.x/2,dim.z/2,m_slab.ground_thickness/2);
  Volume groundVol(nam+"_ground",groundBox,m_slab.ground_mat);
  groundVol.setVisAttributes(m_slab.ground_vis);
  slabVol.placeVolume(groundVol,Position(0,0,y_slab_floor+m_slab.ground_thickness/2));
  y_slab_floor += m_slab.ground_thickness;

  // Si layer
  // we place a big plane of Si and inside it the Si wafers, to simplify the gard ring placements
  Box     siBox(dim.x/2,dim.z/2,m_slab.sensitive_thickness/2);
  Volume  siVol(nam+"_sensitive",siBox,m_slab.sensitive_mat);
  siVol.setVisAttributes(m_slab.sensitive_vis);
  slabVol.placeVolume(siVol,Position(0,0,y_slab_floor + m_slab.sensitive_thickness/2));

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

  //::printf("%s> ...slab dim: %.4f x %.4f x %.4f  Grnd:%.4f %.4f %.4f Sensitive:%.4f %.4f %.4f \n",
  //  nam.c_str(), dim.x/2,dim.z/2,dim.y/2,dim.x/2,dim.z/2,m_slab.ground_thickness/2,
  //  dim.x/2,dim.z/2,m_slab.sensitive_thickness/2);

  // As the same method builds both barrel and end cap
  // slabs, place the wafers along the biggest axe
  if (barrel)     {
    // Barrel
    double real_wafer_size_x = wafer_dim_x + 2 * m_guard_ring_size;      
    int    num_wafer_x       = int(floor(dim.x / real_wafer_size_x));
    double wafer_pos_x       = -dim.x/2 + m_guard_ring_size +	wafer_dim_x /2 ;
    // ::printf("%s> ...Building slab for barrel: wafer dim: %.4f x %.4f real:%.4f [%d] pos:%4.f\n",
    //	     nam.c_str(), wafer_dim_x, wafer_dim_z, real_wafer_size_x, num_wafer_x, wafer_pos_x);
    for (int iwaf = 1; iwaf < num_wafer_x + 1; iwaf++)      {
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
    double resting_dim_x = dim.x - (wafer_dim_x + 2 * m_guard_ring_size) * num_wafer_x;      
    if(resting_dim_x > (cell_dim_x + 2 * m_guard_ring_size))	{
      int N_cells_x_remaining =	int(floor((resting_dim_x - 2 * m_guard_ring_size)/cell_dim_x));
      wafer_dim_x = N_cells_x_remaining * cell_dim_x;
      Box    waf_box(wafer_dim_x/2, wafer_dim_z/2, m_slab.sensitive_thickness/2);
      Volume waf_vol(nam+"_wafer_rest",waf_box,m_slab.sensitive_mat);
      //waf_vol.setLimits(pULimits);
      waf_vol.setVisAttributes(m_slab.wafer_vis);
      //waf_vol->SetSensitiveDetector(theSD);

      real_wafer_size_x  =  wafer_dim_x + 2 * m_guard_ring_size;
      wafer_pos_x        = -dim.x/2 + num_wafer_x * real_wafer_size_x + real_wafer_size_x/2;
      double wafer_pos_z = -dim.z/2 + m_guard_ring_size + wafer_dim_z/2;
      for (int iwaf_z = 1; iwaf_z < 3; ++iwaf_z)  {
	PlacedVolume pv = siVol.placeVolume(waf_vol,Position(wafer_pos_x,wafer_pos_z,0));
	pv.addPhysVolID("wafer",num_wafer_x*1000 + iwaf_z);
	wafer_pos_z += wafer_dim_z + 2 * m_guard_ring_size;
      }
    }
  }
  else    {    // End caps
    double real_wafer_size_x =  wafer_dim_z + 2 * m_guard_ring_size;
    double wafer_pos_x       = -dim.z/2 + m_guard_ring_size +	wafer_dim_z /2;
    int    num_wafer_x       =  int(floor(dim.z/real_wafer_size_x));      
    //::printf("%s> ...Building slab for endcap: wafer dim: %.4f x %.4f real:%.4f [%d] pos:%4.f\n",
    //	       nam.c_str(), wafer_dim_x, wafer_dim_z, real_wafer_size_x, num_wafer_x, wafer_pos_x);
    for (int iwaf_x=1; iwaf_x < num_wafer_x + 1; iwaf_x++)	{
      double wafer_pos_z = -dim.x/2 + m_guard_ring_size + wafer_dim_x /2;
      for (int iwaf_z = 1; iwaf_z < 3; ++iwaf_z)    {
	PlacedVolume pv = siVol.placeVolume(waferVol,Position(wafer_pos_z,wafer_pos_x,0));
	pv.addPhysVolID("wafer",iwaf_x*1000 + iwaf_z);
	wafer_pos_z += wafer_dim_x + 2 * m_guard_ring_size;
      }
      wafer_pos_x += wafer_dim_z + 2 * m_guard_ring_size;
    }
    // Magic wafers to complete the slab...
    // (wafers with variable number of cells just to complete the slab. in reality we think that
    // we'll have just a few models of special wafers for that.
    double resting_dim_x = dim.z - (wafer_dim_z + 2 * m_guard_ring_size) * num_wafer_x;
    if(resting_dim_x > (cell_dim_z + 2 * m_guard_ring_size))   {
      int N_cells_x_remaining = int(std::floor((resting_dim_x - 2 * m_guard_ring_size)/cell_dim_z));
      wafer_dim_x = N_cells_x_remaining * cell_dim_z;
      Box    waf_box(wafer_dim_z/2, wafer_dim_x/2, m_slab.sensitive_thickness/2);
      Volume waf_vol(nam+"_wafer", waf_box, m_slab.sensitive_mat);
      waf_vol.setVisAttributes(m_slab.sensitive_vis);
      //waf_vol.setLimits(pULimits);
      //waf_vol->SetSensitiveDetector(theSD);
      wafer_pos_x        = -dim.z/2 + num_wafer_x * real_wafer_size_x + (wafer_dim_x + 2 * m_guard_ring_size)/2;
      real_wafer_size_x  =  wafer_dim_x + 2 * m_guard_ring_size;
      double wafer_pos_z = -dim.x/2 + m_guard_ring_size + wafer_dim_z /2;
      for (int iwaf_z = 1; iwaf_z < 3; iwaf_z++)    {
	PlacedVolume pv = siVol.placeVolume(waf_vol,Position(wafer_pos_z,wafer_pos_x,0));
	pv.addPhysVolID("wafer",num_wafer_x*1000 + iwaf_z);
	wafer_pos_z += wafer_dim_z + 2 * m_guard_ring_size;
      }
    }
  }

  // Glue space as just a air gap, we don't care about a few points of glue...
  y_slab_floor += (m_slab.sensitive_thickness+m_slab.glue_thickness);
  // The PCB layer, the copper and the shielding are placed as a big G10 layer,
  // as the copper and the shielding ones are very tiny.
  Box    pcbShieldBox(dim.x/2,dim.z/2,x_shield.thickness()/2);
  Volume pcbShieldVol(nam+"_shield",pcbShieldBox,lcdd.material(x_shield.materialStr()));
  pcbShieldVol.setVisAttributes(lcdd.visAttributes(x_shield.visStr()));
  slabVol.placeVolume(pcbShieldVol,Position(0,0,y_slab_floor+x_shield.thickness()/2));
  return slabVol;
}

/// Build Endcap Standard Module 
Volume ECAL::buildEndcap(bool Zminus, const Endcap_t& endcap, const Solid& siSolid, const Volume& siVol, const Rotation& rot)   {
  // While waiting for more geometric details,
  // build a simple Endcap using a fiber polyhedra
  // and substract the center box
  string  nam = name();
  PolyhedraRegular hedra(8, 0, endcap.rmax, m_endcap.thickness);
  SubtractionSolid solid(hedra, m_center_tube, Position(), rot);
  Volume           endcap_vol(nam+"_endcap", solid,lcdd.material(x_shield.materialStr()));
  endcap_vol.setVisAttributes(lcdd.visAttributes(x_endcap.visStr()));

  //----------------------------------------------------
  // Radiator plates in the EndCap structure also as polyhedra, 
  // and radiator plates in the slab of EndCap Rings as box less Tub
  //-------------------------------------------------------
  Volume vol_ringL1, vol_ringL2, vol_ringL3;
  Volume vol_radL1,  vol_radL2,  vol_radL3;

  double r_inner = m_lateral_face_thickness;
  double r_outer = endcap.rmax - m_lateral_face_thickness;
  double box_dim = (m_endcap_center_box_size - m_lateral_face_thickness)/ 2.;
  VisAttr ring_vis = lcdd.visAttributes("EcalRingVis");

  if(m_layers[0].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, r_inner, r_outer, m_layers[0].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, Position(), rot);
    vol_radL1 = Volume(nam+"_ECL1_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL1.setVisAttributes(m_radiatorVis);
    // plate for slab in ring
    Box              box_ring(box_dim,box_dim, m_layers[0].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,Position(), rot);
    vol_ringL1 = Volume(nam+"_ECL1_ring",sol_ring,m_layers[0].rad_mat);
    vol_ringL1.setVisAttributes(ring_vis);
  }
  if(m_layers[1].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, r_inner, r_outer, m_layers[1].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, Position(), rot);
    vol_radL2 = Volume(nam+"_ECL2_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL2.setVisAttributes(m_radiatorVis);
    // plate for slab in ring
    Box              box_ring(box_dim,box_dim, m_layers[1].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,Position(), rot);
    vol_ringL2 = Volume(nam+"_ECL2_ring",sol_ring,m_layers[0].rad_mat);
    vol_ringL2.setVisAttributes(ring_vis);
  }
  if(m_layers[2].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, r_inner, r_outer, m_layers[2].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, Position(), rot);
    vol_radL3 = Volume(nam+"_ECL3_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL3.setVisAttributes(m_radiatorVis);
    // plate for slab in ring
    Box box_ring(box_dim,box_dim, m_layers[2].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,Position(), rot);
    vol_ringL3 = Volume(nam+"_ECL3_ring",sol_ring,m_layers[2].rad_mat);
    vol_ringL3.setVisAttributes(ring_vis);
  }

  //-------------------------------------------------------
  // Radiator and towers placements inside the Endcap module
  //-------------------------------------------------------

  // We count the layers starting from IP and from 1, so odd layers should be 
  // inside slabs and even ones on the structure.
  double z_floor = -m_endcap.thickness/2 + x_front.thickness() + N_FIBERS_ALVOULUS * m_fiber_thickness;
  //
  // ATTENTION, TWO LAYERS PER LOOP AS THERE IS ONE INSIDE THE ALVEOLUS.
  //
  double y_slab_si_offset = /*-slab_dim_y/2*/ -m_slab.total_thickness/2 + m_slab.ground_thickness + m_slab.sensitive_thickness/2;
  int num_layers = m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer;
  Rotation rotOdd(M_PI,M_PI,0);
  Rotation rotEven(0,M_PI,0);
  Volume vol_rad, vol_ring;
  vector<Volume>* tower_vols = 0;
  PlacedVolume pv;
  //num_layers = 2;
  for(int layer_id = 1; layer_id <= num_layers; layer_id+=2)   {
    // place the tower layer for the four modules
    double rad_thickness = m_layers[0].thickness;
    //::printf("%s> installing layer %d....\n",name(),layer_id);
    rad_thickness = radiatorThickness(layer_id);
    if(layer_id <= m_layers[0].nLayer) {
      vol_ring      = vol_ringL1;
      vol_rad       = vol_radL1;
      tower_vols    = &EC_Towers[0];
    }
    else if(layer_id <= m_layers[0].nLayer + m_layers[1].nLayer) {
      vol_ring      = vol_ringL2;
      vol_rad       = vol_radL2;
      tower_vols    = &EC_Towers[1];
    }
    else if(layer_id <= m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer) {
      vol_ring      = vol_ringL3;
      vol_rad       = vol_radL3;
      tower_vols    = &EC_Towers[2];
    }
#if 0
    const Rotation& _r=rot_e;
    ::printf("Stave:%d  Tower:%d  Pos: %f %f %f Rot:%f %f %f, Slab:%f %f %f     Tower:%f %f %f\n",	       
	     istave,itow, pos.x, pos.y, pos.z,
	     _r.theta,_r.phi,_r.psi,
	     slab_sol.x(),slab_sol.y(),slab_sol.z(),
	     tower_sol.x(),tower_sol.y(),tower_sol.z());
#endif
    // Build EC Alveolus
    double AlveolusThickness = 2 * m_slab.total_thickness + rad_thickness + 2 * m_fiber_thickness;
    for(size_t istave=1, num_staves=4; istave <= num_staves; ++istave)  {
      double angle_module = M_PI/2 * ( istave - 1 );
      Rotation rotodd_s1(rotOdd), rot_e(rotEven);
      rotodd_s1.rotateZ(angle_module);
      rot_e.rotateZ(angle_module);

      //if(layer_id==1)
      // theEndCapSD->SetStaveRotationMatrix(istave,angle_module);

      for(size_t itow=0, ntower=EC_TowerSlabs.size(); itow < ntower; itow++)	{
	Volume   tower_slab= EC_TowerSlabs[itow];
	Volume   tower_vol = (*tower_vols)[itow];
	Position tower_pos = EC_TowerXYCenters[itow];
	Box      tower_sol = tower_vol.solid();
	Box      slab_sol  = tower_slab.solid();
	Position pos(tower_pos);

	// Place the first slab
	pos.z = z_floor + m_slab.total_thickness/2.0;
	pos.rotateZ(angle_module);
	pv = endcap_vol.placeVolume(tower_slab,pos,rotodd_s1);
	pv.addPhysVolID("Layer",istave*100000 + (itow+1) * 1000 + layer_id);
	if (istave == 1 && itow == 0 && !Zminus) {
	  //  theEndCapSD->AddLayer(layer_id,-pos.x-tower_sol.x(),pos.z-Si_Slab_Y_offset,pos.y-tower_sol.y());
	}

	// reinitialise pos as it's now rotated for the slab and place radiator inside alveolus
	pos    = tower_pos;
	pos.z  = z_floor + m_slab.total_thickness/2;
	// update pos to take in account slab + fiber dim
	pos.z += m_slab.total_thickness/2 + m_fiber_thickness + rad_thickness/2;
	pos.rotateZ(angle_module);
	pv = endcap_vol.placeVolume(tower_vol,pos,rot_e);

	// Place the second slab
	pos.z += rad_thickness/2 + m_fiber_thickness + m_slab.total_thickness /2;
	Rotation rotodd_s2 = rotodd_s1;
	rotodd_s2.rotateX(M_PI);
	pv = endcap_vol.placeVolume(tower_slab,pos,rotodd_s2);
	pv.addPhysVolID("Layer",istave*100000 + (itow+1) * 1000 + layer_id+1);
	if (istave == 1 && itow == 0 && !Zminus) {
	  //theEndCapSD->AddLayer(layer_id+1,-pos.x-tower_sol.x(),pos.z-Si_Slab_Y_offset,pos.y-tower_sol.y());
	}
      }
    }

    {  // ====> Build EC RingAlveolus 
      double z_ring = z_floor;
      // Place Si 1 (in z_ring + m_slab.total_thickness / 2)
      pv = endcap_vol.placeVolume(siVol,Position(0,0,z_ring+m_slab.total_thickness/2));
      pv.addPhysVolID("Layer",layer_id);
      if(!Zminus) {    // set layer in SD
	//theEndCapRingSD->AddLayer(layer_id,-siBox.x(),-siBox.y(),z_ring+m_slab.total_thickness/2);
      }
      z_ring += m_slab.total_thickness + m_fiber_thickness;  
      // Place Radiator ring
      endcap_vol.placeVolume(vol_ring,Position(0,0,z_ring+rad_thickness/2));
      z_ring += rad_thickness + m_fiber_thickness;
  
      // Place Si 2
      pv = endcap_vol.placeVolume(siVol,Position(0,0,z_ring+m_slab.total_thickness/2));
      pv.addPhysVolID("Layer",layer_id + 1);
      if(!Zminus) {
	//theEndCapRingSD->AddLayer(layer_id+1,-siBox.x(),-siBox.y(),z_ring + m_slab.total_thickness/2);
      }
    }
    z_floor += AlveolusThickness + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness;
    // Place a radiator layer if the number of layers is not complete
    if( layer_id == num_layers) break;
    endcap_vol.placeVolume(vol_rad,Position(0,0,z_floor+rad_thickness/2),Rotation(0,0,M_PI/8));
    // update the z_floor
    z_floor += rad_thickness + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness;
  }
  return endcap_vol;
}

///  Barrel Standard Module
Volume ECAL::buildBarrelStave(const Barrel_t& barrel) {
  string nam = name();
  Trapezoid barrelTrd(barrel.bottom/2, barrel.top/2, barrel.z/2, barrel.z/2, barrel.thickness/2);
  Volume    barrelVol(nam+"_barrel",barrelTrd,lcdd.material(x_barrel.materialStr()));
  barrelVol.setVisAttributes(lcdd.visAttributes(x_barrel.visStr()));

  // We count the layers starting from IP and from 1, so odd layers should be inside slabs and
  // even ones on the structure.
  // The structure W layers are here big plans, as the gap between each W plate is too small 
  // to create problems. The even W layers are part of H structure placed inside the alveolus.
  double y_floor = x_front.thickness() + N_FIBERS_ALVOULUS * m_fiber_thickness;
  double z_pos;
  PlacedVolume pv;
  for(int layer_id = 1; layer_id < m_numLayer+1; layer_id+=2) {// ATTENTION, TWO LAYERS PER LOOP
    // build and place the several Alveolus with 
    // the slabs and the radiator layer inside.
    double rad_thick = radiatorThickness(layer_id);
    double alveolus_dim_y = 2 * m_slab.total_thickness + rad_thick + 2 * m_fiber_thickness;

    //------ BuildBarrelAlveolus ------
    double alveolus_dim_x = barrel.bottom - 2*(y_floor+alveolus_dim_y)*std::tan(M_PI/8);
    double x_off, y_off, y_fl;
    // To simplify we place each slab and the radiator layer directly into the fiber module.
    //
    // Build a slab:
    Volume slabVol = buildSlab(true,Dimension(alveolus_dim_x,m_slab.total_thickness,m_alveolus.z));

    // Place the Slab and radiator inside the H, here directly into the module fiber as the
    // H structure is also built in fiber.
    double z_tower_center = -barrel.z /2 + m_lateral_face_thickness 
      + m_fiber_thickness * N_FIBERS_ALVOULUS + m_slab.shield_thickness
      + m_slab.h_fiber_thickness + m_alveolus.z /2;  

    Dimension radDim1(alveolus_dim_x,rad_thick,m_alveolus.z);
    Volume radVol1 = buildRadiator(nam+"_barrel_radiator1",radDim1,barrel.radiatorMaterial);
    for (int itow = m_barrel.numTowers; itow > 0; --itow )    {
      y_fl = y_floor;
      x_off = 0; // to be calculed
      y_off = y_fl - barrel.thickness/2 + m_slab.total_thickness/2;

      // Place First Slab
      pv = barrelVol.placeVolume(slabVol,Position(x_off,z_tower_center,y_off),Rotation(M_PI,0,0));
      pv.addPhysVolID("tower",itow * 1000 + layer_id);
#if 0
      if (itow == Ecal_barrel_number_of_towers)  {
	theBarrelSD->AddLayer(layer_id,
			      x_off - ((G4Box *)SlabLog->GetSolid())->GetXHalfLength(),
			      Ecal_inner_radius + barrel.thickness/2 + y_off - Si_Slab_Y_offset,
			      z_tower_center - ((G4Box *)SlabLog->GetSolid())->GetYHalfLength());
      }
#endif
      y_fl += m_slab.total_thickness + m_fiber_thickness;
      
      // Radiator layer "inside" alveolus
      y_off  = -barrel.thickness/2 + y_fl + rad_thick/2.;
      pv     =  barrelVol.placeVolume(radVol1,Position(0,z_tower_center,y_off));
      pv.addPhysVolID("tower",itow * 1000 + layer_id);

      y_fl  +=  rad_thick + m_fiber_thickness;
      y_off  = -barrel.thickness/2 + y_fl + m_slab.total_thickness/2;
      // Second Slab: starts from bottom to up
      pv = barrelVol.placeVolume(radVol1,Position(0,z_tower_center,y_off));
      pv.addPhysVolID("tower",itow * 1000 + layer_id + 1);
#if 0
      if (itow == Ecal_barrel_number_of_towers)  	{
	theBarrelSD->AddLayer(layer_id + 1,
			      x_off - ((G4Box *)SlabLog->GetSolid())->GetXHalfLength(),
			      Ecal_inner_radius + barrel.thickness/2 + y_off + Si_Slab_Y_offset,
			      z_tower_center - ((G4Box *)SlabLog->GetSolid())->GetYHalfLength());
      }
#endif
      z_tower_center += m_alveolus.z + 
	2 * m_fiber_thickness * N_FIBERS_ALVOULUS +
	2 * m_slab.h_fiber_thickness + 
	2 * m_slab.shield_thickness;
    }
    //---------------------------------
    // update the y_floor
    y_floor += (alveolus_dim_y + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness);
    int even_layer = layer_id + 1;
    if ( even_layer > m_numLayer )
      continue;

    // Build and place the structure radiator layer into the module
    double radiator_dim_x = barrel.bottom - 2*(y_floor+rad_thick)*std::tan(M_PI/8);
    double radiator_dim_z = barrel.z - 2.*m_lateral_face_thickness - 2*N_FIBERS_W_STRUCTURE * m_fiber_thickness;
    
    Dimension radDim2(radiator_dim_x,rad_thick,radiator_dim_z);
    Volume radVol2 = buildRadiator(nam+"barrel_radiator2",radDim2,barrel.radiatorMaterial);
    pv = barrelVol.placeVolume(radVol2,Position(0,0,-barrel.thickness/2+y_floor+rad_thick/2));

    // update the y_floor
    y_floor += (rad_thick + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness);
  }
  return barrelVol;
}


static Ref_t 
create_element(LCDD& lcdd, const xml_h& elt, SensitiveDetector& sens)  {
  ECAL ecal(lcdd,elt,sens);
  return ecal;
}
DECLARE_DETELEMENT(Tesla_SEcal03,create_element);
