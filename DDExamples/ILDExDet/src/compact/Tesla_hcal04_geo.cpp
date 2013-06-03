// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Detector.h"
#include "TGeoTube.h"

namespace DD4hep {
  namespace Geometry {
    struct Hcal04Data : public DetElement::Object {
      enum {HCALBARREL = 1, HCALENDCAPPLUS=2, HCALENDCAPMINUS=3 };
      enum {SIDE_PLUS=0, SIDE_MINUS=1 };

      struct Layer {
	int       id;
	union Vals {
	  struct BL { double y_offset, dim_x, dim_z; } layer;
	  struct EL { double z_offset, dim_z;        } end_layer;
	  struct EC { double z_offset, dummy;        } endcap_layer;
	} values;
      };
      struct Layers : public std::vector<Layer>    {      };
      struct Stave {
	int    id;
	double phi;
	double inner_r;
	double z_offset;
      };
      struct Staves : public std::vector<Stave>    {      };
      struct Module {
	int    id;
	int    type;
	double z_offset;
      };
      struct Modules : public std::vector<Module>  {      };

      struct RPC {
	double g10_thickness;
	double spacer_thickness;
	double spacer_gap;
	struct { double thickness; Material material; VisAttr vis; } radiator, glass, gas;
	struct { double thickness, gap; } spacer;
      } m_rpc;

      struct Barrel : public DetElement  {
	double       bottom_dim_x;
	double       middle_dim_x;
	double       top_dim_x;
	double       y1_for_x, y2_for_x;
	double       y1_for_z, y2_for_z, y3_for_z, top_end_dim_z;

        double       inner_r;
	double       module_dim_z;
	double       cell_dim_x, cell_dim_z;
	double       chamber_thickness;
	double       chamber_dim_z;
	int          numLayer;
	Layers       layers, end_layers;
	Staves       staves;
	Modules      modules;
	SensitiveDetector sensRegular, sensEndModule;
	/// Helper function to allow assignment
	DetElement& operator=(const DetElement& d)  { return this->DetElement::operator=(d); }
      } m_barrel;

      struct Endcap : public DetElement  {
	Layers layers;
	double rmin, rmax, dz;
	double chamber_thickness;
	SensitiveDetector sensDet;
	struct EndcapSide : public DetElement { 
	  int id; 
	  Position position; 
	  DetElement& operator=(const DetElement& d) 
	  { return this->DetElement::operator=(d); }
	} side[2];
      } m_endcap;

      VisAttr     m_scintVis;
      VisAttr     m_moduleVis;
      VisAttr     m_endModuleVis;
      VisAttr     m_endcapModuleVis;
      VisAttr     m_chamberVis;
      Material    m_scintMaterial;
      Material    m_radiatorMat;
      LimitSet    m_limits;
      double      m_fiberGap;
      std::string m_model;
      DetElement  self;
      LCDD*       lcdd;
      std::string name;
    };
    
    struct Hcal04 : public Hcal04Data  {
      /// Default constructor
      Hcal04() : Hcal04Data()     {  self = Ref_t(this); }
      /// Detector construction function
      DetElement construct(LCDD& lcdd, xml_det_t e);
      /// Build and place Barrel Regular Modules
      void buildBarrelRegularModules(Volume assembly);
      /// Build and place Barrel End Modules
      void buildBarrelEndModules(Volume assembly);
      // Build and place EndCap Modules
      void buildEndcaps(Volume assembly);
      /// Build Box with RPC1 chamber
      Volume buildRPC1Box(const std::string& nam, Box box, const Layer& layer, SensitiveDetector& sensdet);
    };
  }
}

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static double s_geo_tolerance = 1e-10;

