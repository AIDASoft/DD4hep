// $Id$
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

namespace DD4hep { namespace Geometry {
  struct ClicYoke01Data : public DetElement::Object  {
    typedef Position Dimension;

    vector<pair<Material,pair<double,double> > > rpc_layers;
    pair<double,double> rpc_last_layer;

    int symmetry;
    int num_layer;
    double yokeThickness;
    double zStartEndcap;
    double iron_thickness;
    double layer_thickness;
    double yokeBarrelEndcapGap;
    double cell_dim_x;
    double cell_dim_z;
    double zEndYoke;
    double tiltAngle;
    double zEndcap;

    double HCAL_z;
    double HCAL_plug_gap;
    double HCAL_plug_zpos;
    double HCAL_plug_thickness;
    double HCAL_R_max;

    double rpc_total_thickness;
    double rpc_chamber_thickness;
    double rpc_chamber_position; 

    bool m_hasPlug;

    Material yokeMat;
    Material rpcGasMat;
    VisAttr  yokeVis;
    VisAttr  plugVis;
    VisAttr  rpcGasVis;
    VisAttr  chamberVis;
    LimitSet limitSet;
    struct Radii { 
      double inner_r, outer_r; 
      VisAttr vis;
    } endcap, barrel;
    struct SubComponent : public DetElement  {
      SensitiveDetector sensDet;
      /// Helper function to allow assignment
      DetElement& operator=(const DetElement& d)  { return this->DetElement::operator=(d); }
    } m_barrel, m_endcapPlus, m_endcapMinus, m_plugPlus, m_plugMinus;

    LCDD*       lcdd;
    std::string name;
    DetElement  self;
  };

  struct ClicYoke01 : public ClicYoke01Data  {
    /// Standard constructor
    ClicYoke01() : ClicYoke01Data() {}
    /// Detector construction function
    DetElement construct(LCDD& lcdd, xml_det_t e);

    /// Collect properties of RPC layers from XML description
    void collectRPClayers(xml_comp_t x_rpc);
    /// Collect the necessary sensitive detectors
    void collectSensitiveDetectors(xml_det_t e);
    /// Build the Yoke barrel logical volume ready for placement
    Volume buildBarrel();
    /// Build the endcap logical volume ready for placement
    Volume buildEndcap();
    /// Build the Yoke plug logical volume ready for placement
    Volume buildPlug();
  };
}}

/// Collect the necessary sensitive detectors
void ClicYoke01::collectSensitiveDetectors(xml_det_t e)   {
  xml_comp_t c = e.child(_U(barrel));

  // Setup the sensitive detectors for barrel, endcap+ and endcap-
  m_barrel.sensDet = SensitiveDetector("YokeBarrel","yoke");
  Readout ro = lcdd->readout(c.readoutStr());
  m_barrel.sensDet.setReadout(ro);
  m_barrel.sensDet.setHitsCollection(ro.name());
  m_barrel = DetElement(self,name+"_barrel",e.id());

  c = e.child(_U(endcap));
  m_endcapMinus.sensDet = m_endcapPlus.sensDet = SensitiveDetector("YokeEndcap","yoke");
  ro = lcdd->readout(c.readoutStr());
  m_endcapPlus.sensDet.setReadout(ro);
  m_endcapPlus.sensDet.setHitsCollection(ro.name());
  m_endcapPlus = DetElement(self,name+"_endcap_plus",e.id());

  if ( m_hasPlug ) {
    c = e.child(_U(endcap));
    m_plugMinus.sensDet = m_plugPlus.sensDet = SensitiveDetector("YokePlug","yoke");
    ro = lcdd->readout("YokePlugHits");
    m_plugPlus.sensDet.setReadout(ro);
    m_plugPlus.sensDet.setHitsCollection(ro.name());
    m_plugPlus = DetElement(self,name+"_plug_plus",e.id());
  }
}

/// Collect properties of RPC layers from XML description
void ClicYoke01::collectRPClayers(xml_comp_t x_rpcs)   {
  // calculation of the sensitive layer offset within the RPC structure
  rpc_total_thickness   = 0;
  rpc_chamber_thickness = 0.0;
  rpc_chamber_position  = 0.0;  
  for(xml_coll_t c(x_rpcs,_U(rpc)); c; ++c)  {
    xml_comp_t x(c);
    double dzPiece = x.thickness();    
    Material mat = lcdd->material(x.materialStr());
    if ( mat.ptr() == rpcGasMat.ptr() ) 	{
      rpc_chamber_position += dzPiece/2;
      rpc_chamber_thickness = dzPiece;
    }
    else if ( rpc_chamber_thickness == 0.0 )  {
      rpc_chamber_position += dzPiece;
    }
    rpc_layers.push_back(make_pair(lcdd->material(x.materialStr()),make_pair(rpc_total_thickness,dzPiece)));
    rpc_total_thickness += dzPiece;
  }
  rpc_last_layer = rpc_layers[rpc_layers.size()-1].second;
}

