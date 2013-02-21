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
#include "Math/RotationZ.h"

namespace DD4hep { namespace Geometry {
  struct SEcal03Data : public DetElement::Object  {
    typedef Position Dimension;
    enum { ECALBARREL=1, ECALENDCAPPLUS=2, ECALENDCAPMINUS=3 };
    enum { SIDE_PLUS=0, SIDE_MINUS=1 };
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
      Slab() : h_fiber_thickness(0), shield_thickness(0), ground_thickness(0),
	       sensitive_thickness(0), copper_thickness(0), glue_thickness(0),
	       total_thickness(0) {}
    } m_slab;

    struct Layer   {
      int      nLayer;
      double   thickness;
      Material rad_mat;
    } m_layers[5];


    Volume    m_center_tube;
    Position  m_alveolus;
    struct Barrel : public DetElement  {
      typedef std::vector<Layer*> Layers;
      
      int      numTowers;
      double   bottom;
      double   top;
      double   thickness;
      double   inner_r;
      double   dim_z;
      Material radiatorMaterial;
      Material material;
      VisAttr  vis;
      Layers   layers;
      int      numStaves, numModules;
      DetElement module[9][6];
      SensitiveDetector sensDet;
      /// Helper function to allow assignment
      DetElement& operator=(const DetElement& d)  { return this->DetElement::operator=(d); }
    } m_barrel;

    struct Endcap : public DetElement  {
      typedef std::vector<Layer*> Layers;
      
      double rmin;
      double rmax;
      double dim_z;
      double thickness;
      VisAttr      vis;
      Layers layers;
      SensitiveDetector sensDet;
      SensitiveDetector ringSD;
      DetElement side[2];
      /// Helper function to allow assignment
      DetElement& operator=(const DetElement& d)  { return this->DetElement::operator=(d); }
    } m_endcap;

    struct Shield {
      Material material;
      VisAttr  vis;
      double   thickness;
    } m_shield;
    int       m_numLayer;
    double    m_front_thickness;
    double    m_lateral_face_thickness;
    double    m_fiber_thickness;
    double    m_support_thickness;
    double    m_guard_ring_size;
    double    m_cell_size;
    double    m_cables_gap;
    double    m_endcap_center_box_size;
    double    m_centerTubDisplacement;
    VisAttr   m_radiatorVis;
    LimitSet  m_limits;

    std::vector<Volume>    EC_TowerSlabs;
    std::vector<Volume>    EC_Towers[3];
    std::vector<Position>  EC_TowerXYCenters;
    LCDD*       lcdd;
    std::string name;
    DetElement  self;
  };

  struct SEcal03 : public SEcal03Data  {
    /// Standard constructor
    SEcal03() : SEcal03Data() {}
    /// Detector construction function
    DetElement construct(LCDD& lcdd, xml_det_t e);
    /// Simple access to the radiator thickness depending on the layer
    double radiatorThickness(int layer_id)   const;
    /// Build Endcap Standard Module 
    Volume buildEndcap(DetElement det,bool Zminus, const Endcap& endcap, const Solid& siSolid, const Volume& siVol, const Rotation& rot);
    /// Build barrel volume
    std::pair<DetElement,Volume> buildBarrelStave(const Barrel& barrel);
    /// Build slab 
    Volume buildSlab(const std::string& prefix, bool barrel, const Dimension dim, SensitiveDetector& sd);
    // Build radiator solid
    Volume buildRadiator(const std::string& name, const Dimension& dim, const Material& mat);
  };
}}

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

#define N_FIBERS_ALVOULUS 3
#define N_FIBERS_W_STRUCTURE 2