/// Detector construction function
DetElement Hcal04::construct(LCDD& detector_description, xml_det_t x_det)  {
  lcdd     = &detector_description;
  name     = x_det.nameStr();
  self->SetName(name.c_str());
  self->SetTitle(x_det.typeStr().c_str());

  xml_comp_t  x_barrel        = x_det.child(_U(barrel));
  xml_comp_t  x_endcap        = x_det.child(_U(endcap));
  xml_comp_t  x_param         = x_det.child(_U(param));
  xml_comp_t  x_rpc           = x_det.child(_U(rpc));
  xml_comp_t  x_scintillator  = x_det.child(_U(scintillator));
  xml_comp_t  x_module        = x_barrel.child(_U(module));
  xml_comp_t  x_end_module    = x_barrel.child(_U(end_module));
  xml_comp_t  x_endcap_module = x_endcap.child(_U(module));

  Assembly assembly(name);
  Volume motherVol  = lcdd->pickMotherVolume(Ref_t(this));

  // Visualisation attributes
  VisAttr vis       = lcdd->visAttributes(x_det.visStr());
  m_moduleVis       = lcdd->visAttributes(x_module.visStr());
  m_endModuleVis    = lcdd->visAttributes(x_end_module.visStr());
  m_endcapModuleVis = lcdd->visAttributes(x_endcap_module.visStr());
  m_chamberVis      = lcdd->visAttributes(x_param.attr<string>(_Unicode(chamber_vis)));
  m_scintVis        = lcdd->visAttributes(x_scintillator.visStr());
  m_scintMaterial   = lcdd->material(x_scintillator.materialStr());

  //--------- BarrelHcal Sensitive detector -----
  m_model           = x_param.attr<string>(_Unicode(sensitive_model));
  m_fiberGap        = x_param.attr<double>(_Unicode(fiber_gap));
  m_radiatorMat     = lcdd->material(x_param.attr<string>(_Unicode(radiator_material)));

  m_barrel.chamber_thickness = x_barrel.attr<double>(_Unicode(chamber_thickness));
  m_barrel.numLayer   = x_barrel.attr<int>(_Unicode(num_layer));
  m_barrel.inner_r    = x_barrel.inner_r();

  // Paremeters for regular barrel modules
  m_barrel.chamber_dim_z = x_module.attr<double>(_Unicode(chamber_dim_z));
  m_barrel.module_dim_z  = x_module.attr<double>(_Unicode(dim_z));
  m_barrel.bottom_dim_x  = x_module.attr<double>(_Unicode(bottom_dim_x));
  m_barrel.middle_dim_x  = x_module.attr<double>(_Unicode(middle_dim_x));
  m_barrel.top_dim_x     = x_module.attr<double>(_Unicode(top_dim_x));
  m_barrel.y1_for_x      = x_module.attr<double>(_Unicode(y_dim1_for_x));
  m_barrel.y2_for_x      = x_module.attr<double>(_Unicode(y_dim2_for_x));
  m_barrel.cell_dim_x    = x_module.attr<double>(_Unicode(cell_dim_x));
  m_barrel.cell_dim_z    = x_module.attr<double>(_Unicode(cell_dim_z));

  // Parameters for barrel end-module
  m_barrel.y1_for_z      = x_end_module.attr<double>(_Unicode(y_dim1_for_z));
  m_barrel.y2_for_z      = x_end_module.attr<double>(_Unicode(y_dim2_for_z));
  m_barrel.y3_for_z      = x_end_module.attr<double>(_Unicode(y_dim3_for_z));
  m_barrel.top_end_dim_z = x_end_module.attr<double>(_Unicode(top_end_dim_z));

  // if RPC1 read the RPC parameters
  if ( m_model == "RPC1" )   {
    xml_comp_t x_rad       = x_rpc.child(_U(radiator));
    xml_comp_t x_gas       = x_rpc.child(_U(gas));
    xml_comp_t x_glass     = x_rpc.child(_U(glass));
    xml_comp_t x_spacer    = x_rpc.child(_U(spacer));
    m_rpc.spacer.thickness   = x_spacer.thickness();
    m_rpc.spacer.gap         = x_spacer.gap();
    m_rpc.radiator.thickness = x_rad.thickness();
    m_rpc.radiator.material  = lcdd->material(x_rad.materialStr());
    m_rpc.gas.thickness      = x_gas.thickness();
    m_rpc.gas.material       = lcdd->material(x_gas.materialStr());
    m_rpc.gas.vis            = lcdd->visAttributes(x_gas.visStr());
    m_rpc.glass.thickness    = x_glass.thickness();
    m_rpc.glass.material     = lcdd->material(x_glass.materialStr());
    m_rpc.glass.vis          = lcdd->visAttributes(x_glass.visStr());
  }
  { // Read the barrel layers
    Stave stave;
    Layer layer;
    Module module;
    m_barrel.layers.clear();
    for(xml_coll_t c(x_barrel.child(_U(layers)),_U(layer)); c; ++c) {
      xml_comp_t l(c);
      layer.id = l.id();
      layer.values.layer.dim_x    = l.dim_x();
      layer.values.layer.y_offset = l.y_offset();
      m_barrel.layers.push_back(layer);
    }
    assert(m_barrel.numLayer != m_barrel.layers.size());
    // Read the barrel end-layers
    m_barrel.end_layers.clear();
    for(xml_coll_t c(x_barrel.child(_Unicode(end_layers)),_Unicode(end_layer)); c; ++c) {
      xml_comp_t l(c);
      layer.id = l.id();
      layer.values.end_layer.dim_z    = l.dim_z();
      layer.values.end_layer.z_offset = l.z_offset();
      m_barrel.end_layers.push_back(layer);
    }
    assert(m_barrel.numLayer != m_barrel.end_layers.size());
    m_barrel.staves.clear();
    for(xml_coll_t c(x_barrel.child(_U(staves)),_U(stave)); c; ++c) {
      xml_comp_t s(c);
      stave.id = s.id();
      stave.phi = s.phi();
      //stave.inner_r = s.inner_r();
      //stave.z_offset = s.z_offset();
      m_barrel.staves.push_back(stave);
    }
    m_barrel.modules.clear();
    for(xml_coll_t c(x_barrel.child(_U(modules)),_U(module)); c; ++c) {
      xml_comp_t m(c);
      module.id = m.id();
      module.type = m.attr<int>(_U(type));
      module.z_offset = m.z_offset();
      m_barrel.modules.push_back(module);
    }
  }
  { // Read the endcap structure
    int cnt=0;
    Layer layer;
    for(xml_coll_t c(x_endcap.child(_Unicode(positions)),_U(position)); c; ++c, ++cnt) {
      xml_comp_t ec(c);
      m_endcap.side[cnt].id = ec.id();
      m_endcap.side[cnt].position = Position(ec.x(),ec.y(),ec.z());
    }
    m_endcap.rmin = x_endcap_module.rmin();
    m_endcap.rmax = x_endcap_module.rmax();
    m_endcap.dz   = x_endcap_module.dz();
    m_endcap.chamber_thickness = m_barrel.chamber_thickness;

    cout << name << ": End cap: rmin=" << m_endcap.rmin << " rmax=" << m_endcap.rmax << " dz=" << m_endcap.dz << endl;
    // Read the endcap layers
    m_endcap.layers.clear();
    for(xml_coll_t c(x_endcap.child(_U(layers)),_U(layer)); c; ++c) {
      xml_comp_t x_layer(c);
      layer.id = x_layer.id();
      layer.values.endcap_layer.dummy = 0;
      layer.values.endcap_layer.z_offset = x_layer.z_offset();
      m_endcap.layers.push_back(layer);
    }
  } 

#if 0
  // The cell boundaries does not really exist as G4 volumes. So,
  // to avoid long steps over running  several cells, the 
  // theMaxStepAllowed inside the sensitive material is the
  // pad smaller x or z dimension.
  theMaxStepAllowed= std::min(m_barrel.cells.x,m_barrel.cells.z);
  //  Hcal  barrel regular modules
  theBarrilRegSD =     new SD(m_barrel.cells.x,m_barrel.cells.z,chamber_tickness,HCALBARREL,"HcalBarrelReg");
  // Hcal  barrel end modules
  theBarrilEndSD = new SD(m_barrel.cells.x,m_barrel.cells.z,chamber_tickness,HCALBARREL,"HcalBarrelEnd");
  // Hcal  endcap modules
  theENDCAPEndSD = new HECSD(m_barrel.cells.x,m_barrel.cells.z,chamber_tickness,HCALENDCAPMINUS,"HcalEndCaps");
#endif

  m_barrel = DetElement(self,"barrel",HCALBARREL);

  // Setup the sensitive detectors for barrel, endcap+ and endcap-
  SensitiveDetector& sd = m_barrel.sensRegular = SensitiveDetector("HcalBarrelRegular","calorimeter");
  Readout ro = lcdd->readout(x_module.readoutStr());
  sd.setHitsCollection(ro.name());
  sd.setReadout(ro);
  lcdd->addSensitiveDetector(sd);

  sd = m_barrel.sensEndModule = SensitiveDetector("HcalBarrelEndModule","calorimeter");
  ro = lcdd->readout(x_end_module.readoutStr());
  sd.setHitsCollection(ro.name());
  sd.setReadout(ro);
  lcdd->addSensitiveDetector(sd);
  
  sd = m_endcap.sensDet = SensitiveDetector("HcalEndcap","calorimeter");
  ro = lcdd->readout(x_endcap_module.readoutStr());
  sd.setHitsCollection(ro.name());
  sd.setReadout(ro);
  lcdd->addSensitiveDetector(sd);

  // User limits for this sub detector
  m_limits = lcdd->limitSet(x_det.limitsStr());
  
  // Barrel
  buildBarrelRegularModules(assembly);
  buildBarrelEndModules(assembly);
  
  // EndCap Modules
  buildEndcaps(assembly);

  PlacedVolume pv = motherVol.placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  m_barrel.setPlacement(pv);
  self.setPlacement(pv);
  return self;
}

