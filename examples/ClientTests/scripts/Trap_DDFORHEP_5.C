#include "TGeoManager.h"
#include "TGeoArb8.h"
#include "TGeoVolume.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include <iostream>

void Trap_DDFORHEP_5(int arg=0)   {
  TGeoManager* mgr = gGeoManager;
  if ( !mgr ) mgr = new TGeoManager();

  TGeoVolume* world = new TGeoVolume("World",new TGeoBBox(30,30,30),mgr->GetMedium("Air"));
  TGeoBBox* box = new TGeoBBox("World",10, 10, 1);
  TGeoShape* hole_shape = arg 
    ? new TGeoTrap(4.2, 0., 0., 2., 4.,  3., 0., 2., 4., 3., 0.) 
    // Ill defined trap:  ? new TGeoTrap(4.2, 0., 0., 2., 4.,  3., 0., 0., 2., 4., 3.) 
    : new TGeoBBox(3, 3, 1.01);
  // Construct subtraction
  TGeoSubtraction* sub = new TGeoSubtraction(box, hole_shape, gGeoIdentity, gGeoIdentity);
  TGeoCompositeShape* comp = new TGeoCompositeShape("composite",sub);
  TGeoVolume* vol = new TGeoVolume("Test_composites",comp,mgr->GetMedium("Iron"));

  world->AddNode(vol,1);

  // Show the hole as a seperate volume in the world
  TGeoVolume* hole_seperate = new TGeoVolume("Hole",hole_shape,mgr->GetMedium("Iron"));
  world->AddNode(hole_seperate,2,new TGeoTranslation(20,0,0));

  std::cout << "Test subtraction of a " << hole_shape->IsA()->GetName() 
	    << " shape from a box." << std::endl;

  // Close geometry and display
  mgr->SetTopVolume(world);
  mgr->CloseGeometry();
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  mgr->GetTopNode()->Draw("ogl");
}