/// Build the endcap logical volume ready for placement
Volume ClicYoke01::buildEndcap()   {
  PolyhedraRegular solid(symmetry,endcap.inner_r,endcap.outer_r,yokeThickness);
  Volume           envelope(name+"_endcap_envelope",solid,yokeMat);
  PolyhedraRegular chamberSolid(symmetry,endcap.inner_r,barrel.outer_r,layer_thickness);
  Volume           chamberVol(name+"_endcap",chamberSolid,lcdd->air());
  PlacedVolume     pv;

  if ( rpc_last_layer.first+rpc_last_layer.second > layer_thickness ) {
    throw runtime_error("Overfull RPC layer in yoke endcap!");
  }
  envelope.setVisAttributes(endcap.vis);
  chamberVol.setVisAttributes(chamberVis);
  for(size_t i=0; i<rpc_layers.size(); ++i)   {
    Material m = rpc_layers[i].first;
    double   t = rpc_layers[i].second.second;
    PolyhedraRegular h(symmetry,endcap.inner_r,barrel.outer_r,t);
    Volume   v(name+_toString(i,"_endcap_layer%d_")+m.name(),h,m);
    double zpos = -layer_thickness/2+rpc_layers[i].second.first+t/2;
    if ( m.ptr() == rpcGasMat.ptr() )   {
      v.setLimitSet(limitSet);
      v.setVisAttributes(rpcGasVis);
      v.setSensitiveDetector(m_barrel.sensDet);
    }
    chamberVol.placeVolume(v,Position(0,zpos,0));
  }

  double shift_middle, shift_sensitive;
  for(int i = 0; i < num_layer; i++)      {
    if( i != (num_layer - 1))    {
      shift_middle    = -yokeThickness/2 + iron_thickness*(i+1) + (i+0.5)*layer_thickness; 
      shift_sensitive = zEndcap-yokeThickness/2 + iron_thickness*(i+1) + i*layer_thickness +rpc_chamber_position; 
    }
    else    {      // there is also endlayer in the endcap 
      shift_middle    = (yokeThickness - layer_thickness)/2; 
      shift_sensitive = zEndcap + shift_middle + rpc_chamber_position;
    }
#if 0
    muonECSD->AddLayer(i + 1, 0.0, 0.0, shift_sensitive); 
#endif
    //cout << "Place endcap layer " << i << endl;
    pv = envelope.placeVolume(chamberVol,Position(0,0,shift_middle));
    pv.addPhysVolID("layer",i);
  }    // end loop over endcap layers
  return envelope;
}