/// Build and place Barrel Regular Modules
void Hcal04::buildBarrelRegularModules(Volume assembly)   {
  DetElement module("module",0);

  // Attention: on bâtit le module dans la verticale à cause du G4Trd et on le tourne avant de le positioner
  Trapezoid trdBottom(m_barrel.bottom_dim_x/2, m_barrel.middle_dim_x/2,m_barrel.module_dim_z/2, m_barrel.module_dim_z/2, m_barrel.y1_for_x/2);
  Trapezoid trdTop   (m_barrel.middle_dim_x/2, m_barrel.top_dim_x/2,   m_barrel.module_dim_z/2, m_barrel.module_dim_z/2, m_barrel.y2_for_x/2);

  UnionSolid modSolid (trdBottom,trdTop,Position(0,0,(m_barrel.y1_for_x+m_barrel.y2_for_x)/2));
  Volume     modVolume("regModule",modSolid,m_radiatorMat);

  modVolume.setVisAttributes(m_moduleVis);
  double chambers_y_off_correction = m_barrel.y2_for_x/2;
  double bottomDimY = m_barrel.y1_for_x/2;

  PlacedVolume pv;
  for(Layers::const_iterator i=m_barrel.layers.begin(); i != m_barrel.layers.end(); ++i)  {
    const Layer& lay = *i;
    DetElement det_layer(module,_toString(lay.id,"layer%d"),lay.id);
    double dim_x = lay.values.layer.dim_x/2;
    double thickness = m_barrel.chamber_thickness/2;
    
    Box  chamberBox(dim_x,m_barrel.chamber_dim_z/2,thickness);
    Volume chamberVol;
    if ( m_model == "scintillator")      {	
      //fg: introduce (empty) fiber gap - should be filled with fibres and cables
      Box    scintBox(dim_x,m_barrel.chamber_dim_z/2,thickness - m_fiberGap/2);
      Volume scintVol(_toString(lay.id,"scint%d"),scintBox,m_scintMaterial);
      scintVol.setSensitiveDetector(m_barrel.sensRegular);
      scintVol.setVisAttributes(m_scintVis);
      scintVol.setLimitSet(m_limits);
      chamberVol = Volume(_toString(lay.id,"chamber%d"),chamberBox,lcdd->air());
      pv = chamberVol.placeVolume(scintVol,Position(0,0,-m_fiberGap/2));
    }
    else if (m_model == "RPC1")	{
      chamberVol = buildRPC1Box(_toString(lay.id,"layer_%d"),chamberBox,lay,m_barrel.sensRegular);
    }
    else    {
      throw runtime_error("Unknown sensitive model:"+m_model+" for detector hcal04");
    }
    chamberVol.setVisAttributes(m_chamberVis);
    pv = modVolume.placeVolume(chamberVol,Position(0,0,lay.values.layer.y_offset+chambers_y_off_correction));
    pv.addPhysVolID("layer",lay.id);
    det_layer.setPlacement(pv);
  }

  DetElement det_mod;
  double Y = m_barrel.inner_r+m_barrel.y1_for_x/2;
  for(Staves::const_iterator i=m_barrel.staves.begin(); i!=m_barrel.staves.end(); ++i) {
    const Stave& s = *i;
    Rotation rot(s.phi,-M_PI/2,0);
    DetElement stave_det(m_barrel,_toString(s.id,"reg_stave%d"),s.id*10);
    for(Modules::const_iterator j=m_barrel.modules.begin(); j!=m_barrel.modules.end(); ++j) {
      const Module& m = *j;
      if ( m.type == 1 )   {
	Position pos(0,Y,m.z_offset);
	if ( !det_mod.isValid() ) {
	  module._data().id = m.id;
	  module->SetName(_toString(m.id,"module%d").c_str());
	  det_mod = module;
	}
	else {
	  det_mod = module.clone(_toString(m.id,"module%d"),m.id);
	}
	pv = assembly.placeVolume(modVolume,RotateZ(pos,s.phi),rot);
	pv.addPhysVolID("module",HCALBARREL*100+s.id*10+m.id);
	stave_det.add(det_mod);
	det_mod.setPlacement(pv);
      }
    }
  }
}

