//==========================================================================
// AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : Jongho.Lee
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens)  {
  xml_det_t  x_det  (e);
  xml_comp_t x_box      = x_det.envelope();
  xml_dim_t  pos        = x_det.position();
  string     det_name   = x_det.nameStr();
  DetElement det(det_name,x_det.id());
  //double     env_dz     .dz()/2.0;
  double     env_width  = x_box.width();
  double     env_length = x_box.length();
  double     env_height = x_box.height();
  // If the cone should be removed and replaced by an assembly,
  // comment the upper to and enable the lower line
  Assembly   env_vol(det_name);
  PlacedVolume pv;
  SensitiveDetector sd = sens;

  sd.setType("tracker");
  // Set visualization, limits and region (if present)
  env_vol.setRegion(description, x_det.regionStr());
  env_vol.setLimitSet(description, x_det.limitsStr());
  env_vol.setVisAttributes(description, x_det.visStr());
  printout(INFO,"CODEX-b box","%s width=%g length=%g height= %g",
           det_name.c_str(), env_width, env_length, env_height  );
 
  ///// Test for 1 layer into a CODEXb-box /////
  int type_st_num = 0;
  for(xml_coll_t i(x_det, _U(station)); i; ++i, ++type_st_num) {
    xml_comp_t station  = i;
    double    st_width  = station.width();
    double    st_thick  = station.thickness();
    double    st_height = station.height();
    double    st_x      = station.x()+pos.x()-env_width;
    double    st_y      = station.y()+pos.y();
    double    st_z      = station.z()+pos.z();
    int       st_repeat = station.repeat(); 
    double    st_gap       = station.gap();
    string    st_nam = _toString(type_st_num, "CODEX_station_type_%d");
    Box       st_box(st_thick, st_height, st_width);
    Material  st_mat(description.material(station.attr<string>(_U(material))));
    Volume    st_vol(st_nam,st_box,st_mat);
    st_vol.setVisAttributes(description, station.visStr());

    int type_la_num = 0;
    xml_comp_t layer       = x_det.child(_U(layer));
    double    la_width     = layer.width();
    double    la_thick     = layer.thickness();
    double    la_height    = layer.height();
    //double    la_gap       = layer.gap();
    double    la_x         = 0;
    double    la_y         = 0;
    double    la_z         = 0;
    int       la_repeat    = layer.repeat();
    double    la_pitch     = layer.distance();
    string    la_nam       = _toString(type_la_num, "CODEX_layer_type_%d");
    Box       la_box(la_thick, la_height, la_width);
    Material  la_mat(description.material(layer.attr<string>(_U(material))));
    Volume    la_vol(la_nam, la_box, la_mat);

    la_vol.setVisAttributes(description, layer.visStr());

    if( layer.isSensitive() ) {
      la_vol.setSensitiveDetector(sd);
    }

    la_x = -st_thick + la_pitch/2.0;
    vector<PlacedVolume> la_places;
    for(int k=0, layer_number=1; k < la_repeat; ++k, ++layer_number)   {
      pv = st_vol.placeVolume(la_vol,Position(la_x, la_y, la_z));
      pv.addPhysVolID("layer", layer_number);
      printout(INFO, "CODEX-b-layer"," Station: %s layer %d x=%g y=%g z=%g", st_vol.name(), layer_number, la_x, la_y, la_z);
      la_places.push_back(pv);
      la_x += la_pitch; 
    }
    
    for (int j=0, station_number=1; j < st_repeat; ++j, ++station_number )    {
      DetElement st_det(det, _toString(station_number, "Station%d"), x_det.id());
      pv = env_vol.placeVolume(st_vol, Position(st_x, st_y, st_z));
      pv.addPhysVolID("station", station_number);
      st_det.setPlacement(pv);
      printout(INFO, "CODEX-b-station"," Station: %d x=%g y=%g z=%g", station_number,st_x, st_y, st_z);
      for(int k=0, layer_number=1; k < la_repeat; ++k, ++layer_number)   {
        DetElement la_det(st_det, _toString(layer_number, "Layer%d"), x_det.id());
        la_det.setPlacement(la_places[k]);
      }
      st_x += st_gap; 
    } // station loop
  }

  if ( x_det.isSensitive() )   {   
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    env_vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
  }
  Volume mother = description.pickMotherVolume(det);
  //Transform3D trafo(Position(pos.x(),pos.y(),pos.z()));
  //Direction dir = trafo*Direction(0,0,1.);
  pv = mother.placeVolume(env_vol);
  if ( x_det.hasAttr(_U(id)) )  {
    pv.addPhysVolID("system",x_det.id());
  }
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(DD4hep_CODEXb_layer,create_element)
