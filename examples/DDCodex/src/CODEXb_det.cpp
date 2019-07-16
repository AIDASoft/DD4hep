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
  //int type_la_num = 0;
  //xml_comp_t layer       = x_det.child(_U(layer));
  //double    la_width     = layer.width();
  //double    la_thick     = layer.thickness();
  //double    la_height    = layer.height();
  ////double    la_gap       = layer.gap();
  //double    la_x         = 0;
  //double    la_y         = 0;
  //double    la_z         = 0;
  //string    la_nam       = _toString(type_la_num, "CODEX_layer_type_%d");
  //Box       la_box(la_thick, la_height, la_width);
  //Material  la_mat(description.material(layer.attr<string>(_U(material))));
  //Volume    la_vol(la_nam, la_box, la_mat);

  //la_vol.setVisAttributes(description, layer.visStr());

  //if( layer.isSensitive() ) {
  //  la_vol.setSensitiveDetector(sd);
  //}

  /// Now we build the staions
  int type_st_num = 0;
  typedef std::map<Volume,std::vector<PlacedVolume> > Stations;
  Stations station_layers;
  vector<Volume> station_vols;
  for(xml_coll_t i(x_det, _U(station)); i; ++i) {
    xml_comp_t station  = i;
    //xml_dim_t st_layers = station.child(_U(layers));
    xml_comp_t st_layers = station.child(_U(layers));
    int        st_N_lay  = st_layers.number();
    double     layer_gap = st_layers.gap();
    double     la_width  = st_layers.width();
    double     la_thick  = st_layers.thickness();
    double     la_height = st_layers.height();
    double     la_x      = 0;
    double     la_y      = 0;
    double     la_z      = 0;
    string     la_nam    = st_layers.nameStr();
    Box        la_box(la_thick, la_height, la_width);
    Material   la_mat(description.material(st_layers.attr<string>(_U(material))));
    Volume     la_vol(la_nam, la_box, la_mat);

    string     st_name   = station.nameStr();
    double     st_width  = station.width();
    double     layer_dist= layer_gap;
    double     st_thick  = (st_N_lay-1)*layer_dist/2.0 + layer_gap;
    double     st_height = station.height();
    string     st_nam    = _toString(type_st_num, "CODEX_sub_station_%d");
    Box        st_box(st_thick, st_height, st_width);
    Volume     st_vol(st_name,st_box,description.air());

    la_vol.setVisAttributes(description, st_layers.visStr());
    if( st_layers.isSensitive() ) {
      cout << "Station:" << st_name << " layer:" << la_nam << endl;
      la_vol.setSensitiveDetector(sd);
    }
    printout(INFO, "CODEX-b station size"," Thick: % g, width: % g, height: % g", st_thick, st_width, st_height);

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
  for(xml_coll_t i(x_det, _U(station)); i; ++i, ++num_super) {
    xml_comp_t station      = i;
    Volume     st_vol       = station_vols[num_super];
    double     st_dist      = station.distance();
    double     st_thick     = station.thickness();
    xml_dim_t  super_pos    = station.position();
    double     super_height = station.height();
    double     super_width  = station.width();
    int        super_repeat = station.repeat();
    double     super_angle  = station.angle(); 
    double     super_thick  = st_dist/2.0*(super_repeat-1) + st_thick*super_repeat;
    string     super_name   = station.nameStr();
    //Box        super_box(super_width+50.0,super_width+50.0,super_height+50.0);
    //Volume     super_vol(_toString(num_super,"Super%d"),super_box,description.air());
    Assembly   super_vol(_toString(num_super,"Super%d"));

    printout(INFO, "CODEX-b super station size"," Thick: % g, width: % g, height: % g", super_thick, super_width+30.0, super_height+30.0);
    double     face_x = -super_height;
    double     super_x = -super_thick + st_thick;
    double     inner_x = -st_dist*((super_repeat-1)/2.0); 
    for (int j=0; j < super_repeat; ++j )    {
      if( super_name.compare("face_station") == 0 && j < 2 ) { 
          Transform3D trafo(RotationZYX(0,0,0),Position(face_x, 0, 0)); 
          pv = super_vol.placeVolume(st_vol,trafo); 
      }
      if( super_name.compare("face_station") == 0 && j >=2 && j < 4 ) { 
          if( j == 2 ) face_x = -super_height; 
          Transform3D trafo(RotationZYX(0,super_angle,0),Position(0, 0, face_x)); 
          pv = super_vol.placeVolume(st_vol,trafo); 
      }
      if( super_name.compare("face_station") == 0 && j >=4 && j < 6 ) { 
          if( j == 4 ) face_x = -super_height; 
          Transform3D trafo(RotationZYX(super_angle,0,0),Position(0, face_x, 0)); 
          pv = super_vol.placeVolume(st_vol,trafo); 
      }  
      if( super_name.compare("inner_station") == 0 )   {
        pv = super_vol.placeVolume(st_vol, Position(inner_x, 0, 0));
      }
      //pv = super_vol.placeVolume(st_vol, Position(super_x, 0, 0));
      pv.addPhysVolID("station", j+1);
      printout(INFO, "CODEX-b-station"," Station: %d x=%g y=%g z=%g", j+1, inner_x);
      super_stations[st_vol].push_back(pv);
      super_x += st_dist + 2.0*st_thick;
      //inner_x += st_dist + 2.0*st_thick;
      inner_x += st_dist;
      face_x += 2.0*super_height + st_thick/10.;
    }
    pv = env_vol.placeVolume(super_vol,Position(super_pos.x(),super_pos.y(),super_pos.z()));
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
  xml_dim_t  pos = x_det.position();
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
