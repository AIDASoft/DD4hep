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
#include "TGeoMatrix.h"
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
//     std::cout<<ep.placements().size()<<std::endl;
//     for ( int pv=0;pv<ep.placements().size() ; pv++ )
//       {
//     	std::cout<< ep.placements()[pv].volume().solid().name() <<" "
//     		 <<ep.placements()[pv].motherVol().solid().name() <<std::endl;
//       }
    TGeoMatrix *nm=ep.placements()[0]->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    double zpos=trans[2];
       
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
    //magic point, short term fix until root bug solved, seems to effect functionality of TGeoManager
    TGeoNode *mynode=geoManager->FindNode(c0,c1,500);
        
    /////////////////////////////////////////////////////
    //  METHOD 1: via the navigator                    //
    /////////////////////////////////////////////////////
    
    mynode=geoManager->FindNode(c0,c1,zpos);
//     if(mynode)
//       std::cout<<mynode<<" "<<mynode->GetName()<<std::endl;
//     else
//       std::cout<<"no node found here"<<std::endl;
    
    /////////////////////////////////////////////////////
    //  METHOD 2: loop all modules and check           //
    /////////////////////////////////////////////////////
    Double_t point[3];
    Double_t point_local[3];
    point[0]=c0;
    point[1]=c1;
    point[2]=zpos;
    geoManager->SetCurrentPoint(c0,c1,zpos);
    geoManager->MasterToLocal(point, point_local);
    std::cout<<"Local: "<<point_local[0]<<" "<<point_local[1]<<" "<<point_local[2]<<std::endl;
    bool onMod=false;
    std::map<std::string,DetElement>::const_iterator it;
    for ( it=ep.children().begin() ; it != ep.children().end(); it++ )
      {
	onMod=it->second.volume().solid()->Contains(point_local);
	std::cout<<it->second.volume()->GetName()<<" "<<onMod<<std::endl;
	if(onMod)
	  return onMod;
      }
   return onMod;
  }
  
  
  DDTPCModule GearTPC::getNearestModule(double c0, double c1, int endplate) const{
    DetElement ep;
    if(endplate==1)
      ep= child("TPC_EndPlate_negativ");
    else
      ep= child("TPC_EndPlate");
    TGeoManager *geoManager = ep.volume()->GetGeoManager();
   
  }
  
}