///  Barrel End Modules
void Hcal04::buildBarrelEndModules(Volume assembly)    {
  DetElement module(name+"_module",0);
  double YZ1H = m_barrel.y1_for_z/2;
  double YZ2H = m_barrel.y2_for_z/2;
  double YZ3H = m_barrel.y3_for_z/2;
  double TZ   = m_barrel.top_end_dim_z/2;
  double BHX  = m_barrel.bottom_dim_x/2;
  double MHX  = m_barrel.middle_dim_x/2;
  double THX  = m_barrel.top_dim_x/2;
  double YX1H = m_barrel.y1_for_x/2;
  double YX2H = m_barrel.y2_for_x/2;
  double DHZ  = m_barrel.module_dim_z/2;

  // Attention: on bâtit le module dans la verticale à cause des G4Trd et on le tourne avant de le positioner
  double MHXZ = BHX+(YZ1H+YZ2H)*(MHX-BHX)/YX1H;
  Trapezoid  base1(BHX,MHXZ,DHZ,DHZ,YZ1H+YZ2H);
  Trapezoid  base2(MHXZ,MHX,TZ,TZ,YX1H-(YZ1H+YZ2H));
  UnionSolid base(base1,base2,Position(0,TZ-DHZ,YX1H));
  Trapezoid  topTrd(MHX,THX,TZ,TZ,YX2H);
  UnionSolid trd(base,topTrd,Position(0,TZ-DHZ,2*YX1H-(YZ1H+YZ2H)+YX2H));

  double MHXZ1  = BHX+((YZ1H+YZ2H)-(TZ-DHZ))*(MHX-BHX)/YX1H;
  double pDX    = TZ-DHZ;
  double pDz    = std::sqrt(4*YZ2H*YZ2H+pDX*pDX)/2;
  double pTheta = M_PI/2 - std::atan(2*pDz/pDX);
  Trap trap(pDz,-pTheta,0,MHXZ1,s_geo_tolerance,s_geo_tolerance,0,MHXZ,pDX,pDX,0);
  UnionSolid ensemble(trd, trap,Position(0,DHZ+pDX-pDz*std::tan(pTheta),YZ1H+YZ2H-pDz),Rotation(0,0,M_PI/2));
  Volume endBarrelVol(name+"_endModule",ensemble,m_radiatorMat);
  endBarrelVol.setVisAttributes(m_endModuleVis);

  // Chambers in the Hcal Barrel 
  //------------------------------------------------------------------------------------------------------
  double chambers_y_off_correction = m_barrel.y3_for_z/2;
  PlacedVolume pv;
  
  double ydh = m_barrel.chamber_thickness/2;
  double zdh = m_barrel.chamber_dim_z/2;
  for(size_t i=0; i < m_barrel.layers.size(); ++i)  {
    int layer_id = m_barrel.layers[i].id;
    DetElement det_layer(module,_toString(layer_id,"layer%d"),layer_id);
    const Layer::Vals::BL& blay = m_barrel.layers[i].values.layer;
    const Layer::Vals::EL& elay = m_barrel.end_layers[i].values.end_layer;
    double xdh = blay.dim_x/2;
    Box chamberBox(xdh,zdh,ydh);
    Volume chamberVol;
    if( m_model == "scintillator" )	{
      //fg: introduce (empty) fiber gap - should be filled with fibres and cables
      // fiber gap can't be larger than total chamber
      assert( (ydh - m_fiberGap/2) > 0.);
      Box scintBox(xdh,zdh,ydh - m_fiberGap  / 2);
      Volume scintVol(name+_toString(layer_id,"_scint%d"),scintBox,m_scintMaterial);
      scintVol.setSensitiveDetector(m_barrel.sensEndModule);
      scintVol.setVisAttributes(m_scintVis);
      scintVol.setLimitSet(m_limits);
      chamberVol = Volume(name+_toString(layer_id,"_chamber%d"),chamberBox,lcdd->air());
      pv = chamberVol.placeVolume(scintVol,Position(0,0,-m_fiberGap/2));
    }
    else if (m_model == "RPC1")	{
      string nam = name+_toString(layer_id,"_layer_%d");
      chamberVol = buildRPC1Box(nam,chamberBox,m_barrel.end_layers[i],m_barrel.sensEndModule);
    }
    else    {
      throw runtime_error("Unknown sensitive model:"+m_model+" for detector hcal04");
    }
    chamberVol.setVisAttributes(m_chamberVis);
    pv = endBarrelVol.placeVolume(chamberVol,Position(0,elay.z_offset,blay.y_offset+chambers_y_off_correction));
    pv.addPhysVolID("layer",layer_id);
  }

  //------------------------------------------------------------------------------------------------------
  // Barrel End Module placements
  bool first = true;
  double Y = m_barrel.inner_r+YZ1H+YZ2H;
  int stave_inv [8] = {1,8,7,6,5,4,3,2};
  for(Staves::const_iterator i=m_barrel.staves.begin(); i!=m_barrel.staves.end(); ++i) {
    const Stave& s = *i;
    DetElement stave_det(m_barrel,_toString(s.id,"end_stave%d"),s.id*10);
    for(Modules::const_iterator j=m_barrel.modules.begin(); j!=m_barrel.modules.end(); ++j) {
      const Module& m = *j;
      if ( m.type == 2 )   {
	int stave_id = m.z_offset>0 ? s.id : stave_inv[s.id-1];
	Rotation rot(s.phi,-M_PI/2,(m.z_offset>0 ? M_PI : 0));
	Position pos(0,Y,m.z_offset);
	DetElement det_mod = first ? module : module.clone(_toString(m.id,"module%d"),m.id);
	if (first) {
	  first = false;
	  module._data().id = m.id;
	  module->SetName(_toString(m.id,"module%d").c_str());
	}
	pv = assembly.placeVolume(endBarrelVol,pos=RotateZ(pos,s.phi),rot);
	pv.addPhysVolID("module",HCALBARREL*100+stave_id*10+m.id);
	stave_det.add(det_mod);
	det_mod.setPlacement(pv);
	//theBarrilEndSD->SetStaveRotationMatrix(stave_id,s.phi);
	//theBarrilEndSD->SetModuleZOffset(m.id,m.z_offset);
      }
    }
  }
}