/// Build the Yoke plug logical volume ready for placement
Volume ClicYoke01::buildPlug() {
#if 0
  muonSD* muonPlugSD =  new muonSD(cell_dim_x, cell_dim_z, rpc_chamber_thickness, 2, "MuonPlug", 1);
  RegisterSensitiveDetector(muonPlugSD);
#endif
  Volume           gasVol;
  PlacedVolume     pv;
  /*	
    fg: the plug should have same outer radius as the hcal endcap 
    note: HCAL_R_max is outer edge radius of  the barrel with 16-fold symmentry -> cos(pi/16)
    the hcal endcap only extends to an 'assumed' octagonal  barrel that fits into the 16-fold shape
    -> cos(pi/8)
  */
  HCAL_R_max = HCAL_R_max*cos(M_PI/16) *cos(M_PI/8);
  /* fg:  introduce #layers in plug*/
  unsigned int nLayerPlug = 1 ; /* ILD plug is 100mm Fe - no instrumentation !*/
 
  if (HCAL_plug_thickness < 0.0 || HCAL_plug_thickness < nLayerPlug * layer_thickness)   {
    throw runtime_error("Plug thickness negative or thinner then nLayerPlug layers of rpc!");
  }
  PolyhedraRegular plugSolid(symmetry,endcap.inner_r,HCAL_R_max,HCAL_plug_thickness);
  Volume           plugVol  (name+"_plug",plugSolid,yokeMat);
  PolyhedraRegular chamberSolid(symmetry,endcap.inner_r,HCAL_R_max,layer_thickness);
  Volume           chamberVol  (name+"_plug_layers",chamberSolid,lcdd->air());

  plugVol.setVisAttributes(plugVis);
  chamberVol.setVisAttributes(chamberVis);
  // ******* plug detector  ( 5 layers ) see TESLA-tdr IV p.122
  // plug step: plays the role of absorber thickness in the plug, assuming the total thickness of the
  //  plug is given by:
  //  HCAL_plug_thickness = nlayers * (abs_plug + sens_plug)
  double plug_step = (HCAL_plug_thickness - nLayerPlug * layer_thickness)/nLayerPlug; /* fg: nL or nL +1 ?*/  

  for(size_t i=0; i<rpc_layers.size(); ++i)   {
    Material m = rpc_layers[i].first;
    double   t = rpc_layers[i].second.second;
    PolyhedraRegular h(symmetry,endcap.inner_r,HCAL_R_max,t);
    Volume   v(name+_toString(i,"_plug_layer%d_")+m.name(),h,m);
    double zpos = -layer_thickness/2+rpc_layers[i].second.first+t/2;
    if ( m.ptr() == rpcGasMat.ptr() )   {
      v.setLimitSet(limitSet);
      v.setVisAttributes(rpcGasVis);
      v.setSensitiveDetector(m_plugPlus.sensDet);
    }
    chamberVol.placeVolume(v,Position(0,zpos,0));
  }
  double bigAbsorber = 150;
  for(unsigned int i = 0; i < nLayerPlug; i++)   {
    double shift           = -HCAL_plug_thickness/2 + bigAbsorber + layer_thickness/2; 
    double shift_sensitive =  HCAL_plug_zpos - HCAL_plug_thickness/2 + bigAbsorber + rpc_chamber_position;
    if ( std::abs(shift) > HCAL_plug_thickness/2 )  {
      throw runtime_error("Too thin plug, please set COIL_EXTRA_SIZE  and/or COIL_YOKE_LATERAL_CLEARANCE correctly!");
    }
    ///// muonPlugSD->AddLayer(i + 1, 0.0, 0.0, shift_sensitive);	    
    pv = plugVol.placeVolume(chamberVol,Position(0,0,shift));
    pv.addPhysVolID("layer",i+1);
  }  //   end loop over plug layers
  return plugVol;
}

