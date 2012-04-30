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
#include "TFile.h"
#include "TGeoTube.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "DDTPCEndPlate.h"
#include <iostream>
#include <limits>

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
  
  DDTPCModule GearTPC::getModule(int ID, int endplate) const {
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
    
    //find z position of endplate
    TGeoMatrix *nm=ep.placements()[0]->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    double zpos=trans[2];
       
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
    TGeoNode *mynode=geoManager->FindNode(c0,c1,zpos);
    Double_t point[3];
    Double_t point_local[3];
    point[0]=c0;
    point[1]=c1;
    point[2]=zpos;
    ep.placements()[0]->MasterToLocal(point, point_local);
    
    bool onMod=false;
    std::map<std::string,DetElement>::const_iterator it;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
	Double_t point_local_node[3];
 	it->second.placements()[0]->MasterToLocal(point_local, point_local_node);
	onMod=it->second.volume().solid()->Contains(point_local_node);
	if(onMod)
	  {
	    std::cout<<"Point is on "<<it->second.volume()->GetName()<<" id: "<<it->second.id()<<std::endl;
	    return onMod;
	  }
      }
    return onMod;
  }
  
  
  DDTPCModule GearTPC::getNearestModule(double c0, double c1, int endplate) const{
    DetElement ep;
    if(endplate==1)
      ep= child("TPC_EndPlate_negativ");
    else
      ep= child("TPC_EndPlate");
   
    //find z position of endplate
    TGeoMatrix *nm=ep.placements()[0]->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    double zpos=trans[2];
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
    TGeoNode *mynode=geoManager->FindNode(c0,c1,zpos);
    Double_t point[3];
    Double_t point_local[3];
    point[0]=c0;
    point[1]=c1;
    point[2]=zpos;
    ep.placements()[0]->MasterToLocal(point, point_local);
    geoManager->SetCurrentPoint(point_local);
    bool onMod=false;
    std::map<std::string,DetElement>::const_iterator it;
    //check if any of the modules contains that point
    double safe_dist=numeric_limits<double>::max();
    DDTPCModule neighbour;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
 	Double_t point_local_node[3];
 	it->second.placements()[0]->MasterToLocal(point_local, point_local_node);
  	onMod=it->second.volume().solid()->Contains(point_local_node);

	if(onMod)
	  {
	    std::cout<<"Point is on "<<it->second.volume()->GetName()<<" "<<it->second.volume().solid()->GetName()<<" id: "<<it->second.id()<<std::endl;
	    return it->second;
	  }
	
	//if not on module, compute distance from point to each shape
	//FIXME: not sure if this is exact. Sometimes more than one module has the same safety distance.
	Tube       tube  = it->second.volume().solid();
	double dist=tube->Safety(point_local_node,0);
	if(dist<safe_dist)
	  {
	    safe_dist=dist;
	    neighbour=it->second;
	  }
      }
    std::cout<<"MINIMUM: "<<safe_dist<<" for "<<neighbour.placements()[0]->GetName()<<" "<<neighbour.id()<<std::endl;
    return neighbour;
  }
  
}
