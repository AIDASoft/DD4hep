//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank/M.Clemencic
//
//==========================================================================

// Include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"

#include <iostream>
#include <map>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace  {
  struct MyDetExtension  {
    int idD, Ni, Nj;
    double posDX, posDY, posDZ;
    double dimDX, dimDY, dimDZ;
    double pixelX, pixelY, pixelZ;
    DetElement detector;

    MyDetExtension(DetElement e) : idD(0), Ni(0), Nj(0),
        posDX(0.0), posDY(0.0), posDZ(0.0),
        dimDX(0.0), dimDY(0.0), dimDZ(0.0),
        pixelX(0.0), pixelY(0.0), pixelZ(0.0), detector(e) {}
    MyDetExtension(const MyDetExtension& e, DetElement d) 
      : idD(e.idD), Ni(e.Ni), Nj(e.Nj),
        posDX(e.posDX), posDY(e.posDY), posDZ(e.posDZ),
        dimDX(e.dimDX), dimDY(e.dimDY), dimDZ(e.dimDZ),
        pixelX(e.pixelX), pixelY(e.pixelY), pixelZ(e.pixelZ),
        detector(d)						       
    {
    }
  };
}
typedef MyDetExtension DetectorExtension;

static Ref_t create_detector(Detector &description, xml_h e, SensitiveDetector sens)  {
  xml_det_t x_det = e;	//xml-detelemnt of the detector taken as an argument
  string det_name = x_det.nameStr();	//det_name is the name of the xml-detelement
  Assembly assembly (det_name);
  int detectors_id = x_det.id();

  DetElement sdet(det_name,x_det.id());        //sdet is the detelement of the detector!!(actually is a Handle,already a pointer to m_element)
  DetectorExtension* ext = new MyDetExtension(sdet);
  sdet.addExtension<MyDetExtension>(ext);
  ext->idD= detectors_id;


  xml_coll_t mip(x_det, _U(position));
  xml_comp_t det_po = mip;

  double det_y = det_po.y();     // det_y is the y dimension of the xml-detelement
  double det_x = det_po.x();     // det_x is the x dimension of  the xml-detelement
  double det_z = det_po.z();     // det_z is the z dimension of the xml-detelement
  ext->posDY = det_y;
  ext->posDX = det_x;
  ext->posDZ = det_z;

  xml_coll_t dim(x_det, _U(dimensions));
  xml_comp_t det_dim = dim;
  double dim_x = det_dim.x();    // det_x is the x dimension of  the xml-detelement
  double dim_y = det_dim.y();    // det_y is the y dimension of the xml-detelement
  double dim_z = det_dim.z();    // det_z is the z dimension of the xml-detelement

  Material mat = description.material("Silicon");

  Volume motherVol = description.pickMotherVolume(sdet); //the mothers volume of our detector

  xml_coll_t mi(x_det, _U(module));
  xml_comp_t dtc_mod = mi;	    // considering the module-pixel of the detector
  double pixelX = dtc_mod.x();  // The x dimension of the module
  double pixelY = dtc_mod.y();  // The y dimension of the module
  double pixelZ = dtc_mod.z();  // The z dimension of the module

  int Ni = dim_x/pixelX;         // how many pixels in the x dimension
  int Nj = dim_y/pixelY;

  ext->dimDX = dim_x;
  ext->dimDY = dim_y;
  ext->dimDZ = dim_z;
  ext->pixelX = pixelX;
  ext->pixelY = pixelY;
  ext->pixelZ = pixelZ;
  ext->Ni= Ni;
  ext->Nj= Nj;


  for (int i= 0; i<Ni; i++){	//lets start from 100 and when getting the x dimensions of the detector, use num_pixels
    for (int j= 0; j<Nj; j++){
      //Position ppxl = Position(pixelX*i, pixelY*j, pixelZ);	//The local position of its pixel
      //mi_id = i+j +(Nj-1)*j ;   //finding the id of the pixel according to its' position
      //printout(INFO,det_name,"Giving to this pixel with position (%7.3f, %7.3f, %7.3f) id:%d",ppxl.x(),ppxl.y(),ppxl.z(),mi_id);
    }
  }
  Volume m_volume(det_name, Box(dim_x, dim_y, dim_z), mat);	//as parameters it needs name,solid,material
  m_volume.setVisAttributes(description.visAttributes(x_det.visStr()));	//I DONT MIND ABOUT THIS!
  m_volume.setLimitSet(description,x_det.limitsStr());
  m_volume.setRegion(description,x_det.regionStr());
  m_volume.setSensitiveDetector(sens);

  PlacedVolume pv1, pv2;
  // det_x,det_y,det_z are the dimensions of the detector in space
  pv1 = assembly.placeVolume(m_volume,Transform3D(Position(det_x,det_y,det_z)));
  if ( x_det.hasChild(_U(reflect)) )   {
    /// Reflect in XY-plane
    pv2 = assembly.placeVolume(m_volume,Transform3D(Rotation3D(1., 0., 0., 0., 1., 0., 0., 0., -1.),
                                                    Position(det_x,det_y,-det_z)));
  }
  xml_comp_t dtctr = x_det;
  if ( dtctr.isSensitive() ) {
    // Set volume attributes
    sens.setType("tracker");
    pv1.addPhysVolID("side",0);
    if ( pv2.isValid() )  {
      pv2.addPhysVolID("side",1);
    }
  }
  auto pv = motherVol.placeVolume(assembly);
  pv.addPhysVolID("system",detectors_id);
  sdet.setPlacement(pv);
  // Support additional test if Detector_InhibitConstants is set to TRUE
  description.constant<double>("world_side");
  return sdet;
}
DECLARE_DETELEMENT(MiniTelPixel,create_detector)
