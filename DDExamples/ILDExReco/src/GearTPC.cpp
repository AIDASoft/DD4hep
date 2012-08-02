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
    //defined as distance between cathode and endplate surface
    //for now assume symmetric setups, meaining if there are two endplates
    //they are symmetric around the cathode
    DetElement cathode   = child("TPC_Cathode");
    if(!cathode.isValid())
      throw OutsideGeometryException("TPC cathode not found!");
    //positions in z
    TGeoMatrix *nm=cathode.placement()->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    double z_ep=getEndPlateZPosition(0);
    double z_cath=trans[2];
    //thickness in z
    double th_ep=getEndPlateThickness(0);
    Tube       tube_cathode=cathode.volume().solid();
    double th_cath=tube_cathode->GetDz();
    return fabs(z_ep-z_cath)-(th_ep+th_cath);
  }

  double GearTPC::getEndPlateThickness(int endplate) const {
    DetElement ep=getEndPlate(endplate); 
    Tube       tube  = ep.volume().solid();
    return tube->GetDz();
  }

  double GearTPC::getEndPlateZPosition(int endplate) const {
    DetElement ep=getEndPlate(endplate);
    //find z position of endplate via the matrix of the placement
    TGeoMatrix *nm=ep.placement()->GetMatrix();
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
	std::cout << it->first << "  " << it->second._data().id <<" "<<std::endl;
	for ( it2=it->second.children().begin() ; it2 != it->second.children().end(); it2++ )
	  std::cout <<"   "<< it2->first << "  " << it2->second._data().id << " "<<it2->second.placement().isValid()<<std::endl;
      }
  }
  
  TPCModule GearTPC::getModule(int ID, int endplate) const {
    //ID is defined in the compact xml
    if(ID>getNModules(endplate))
      throw OutsideGeometryException("TPC Module not found!");
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
    //x and y comes in in world coordinates
    DetElement ep=getEndPlate(endplate);
    double zpos=getEndPlateZPosition(endplate);
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
    // find the node at this position
    TGeoNode *mynode=geoManager->FindNode(c0,c1,zpos);
    
    bool onMod=false;
    std::map<std::string,DetElement>::const_iterator it;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
	Position global(c0,c1,zpos), local;
	it->second.worldToLocal(global,local);
	Double_t point_local[3]={local.x,local.y,local.z};
    	onMod=it->second.volume().solid()->Contains(point_local);
	if(onMod)
	  {
	    //std::cout<<"Point is on "<<it->second.volume()->GetName()<<" id: "<<it->second.id()<<std::endl;
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
     
    bool onMod=false;
    std::map<std::string,DetElement>::const_iterator it;
    //check if any of the modules contains that point
    double safe_dist=numeric_limits<double>::max();
    TPCModule neighbour;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
	//trafo of input world coordinates to local module system
	Position global(c0,c1,zpos), local;
	it->second.worldToLocal(global,local);
	Double_t point_local[3]={local.x,local.y,local.z};
    	onMod=it->second.volume().solid()->Contains(point_local);

	if(onMod)
	  {
	    //std::cout<<"Point is on "<<it->second.volume()->GetName()<<" "<<it->second.volume().solid()->GetName()<<" id: "<<it->second.id()<<std::endl;
	    return it->second;
	  }
	
	//if not on module, compute distance from point to each shape
	//FIXME: not sure if this is exact. Sometimes more than one module has the same safety distance.
	double dist=it->second.volume().solid()->Safety(point_local,0);
	if(dist<safe_dist)
	  {
	    safe_dist=dist;
	    neighbour=it->second;
	  }
      }
    // std::cout<<"MINIMUM: "<<safe_dist<<" for "<<neighbour.placement()->GetName()<<" "<<neighbour.id()<<std::endl;
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