/// Build EndCap Modules
void Hcal04::buildEndcaps(Volume assembly) {
  int n_edges = 8;
  DetElement       sdet("side_plus",HCALENDCAPPLUS);
  PolyhedraRegular hedra(n_edges,m_endcap.rmin,m_endcap.rmax,m_endcap.dz*2);
  Volume           modVolume("endcap",hedra,m_radiatorMat);
  modVolume.setVisAttributes(m_endcapModuleVis);
  modVolume.setVisAttributes(m_endModuleVis);

  // build and place the chambers in the Hcal Endcaps
  PolyhedraRegular chamberSolid(n_edges,m_endcap.rmin,m_endcap.rmax,m_endcap.chamber_thickness);
  Volume chamberVol;
  if(m_model == "scintillator")    {
    //fg: introduce (empty) fiber gap - should be filled with fibres and cables
    double scintHalfWidth = m_endcap.chamber_thickness - m_fiberGap/2;
    double zplanes[2] = {-scintHalfWidth,scintHalfWidth};

    // fiber gap can't be larger than total chamber
    assert( scintHalfWidth > 0. ) ;
    chamberVol = Volume("chamber",chamberSolid,lcdd->air());
    PolyhedraRegular scintSolid(n_edges,m_endcap.rmin,m_endcap.rmax,zplanes);
    Volume           scintVol  ("scintillator",scintSolid,m_scintMaterial);
    scintVol.setVisAttributes(m_scintVis);
    // only scinitllator is sensitive
    //ScintLog->SetSensitiveDetector(theENDCAPEndSD);
    chamberVol.placeVolume(scintVol,Position(0,0,-m_fiberGap));
  }
  else if (m_model == "RPC1")      {
    chamberVol = Volume("rpc1",chamberSolid,m_rpc.radiator.material);
    PolyhedraRegular glassSolid(n_edges,m_endcap.rmin,m_endcap.rmax,m_rpc.glass.thickness);
    Volume           glassVol("glass",glassSolid,m_rpc.glass.material);
    glassVol.setVisAttributes(m_rpc.glass.vis);

    // build the gas gap
    PolyhedraRegular gasSolid(n_edges,m_endcap.rmin,m_endcap.rmax,m_rpc.gas.thickness);
    Volume           gasVol("gas",gasSolid,m_rpc.gas.material);
    gasVol.setVisAttributes(m_rpc.gas.vis);
    gasVol.setSensitiveDetector(m_endcap.sensDet);
    gasVol.setLimitSet(m_limits);

    // placing the all. 
    // Z offset starts pointing to the chamber border.
    double z_offset = m_endcap.dz;
    // first glass after the g10_thickness
    z_offset += m_rpc.radiator.thickness + m_rpc.glass.thickness/2.;
    chamberVol.placeVolume(glassVol,Position(0,0,z_offset));
    // set Z offset to the next first glass border
    z_offset += m_rpc.glass.thickness/2;      
    // gas gap placing 
    z_offset += m_rpc.gas.thickness/2;
    chamberVol.placeVolume(gasVol,Position(0,0,z_offset));
    // set ZOffset to the next gas gap border
    z_offset += m_rpc.gas.thickness/2;
    // second glass, after the g10_thickness, the first glass and the gas gap.
    z_offset += m_rpc.glass.thickness/2;
    chamberVol.placeVolume(glassVol,Position(0,0,z_offset));
  }
  else  {
    throw runtime_error("Invalid sensitive model in the dababase for "+name+"!");
  }
  PlacedVolume pv;
  chamberVol.setVisAttributes(m_chamberVis);
  // standard endcap chamber placements
  for(Layers::const_iterator i=m_endcap.layers.begin(); i != m_endcap.layers.end(); ++i)  {
    const Layer& layer = *i;
    DetElement det_layer(sdet,_toString(layer.id,"layer%d"),layer.id);
    pv = modVolume.placeVolume(chamberVol,Position(0,0,layer.values.endcap_layer.z_offset));
    pv.addPhysVolID("layer",layer.id);
    det_layer.setPlacement(pv);
  }

  // Now place the endcaps
  pv = assembly.placeVolume(modVolume,m_endcap.side[SIDE_PLUS].position,Rotation(M_PI/8,0,0));
  pv.addPhysVolID("module",HCALENDCAPPLUS*100+16);
  sdet.setPlacement(pv);
  self.add(sdet);
  m_endcap.side[SIDE_PLUS] = sdet;

  pv = assembly.placeVolume(modVolume,m_endcap.side[SIDE_MINUS].position,Rotation(M_PI/8,M_PI,M_PI));
  pv.addPhysVolID("module",HCALENDCAPMINUS*100+16);
  sdet = sdet.clone("endcap_minus",HCALENDCAPMINUS);
  self.add(sdet);
  sdet.setPlacement(pv);
  m_endcap.side[SIDE_MINUS] = sdet;

  //theENDCAPEndSD->SetModuleZOffset(0,fabs(Z1));
  //theENDCAPEndSD->SetModuleZOffset(6,fabs(Z1));
}