/// Detector construction function
DetElement SEcal03::construct(LCDD& l, xml_det_t x_det)  {
  lcdd = &l;
  name = x_det.nameStr();
  self.assign(dynamic_cast<Value<TNamed,SEcal03>*>(this),name,x_det.typeStr());
  self._data().id = x_det.id();
  xml_comp_t x_param             = x_det.child(_U(param));
  xml_comp_t x_barrel            = x_det.child(_X(barrel));
  xml_comp_t x_endcap            = x_det.child(_X(endcap));
  xml_comp_t x_shield            = x_det.child(_U(pcbshield));
  xml_comp_t m_xml_slab          = x_det.child(_U(slab));
  xml_comp_t m_xml_slab_shield   = m_xml_slab.child(_U(shielding));
  xml_comp_t m_xml_slab_copper   = m_xml_slab.child(_U(copper));
  xml_comp_t m_xml_slab_sensitive= m_xml_slab.child(_U(sensitive));
  xml_comp_t m_xml_slab_ground   = m_xml_slab.child(_U(ground));
  xml_comp_t m_xml_slab_glue     = m_xml_slab.child(_U(glue));

  double endcap_extra_size       = x_endcap.attr<double>(_U(extra_size));
  double crossing_angle          = x_param.crossing_angle();
  Assembly  assembly(name+"_assembly");
  // Hosting volume
  Volume motherVol               = lcdd->pickMotherVolume(self);
  // User limits for this sub detector
  m_limits                       = lcdd->limitSet(x_det.limitsStr());

  m_cables_gap                   = x_param.attr<double>(_U(cables_gap));
  m_lateral_face_thickness       = x_param.attr<double>(_U(lateral_face_thickness));
  m_fiber_thickness              = x_param.attr<double>(_U(fiber_thickness));
  m_cell_size                    = x_param.attr<double>(_U(cell_size));
  m_guard_ring_size              = x_param.attr<double>(_U(guard_ring_size));
  m_front_thickness              = x_param.attr<double>(_U(front_face_thickness));
  m_support_thickness            = x_param.attr<double>(_U(support_thickness));

  m_shield.vis                   = lcdd->visAttributes(x_shield.visStr());
  m_shield.material              = lcdd->material(x_shield.materialStr());
  m_shield.thickness             = x_shield.thickness();

  m_slab.vis                     = lcdd->visAttributes("EcalSlabVis");
  m_slab.h_fiber_thickness       = m_xml_slab.attr<double>(_U(h_fiber_thickness));
  m_slab.shield_thickness        = m_xml_slab_shield.thickness();
  m_slab.ground_thickness        = m_xml_slab_ground.thickness();
  m_slab.ground_mat              = lcdd->material(m_xml_slab_ground.materialStr());
  m_slab.ground_vis              = lcdd->visAttributes("EcalSlabGroundVis");
  m_slab.sensitive_thickness     = m_xml_slab_sensitive.thickness();
  m_slab.sensitive_mat           = lcdd->material(m_xml_slab_sensitive.materialStr());
  m_slab.sensitive_vis           = lcdd->visAttributes("EcalSlabSensitiveVis");
  m_slab.copper_thickness        = m_xml_slab_copper.thickness();
  m_slab.copper_mat              = lcdd->material(m_xml_slab_copper.materialStr());
  m_slab.glue_thickness          = m_xml_slab_glue.thickness();
  m_slab.wafer_vis               = lcdd->visAttributes("EcalWaferVis");
  m_radiatorVis                  = lcdd->visAttributes("EcalRadiatorVis");

  size_t i=0;
  for(xml_coll_t c(x_det,_X(layer)); c; ++c, ++i) {
    xml_comp_t layer(c);
    m_layers[i].nLayer    = layer.repeat();
    m_layers[i].thickness = layer.thickness();
    m_layers[i].rad_mat   = lcdd->material(layer.materialStr());
    m_barrel.layers.push_back(&m_layers[i]);
    m_endcap.layers.push_back(&m_layers[i]);
  }
  m_numLayer = m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer;
  m_slab.total_thickness  =   m_slab.shield_thickness +    m_slab.copper_thickness + 
    m_shield.thickness      + m_slab.sensitive_thickness + m_slab.glue_thickness +
    m_slab.ground_thickness + x_param.attr<double>(_U(alveolus_gap)) / 2;
  double total_thickness =
    m_layers[0].nLayer * m_layers[0].thickness +
    m_layers[1].nLayer * m_layers[1].thickness +
    m_layers[2].nLayer * m_layers[2].thickness +
    int(m_numLayer/2) * (N_FIBERS_W_STRUCTURE * 2 *  m_fiber_thickness) + // fiber around W struct layers
    (m_numLayer + 1) * (m_slab.total_thickness + (N_FIBERS_ALVOULUS + 1 ) * m_fiber_thickness) + // slabs plus fiber around and inside
    m_support_thickness + m_front_thickness;

  m_barrel.numTowers        = x_barrel.attr<int>(_U(towers));
  m_barrel.thickness        = total_thickness;
  m_barrel.inner_r          = x_barrel.inner_r();
  m_barrel.dim_z            = 2 * x_barrel.zhalf() / 5;
  m_barrel.bottom           = 2 * std::tan(M_PI/8) * m_barrel.inner_r + 2*std::tan(M_PI/8) * m_barrel.thickness;
  m_barrel.top              = 2 * std::tan(M_PI/8) * m_barrel.inner_r; //m_barrel.bottom -  2* m_barrel.thickness;
  m_barrel.vis              = lcdd->visAttributes(x_barrel.visStr());
  m_barrel.material         = lcdd->material(x_barrel.materialStr());
  m_barrel.radiatorMaterial = lcdd->material(x_barrel.attr<string>(_U(radiatorMaterial)));

  double module_z_offset    = m_barrel.dim_z*2.5 + m_cables_gap + m_barrel.thickness/2.;
  m_endcap.vis              = lcdd->visAttributes(x_endcap.visStr());
  m_endcap.thickness        = total_thickness;
  m_endcap.rmin             = x_barrel.inner_r();
  m_endcap.rmax             = x_barrel.inner_r() + m_endcap.thickness + endcap_extra_size;
  m_endcap.dim_z            = module_z_offset;

  m_alveolus.SetZ( (m_barrel.dim_z - 2*m_lateral_face_thickness) / m_barrel.numTowers - 
		   2 * N_FIBERS_ALVOULUS  * m_fiber_thickness  - 
		   2 * m_slab.h_fiber_thickness -
		   2 * m_slab.shield_thickness);

  double siPlateSize = x_endcap.attr<double>(_U(center_box_size)) -
    2 * m_lateral_face_thickness - 2 * x_endcap.attr<double>(_U(ring_gap));
  
  m_centerTubDisplacement = m_endcap.dim_z * std::tan(crossing_angle);
  m_center_tube   = Tube(0,m_endcap.rmin,total_thickness);
  Box              ec_ringSiBox   (siPlateSize/2,siPlateSize/2,m_slab.sensitive_thickness/2);
  SubtractionSolid ec_ringSiSolid1(ec_ringSiBox,m_center_tube,Position(m_centerTubDisplacement,0,0),Rotation());
  Volume           ec_ringSiVol1  ("ring_plus",ec_ringSiSolid1,m_slab.sensitive_mat);
  SubtractionSolid ec_ringSiSolid2(ec_ringSiBox,m_center_tube,Position(-m_centerTubDisplacement,0,0),Rotation());
  Volume           ec_ringSiVol2  ("ring_minus",ec_ringSiSolid2,m_slab.sensitive_mat);

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
  double y_mid   =  m_endcap.rmax * std::tan(M_PI/8) - m_lateral_face_thickness;
  double y_bot   =  m_endcap_center_box_size/2 + m_lateral_face_thickness;  
    
  double x_left  = -m_endcap.rmax + m_lateral_face_thickness;
  double x_right =  m_endcap_center_box_size/2 - m_lateral_face_thickness;
  double x_mid   = -y_mid;

  double fiber_inter_alveolus =
    2 * (m_slab.h_fiber_thickness + m_slab.shield_thickness + N_FIBERS_ALVOULUS * m_fiber_thickness);
  
  // The alveolus size takes in account the module Z size
  // but also 4 fiber layers for the alveoulus wall, the all
  // divided by the number of towers
  m_alveolus.SetY(m_alveolus.Z());

  double inc_x = (x_mid-x_left ) / (y_top-y_mid);
  double inc_y = m_alveolus.Y() + fiber_inter_alveolus;
  int itower = -1;
  for(double last_dim_x=0, y_floor=y_bot+m_alveolus.Y(); y_floor < y_top; y_floor += inc_y )    {
    m_alveolus.SetX(x_right - x_left - ((y_floor <= y_mid) ? 0 : (y_floor - y_mid) * inc_x));
    // While the towers have the same shape use the same logical volumes and parameters.
    if(last_dim_x != m_alveolus.X())      {
      ++itower;
      //printf("%s> Build slab x=%f y=%f\n",name().c_str(),m_alveolus.X(),m_alveolus.Y());
      EC_TowerSlabs.push_back(buildSlab(name+_toString(itower,"_endcap_tower%d"),false,
					Dimension(m_alveolus.Y(),m_slab.total_thickness,m_alveolus.X()),
					m_endcap.sensDet));
      if( m_layers[0].nLayer > 0 )
	EC_Towers[0].push_back(buildRadiator(name+_toString(int(EC_Towers[0].size()),"_endcap_T1_layer%d"),
					     Dimension(m_alveolus.Y(),m_layers[0].thickness,m_alveolus.X()),m_layers[0].rad_mat));
      if( m_layers[1].nLayer > 0 )
	EC_Towers[1].push_back(buildRadiator(name+_toString(int(EC_Towers[1].size()),"_endcap_T2_layer%d"),
					     Dimension(m_alveolus.Y(),m_layers[1].thickness,m_alveolus.X()),m_layers[1].rad_mat));
      if( m_layers[2].nLayer > 0 )
	EC_Towers[2].push_back(buildRadiator(name+_toString(int(EC_Towers[2].size()),"_endcap_T3_layer%d"),
					     Dimension(m_alveolus.Y(),m_layers[2].thickness,m_alveolus.X()),m_layers[2].rad_mat));
      last_dim_x = m_alveolus.X();
    }
    else   {
      EC_TowerSlabs.push_back(EC_TowerSlabs.back());
      if( m_layers[0].nLayer > 0 ) EC_Towers[0].push_back(EC_Towers[0].back());
      if( m_layers[1].nLayer > 0 ) EC_Towers[1].push_back(EC_Towers[1].back());
      if( m_layers[2].nLayer > 0 ) EC_Towers[2].push_back(EC_Towers[2].back());
    }
    //EC_TowerXYCenters.push_back(Position(-(y_floor + m_alveolus.Y()/2),-(x_right - m_alveolus.X()/2),0));
    EC_TowerXYCenters.push_back(Position(-(y_floor + m_alveolus.Y()/2),-(x_right - m_alveolus.X()/2 + inc_y),0));
  }

  // Setup the sensitive detectors for barrel, endcap+ and endcap-
  SensitiveDetector sd = m_barrel.sensDet = SensitiveDetector("EcalBarrel","calorimeter");
  Readout ro = lcdd->readout(x_barrel.readoutStr());
  sd.setHitsCollection(ro.name());
  sd.setReadout(ro);
  lcdd->addSensitiveDetector(sd);

  sd = m_endcap.sensDet = SensitiveDetector("EcalEndcapRings","calorimeter");
  ro = lcdd->readout(x_endcap.attr<string>(_U(ring_readout)));
  sd.setHitsCollection(ro.name());
  sd.setReadout(ro);
  lcdd->addSensitiveDetector(sd);

  sd = m_endcap.ringSD = SensitiveDetector("EcalEndcap","calorimeter");
  ro = lcdd->readout(x_endcap.readoutStr());
  sd.setHitsCollection(ro.name());
  sd.setReadout(ro);
  lcdd->addSensitiveDetector(sd);

  ec_ringSiVol1.setSensitiveDetector(m_endcap.ringSD);
  ec_ringSiVol2.setSensitiveDetector(m_endcap.ringSD);
  ec_ringSiVol1.setVisAttributes(m_endcap.vis);
  ec_ringSiVol2.setVisAttributes(m_endcap.vis);
  ec_ringSiVol1.setLimitSet(m_limits);
  ec_ringSiVol2.setLimitSet(m_limits);

  Volume vol;
  PlacedVolume pv;
  DetElement det;
  m_barrel = DetElement(self,"barrel",ECALBARREL);
  m_endcap = DetElement(self,"endcaps",0);
  det = DetElement(m_endcap,"plus",ECALENDCAPPLUS);
  vol = buildEndcap(det,false,m_endcap,ec_ringSiSolid1,ec_ringSiVol1,Rotation());
  pv  = assembly.placeVolume(vol,Position(0,0,module_z_offset),Rotation(0,0,0));
  det.setPlacement(pv);
  m_endcap.side[SIDE_PLUS] = det;

  det = DetElement(m_endcap,"minus",ECALENDCAPMINUS);
  vol = buildEndcap(det,true,m_endcap,ec_ringSiSolid2,ec_ringSiVol2,Rotation());
  pv  = assembly.placeVolume(vol,Position(0,0,-module_z_offset),Rotation(0,M_PI,0));
  det.setPlacement(pv);
  m_endcap.side[SIDE_MINUS] = det;

  std::pair<DetElement,Volume> stave = buildBarrelStave(m_barrel);  
  // BarrelStandardModule placements
  double X = 0;//m_barrel.thickness * std::tan(M_PI/4.);
  double Y = m_barrel.inner_r + m_barrel.thickness/2;
  // theBarrelSD->SetStandardXOffset(X);
  ::memset(&m_barrel.module[0][0],0,sizeof(m_barrel.module));
  m_barrel.numStaves = 8;
  m_barrel.numModules = 5;

  DetElement mod_det;
  for(int stav_id=1; stav_id < 9; ++stav_id)   {
    int stave_ident = ECALBARREL*100+stav_id;
    DetElement stave_det(m_barrel,_toString(stav_id,"stave%d"),stave_ident);
    Assembly staveVol(_toString(stav_id,"stave%d"));
    for(int mod_id=1; mod_id < 6; ++mod_id)   {
      double phi = (stav_id-1) * M_PI/4.;
      double z_offset =  (2.*mod_id-6.)*m_barrel.dim_z/2.;
      Rotation rot(phi,M_PI/2,0);
      Position pos(X,Y,z_offset);
      int mod_ident = stave_ident+mod_id;
      pv = staveVol.placeVolume(stave.second,RotateZ(pos,phi),rot);
      pv.addPhysVolID("barrel",ECALBARREL*100+stav_id*10+mod_id);
      if ( !mod_det.isValid() ) { // same as if(first) ...
	stave.first->SetName(_toString(mod_id,"module%d").c_str());
	stave.first._data().id = mod_ident;
	mod_det = stave.first;
      }
      else  {
	mod_det = stave.first.clone(_toString(mod_id,"module%d"),mod_ident);
      }
      stave_det.add(mod_det);
      mod_det.setPlacement(pv);
      m_barrel.module[stav_id-1][mod_id-1] = mod_det;
      //::printf("Place Barrel stave:%d,%d Phi:%.4f Pos: %.2f %.2f %.2f \n",stav_id,mod_id,phi,
      //	       pos.X(),pos.Y(),pos.Z());
      //theBarrelSD->SetStaveRotationMatrix(stav_id,phi);
      //theBarrelSD->SetModuleZOffset(mod_id,z_offset);
    }
    pv = assembly.placeVolume(staveVol);
    stave_det.setPlacement(pv);
  }

  assembly.setVisAttributes(lcdd->visAttributes(x_det.visStr()));
  pv = motherVol.placeVolume(assembly);
  m_barrel.setPlacement(pv);
  m_endcap.setPlacement(pv);
  self.setPlacement(pv);
  return self;
}

