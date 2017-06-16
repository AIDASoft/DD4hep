//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#include "DDRec/DetectorData.h"

#include "DD4hep/DetFactoryHelper.h"

#include "XML/Layering.h"
#include "XML/Utilities.h"


#include "TGeoTrd2.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t     x_det     = e;
  int           det_id    = x_det.id();
  string        det_name  = x_det.nameStr();
  DetElement    sdet      (det_name,det_id);


  // --- create an envelope volume and position it into the world ---------------------

  Volume envelope = xml::createPlacedEnvelope( description,  e , sdet ) ;
  xml::setDetectorTypeFlag( e, sdet ) ;

  if( description.buildType() == BUILD_ENVELOPE ) return sdet ;

  //-----------------------------------------------------------------------------------


  Material air = description.air();
  PlacedVolume pv;
  int n = 0;

  //added code by Thorben Quast for event display
  rec::LayeredCalorimeterData* solenoidData = new rec::LayeredCalorimeterData;
  solenoidData->inner_symmetry = 0;
  solenoidData->outer_symmetry = 0;
  solenoidData->layoutType = rec::LayeredCalorimeterData::BarrelLayout ;

  double inner_radius= std::numeric_limits<double>::max();
  double outer_radius= 0;

  double detZ= 0.;

  for(xml_coll_t i(x_det,_U(layer)); i; ++i, ++n)  {
    xml_comp_t x_layer = i;
    string  l_name = det_name+_toString(n,"_layer%d");
    double  z    = x_layer.outer_z();
    double  rmin = x_layer.inner_r();
    double  r    = rmin;
    DetElement layer(sdet,_toString(n,"layer%d"),x_layer.id());
    Tube    l_tub (rmin,2*rmin,z);
    Volume  l_vol(l_name,l_tub,air);
    int m = 0;


    for(xml_coll_t j(x_layer,_U(slice)); j; ++j, ++m)  {
      xml_comp_t x_slice = j;
      Material mat = description.material(x_slice.materialStr());
      string s_name= l_name+_toString(m,"_slice%d");
      double thickness = x_slice.thickness();

      //NN: These probably need to be fixed and ced modified to read the extent, rather than the layer
      //added code by Thorben Quast for event display
      rec::LayeredCalorimeterData::Layer solenoidLayer;
      solenoidLayer.distance = r;

      solenoidLayer.inner_thickness = thickness/2.;
      solenoidLayer.outer_thickness = solenoidLayer.inner_thickness ;

      solenoidLayer.cellSize0 = 0;    //equivalent to
      solenoidLayer.cellSize1 = z;    //half extension along z-axis
      solenoidData->layers.push_back(solenoidLayer);

      Tube   s_tub(r,r+thickness,z,2*M_PI);
      Volume s_vol(s_name, s_tub, mat);

      r += thickness;
      if ( x_slice.isSensitive() ) {
	sens.setType("tracker");
	s_vol.setSensitiveDetector(sens);
      }
      // Set Attributes
      s_vol.setAttributes(description,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
      pv = l_vol.placeVolume(s_vol);
      // Slices have no extra id. Take the ID of the layer!
      pv.addPhysVolID("slice",m);
    }
    l_tub.setDimensions(rmin,r,z);

    if (rmin < inner_radius)
      inner_radius = rmin;

    if (z>detZ)
      detZ = z;

    if (r>outer_radius)
      outer_radius = r;

    l_vol.setVisAttributes(description,x_layer.visStr());

    pv = envelope.placeVolume(l_vol);
    pv.addPhysVolID("layer",n);
    layer.setPlacement(pv);
  }
  if ( x_det.hasAttr(_U(combineHits)) ) {
    sdet.setCombineHits(x_det.combineHits(),sens);
  }

  /// extent of the calorimeter in the r-z-plane [ rmin, rmax, zmin, zmax ] in mm.
  solenoidData->extent[0] = inner_radius ;
  solenoidData->extent[1] = outer_radius ;
  solenoidData->extent[2] = 0. ;
  solenoidData->extent[3] = detZ;

  //added code by Thorben Quast for event display
  sdet.addExtension< rec::LayeredCalorimeterData >( solenoidData ) ;

  return sdet;

}

DECLARE_DETELEMENT(dd4hep_Solenoid_o1_v01,create_detector)
