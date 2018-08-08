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
  int        det_id     = x_det.id();
  string     det_name   = x_det.nameStr();
  DetElement det(det_name,x_det.id());
  //double     env_dz     .dz()/2.0;
  double     env_thick  = x_box.thickness();
  double     env_length = x_box.length();
  double     env_height = x_box.height();
  Box        env_box(env_thick/2.0,env_length,env_height);
  Volume     env_vol(det_name,env_box,description.air());
  PlacedVolume pv;
  SensitiveDetector sd = sens;

  sd.setType("tracker");
  // Set visualization, limits and region (if present)
  env_vol.setRegion(description, x_det.regionStr());
  env_vol.setLimitSet(description, x_det.limitsStr());
  env_vol.setVisAttributes(description, x_det.visStr());
  printout(INFO,"CODEX-b box","%s thickness=%g length=%g height= %g",
           det_name.c_str(), env_thick, env_length, env_height  );
 
  /// Build the single layer volume:
  int type_la_num = 0;
  xml_comp_t layer       = x_det.child(_U(layer));
  double    la_width     = layer.width();
  double    la_thick     = layer.thickness();
  double    la_height    = layer.height();
  //double    la_gap       = layer.gap();
  double    la_x         = 0;
  double    la_y         = 0;
  double    la_z         = 0;
  string    la_nam       = _toString(type_la_num, "CODEX_layer_type_%d");
  Box       la_box(la_thick, la_height, la_width);
  Material  la_mat(description.material(layer.attr<string>(_U(material))));
  Volume    la_vol(la_nam, la_box, la_mat);

  la_vol.setVisAttributes(description, layer.visStr());

  if( layer.isSensitive() ) {
    la_vol.setSensitiveDetector(sd);
  }

  /// Now we build the staions
  int type_st_num = 0;
  typedef std::map<Volume,std::vector<PlacedVolume> > Stations;
  Stations station_layers;
  vector<Volume> station_vols;
  for(xml_coll_t i(x_det, _U(station)); i; ++i) {
    xml_comp_t station  = i;
    xml_dim_t st_layers = station.child(_U(layers));
    int       st_N_lay  = st_layers.number();
    double    layer_gap = st_layers.gap();
    string    st_name   = station.nameStr();
    double    st_width  = station.width();
    double    layer_dist= layer_gap + 2.0*la_thick;
    double    st_thick  = (st_N_lay-1)*layer_dist/2.0 + layer_gap;
    double    st_height = station.height();
    string    st_nam    = _toString(type_st_num, "CODEX_sub_station_%d");
    Box       st_box(st_thick, st_height, st_width);
    Material  st_mat(description.material(station.attr<string>(_U(material))));
    Volume    st_vol(st_nam,st_box,st_mat);

    st_vol.setVisAttributes(description, station.visStr());
    station_vols.push_back(st_vol);
    la_x = -st_thick + layer_gap;
    for(int k=0; k < st_N_lay; ++k)   {
      pv = st_vol.placeVolume(la_vol,Position(la_x+la_box.x(), la_y, la_z));
      pv.addPhysVolID("layer", k+1);
      printout(INFO, "CODEX-b-layer"," Station: %s layer %d x=%g", st_vol.name(), k+1, la_x);
      station_layers[st_vol].push_back(pv);
      la_x += layer_dist;
    }
  }

  /// Now we build the super-staions
  Stations super_stations;
  int num_super = 0;
  vector<PlacedVolume> super_places;
  Box first_box;
  for(xml_coll_t i(x_det, _U(station)); i; ++i, ++num_super) {
    xml_comp_t station      = i;
    Volume     st_vol       = station_vols[num_super];
    double     st_dist      = station.distance();
    double     st_thick     = station.thickness();
    xml_dim_t  super_pos    = station.position();
    double     super_height = station.height();
    double     super_width  = station.width();
    int        super_repeat = station.repeat();
    double     super_thick  = st_dist/2.0*(super_repeat-1) + st_thick*super_repeat;
    Box        super_box(super_thick,super_width+30.0,super_height+30.0);
    Material   super_mat(description.material(station.attr<string>(_U(material))));
    Volume     super_vol(_toString(num_super,"Super%d"),super_box,super_mat);

    double     super_x = -super_thick + st_thick;
    for (int j=0; j < super_repeat; ++j )    {
      pv = super_vol.placeVolume(st_vol, Position(super_x, 0, 0));
      pv.addPhysVolID("station", j+1);
      printout(INFO, "CODEX-b-station"," Station: %d x=%g y=%g z=%g", j+1, super_x);
      super_stations[st_vol].push_back(pv);
      super_x += st_dist + 2.0*st_thick;
    }
    if ( !first_box.isValid() ) first_box = super_box;
    pv = env_vol.placeVolume(super_vol,Position(super_pos.x()+first_box.x()-env_thick/2.0,super_pos.y(),super_pos.z()));
    pv.addPhysVolID("super", num_super+1);
    super_places.push_back(pv);
  }

  // Here we build the structural hierarchy of the detector elements
  num_super = 0;
  for(xml_coll_t i(x_det, _U(station)); i; ++i, ++num_super) {
    xml_comp_t station      = i;
    int        super_repeat = station.repeat();
    Volume     st_vol       = station_vols[num_super];
    DetElement super_de(det,_toString(num_super,"DeSuper%d"),det_id);
    super_de.setPlacement(super_places[num_super]);
    for (int j=0; j < super_repeat; ++j )    {
      DetElement station_de(super_de,_toString(j,"DeStation%d"),det_id);
      PlacedVolume station_pv = super_stations[st_vol][j];
      xml_dim_t st_layers = station.child(_U(layers));
      int       st_N_lay  = st_layers.number();
      station_de.setPlacement(station_pv);
      for(int k=0; k < st_N_lay; ++k)   {
        DetElement layer_de(station_de,_toString(k,"DeLayer%d"),det_id);
        PlacedVolume layer_pv = station_layers[st_vol][k];
        layer_de.setPlacement(layer_pv);
      }
    }
  }
  
  Volume mother = description.pickMotherVolume(det);
  xml_dim_t  pos        = x_det.position();
  Transform3D trafo(Position(pos.x(),pos.y(),pos.z()));
  //Direction dir = trafo*Direction(0,0,1.);
  pv = mother.placeVolume(env_vol, trafo);
  if ( x_det.hasAttr(_U(id)) )  {
    pv.addPhysVolID("system",x_det.id());
  }
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(DD4hep_CODEXb_detector,create_element)