/// Build Box with RPC1 chamber
Volume Hcal04::buildRPC1Box(const string& nam, Box box, const Layer& layer, SensitiveDetector& sensdet)   {
  Material g10_mat = lcdd->material("G10");

  // fill the Chamber Envelope with G10
  Volume chamberVol(nam+"_rpc",box,g10_mat);
	     
  // build the RPC glass !!attention!! y<->z
  Box    glassBox(box.x(),box.y(),m_rpc.glass.thickness/2.0);
  Volume glassVol(nam+"_RPC_glass",glassBox,lcdd->material("pyrex"));
  glassVol.setVisAttributes(lcdd->visAttributes("HcalRpcGlassVis"));

  // build the standard spacer !!attention!! y<->z
  Box    spacerBox(box.x(),m_rpc.spacer.thickness/2,m_rpc.gas.thickness/2);
  Volume spacerVol(nam+"_spacer",spacerBox,g10_mat);
  spacerVol.setVisAttributes(lcdd->visAttributes("HcalRpcSpacerVis"));

  Box    gasBox(box.x(),box.y(),m_rpc.gas.thickness/2.0);
  Volume gasVol(nam+"_RPC_gas",glassBox,lcdd->material("RPCGAS1"));
  gasVol.setVisAttributes(lcdd->visAttributes("HcalRpcGasVis"));
  gasVol.setSensitiveDetector(sensdet);
  gasVol.setLimitSet(m_limits);

  // PLugs the sensitive detector HERE!
  // gasVol->SetSensitiveDetector(theSD);
      
  // placing the spacers inside the gas gap
  double MaxY = box.y()-m_rpc.spacer.thickness/2;
  for(double ypos=-box.y() + m_rpc.spacer.thickness/2; ypos < MaxY; ypos += m_rpc.spacer.gap)
    gasVol.placeVolume(spacerVol,Position(0,ypos,0));

  // placing the all. ZOffset starts pointing to the chamber border.
  double zpos = -box.z();
  // first glass border after the g10_thickness
  zpos += m_rpc.radiator.thickness + m_rpc.glass.thickness/2.;
  chamberVol.placeVolume(glassVol,Position(0,0,zpos));

  // set zpos to the next first glass border + gas gap placing 
  zpos += m_rpc.glass.thickness/2. + m_rpc.gas.thickness/2.;
  PlacedVolume pv  = chamberVol.placeVolume(gasVol,Position(0,0,zpos));
  pv.addPhysVolID("layer",layer.id);

  // set zpos to the next gas gap border + second glass
  zpos += m_rpc.gas.thickness/2. + m_rpc.glass.thickness/2.;
  pv  = chamberVol.placeVolume(glassVol,Position(0,0,zpos));
  return chamberVol;
}

static Ref_t create_detector(LCDD& lcdd, xml_h element, Ref_t)  {
  return (new Hcal04())->construct(lcdd,element);
}
DECLARE_SUBDETECTOR(Tesla_hcal04,create_detector);
