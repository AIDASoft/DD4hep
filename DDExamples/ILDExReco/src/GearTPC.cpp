// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A. Muennich
//  
//  Creating the functionality of GEAR::TPCParameters
//====================================================================

#include "GearTPC.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Shapes.h"
#include "TFile.h"
#include "TGeoTube.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include <iostream>
#include <limits>

using namespace std;

namespace DD4hep {
  using namespace Geometry;
  
  GearTPC::GearTPC(const Ref_t& e) : Geometry::DetElement(e) {
  }
  
  double GearTPC::getInnerRadius() const {
    // finding child by name not optimal, maybe better
    // to store the pointer in TPCData for quicker access
    DetElement innerWall   = child("TPC_InnerWall");
    if(!innerWall.isValid())
      throw OutsideGeometryException("Inner TPC wall not found!");
    Tube       tube  = innerWall.volume().solid();
    return tube->GetRmin();
  }
  double GearTPC::getOuterRadius() const {
    DetElement outerWall   = child("TPC_OuterWall");
    if(!outerWall.isValid())
      throw OutsideGeometryException("Outer TPC wall not found!");
    Tube       tube  = outerWall.volume().solid();
    return tube->GetRmax();
  }

  double GearTPC::getMaxDriftLength() const {
    DetElement gas   = child("TPC_GasVolume");
    if(!gas.isValid())
      throw OutsideGeometryException("TPC gas volume not found!");
    Tube       tube  = gas.volume().solid();
    return tube->GetDz();
  }

  double GearTPC::getEndPlateThickness(int endplate) const {
    DetElement ep=getEndPlate(endplate); 
    Tube       tube  = ep.volume().solid();
    return tube->GetDz();
  }

  double GearTPC::getEndPlateZPosition(int endplate) const {
    DetElement ep=getEndPlate(endplate);
    //find z position of endplate via the matrix of the placement
    TGeoMatrix *nm=ep.placements()[0]->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    return trans[2];
  }


  int GearTPC::getNModules(int endplate) const {
    //only works if the endplate has only modules as children
    //otherwise a name check is required
    DetElement ep=getEndPlate(endplate);
    return ep.children().size();;
  }

  void GearTPC::listDetElements() const {
    std::map<std::string,DetElement>::const_iterator it;
    std::map<std::string,DetElement>::const_iterator it2;
    for ( it=children().begin() ; it != children().end(); it++ )
      {
	std::cout << it->first << "  " << it->second._data().id <<" "<<it->second.placements().size()<<std::endl;
	for ( it2=it->second.children().begin() ; it2 != it->second.children().end(); it2++ )
	  std::cout <<"   "<< it2->first << "  " << it2->second._data().id << " "<<it2->second.placements().size()<<std::endl;
      }
  }
  
  TPCModule GearTPC::getModule(int ID, int endplate) const {
    //ID is defined in the compact xml
    DetElement ep=getEndPlate(endplate);
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
    DetElement ep=getEndPlate(endplate);
    double zpos=getEndPlateZPosition(endplate);
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
    TGeoNode *mynode=geoManager->FindNode(c0,c1,zpos);
    Double_t point[3];
    Double_t point_local[3];
    point[0]=c0;
    point[1]=c1;
    point[2]=zpos;
    //FIXME: careful: master is mother not global=world, input is in world coordinates
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
  
  
  TPCModule GearTPC::getNearestModule(double c0, double c1, int endplate) const{
    DetElement ep=getEndPlate(endplate);
    double zpos=getEndPlateZPosition(endplate);
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
    TGeoNode *mynode=geoManager->FindNode(c0,c1,zpos);
    Double_t point[3];
    Double_t point_local[3];
    point[0]=c0;
    point[1]=c1;
    point[2]=zpos;
   //FIXME: careful: master is mother not global=world, input is in world coordinates
    ep.placements()[0]->MasterToLocal(point, point_local);
    geoManager->SetCurrentPoint(point_local);
    bool onMod=false;
    std::map<std::string,DetElement>::const_iterator it;
    //check if any of the modules contains that point
    double safe_dist=numeric_limits<double>::max();
    TPCModule neighbour;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
 	Double_t point_local_node[3];
 	it->second.placements()[0]->MasterToLocal(point_local, point_local_node);
  	onMod=it->second.volume().solid()->Contains(point_local_node);

	if(onMod)
	  {
	    //std::cout<<"Point is on "<<it->second.volume()->GetName()<<" "<<it->second.volume().solid()->GetName()<<" id: "<<it->second.id()<<std::endl;
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
    // std::cout<<"MINIMUM: "<<safe_dist<<" for "<<neighbour.placements()[0]->GetName()<<" "<<neighbour.id()<<std::endl;
    return neighbour;
  }


  std::vector<TPCModule> GearTPC::getModules(int endplate) const {
    DetElement ep=getEndPlate(endplate);
     
    std::vector<TPCModule> allmods;
    std::map<std::string,DetElement>::const_iterator it;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
	allmods.push_back(it->second);
      }
    return allmods;
  }
  
 DetElement GearTPC::getEndPlate(int endplate) const {
    DetElement ep;
    if(endplate==1)
      ep= child("TPC_EndPlate_negativ");
    else
      ep= child("TPC_EndPlate");
    if(!ep.isValid())
      throw OutsideGeometryException("TPC endplate not found!");

    return ep;
  }
 
}
