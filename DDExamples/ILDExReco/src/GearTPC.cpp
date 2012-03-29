// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "GearTPC.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Shapes.h"
#include "TGeoTube.h"
#include "TGeoManager.h"
#include "DDTPCEndPlate.h"
#include <iostream>

using namespace std;

namespace DD4hep {
  using namespace Geometry;
  
  GearTPC::GearTPC(const Ref_t& e) : Geometry::DetElement(e) {
  }
  
  double GearTPC::getInnerRadius() const {
    DetElement innerWall   = child("TPC_InnerWall");
    Tube       tube  = innerWall.volume().solid();
    return tube->GetRmin();
  }
  double GearTPC::getOuterRadius() const {
    DetElement outerWall   = child("TPC_OuterWall");
    Tube       tube  = outerWall.volume().solid();
    return tube->GetRmax();
  }

  double GearTPC::getMaxDriftLength() const {
    DetElement gas   = child("TPC_GasVolume");
    Tube       tube  = gas.volume().solid();
    return tube->GetDz();
  }

  double GearTPC::getEndPlateThickness() const {
    DetElement ep   = child("TPC_EndPlate");
    Tube       tube  = ep.volume().solid();
    return tube->GetDz();
  }


  int GearTPC::getNModules(int endplate) const {
    string name;
    if(endplate==1)
      name="TPC_EndPlate_negativ";
    else
      name="TPC_EndPlate";
    
    DDTPCEndPlate ep(child(name));
    
    return ep.getNModules();
  }

  void GearTPC::listDetElements() const {
    std::map<std::string,DetElement>::const_iterator it;
    std::map<std::string,DetElement>::const_iterator it2;
    for ( it=children().begin() ; it != children().end(); it++ )
      {
	std::cout << it->first << "  " << it->second._data().id << std::endl;
	for ( it2=it->second.children().begin() ; it2 != it->second.children().end(); it2++ )
	  std::cout <<"   "<< it2->first << "  " << it2->second._data().id << std::endl;
      }
  }
  
  const DDTPCModule & GearTPC::getModule(int ID, int endplate) const {
    DetElement ep;
    if(endplate==1)
      ep= child("TPC_EndPlate_negativ");
    else
      ep= child("TPC_EndPlate");
    
    string myname;
    std::map<std::string,DetElement>::const_iterator it;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
	if(it->second._data().id==ID)
	  {
	    myname= it->first;
	    break;
	  }
      }
    return ep.child(myname);

  }
  
  bool GearTPC::isInsideModule(double c0, double c1, int endplate) const{
    DetElement ep;
    if(endplate==1)
      ep= child("TPC_EndPlate_negativ");
    else
      ep= child("TPC_EndPlate");
    Tube       tube  = ep.volume().solid();
    //want to use something like:  TGeoNode * FindNode(Double_t x, Double_t y, Double_t z) from TGeoManager
 //    TGeoVolume *rootVolume=ep.volume().TGeoVolume*();
//     TGeoManager *geoManager->GetGeoManager();
//     TGeoNode *mynode=geoManager->FindNode(c0,c1,tube->GetDz());
    return false;
  }
}
