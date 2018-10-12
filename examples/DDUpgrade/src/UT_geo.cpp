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
#include "XML/Utilities.h"
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
    std::map<std::string,Volume> m_volumes;
    std::map<std::string,Volume> m_modules;
    Volume            vol_group_norm, vol_hybrid_norm;
    Volume            vol_group_dual, vol_hybrid_dual;
    Volume            vol_group_quad, vol_hybrid_quad;
    Volume            vol_moduleA, vol_moduleB, vol_moduleC;
    Volume            vol_frame, vol_box;
    Volume            vol_uta, vol_utb;
    double            beamHoleRadius;

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
    void build_volumes();
    void build_jacket(Volume parent_vol, xml_comp_t x_jacket);
    void build_station(DetElement sdet, xml_comp_t x_station);
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

  void UTBuild::build_volumes()   {
    for(xml_coll_t ivol(x_det.child(_U(volumes)),_U(module)); ivol; ++ivol)  {
      Volume volume;
      xml_comp_t x_vol = ivol;
      xml_comp_t x_shape = x_vol.child(_U(solid), false);
      if ( x_shape.ptr() )   {
        Solid solid = xml::createShape(description, x_shape.typeStr(), x_shape);
        Material mat = description.material(x_vol.materialStr());
        volume = Volume(x_vol.nameStr(), solid, mat);
      }
      else  {
        volume = Assembly(x_vol.nameStr());
      }
      m_volumes[x_vol.nameStr()] = volume;
    }
#if 0
    for(xml_coll_t ivol(x_det.child(_U(volumes)),_U(module)); ivol; ++ivol)  {
      volume.setAttributes(description, x_vol.regionStr(), x_vol.limitsStr(), x_vol.visStr());
      for(xml_coll_t i(ivol,_U(physvol)); i; ++i)  {
        xml_comp_t x_phys = i;
        
      }
      m_volumes[x_vol.nameStr()] = volume;
    }
#endif
  }    
  
  void UTBuild::build_layers()   {

  }

  void UTBuild::build_jacket(Volume parent_vol, xml_comp_t x_jacket)   {
    xml_dim_t      x_pos = x_jacket.position();
    Assembly       jacket_vol("lvUTJacket");
    for(xml_coll_t ivol(x_jacket,_U(physvol)); ivol; ++ivol)  {
      xml_dim_t  x_v = ivol;
      xml_dim_t  x_p = x_v.position();
      Volume     vol = m_volumes[x_v.nameStr()];
      jacket_vol.placeVolume(vol, Position(x_p.x(0), x_p.y(0), x_v.z(0)));
    }
    parent_vol.placeVolume(jacket_vol, Position(x_pos.x(0), x_pos.y(0), x_pos.z(0)));      
  }
  
  void UTBuild::build_station(DetElement det, xml_comp_t x_station)  {
    Material   mat   = description.material(x_station.materialStr());
    Volume     station_vol;
    Position   station_pos;
    {
      xml_dim_t x_solid = x_station.child(_U(envelope));
      xml_dim_t x_box   = x_solid.child(_U(box));
      xml_dim_t x_tub   = x_solid.child(_U(tube));
      xml_dim_t x_pos   = x_station.position();
      Box       box(x_box.x(), x_box.y(), x_box.z());
      Tube      tub(x_tub.rmin(0.0), x_tub.rmax(), x_tub.z());
      if ( x_pos ) station_pos = Position(x_pos.x(0), x_pos.y(0), x_pos.z(0));
      station_vol = Volume("lv"+x_station.nameStr(), SubtractionSolid(box, tub), mat);
      station_vol.setAttributes(description, x_station.regionStr(), x_station.limitsStr(), x_station.visStr());
    }
    for(xml_coll_t ilayer(x_station.child(_U(layers)),_U(layer)); ilayer; ++ilayer)  {
      xml_comp_t x_layer   = ilayer;
      xml_dim_t  x_solid   = x_layer.child(_U(envelope));
      xml_dim_t  x_p       = x_layer.position();
      xml_dim_t  x_box     = x_solid.child(_U(box));
      xml_dim_t  x_tub     = x_solid.child(_U(tube));
      xml_dim_t  x_modules = x_layer.child(_U(modules));
      xml_comp_t x_balcony = x_layer.child(_LBU(balcony));
      xml_dim_t  x_balcony_box = x_balcony.child(_U(box));
      //xml_dim_t  x_balcony_par = x_balcony.child(_U(params));

      string     layer_nam = x_layer.nameStr();
      Box        layer_box(x_box.x(), x_box.y(), x_box.z());
      Tube       layer_tub(x_tub.rmin(0.0), x_tub.rmax(), x_tub.z());
      Volume     layer_vol("lv"+layer_nam, SubtractionSolid(layer_box, layer_tub), mat);
      Position   layer_pos(x_p.x(0.0), x_p.y(0.0), x_p.z(0.0));

      Material   balcony_mat = description.material(x_balcony.materialStr());
      Box        balcony_box(x_balcony_box.x(0.0), x_balcony_box.y(0.0), x_balcony_box.z(0.0));
      Volume     balcony_vol(x_balcony.nameStr(),balcony_box,balcony_mat);

      layer_vol.setVisAttributes(description, x_layer.visStr());
      balcony_vol.setVisAttributes(description, x_balcony.visStr());
      for(xml_coll_t ibalc(x_balcony,_U(position)); ibalc; ++ibalc)  {
        xml_dim_t  x_pos = ibalc;
        layer_vol.placeVolume(balcony_vol,Position(x_pos.x(0.0), x_pos.y(0.0), x_pos.z(0.0)));
      }
      for(xml_coll_t imodule(x_modules,_U(module)); imodule; ++imodule)  {
        xml_comp_t x_mod = imodule;
        xml_dim_t  x_pos = x_mod.position();
        xml_dim_t  x_rot = x_mod.child(_U(rotation),false);
        string     typ   = x_mod.typeStr();
        Volume     vol   = m_modules[typ];
        if ( x_pos.ptr() && x_rot.ptr() )   {
          Position    p(x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0));
          RotationZYX r(x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0));
          layer_vol.placeVolume(vol, Transform3D(r,p));
        }
        else if ( x_rot.ptr() )  {
          RotationZYX r(x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0));
          layer_vol.placeVolume(vol, r);
        }
        else if ( x_pos.ptr() )   {
          layer_vol.placeVolume(vol, Position(x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0)));
        }
        else   {
          layer_vol.placeVolume(vol);
        }
      }
      station_vol.placeVolume(layer_vol, layer_pos);
    }
    det.volume().placeVolume(station_vol, station_pos);
  }
  
  DetElement UTBuild::instantiate()   {
    Volume  sdet_vol;//WRONG: (x_env.nameStr(), m_volumes["lvUT"], description.air());
    /*
     *   See DDDB/UUT/geometry.xml for the structure
     */
    if ( m_build_frame ) { // Build  lvUTFrame
      sdet_vol.placeVolume(m_volumes["lvUTFrame"]);
    }
    if ( m_build_box )  { // Build  lvUTBox
      Position pos;
      sdet_vol.placeVolume(m_volumes["lvUTBox"], pos);
    }
    if ( m_build_uta )   { // Build  lvUTa
      build_station(sdet, x_env.child(_LBU(uta)));
    }
    if ( m_build_utb )   { // Build  lvUTb
      build_station(sdet, x_env.child(_LBU(utb)));
    }

    if ( m_build_jacket )  { // Build  lvUTJacket
      build_jacket(sdet_vol, x_env.child(_LBU(jacket)));
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
  b.build_volumes();
  b.build_sensors();
  b.build_hybrids();
  b.build_modules();
  b.build_layers();
  DetElement sdet = b.instantiate();
  return sdet;
}
DECLARE_DETELEMENT(DD4hep_UT,create_element)
