#include "TGeoManager.h"
#include "TGeoVolume.h"

void test_shape()  {
  Double_t pars[] = {0,0,0,0,0,0,0,0,0,0,0,0};
  TGeoManager*  mgr=new TGeoManager();
  TGeoBBox*     world = new TGeoBBox("World",200,200,200);
  TGeoMaterial* mat_world = new TGeoMaterial("Air");
  TGeoMedium*   med_world = new TGeoMedium("Air",2,mat_world,pars);
  TGeoVolume*   vol_world = new TGeoVolume("World",world,med_world);

  //TGeoConeSeg*  tr        = new TGeoConeSeg("Tracker",60,5,60, 5,60, 0,360);
  TGeoTubeSeg*  tr        = new TGeoTubeSeg("Tracker",5,60, 60, 0,360);
  TGeoMaterial* mat_tr    = new TGeoMaterial("Iron");
  TGeoMedium*   med_tr    = new TGeoMedium("Iron",1,mat_tr,pars);
  TGeoVolume*   vol_tr    = new TGeoVolume("tracker",tr,med_tr);

  vol_world->AddNode(vol_tr,1);
  mgr->SetTopVolume(vol_world);
  mgr->CloseGeometry();
  vol_world->Draw("ogl");
}