/// Build the Yoke barrel logical volume ready for placement
Volume ClicYoke01::buildBarrel() {
  /// notice that the barrel is "short" and endcap has full radius !!! 
  PolyhedraRegular barrelSolid(symmetry,barrel.inner_r,barrel.outer_r,2*(zStartEndcap-yokeBarrelEndcapGap));
  Volume           barrelVol(name+"_barrel",barrelSolid,yokeMat);
  double           angle  = 2*M_PI / symmetry;
  Volume           gasVol;
  PlacedVolume     pv;

  barrelVol.setVisAttributes(barrel.vis);
 
  for(int i = 0; i < num_layer+1; i++)      {
    double dx, dy, radius_low, radius_mid, radius_sensitive;
    if (i == 0)	  {
      radius_low = barrel.inner_r;
      radius_mid = radius_low + 0.5 * layer_thickness;      // middle of sensitive layer, where to place the chambers
      radius_sensitive = radius_low + rpc_chamber_position; // y-center of the layer
    }
    else if( i != num_layer)	  {
      radius_low       = barrel.inner_r + i * (iron_thickness + layer_thickness); 
      radius_mid       = radius_low + 0.5 * layer_thickness;  
      radius_sensitive = radius_low + rpc_chamber_position;
    }
    else  {   // there is always the layer on the "top" of the yoke that doesn't depend on the number of layers inside
      radius_low       = barrel.outer_r - layer_thickness; 
      radius_mid       = radius_low + 0.5 * layer_thickness;  
      radius_sensitive = radius_low + rpc_chamber_position;
    }
    dx = radius_low * std::tan(angle/2) - 0.05;          // safety margines of 0.1 mm
    dy = (zStartEndcap - yokeBarrelEndcapGap)/2. - 0.05; // safety margines 
#if 0
    m_barrel.sensDet->AddLayer(i+1, dx, radius_sensitive, 0.0); 
#endif
    string chamber_name = name+_toString(i,"_barrel_stave_layer%d");
    Box    chamberBox(dx,layer_thickness/2.0,dy);
    Volume chamberVol(chamber_name,chamberBox,lcdd->air());
    chamberVol.setVisAttributes(chamberVis);
    if ( rpc_last_layer.first+rpc_last_layer.second > chamberBox.y() ) {
      throw runtime_error("Overfull RPC layer in yoke barrel!");
    }
    for(size_t j=0; j<rpc_layers.size(); ++j)   {
      Material m = rpc_layers[j].first;
      double   t = rpc_layers[j].second.second;
      Volume   v(chamber_name+_toString(j,"_rpc%d"),Box(dx,t/2,dy),m);
      if ( m.ptr() == rpcGasMat.ptr() )   {
	v.setLimitSet(limitSet);
	v.setVisAttributes(rpcGasVis);
	v.setSensitiveDetector(m_barrel.sensDet);
	pv = chamberVol.placeVolume(v,Position(0,-chamberBox.y()+rpc_chamber_position,0));
	pv.addPhysVolID("layer",i);
      }
      else {
	double zpos = -chamberBox.y() + rpc_layers[j].second.first + t/2;
	chamberVol.placeVolume(v,Position(0,zpos,0));
      }
    }

    Rotation rot;
    Position pos_pos(radius_mid,0, (zStartEndcap-yokeBarrelEndcapGap)/2);
    Position pos_neg(radius_mid,0,-(zStartEndcap-yokeBarrelEndcapGap)/2);
    for(int j = 0; j < symmetry; j++)	  {
      int mod_id = j+1;
      double phi = angle*(j+1)+tiltAngle;
      rot.SetPhi(-phi);
#if 0
      m_barrel.sensDet->SetStaveRotationMatrix(j+1, phi);
#endif
      // The VolIDs are wrong, but what is in the original code is plain brainless!
      Transform3D tr(rot,RotateZ(pos_pos,phi+M_PI/2));
      pv = barrelVol.placeVolume(chamberVol,tr);
      pv.addPhysVolID("module",mod_id);

      // Equivalent construct without a Transform3D for the negative side:
      pv = barrelVol.placeVolume(chamberVol,RotateZ(pos_neg,phi+M_PI/2),rot);
      pv.addPhysVolID("module",-mod_id);
    }
  }  // end loop over i (layers)
  return barrelVol;
}