/// Simple access to the radiator thickness depending on the layer
double SEcal03::radiatorThickness(int layer_id)   const  {
  if(layer_id <= m_layers[0].nLayer)
    return m_layers[0].thickness;
  else if(layer_id <= (m_layers[0].nLayer + m_layers[1].nLayer))
    return m_layers[1].thickness;
  return m_layers[2].thickness;
}

// Build radiator solid
Volume SEcal03::buildRadiator(const string& name, const Dimension& dim, const Material& mat)   {
  Volume vol(name+"_radiator",Box(dim.X()/2,dim.Z()/2,dim.Y()/2),mat);
  vol.setVisAttributes(m_radiatorVis);
  return vol;
}

/// Build slab 
Volume SEcal03::buildSlab(const string& prefix, bool barrel, Dimension dim, SensitiveDetector& sd) {
  // Slab solid: hx, hz, hy
  Box slabBox(dim.X()/2,dim.Z()/2,dim.Y()/2);
  Volume slabVol(prefix+"_slab",slabBox,lcdd->air());
  slabVol.setVisAttributes(m_slab.vis);
  double y_slab_floor  = -dim.Y()/2;

  // Ground plate
  Box    groundBox(dim.X()/2,dim.Z()/2,m_slab.ground_thickness/2);
  Volume groundVol(prefix+"_ground",groundBox,m_slab.ground_mat);
  groundVol.setVisAttributes(m_slab.ground_vis);
  slabVol.placeVolume(groundVol,Position(0,0,y_slab_floor+m_slab.ground_thickness/2));
  y_slab_floor += m_slab.ground_thickness;

  // Si layer
  // we place a big plane of Si and inside it the Si wafers, to simplify the gard ring placements
  Box     siBox(dim.X()/2,dim.Z()/2,m_slab.sensitive_thickness/2);
  Volume  siVol(prefix+"_sensitive",siBox,m_slab.sensitive_mat);
  siVol.setVisAttributes(m_slab.sensitive_vis);
  slabVol.placeVolume(siVol,Position(0,0,y_slab_floor + m_slab.sensitive_thickness/2));

  // Then we place the Si wafers inside the big Si plane
  //
  // User limits to limit the step to stay inside the cell
  //G4UserLimits* pULimits = new G4UserLimits(theMaxStepAllowed);

  // Normal squared wafers
  double cell_dim_x = m_cell_size;
  double total_Si_dim_z = m_alveolus.Z();
  double util_SI_wafer_dim_z = total_Si_dim_z/2 -  2 * m_guard_ring_size;
  double cell_dim_z     =  util_SI_wafer_dim_z / std::floor(util_SI_wafer_dim_z/cell_dim_x);
  int    N_cells_in_Z   = int(util_SI_wafer_dim_z/cell_dim_z);
  int    N_cells_in_X   = N_cells_in_Z;
  
  cell_dim_x = cell_dim_z;

  double wafer_dim_x = N_cells_in_X * cell_dim_x;
  double wafer_dim_z = N_cells_in_Z * cell_dim_z;

  Box     waferBox(wafer_dim_x/2, wafer_dim_z/2, m_slab.sensitive_thickness/2);
  Volume  waferVol(prefix+"_wafer",waferBox,m_slab.sensitive_mat);
  waferVol.setVisAttributes(m_slab.wafer_vis);
  waferVol.setSensitiveDetector(sd);
  waferVol.setLimitSet(m_limits);

  //::printf("%s> ...slab dim: %.4f x %.4f x %.4f  Grnd:%.4f %.4f %.4f Sensitive:%.4f %.4f %.4f \n",
  //  name.c_str(), dim.X()/2,dim.Z()/2,dim.Y()/2,dim.X()/2,dim.Z()/2,m_slab.ground_thickness/2,
  //  dim.X()/2,dim.Z()/2,m_slab.sensitive_thickness/2);

  // As the same method builds both barrel and end cap
  // slabs, place the wafers along the biggest axe
  if (barrel)     {
    // Barrel
    double real_wafer_size_x = wafer_dim_x + 2 * m_guard_ring_size;      
    int    num_wafer_x       = int(floor(dim.X() / real_wafer_size_x));
    double wafer_pos_x       = -dim.X()/2 + m_guard_ring_size +	wafer_dim_x /2 ;
    // ::printf("%s> ...Building slab for barrel: wafer dim: %.4f x %.4f real:%.4f [%d] pos:%4.f\n",
    //	     name.c_str(), wafer_dim_x, wafer_dim_z, real_wafer_size_x, num_wafer_x, wafer_pos_x);
    for (int iwaf = 1; iwaf < num_wafer_x + 1; iwaf++)      {
      double wafer_pos_z = -dim.Z()/2 + m_guard_ring_size + wafer_dim_z/2;
      for (int n_wafer_z = 1; n_wafer_z < 3; n_wafer_z++)	    {
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
    double resting_dim_x = dim.X() - (wafer_dim_x + 2 * m_guard_ring_size) * num_wafer_x;      
    if(resting_dim_x > (cell_dim_x + 2 * m_guard_ring_size))	{
      int N_cells_x_remaining =	int(floor((resting_dim_x - 2 * m_guard_ring_size)/cell_dim_x));
      wafer_dim_x = N_cells_x_remaining * cell_dim_x;
      Box    waf_box(wafer_dim_x/2, wafer_dim_z/2, m_slab.sensitive_thickness/2);
      Volume waf_vol(prefix+"_wafer_rest",waf_box,m_slab.sensitive_mat);
      waf_vol.setVisAttributes(m_slab.wafer_vis);
      waf_vol.setSensitiveDetector(sd);
      waf_vol.setLimitSet(m_limits);

      real_wafer_size_x  =  wafer_dim_x + 2 * m_guard_ring_size;
      wafer_pos_x        = -dim.X()/2 + num_wafer_x * real_wafer_size_x + real_wafer_size_x/2;
      double wafer_pos_z = -dim.Z()/2 + m_guard_ring_size + wafer_dim_z/2;
      for (int iwaf_z = 1; iwaf_z < 3; ++iwaf_z)  {
	PlacedVolume pv = siVol.placeVolume(waf_vol,Position(wafer_pos_x,wafer_pos_z,0));
	pv.addPhysVolID("wafer",num_wafer_x*1000 + iwaf_z);
	wafer_pos_z += wafer_dim_z + 2 * m_guard_ring_size;
      }
    }
  }
  else    {    // End caps
    double real_wafer_size_x =  wafer_dim_z + 2 * m_guard_ring_size;
    double wafer_pos_x       = -dim.Z()/2 + m_guard_ring_size +	wafer_dim_z /2;
    int    num_wafer_x       =  int(floor(dim.Z()/real_wafer_size_x));      
    //::printf("%s> ...Building slab for endcap: wafer dim: %.4f x %.4f real:%.4f [%d] pos:%4.f\n",
    //	       name.c_str(), wafer_dim_x, wafer_dim_z, real_wafer_size_x, num_wafer_x, wafer_pos_x);
    for (int iwaf_x=1; iwaf_x < num_wafer_x + 1; iwaf_x++)	{
      double wafer_pos_z = -dim.X()/2 + m_guard_ring_size + wafer_dim_x /2;
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
    double resting_dim_x = dim.Z() - (wafer_dim_z + 2 * m_guard_ring_size) * num_wafer_x;
    if(resting_dim_x > (cell_dim_z + 2 * m_guard_ring_size))   {
      int N_cells_x_remaining = int(std::floor((resting_dim_x - 2 * m_guard_ring_size)/cell_dim_z));
      wafer_dim_x = N_cells_x_remaining * cell_dim_z;
      Box    waf_box(wafer_dim_z/2, wafer_dim_x/2, m_slab.sensitive_thickness/2);
      Volume waf_vol(prefix+"_wafer_rest", waf_box, m_slab.sensitive_mat);
      waf_vol.setVisAttributes(m_slab.sensitive_vis);
      waf_vol.setSensitiveDetector(sd);
      waf_vol.setLimitSet(m_limits);
      wafer_pos_x        = -dim.Z()/2 + num_wafer_x * real_wafer_size_x + (wafer_dim_x + 2 * m_guard_ring_size)/2;
      real_wafer_size_x  =  wafer_dim_x + 2 * m_guard_ring_size;
      double wafer_pos_z = -dim.X()/2 + m_guard_ring_size + wafer_dim_z /2;
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
  Box    pcbShieldBox(dim.X()/2,dim.Z()/2,m_shield.thickness/2);
  Volume pcbShieldVol(prefix+"_pcbshield",pcbShieldBox,m_shield.material);
  pcbShieldVol.setVisAttributes(m_shield.vis);
  slabVol.placeVolume(pcbShieldVol,Position(0,0,y_slab_floor+m_shield.thickness/2));
  return slabVol;
}

/// Build Endcap Standard Module 
Volume SEcal03::buildEndcap(DetElement det,bool Zminus, const Endcap& endcap, 
			    const Solid& siSolid, const Volume& siVol, const Rotation& rot)
{
  // While waiting for more geometric details, build a simple Endcap using 
  // a fiber polyhedra and substract the center box
  string           nam  = det.name();
  double           disp = Zminus ? -m_centerTubDisplacement : m_centerTubDisplacement;
  Position         displacement(disp,0,0);
  PolyhedraRegular hedra(8,M_PI/8,0,endcap.rmax,m_endcap.thickness);
  SubtractionSolid solid(hedra,m_center_tube,displacement);
  Volume           endcap_vol(nam+"_endcap", solid,m_shield.material);
  endcap_vol.setVisAttributes(m_endcap.vis);

  //----------------------------------------------------
  // Radiator plates in the EndCap structure also as polyhedra, 
  // and radiator plates in the slab of EndCap Rings as box less Tub
  //-------------------------------------------------------
  Volume vol_ringL1, vol_ringL2, vol_ringL3;
  Volume vol_radL1,  vol_radL2,  vol_radL3;

  double  r_inner  = m_lateral_face_thickness;
  double  r_outer  = endcap.rmax - m_lateral_face_thickness;
  double  box_dim  = (m_endcap_center_box_size - m_lateral_face_thickness)/ 2.;
  VisAttr ring_vis = lcdd->visAttributes("EcalRingVis");
  
  if(m_layers[0].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, M_PI/8, r_inner, r_outer, m_layers[0].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, displacement);
    vol_radL1 = Volume(nam+"_ECL1_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL1.setVisAttributes(m_radiatorVis);
    // plate for slab in ring
    Box              box_ring(box_dim,box_dim, m_layers[0].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,displacement,rot);
    vol_ringL1 = Volume(nam+"_ECL1_ring",sol_ring,m_layers[0].rad_mat);
    vol_ringL1.setVisAttributes(ring_vis);
  }
  if(m_layers[1].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, M_PI/8, r_inner, r_outer, m_layers[1].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, displacement, rot);
    vol_radL2 = Volume(nam+"_ECL2_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL2.setVisAttributes(m_radiatorVis);
    // plate for slab in ring
    Box              box_ring(box_dim,box_dim, m_layers[1].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,displacement, rot);
    vol_ringL2 = Volume(nam+"_ECL2_ring",sol_ring,m_layers[0].rad_mat);
    vol_ringL2.setVisAttributes(ring_vis);
  }
  if(m_layers[2].nLayer > 0 )    {
    PolyhedraRegular hedra_rad(8, M_PI/8, r_inner, r_outer, m_layers[2].thickness);
    SubtractionSolid sol_rad(hedra_rad, m_center_tube, displacement, rot);
    vol_radL3 = Volume(nam+"_ECL3_radiator",sol_rad, m_layers[0].rad_mat);
    vol_radL3.setVisAttributes(m_radiatorVis);
    // plate for slab in ring
    Box box_ring(box_dim,box_dim, m_layers[2].thickness/2);
    SubtractionSolid sol_ring(box_ring,m_center_tube,displacement, rot);
    vol_ringL3 = Volume(nam+"_ECL3_ring",sol_ring,m_layers[2].rad_mat);
    vol_ringL3.setVisAttributes(ring_vis);
  }

  //-------------------------------------------------------
  // Radiator and towers placements inside the Endcap module
  //-------------------------------------------------------

  // We count the layers starting from IP and from 1, so odd layers should be 
  // inside slabs and even ones on the structure.
  double z_floor = -m_endcap.thickness/2 + m_front_thickness + N_FIBERS_ALVOULUS * m_fiber_thickness;
  //
  // ATTENTION, TWO LAYERS PER LOOP AS THERE IS ONE INSIDE THE ALVEOLUS.
  //
  double     y_slab_si_offset = /*-slab_dim_y/2*/ -m_slab.total_thickness/2 + \
             m_slab.ground_thickness + m_slab.sensitive_thickness/2;
  int        num_layers = m_layers[0].nLayer + m_layers[1].nLayer + m_layers[2].nLayer;
  Rotation   rotOdd(M_PI,0,M_PI);
  Rotation   rotEven(M_PI,0,0);
  Volume     vol_rad, vol_ring;
  vector<Volume>* tower_vols = 0;
  PlacedVolume pv;
  //num_layers = 2;
  for(int layer_id = 1; layer_id <= num_layers; layer_id+=2)   {
    // place the tower layer for the four modules
    double rad_thickness = m_layers[0].thickness;
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
    // Build EC Alveolus
    double AlveolusThickness = 2 * m_slab.total_thickness + rad_thickness + 2 * m_fiber_thickness;
    for(size_t istave=1, num_staves=4; istave <= num_staves; ++istave)  {
      double   angle_module = M_PI/2 * (istave - 1);
      //if(layer_id==1)
      // theEndCapSD->SetStaveRotationMatrix(istave,angle_module);
      //if ( !(istave==1 || istave==3) ) continue;
      //if ( istave>3 ) continue;

      for(size_t itow=0, ntower=EC_TowerSlabs.size(); itow < ntower; itow++)	{
	Volume   tower_slab = EC_TowerSlabs[itow];
	Volume   tower_vol  = (*tower_vols)[itow];
	Position tower_pos  = EC_TowerXYCenters[itow];
	Rotation rot_o=rotOdd*RotationZ(angle_module), rot_e=rotEven*RotationZ(angle_module);

	if ( (istave%2) == 1 ) tower_pos.SetY(-tower_pos.Y());

	// Place the first slab
	Position pos = tower_pos;
	pos.SetZ(z_floor + m_slab.total_thickness/2);
	pos = RotateZ(pos,angle_module);
	pv  = endcap_vol.placeVolume(tower_slab,rot_o,pos);
	pv.addPhysVolID("Layer",istave*100000 + (itow+1) * 1000 + layer_id);
	if (istave == 1 && itow == 0 && !Zminus) {
	  //Box      tower_sol  = tower_vol.solid();
	  //  theEndCapSD->AddLayer(layer_id,-pos.x-tower_sol.x(),pos.z-Si_Slab_Y_offset,pos.y-tower_sol.y());
	}
	// reinitialise pos as it's now rotated for the slab and place radiator inside alveolus
	pos = tower_pos;
	pos.SetZ(z_floor + m_slab.total_thickness/2);
	// update pos to take in account slab + fiber dim
	pos.SetZ(pos.Z() + m_slab.total_thickness/2 + m_fiber_thickness + rad_thickness/2);
	pos = RotateZ(pos,angle_module);
	pv = endcap_vol.placeVolume(tower_vol,rot_e,pos);

	// Place the second slab
	pos.SetZ(pos.Z()+rad_thickness/2+m_fiber_thickness+m_slab.total_thickness/2);
	rot_o = (rotOdd*RotationX(M_PI))*RotationZ(angle_module);
	if ( istave%2 == 1 ) rot_o = rot_o*RotationX(M_PI);
	pv = endcap_vol.placeVolume(tower_slab,rot_o,pos);
	pv.addPhysVolID("Layer",istave*100000 + (itow+1) * 1000 + layer_id+1);
	if (istave == 1 && itow == 0 && !Zminus) {
	  //Box      tower_sol  = tower_vol.solid();
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
    endcap_vol.placeVolume(vol_rad,Position(0,0,z_floor+rad_thickness/2));
    // update the z_floor
    z_floor += rad_thickness + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness;
  }
  return endcap_vol;
}

///  Barrel Standard Module
pair<DetElement,Volume> SEcal03::buildBarrelStave(const Barrel& barrel) {
  DetElement staveDet(name+"_barrel_stave",0);
  Trapezoid  staveTrd(barrel.bottom/2, barrel.top/2, barrel.dim_z/2, barrel.dim_z/2, barrel.thickness/2);
  Volume     staveVol(name+"_barrer_stave",staveTrd,m_barrel.material);
  staveVol.setVisAttributes(m_barrel.vis);

  // We count the layers starting from IP and from 1, so odd layers should be inside slabs and
  // even ones on the structure.
  // The structure W layers are here big plans, as the gap between each W plate is too small 
  // to create problems. The even W layers are part of H structure placed inside the alveolus.
  double y_floor = m_front_thickness + N_FIBERS_ALVOULUS * m_fiber_thickness;
  double z_pos;
  PlacedVolume pv;

  for(int layer_id = 1; layer_id < m_numLayer+1; layer_id+=2) {// ATTENTION, TWO LAYERS PER LOOP
    string l_nam = name+_toString(layer_id,"_barrel_stave_layer%d");
    DetElement layerDet(staveDet,_toString(layer_id,"layer%d"),layer_id);
    // build and place the several Alveolus with the slabs and the radiator layer inside.
    double rad_thick = radiatorThickness(layer_id);
    double alveolus_dim_y = 2 * m_slab.total_thickness + rad_thick + 2 * m_fiber_thickness;

    //------ BuildBarrelAlveolus ------
    double alveolus_dim_x = barrel.bottom - 2*(y_floor+alveolus_dim_y)*std::tan(M_PI/8);
    double x_off, y_off, y_fl;
    // To simplify we place each slab and the radiator layer directly into the fiber module.
    //
    // Build a slab:
    Volume slabVol = buildSlab(l_nam,true,
			       Dimension(alveolus_dim_x,m_slab.total_thickness,m_alveolus.Z()),
			       m_barrel.sensDet);
    // Place the Slab and radiator inside the H, here directly into the module fiber as the
    // H structure is also built in fiber.
    double z_tower_center = -barrel.dim_z /2 + m_lateral_face_thickness 
      + m_fiber_thickness * N_FIBERS_ALVOULUS + m_slab.shield_thickness
      + m_slab.h_fiber_thickness + m_alveolus.Z() /2;  

    Dimension radDim1(alveolus_dim_x,rad_thick,m_alveolus.Z());
    Volume radVol1 = buildRadiator(l_nam+"_radiator1",radDim1,barrel.radiatorMaterial);
    for (int itow = m_barrel.numTowers; itow > 0; --itow )    {
      y_fl  = y_floor;
      x_off = 0; // to be calculed
      y_off = y_fl - barrel.thickness/2 + m_slab.total_thickness/2;
      // Place First Slab
      pv = staveVol.placeVolume(slabVol,Position(x_off,z_tower_center,y_off),Rotation(0,M_PI,0));
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
      y_off  = -barrel.thickness/2 + y_fl + rad_thick/2;
      pv     =  staveVol.placeVolume(radVol1,Position(0,z_tower_center,y_off));
      pv.addPhysVolID("tower",itow * 1000 + layer_id);

      y_fl  +=  rad_thick + m_fiber_thickness;
      y_off  = -barrel.thickness/2 + y_fl + m_slab.total_thickness/2;
      // Second Slab: starts from bottom to up
      pv = staveVol.placeVolume(radVol1,Position(0,z_tower_center,y_off));
      pv.addPhysVolID("tower",itow * 1000 + layer_id + 1);
#if 0
      if (itow == Ecal_barrel_number_of_towers)  	{
	theBarrelSD->AddLayer(layer_id + 1,
			      x_off - ((G4Box *)SlabLog->GetSolid())->GetXHalfLength(),
			      Ecal_inner_radius + barrel.thickness/2 + y_off + Si_Slab_Y_offset,
			      z_tower_center - ((G4Box *)SlabLog->GetSolid())->GetYHalfLength());
      }
#endif
      z_tower_center += m_alveolus.Z() + 
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
    double radiator_dim_z = barrel.dim_z - 2.*m_lateral_face_thickness - 2*N_FIBERS_W_STRUCTURE * m_fiber_thickness;
    
    Dimension radDim2(radiator_dim_x,rad_thick,radiator_dim_z);
    Volume radVol2 = buildRadiator(l_nam+"_radiator2",radDim2,barrel.radiatorMaterial);
    pv = staveVol.placeVolume(radVol2,Position(0,0,-barrel.thickness/2+y_floor+rad_thick/2));

    // update the y_floor
    y_floor += (rad_thick + (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE) * m_fiber_thickness);
  }
  return make_pair(staveDet,staveVol);
}

static Ref_t create_detector(LCDD& lcdd, xml_det_t x_det, Ref_t)  {
  DetElement e = DetElement::create<SEcal03>(x_det.nameStr(),x_det.typeStr(),x_det.id());
  return DetElement::createObject<SEcal03>(x_det.nameStr(),x_det.typeStr(),x_det.id())->construct(lcdd,x_det);
}
DECLARE_SUBDETECTOR(Tesla_SEcal03,create_detector);
