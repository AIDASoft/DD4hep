//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "UpgradeTags.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace   {
  struct xml_params_t  {
    xml_elt_t                     element;
    std::map<std::string, double> data;
    xml_params_t(xml_h params_handle);
    double operator()(const char* name)  const;
  };
  xml_params_t::xml_params_t(xml_h handle) : element(handle)   {
    for(xml_coll_t i(handle,_U(param)); i; ++i)  {
      xml_comp_t c(i);
      data[c.nameStr()] = c.attr<double>(_U(value));
    }
  }
  
  double xml_params_t::operator()(const char* name)  const  {
    auto i = name ? data.find(name) : data.end();
    if ( i != data.end() ) return (*i).second;
    stringstream str;
    xml::dump_tree(element, str);
    except("XML-Parameters","+++ No parameter with name: %s present:%s",
           name ? name : "UNKNOWN", str.str().c_str());
    return 0.0;
  }

  struct UTBuild   {
    Detector&         description;
    xml_det_t         x_det;
    xml_comp_t        x_env;
    xml_params_t      x_params;
    string            det_name;
    DetElement        sdet;
    SensitiveDetector sd;
    Volume            vol_group_norm, vol_hybrid_norm;
    Volume            vol_group_dual, vol_hybrid_dual;
    Volume            vol_group_quad, vol_hybrid_quad;
    Volume            vol_moduleA, vol_moduleB, vol_moduleC;
    Volume            vol_frame, vol_box;
    Volume            vol_uta, vol_utb;
    double            beamHoleRadius;

    double            frameOuterWidth;
    double            frameOuterHeight;
    double            frameInnerWidth;
    double            frameInnerHeight;
    double            frameThick;
    double            boxOuterThickness;
    double            vboxWidth;
    double            vboxHeight;
    double            vholeRadius;

    double            moduleWidth;
    double            moduleHeight;
    double            moduleThick;
    double            moduleHybridStepY;
    double            moduleHybridZ;
    
    double            stationThick;
    double            layerDz;
    double            layerThick;

    double            sensorWidth;
    double            sensorHeight;

    double            sensorSiZ;
    double            sensorSiWidth;
    double            sensorSiHeight;
    double            sensorSiThick;
    double            sensorSiOverlapX;
    double            sensorSiOverlapY;

    double            sensorAlZ;
    double            sensorAlThick;
    double            sensorGRZ;
    double            sensorGRWidth;
    double            sensorGRThick;

    double            asicWidth;
    double            asicHeight;
    double            asicThick;

    double            hybridWidth;
    double            hybridHeight;
    double            hybridThick;
    double            hybridFlexWidth;
    double            hybridFlexHeight;
    double            hybridFlexThick;
    double            hybridAsicY;
    double            hybridAsicZbase;
    double            hybridFlexY;
    double            hybridFlexZ;
    double            hybridSensorZ;

    double            balconyYposX;
    
    // Debug flags
    bool              m_build_uta = true;
    bool              m_build_utb = true;
    bool              m_build_frame = true;
    bool              m_build_box   = true;
    bool              m_build_jacket = true;
    bool              m_build_layers = true;
    bool              m_build_sensors = true;
    
    UTBuild(Detector& description, xml_elt_t e, Ref_t sens);
    void build_hybrids();
    void build_sensors();
    void build_modules();
    void build_layers();
    DetElement instantiate();
  };
  UTBuild::UTBuild(Detector& dsc, xml_elt_t e, Ref_t sens)
    : description(dsc), x_det(e), x_env(0), x_params(x_det.params()), sd(sens)
  {
    x_env    = x_det.envelope();
    det_name = x_det.nameStr();
    sdet     = DetElement(det_name,x_det.id());
    beamHoleRadius    = x_params("BeamHoleRadius");

    frameOuterWidth   = x_params("FrameOuterWidth");
    frameOuterHeight  = x_params("FrameOuterHeight");
    frameInnerWidth   = x_params("FrameInnerWidth");
    frameInnerHeight  = x_params("FrameInnerHeight");
    frameThick        = x_params("FrameThickness");
    boxOuterThickness = x_params("BoxOuterThickness");
    
    vboxWidth         = x_params("VirtualBoxWidth");
    vboxHeight        = x_params("VirtualBoxHeight");
    vholeRadius       = x_params("VirtualHoleRadius");

    moduleWidth       = x_params("ModuleWidth");
    moduleHeight      = x_params("ModuleHeight");
    moduleThick       = x_params("ModuleThickness");
    moduleHybridStepY = x_params("ModuleHybridStepY");
    moduleHybridZ     = x_params("ModuleHybridZ");
      
    stationThick      = x_params("StationThickness");
    layerDz           = x_params("LayerDz");
    layerThick        = x_params("LayerThickness");

    sensorWidth       = x_params("SensorWidth");
    sensorHeight      = x_params("SensorHeight");

    sensorSiZ         = x_params("SensorSiZ");
    sensorSiWidth     = x_params("SensorSiWidth");
    sensorSiHeight    = x_params("SensorSiHeight");
    sensorSiThick     = x_params("SensorSiThickness");
    sensorSiOverlapX  = x_params("SensorSiOverlapX");
    sensorSiOverlapY  = x_params("SensorSiOverlapY");

    sensorAlZ         = x_params("SensorAlZ");
    sensorAlThick     = x_params("SensorAlThickness");
    sensorGRZ         = x_params("SensorGRZ");
    sensorGRWidth     = x_params("SensorGRWidth");
    sensorGRThick     = x_params("SensorGRThickness");

    asicWidth         = x_params("AsicWidth");
    asicHeight        = x_params("AsicHeight");
    asicThick         = x_params("AsicThickness");

    hybridWidth       = x_params("HybridWidth");
    hybridHeight      = x_params("HybridHeight");
    hybridThick       = x_params("HybridThickness");
    hybridFlexWidth   = x_params("HybridFlexWidth");
    hybridFlexHeight  = x_params("HybridFlexHeight");
    hybridFlexThick   = x_params("HybridFlexThickness");
    hybridAsicY       = x_params("HybridAsicY");
    hybridAsicZbase   = x_params("HybridAsicZbase");
    hybridFlexY       = x_params("HybridFlexY");
    hybridFlexZ       = x_params("HybridFlexZ");
    hybridSensorZ     = x_params("HybridSensorZ");

    balconyYposX      = x_params("BalconyYposX");

    
    // Process debug flags
    xml_comp_t x_dbg  = x_det.child(_U(debug), false);
    if ( x_dbg.ptr() )   {
      for(xml_coll_t i(x_dbg,_U(item)); i; ++i)  {
        xml_comp_t c(i);
        if ( c.nameStr() == "Build_UTA"     ) m_build_uta = c.attr<bool>(_U(value));
        if ( c.nameStr() == "Build_UTB"     ) m_build_utb = c.attr<bool>(_U(value));
        if ( c.nameStr() == "Build_Jacket"  ) m_build_jacket  = c.attr<bool>(_U(value));
        if ( c.nameStr() == "Build_Layers"  ) m_build_layers  = c.attr<bool>(_U(value));
        if ( c.nameStr() == "Build_Sensors" ) m_build_sensors = c.attr<bool>(_U(value));
        if ( c.nameStr() == "Build_Frame"   ) m_build_frame   = c.attr<bool>(_U(value));
        if ( c.nameStr() == "Build_Box"     ) m_build_box     = c.attr<bool>(_U(value));
        cout << "Dbg:" << c.nameStr() << " " << c.attr<bool>(_U(value)) << endl;
      }
    }
  }

  void UTBuild::build_sensors()   {
    Material mat_air = description.air();
    Material mat_Si  = description.material("Si");
    Material mat_Al  = description.material("Al");

    Volume vol_metal_layer_full("lvMetalLayerFull",
                                Box(sensorWidth, sensorHeight, sensorAlThick), mat_Al);

    SubtractionSolid sol_metal_layer_hole(Box(sensorWidth/2.0, sensorHeight/2.0, sensorGRThick),
                                          Tube(0.0, beamHoleRadius, sensorAlThick*2.0),
                                          Position(sensorSiWidth/2.0-sensorSiOverlapX/2.0,
                                                   sensorSiHeight/2.0-sensorSiOverlapY/2.0,
                                                   0));
    Volume vol_metal_layer_hole ("lvGuardRingHole", sol_metal_layer_hole, mat_Si);
    
    SubtractionSolid sol_guard_ring_full(Box(sensorWidth, sensorHeight, sensorGRThick),
                                         Box(sensorSiWidth, sensorSiHeight, sensorSiThick*2.0));
    Volume vol_guard_ring_full ("lvGuardRingFull", sol_guard_ring_full, mat_Si);
  
    SubtractionSolid sol_guard_ring_frame1(Box(sensorWidth, sensorHeight, sensorGRThick),
                                           Tube(0.0, beamHoleRadius+sensorGRWidth, sensorGRThick*2.0),
                                           Position(sensorSiWidth/2-sensorSiOverlapX/2,
                                                    sensorSiHeight/2-sensorSiOverlapY/2,
                                                    0));
    SubtractionSolid sol_guard_ring_frame(sol_guard_ring_frame1,
                                          Box(sensorSiWidth, sensorSiHeight, sensorGRThick*3.0));
    Volume vol_guard_ring_frame ("lvGuardRingFrame", sol_guard_ring_frame, mat_Si);

    IntersectionSolid sol_guard_ring_arc(Box(sensorWidth, sensorHeight, sensorGRThick),
                                         Tube(beamHoleRadius, beamHoleRadius+sensorGRWidth, sensorGRThick*2.0),
                                         Position((sensorSiWidth-sensorSiOverlapX)/2.0,
                                                  (sensorSiHeight-sensorSiOverlapY)/2.0,
                                                  0));
    Volume vol_guard_ring_arc ("lvGuardRingArc", sol_guard_ring_arc, mat_Si);

    SubtractionSolid sol_sensor_hole(Box(sensorWidth/2.0, sensorHeight/2.0, sensorGRThick),
                                         Tube(0.0, beamHoleRadius+sensorGRWidth, sensorGRThick*2.0),
                                         Position(sensorSiWidth/4.0-sensorSiOverlapX/2.0,
                                                  sensorSiHeight/4.0-sensorSiOverlapY/2.0,
                                                  0));
    Volume vol_sensor_hole ("lvGuardRingHole", sol_sensor_hole, mat_Si);
    
    Box    box_sensor_norm(sensorSiWidth,sensorSiHeight,sensorSiThick);
    Volume vol_sensor_norm("lvSensorNorm", box_sensor_norm, mat_Si);
    Volume vol_sector_norm("lvSectorNorm", box_sensor_norm, mat_air);
    vol_sector_norm.placeVolume(vol_sensor_norm);
    vol_group_norm = Assembly("lvGroupNorm");
    vol_group_norm.placeVolume(vol_metal_layer_full, Position(0,0,sensorAlZ));
    vol_group_norm.placeVolume(vol_guard_ring_full,  Position(0,0,sensorGRZ));
    vol_group_norm.placeVolume(vol_sector_norm,      Position(0,0,sensorSiZ));

    Box    box_sensor_dual(sensorSiWidth/2.0,sensorSiHeight,sensorSiThick);
    Volume vol_sensor_dual("lvSensorDual", box_sensor_dual, mat_Si);
    Volume vol_sector_dual("lvSectorDual", box_sensor_dual, mat_air);
    vol_sector_dual.placeVolume(vol_sensor_dual);
    vol_group_dual = Assembly("lvGroupDual");
    vol_group_dual.placeVolume(vol_metal_layer_full, Position(0,0,sensorAlZ));
    vol_group_dual.placeVolume(vol_guard_ring_full,  Position(0,0,sensorGRZ));
    vol_group_dual.placeVolume(vol_sector_dual,      Position(-sensorSiHeight/4.0,0,sensorSiZ));
    vol_group_dual.placeVolume(vol_sector_dual,      Position( sensorSiHeight/4.0,0,sensorSiZ));

    Box    box_sensor_quad(sensorSiWidth/2.0,sensorSiHeight/2.0,sensorSiThick);
    Volume vol_sensor_quad("lvSensorQuad", Box(sensorSiWidth,sensorSiHeight,sensorSiThick), mat_Si);
    Volume vol_sector_quad("lvSectorQuad", Box(sensorSiWidth,sensorSiHeight,sensorSiThick), mat_air);
    vol_sector_quad.placeVolume(vol_sensor_quad);
    vol_group_quad = Assembly("lvGroupQuad");
    vol_group_quad.placeVolume(vol_metal_layer_hole, Position(0,0,sensorAlZ));
    vol_group_quad.placeVolume(vol_guard_ring_frame, Position(0,0,sensorGRZ));
    vol_group_quad.placeVolume(vol_guard_ring_arc,   Position(0,0,sensorGRZ));
    vol_group_quad.placeVolume(vol_sector_quad,      Position(-sensorSiHeight/4.0,-sensorSiHeight/4.0,sensorSiZ));
    vol_group_quad.placeVolume(vol_sector_quad,      Position( sensorSiHeight/4.0,-sensorSiHeight/4.0,sensorSiZ));
    vol_group_quad.placeVolume(vol_sector_quad,      Position(-sensorSiHeight/4.0, sensorSiHeight/4.0,sensorSiZ));
    vol_group_quad.placeVolume(vol_sector_quad,      Position( sensorSiHeight/4.0, sensorSiHeight/4.0,sensorSiZ));
  }

  void UTBuild::build_hybrids()   {
    Material air = description.air();
    Material mat_hybrid_flex = description.material("UT/HybridFlex");
    Material mat_asic        = description.material("UT/UTAsic");
    VisAttr  hybrid_vis      = description.visAttributes("UT_Hybrid_Vis");
    VisAttr  hybrid_flex_vis = description.visAttributes("UT_Hybrid_Flex_Vis");
    VisAttr  asic_vis        = description.visAttributes("UT_Asic_Vis");
    Box hybrid_norm_box(hybridWidth, hybridHeight, hybridThick);
    Box hybrid_flex_box(hybridFlexWidth, hybridFlexHeight, hybridFlexThick);

    Volume vol_asic  ( "lvAsic", Box(asicWidth,   asicHeight, asicThick), mat_asic);
    Volume vol_asic4 ("lvAsic4", Box(sensorWidth, asicHeight, asicThick*4), mat_asic);
    Volume vol_asic8 ("lvAsic8", Box(sensorWidth, asicHeight, asicThick*8), mat_asic);
    Volume vol_asic16("lvAsic16",Box(sensorWidth, asicHeight, asicThick*16), mat_asic);
    Volume vol_hybrid_flex_full("lvHybridFlex", hybrid_flex_box, mat_hybrid_flex);

    vol_hybrid_norm = Volume("lvHybridNorm", hybrid_norm_box, air);
    vol_hybrid_norm.placeVolume(vol_group_norm,Position(0, 0, hybridSensorZ));
    vol_hybrid_norm.placeVolume(vol_hybrid_flex_full,Position(0, hybridFlexY, hybridFlexZ));

    Tube   hybrid_hole(0.0, beamHoleRadius, hybridFlexThick*2.0);
    Position hybrid_hole_pos((sensorSiWidth-sensorSiOverlapX)/2.0,
                             (sensorSiHeight-sensorSiOverlapY)/2.0,
                             hybridFlexThick*2.0);
    Volume vol_hybrid_flex_hole("lvHybridFlexHole",
                                SubtractionSolid(hybrid_flex_box, hybrid_hole, hybrid_hole_pos),
                                mat_hybrid_flex);

    vol_asic.setVisAttributes(asic_vis);
    vol_asic4.setVisAttributes(asic_vis);
    vol_asic8.setVisAttributes(asic_vis);
    vol_asic16.setVisAttributes(asic_vis);
    vol_hybrid_flex_full.setVisAttributes(hybrid_flex_vis);
    vol_hybrid_flex_hole.setVisAttributes(hybrid_flex_vis);

    vol_hybrid_norm.placeVolume(vol_asic4, Position(0, hybridAsicY, hybridAsicZbase-asicThick*4.0/2.0));
    vol_hybrid_norm.setVisAttributes(hybrid_vis);

    vol_hybrid_dual = Volume("lvHybridDual", hybrid_norm_box, air);
    vol_hybrid_dual.placeVolume(vol_group_dual,      Position(0, 0, hybridSensorZ));
    vol_hybrid_dual.placeVolume(vol_hybrid_flex_full,Position(0, hybridFlexY, hybridFlexZ));
    vol_hybrid_dual.placeVolume(vol_asic8,           Position(0, hybridAsicY, hybridAsicZbase-asicThick*8.0/2.0));
    vol_hybrid_dual.setVisAttributes(hybrid_vis);

    vol_hybrid_quad = Volume("lvHybridQuad", hybrid_norm_box, air);
    vol_hybrid_dual.placeVolume(vol_group_quad,      Position(0, 0, hybridSensorZ));
    vol_hybrid_dual.placeVolume(vol_hybrid_flex_hole,Position(0, hybridFlexY, hybridFlexZ));  // ???? Hole ????
    vol_hybrid_dual.placeVolume(vol_asic16,          Position(0, hybridAsicY, hybridAsicZbase-asicThick*16.0/2.0));
    vol_hybrid_quad.setVisAttributes(hybrid_vis);
  }

  void UTBuild::build_modules()   {
    Box box_module(moduleWidth, moduleHeight, moduleThick);
    RotationZYX rot_Y(0,M_PI,0);
    RotationZYX rot_Z(0,0,M_PI);
    RotationZYX rot_YZ(0,M_PI,M_PI);
    vol_moduleA = Volume("lvModuleA", box_module, description.air());
    vol_moduleB = Volume("lvModuleB", box_module, description.air());
    vol_moduleC = Volume("lvModuleC", box_module, description.air());
    vol_moduleA.setVisAttributes(description, "UT_Module_Vis");
    vol_moduleB.setVisAttributes(description, "UT_Module_Vis");
    vol_moduleC.setVisAttributes(description, "UT_Module_Vis");
    Volume vol, dau, vols[3] = {vol_moduleA, vol_moduleB, vol_moduleC};
    for(int alt=-1, i = -13; i <= 13; i += 2, alt *= -1)     {
      for(size_t ivol=0; ivol<3; ++ivol)   {
        Position pos(0,double(i)/2.0*moduleHybridStepY, double(alt)*moduleHybridZ);
        vol = vols[ivol];
        if      ( ivol > 1 && std::abs(i) < 2 ) dau = vol_hybrid_quad;
        else if ( ivol > 0 && std::abs(i) < 4 ) dau = vol_hybrid_dual;
        else dau = vol_hybrid_norm;

        cout << "Placeing module " << dau.name() << " " << pos << endl;
        if ( i*alt > 0 )
          (i<0) ? vol.placeVolume(dau, Transform3D(rot_Y,pos)) : vol.placeVolume(dau, Transform3D(rot_YZ,pos));
        else
          (i<0) ? vol.placeVolume(dau, pos)        : vol.placeVolume(dau, Transform3D(rot_Z,pos));
      }
    }
  }

  void UTBuild::build_layers()   {

  }
  
  DetElement UTBuild::instantiate()   {
    Box    sdet_box(frameOuterWidth, frameOuterHeight, boxOuterThickness);
    Volume sdet_vol(x_env.nameStr(), sdet_box, description.air());

    sdet_vol.setAttributes(description, x_env.regionStr(), x_env.limitsStr(), x_env.visStr());
    /**
     *   See DDDB/UUT/geometry.xml for the structure
     **/
    if ( m_build_frame ) { // Build  lvUTFrame
      Position pos;
      xml_comp_t x_frame = x_env.child(_U(frame));
      xml_dim_t  x_p     = x_frame.position(false);
      Material   mat     = description.material(x_frame.materialStr());
      Box        b_outer(frameOuterWidth,frameOuterHeight,frameThick);
      Box        b_inner(frameInnerWidth,frameInnerHeight,frameThick*1.1);
      cout << "Frame inner:" << frameInnerWidth << " " << frameInnerHeight << " " << frameThick*1.1 << endl;
      cout << "Frame outer:" << frameOuterWidth << " " << frameOuterHeight << " " << frameThick << endl;
      vol_frame = Volume("lvUTFrame", SubtractionSolid(b_outer, b_inner), mat);
      vol_frame.setAttributes(description, x_frame.regionStr(), x_frame.limitsStr(), x_frame.visStr());
      if ( x_p ) pos = Position(x_p.x(0), x_p.y(0), x_p.z(0));
      sdet_vol.placeVolume(vol_frame, pos);
    }
    if ( m_build_box )  { // Build  lvUTBox
      Position pos;
      xml_comp_t  x_box = x_env.child(_U(box));
      xml_dim_t   x_p   = x_box.position(false);
      Box         b1(x_params("BoxOuterWidth"), x_params("BoxOuterHeight"), x_params("BoxOuterThickness"));
      Box         b2(x_params("BoxInnerWidth"), x_params("BoxInnerHeight"), x_params("BoxInnerThickness"));
      Tube        tub(0.0, beamHoleRadius, x_params("BoxOuterThickness"));
      SubtractionSolid s2(SubtractionSolid(b1, b2), tub);
      vol_box = Volume(x_box.nameStr(), s2, description.material(x_box.materialStr()));
      vol_box.setAttributes(description, x_box.regionStr(), x_box.limitsStr(), x_box.visStr());
      if ( x_p ) pos = Position(x_p.x(0), x_p.y(0), x_p.z(0));
      sdet_vol.placeVolume(vol_box, pos);
    }
    if ( m_build_uta )   { // Build  lvUTa
      xml_comp_t x_uta = x_env.child(_LBU(uta));
      Material   mat   = description.material(x_uta.materialStr());
      Position   pos, pos_layX, pos_layU;
      Volume vol_layX, vol_layU; {
        xml_dim_t x_p = x_uta.position();
        Box       box(vboxWidth, vboxHeight, stationThick);
        Tube      tub(0.0, vholeRadius, stationThick*1.1);
        if ( x_p ) pos = Position(x_p.x(0), x_p.y(0), x_p.z(0));
        vol_uta = Volume("lvUTa", SubtractionSolid(box, tub), mat);
        vol_uta.setAttributes(description, x_uta.regionStr(), x_uta.limitsStr(), x_uta.visStr());
      }  {
        xml_comp_t x_X = x_uta.child(_U(X));
        xml_dim_t  x_p = x_X.position();
        xml_comp_t x_balcony = x_X.child(_LBU(balcony));
        xml_dim_t  x_balcony_box = x_balcony.child(_U(box));
        xml_dim_t  x_balcony_par = x_balcony.child(_U(params));
        double     dz   = x_balcony_par.dz();
        double     step = x_balcony_par.step();
        double     corr = x_balcony_par.correction();
        Box        box(vboxWidth, vboxHeight, layerThick);
        Tube       tub(0.0, vholeRadius, layerThick*1.1);
        Box        box_balcony(x_balcony_box.x(0.0), x_balcony_box.y(0.0), x_balcony_box.z(0.0));
        Volume     vol_balcony(x_balcony.nameStr(),box_balcony,description.material(x_balcony.materialStr()));


        cout << "lvUTa: Step:" << step << " Corr:" << corr << endl;
        
        pos_layX = Position(x_p.x(0.0), x_p.y(0.0), x_p.z(0.0));
        vol_layX = Volume("lvUTaXLayer", SubtractionSolid(box, tub), mat);
        vol_layX.setVisAttributes(description, x_X.visStr());

        vol_balcony.setVisAttributes(description, x_balcony.visStr());
        for(xml_coll_t i(x_balcony,_U(position)); i; ++i)  {
          xml_dim_t  x_pos = i;
          vol_layX.placeVolume(vol_balcony,Position(x_pos.x(0.0), x_pos.y(0.0), x_pos.z(0.0)));
        }
        // Add the modules to the layer(s)
        RotationZYX rot_Y(0,M_PI,0);
        double dx;
        // Region 1 at -X
        corr = 0;
        dz *= 5.0;
        //
#define DEBUG_IT { cout << "DX: " << dx << endl; }
        vol_layX.placeVolume(vol_moduleA, Position(dx=-15.*step-7.*corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=-13.*step+6.*corr,0,-dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=-11.*step-5.*corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=-9.*step+4.*corr,0,-dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=-7.*step-3.*corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=-5.*step+2.*corr,0,-dz));
        DEBUG_IT

        // Region 2 in the middle
        vol_layX.placeVolume(vol_moduleB, Position(dx=-3.*step-corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleC, Position(dx=-1.*step,0,-dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleC, Position(dx= 1.*step,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleB, Position(dx= 3.*step-corr,0,-dz));
        DEBUG_IT

        // Region 3 at +X
        vol_layX.placeVolume(vol_moduleA, Position(dx= 5.*step+2.*corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx= 7.*step-3.*corr,0,-dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx= 9.*step+4.*corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=11.*step-5.*corr,0,-dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=13.*step+6.*corr,0,dz));
        DEBUG_IT
        vol_layX.placeVolume(vol_moduleA, Position(dx=15.*step-7.*corr,0,-dz));
        DEBUG_IT
        // Balconies
        vol_layX.placeVolume(vol_balcony, Position(0,balconyYposX,0));
        vol_layX.placeVolume(vol_balcony, Position(0,-balconyYposX,0));
      }  {
        xml_comp_t x_U = x_uta.child(_U(U));
        xml_dim_t  x_p = x_U.position();
        Box        box(vboxWidth, vboxHeight, layerThick);
        Tube       tub(layerThick, 0.0, vholeRadius*2.0);
        pos_layU = Position(x_p.x(0.0), x_p.y(0.0), x_p.z(0.0));
        vol_layU = Volume("lvUTaULayer", SubtractionSolid(box, tub), mat);
        vol_layU.setVisAttributes(description, x_U.visStr());
      }
      vol_uta.placeVolume(vol_layX, pos_layX);
      vol_uta.placeVolume(vol_layU, pos_layU);
      sdet_vol.placeVolume(vol_uta, pos);
    }
    if ( m_build_utb )  { // Build  lvUTb
      xml_comp_t x_utb = x_env.child(_LBU(utb));
      Material   mat   = description.material(x_utb.materialStr());
      Position   pos, pos_layX, pos_layV;
      Volume vol_layX, vol_layV; {
        xml_dim_t x_p = x_utb.position();
        Box       box(vboxWidth, vboxHeight, stationThick);
        Tube      tub(0.0, vholeRadius, stationThick*1.1);
        pos = Position(x_p.x(0), x_p.y(0), x_p.z(0));
        vol_utb = Volume("lvUTb", SubtractionSolid(box, tub), mat);
        vol_utb.setAttributes(description, x_utb.regionStr(), x_utb.limitsStr(), x_utb.visStr());
      }  {
        xml_comp_t x_X = x_utb.child(_U(X));
        xml_dim_t  x_p = x_X.position();
        Box        box(vboxWidth, vboxHeight, layerThick);
        Tube       tub(0.0, vholeRadius, layerThick*1.1);
        pos_layX = Position(x_p.x(0.0), x_p.y(0.0), x_p.z(0.0));
        vol_layX = Volume("lvUTbXLayer", SubtractionSolid(box, tub), mat);
        vol_layX.setVisAttributes(description, x_X.visStr());
      }  {
        xml_comp_t x_V = x_utb.child(_U(V));
        xml_dim_t  x_p = x_V.position();
        Box        box(vboxWidth, vboxHeight, layerThick);
        Tube       tub(layerThick, 0.0, vholeRadius*2.0);
        pos_layV = Position(x_p.x(0.0), x_p.y(0.0), x_p.z(0.0));
        vol_layV = Volume("lvUTbVLayer", SubtractionSolid(box, tub), mat);
        vol_layV.setVisAttributes(description, x_V.visStr());
      }
      vol_utb.placeVolume(vol_layX, pos_layX);
      vol_utb.placeVolume(vol_layV, pos_layV);
      cout << "UTB position: " << pos << endl;
      sdet_vol.placeVolume(vol_utb, pos);
    }
    if ( m_build_jacket )  { // Build  lvUTJacket
      Position pos;
      xml_comp_t     x_jacket        = x_env.child(_LBU(jacket));
      xml_dim_t      x_p             = x_jacket.position();
      double         coneJacketThick = x_params("ConeJacketThickness");
      double         cylJacketLength = x_params("CylJacketLength");
      double         cylJacketRadius = x_params("CylJacketRadius");
      double         cylJacketThick  = x_params("CylJacketThickness");
      vector<double> rmin{x_params("ConeJacketRadiusZmin"), x_params("ConeJacketRadiusZmax")};
      vector<double> rmax{rmin[0]+coneJacketThick, rmin[1]+coneJacketThick};
      vector<double> z{0,x_params("ConeJacketLength")};

      Tube           cyl (cylJacketRadius,cylJacketRadius+cylJacketThick,cylJacketLength);
      Polycone       con (0, 2.0*M_PI, rmin, rmax, z);
      Material       mat = description.material(x_jacket.materialStr());
      Volume         cyl_vol("lvUTCylJacket",  cyl, mat);
      Volume         con_vol("lvUTConeJacket", con, description.material(x_jacket.materialStr()));
      Assembly       jacket_vol("lvUTJacket");
      jacket_vol.placeVolume(cyl_vol, Position(0,0,x_params("CylJacketZpos")));
      jacket_vol.placeVolume(con_vol, Position(0,0,x_params("ConeJacketZpos")));
      sdet_vol.placeVolume(jacket_vol, Position(x_p.x(0), x_p.y(0), x_p.z(0)));      
    }
    { // Build  lvUX851InUT
    }
    
    sd.setType("tracker");
    PlacedVolume pv;
    Volume mother = description.pickMotherVolume(sdet);
    pv = mother.placeVolume(sdet_vol);
    if ( x_det.hasAttr(_U(id)) )  {
      pv.addPhysVolID("system",x_det.id());
    }
    sdet.setPlacement(pv);
    return sdet;
  }
}

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens)  {
  UTBuild b(description, e, sens);
  b.build_sensors();
  b.build_hybrids();
  b.build_modules();
  b.build_layers();
  DetElement sdet = b.instantiate();
  return sdet;
}
DECLARE_DETELEMENT(DD4hep_UUT,create_element)
  