/// Detector construction function
DetElement ClicYoke01::construct(LCDD& l, xml_det_t x_det)  {
  lcdd     = &l;
  name     = x_det.nameStr();
  self.assign(dynamic_cast<Value<TNamed,ClicYoke01>*>(this),name,x_det.typeStr());
  self._data().id = x_det.id();
  Assembly    assembly(name);
  xml_comp_t  x_yoke       = x_det.child(_Unicode(yoke));
  xml_comp_t  x_muon       = x_det.child(_Unicode(muon));
  symmetry  = x_yoke.attr<double>(_Unicode(symmetry));

  PlacedVolume pv;

  /// Geometry parameters from the geometry environment and from the database
  barrel.inner_r      = lcdd->constant<double>("Yoke_barrel_inner_radius");
  endcap.inner_r      = lcdd->constant<double>("Yoke_endcap_inner_radius");
  yokeThickness       = lcdd->constant<double>("Yoke_thickness"); 
  zStartEndcap        = lcdd->constant<double>("Yoke_Z_start_endcaps");

  /// Simple calculations and some named constants
  zEndYoke            = zStartEndcap + yokeThickness;
  zEndcap             = zEndYoke - yokeThickness / 2;
  tiltAngle           = M_PI/symmetry - M_PI/2; // the yoke should always rest on a flat side
  barrel.outer_r      = barrel.inner_r + yokeThickness; 
  endcap.outer_r      = barrel.outer_r;

  //----------------------------------------- muon -------------------------------
  iron_thickness      = x_muon.attr<double>(_Unicode(iron_thickness));
  layer_thickness     = x_muon.attr<double>(_Unicode(layer_thickness));
  yokeBarrelEndcapGap = x_muon.attr<double>(_Unicode(barrel_endcap_gap));
  cell_dim_x          = x_muon.attr<double>(_Unicode(cell_size));
  cell_dim_z          = x_muon.attr<double>(_Unicode(cell_size)); 
  num_layer           = x_muon.attr<int>(_Unicode(number_of_layers));
  m_hasPlug           = lcdd->constant<int>("Yoke_with_plug") == 1;  

  HCAL_z              = lcdd->constant<double>("calorimeter_region_zmax");
  HCAL_R_max          = lcdd->constant<double>("Hcal_R_max");
  HCAL_plug_gap       = x_muon.attr<double>(_Unicode(Hcal_plug_gap));
  HCAL_plug_thickness = zStartEndcap - HCAL_z - HCAL_plug_gap;
  HCAL_plug_zpos      = zStartEndcap - HCAL_plug_thickness/2;

  /// check for consistensy ...
  double testbarrel = num_layer*(iron_thickness + layer_thickness); 
 
  if( testbarrel >= yokeThickness  || testbarrel >= ( zEndYoke-zStartEndcap))    {
    throw runtime_error("ClicYoke01: Overfull barrel or endcap! check layer number or dimensions !");
  }
  
  // Materials and visualization attributes
  yokeMat    = lcdd->material(x_yoke.materialStr());
  rpcGasMat  = lcdd->material("RPCGAS1");
  barrel.vis = lcdd->visAttributes("YokeBarrelVis");
  endcap.vis = lcdd->visAttributes("YokeEndcapVis");
  rpcGasVis  = lcdd->visAttributes("YokeRPCGasVis");
  yokeVis    = lcdd->visAttributes(x_yoke.visStr());
  plugVis    = lcdd->visAttributes("YokePlugVis");
  chamberVis = lcdd->visAttributes("YokeChamberVis");
  limitSet   = lcdd->limitSet(x_det.limitsStr());

  collectRPClayers(x_det.child(_Unicode(rpcs)));
  collectSensitiveDetectors(x_det);

  //--------------------------------------------------

  // Sensitive detectors
  // the cells are temporary solution to the strips -LCIO problem  put "equivalent" cell size
#if 0
  muonSD*  m_barrel.sensDet = new muonSD(cell_dim_x, cell_dim_z, rpc_chamber_thickness, 1, "MuonBarrel", 1);
  RegisterSensitiveDetector(m_barrel.sensDet);
  muonSD*  muonECSD = new muonSD(cell_dim_x, cell_dim_z, rpc_chamber_thickness, 2, "MuonEndCap", 1);
  RegisterSensitiveDetector(muonECSD);
#endif
  //-------------------------------------------------
  //        Barrel part                               
  //-------------------------------------------------
  {
    Volume barrelVol = buildBarrel();
    pv = assembly.placeVolume(barrelVol,Position(),Rotation(tiltAngle,0,0));
    pv.addPhysVolID("barrel",0);
    m_barrel.setPlacement(pv);
  }
  // -------------------------------------------------
  //  Endcaps part 
  // -------------------------------------------------
  {
    Volume ecVolume = buildEndcap();
    pv = assembly.placeVolume(ecVolume,Position(0,0, zEndcap),Rotation(tiltAngle,0,0));
    pv.addPhysVolID("barrel",1);
    m_endcapPlus.setPlacement(pv);
    pv = assembly.placeVolume(ecVolume,Position(0,0,-zEndcap),Rotation(tiltAngle+M_PI,0,0));
    pv.addPhysVolID("barrel",2);
    m_endcapMinus = m_endcapPlus.clone(name+"_endcap_minus",x_det.id());
    m_endcapMinus.setPlacement(pv);
  }
  // -------------------------------------------------
  //    The Plugs for TESLA TDR models series  <=> LDC00_xx 
  //  -------------------------------------------------
  if( m_hasPlug )      {
    Volume plugVol = buildPlug();
    pv = assembly.placeVolume(plugVol,Position(0,0, HCAL_plug_zpos),Rotation(tiltAngle,0,0));
    pv.addPhysVolID("barrel",3);
    m_plugPlus.setPlacement(pv);
    pv = assembly.placeVolume(plugVol,Position(0,0,-HCAL_plug_zpos),Rotation(tiltAngle+M_PI,0,0));
    pv.addPhysVolID("barrel",4);
    m_plugMinus = m_plugPlus.clone(name+"_plug_minus",x_det.id());
    m_plugMinus.setPlacement(pv);
  }
  // now place the full assembly
  assembly.setVisAttributes(lcdd->visAttributes(x_det.visStr()));
  pv = lcdd->pickMotherVolume(self).placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  self.setPlacement(pv);
  return self;
}

static Ref_t create_detector(LCDD& lcdd, xml_h element, Ref_t)  {
  return (new Value<TNamed,ClicYoke01>())->construct(lcdd,element);
}
DECLARE_SUBDETECTOR(Tesla_ClicYoke01,create_detector);
