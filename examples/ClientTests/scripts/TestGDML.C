#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoElement.h>
#include <TGeoMedium.h>
#include <TSystem.h>
#include <TGDMLWrite.h>
#include <TGeoSystemOfUnits.h>
#include <TGeant4SystemOfUnits.h>


int TestGDML()   {
  TGeoManager::LockDefaultUnits(false);

  //TGeoManager::SetDefaultUnits(TGeoManager::kRootUnits);
  //double unit_of_density = (TGeoUnit::g/TGeoUnit::cm3);
  //double gramm_per_mole  =  (TGeoUnit::g/TGeoUnit::mole);
  TGeoManager::SetDefaultUnits(TGeoManager::kG4Units);
  //double unit_of_density = (TGeant4Unit::g/TGeant4Unit::cm3);
  //double gramm_per_mole  =  (TGeant4Unit::g/TGeant4Unit::mole);
  
  TGeoManager::LockDefaultUnits(true);
  TGeoManager* mgr = new TGeoManager();
  TGeoElement* e = new TGeoElement("H","Hydro",1,3.0);
  e->Print();

  TGeoBBox*     world_shape = new TGeoBBox("WorldBox",100,100,100);
  TGeoMaterial* world_mat   = mgr->GetMaterial("Hydrogen-1");
  if ( !world_mat ) world_mat = new TGeoMaterial("Hydrogen-1", e, 1.0);
  world_mat->Print();
  TGeoMedium* world_med = new TGeoMedium(world_mat->GetName(), 1, world_mat);
  TGeoVolume* world_vol = new TGeoVolume("World", world_shape, world_med);

  TGeoBBox*     inner_shape = new TGeoBBox("InnerBox", 50, 50, 50);
  TGeoMaterial* inner_mat   = mgr->GetMaterial("Hydrogen-2");
  if ( !inner_mat ) inner_mat = new TGeoMaterial("Hydrogen-2", e, 5.0);
  inner_mat->Print();
  TGeoMedium* inner_med = new TGeoMedium(inner_mat->GetName(), 1, inner_mat);
  TGeoVolume* inner_vol = new TGeoVolume("Inner", inner_shape, inner_med);

  world_vol->AddNode(inner_vol, 1);
  mgr->SetTopVolume(world_vol);
  TGDMLWrite wr;
  wr.WriteGDMLfile(mgr, "Test.gdml");
  //world_vol->Draw("ogl");
  gSystem->Exit(0);
  return 0;
}
