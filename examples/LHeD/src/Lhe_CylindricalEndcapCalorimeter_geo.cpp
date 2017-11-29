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
#include "XML/Layering.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t  x_det     = e;
  xml_dim_t  dim       = x_det.dimensions();
  Material   air       = description.air();
  string     det_name  = x_det.nameStr();
  bool       reflect   = x_det.reflect();
  double     zmin      = dim.inner_z();
  double     rmin      = dim.inner_r();
  double     rmax      = dim.outer_r();
  double     totWidth  = Layering(x_det).totalThickness();
  double     z         = zmin;
  Tube       envelope   (rmin,rmax,totWidth/2);
  Volume     envelopeVol(det_name+"_envelope",envelope,air);
  int        layer_num = 1;
  PlacedVolume pv;

  // Set attributes of slice
  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    double layerWidth = 0;
    for(xml_coll_t l(x_layer,_U(slice)); l; ++l)
      layerWidth += xml_comp_t(l).thickness();
    for(int i=0, im=0, repeat=x_layer.repeat(); i<repeat; ++i)  {
      double     zlayer = z;
      string     layer_name = det_name + _toString(layer_num,"_layer%d");
      Volume     layer_vol(layer_name,Tube(rmin,rmax,layerWidth),air);

      for(xml_coll_t l(x_layer,_U(slice)); l; ++l, ++im)  {
        xml_comp_t x_slice = l;
        double     w = x_slice.thickness();
        string     slice_name = layer_name + _toString(m+1,"slice%d");
        Material   slice_mat  = description.material(x_slice.materialStr());
        Volume     slice_vol (slice_name,Tube(rmin,rmax,w),slice_mat);

        if ( x_slice.isSensitive() )  {
          sens.setType("calorimeter");
          slice_vol.setSensitiveDetector(sens);
        }
        slice_vol.setAttributes(description,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
        pv = layer_vol.placeVolume(slice_vol,Position(0,0,z-zlayer-layerWidth/2+w/2));
        pv.addPhysVolID("slice",m+1);
        z += w;
      }
      layer_vol.setVisAttributes(description,x_layer.visStr());

      Position layer_pos(0,0,zlayer-zmin-totWidth/2+layerWidth/2);
      pv = envelopeVol.placeVolume(layer_vol,layer_pos);
      pv.addPhysVolID("layer",layer_num);
      printout(DEBUG,"Calo","CylindricalEndcapCalorimeter: built layer %d -> %s",layer_num,layer_name.c_str());
      ++layer_num;
    }
  }
  envelope.setDimensions(rmin,rmax,totWidth/2);
  // Set attributes of slice
  envelopeVol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  DetElement   sdet(det_name,x_det.id());
  Assembly     assembly(det_name+"_assembly");
  Volume       motherVol = description.pickMotherVolume(sdet);
  PlacedVolume phv = motherVol.placeVolume(assembly);
  phv.addPhysVolID("system",sdet.id());
  sdet.setPlacement(phv);

  DetElement   sdetA(sdet,det_name+(reflect ? "_A" : ""),x_det.id());
  phv = assembly.placeVolume(envelopeVol,Position(0,0,zmin+totWidth/2));
  phv.addPhysVolID("barrel",1);
  sdetA.setPlacement(phv);

  if ( reflect )   {
    phv=assembly.placeVolume(envelopeVol,Transform3D(RotationZ(M_PI),Position(0,0,-zmin-totWidth/2)));
    phv.addPhysVolID("barrel",2);
    /// Create the detector element for the opposite side....
    DetElement   sdetB(sdet,det_name+"_B",x_det.id());
    sdetB.setPlacement(phv);
  }
  return sdet;
}

DECLARE_DETELEMENT(Lhe_CylindricalEndcapCalorimeter,create_detector)
