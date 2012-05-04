#include "DDTPCModule.h"
#include "TPCModuleData.h"
#include "TGeoTube.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include <math.h>
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  DDTPCModule::DDTPCModule(const LCDD&, const string& name, const string& type, int id)
  {
    Value<TNamed,TPCModuleData>* p = new Value<TNamed,TPCModuleData>();
    assign(p,name, type);
    p->id = id;
  }
  
  int DDTPCModule::getID()const {
    return _data().id;
  }
 
  int DDTPCModule::getNPads()const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.thetaBins()* pads.phiBins();
  }
  
  int DDTPCModule::getNRows()const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.thetaBins();
  }

  int DDTPCModule::getNPadsInRow(int row)const {
    ProjectiveCylinder pads= readout().segmentation();
    return pads.phiBins();
  }
  
  double DDTPCModule::getRowHeight(int row)const {
    if(row>getNRows())
      throw OutsideGeometryException("Requested row not on module querried!");
    //all rows are the same for FixedAnglePadLayout=ProjectiveCylinder 
    Tube       tube=volume().solid();
    double module_height= tube->GetRmax()-tube->GetRmin();
    return module_height/getNRows();
  }
  
  int DDTPCModule::getRowNumber(int pad)const {
    if(pad>getNPads())
      throw OutsideGeometryException("Requested pad not on module querried!");
    return pad / (getNPads()/getNRows());
  }
  
  double DDTPCModule::getPadPitch(int pad)const {
    if(pad>getNPads())
      throw OutsideGeometryException("Requested pad not on module querried!");
    int row=getRowNumber(pad);
    Tube tube=volume().solid();
    double pad_radius=tube->GetRmin()+(row+0.5)*getRowHeight(0);
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double pad_angle=module_width/getNPadsInRow(row);
    return pad_radius*pad_angle*M_PI/180.;
  }
  
  int DDTPCModule::getPadNumber(int pad)const {
   if(pad>getNPads())
      throw OutsideGeometryException("Requested pad not on module querried!");
    return pad % (getNPads()/getNRows());
  }

  int DDTPCModule::getPadIndex(int row,int padNr)const {
    if(padNr>(getNPads()/getNRows()))
      throw OutsideGeometryException("Requested pad not on module querried!");
    if(row>getNRows())
      throw OutsideGeometryException("Requested row not on module querried!");
    return padNr + row*(getNPads()/getNRows());
  }

  int DDTPCModule::getRightNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    int row=getRowNumber(pad);
    if(getPadNumber(pad)==getNPadsInRow(row)-1)
      throw OutsideGeometryException("Requested pad is on right edge and has no right neighbour!");
    // if not on edge
    return pad + 1;
  }

  int DDTPCModule::getLeftNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    if(getPadNumber(pad)==0)
      throw OutsideGeometryException("Requested pad is on left edge and has no left neighbour!");
    // if not on edge
    return pad - 1;
  }

  std::vector<double>  DDTPCModule::getPadCenter (int pad) const {
    if(pad>getNPads())
      throw OutsideGeometryException("Requested pad not on module querried!");
    int row=getRowNumber(pad);
    Tube tube=volume().solid();
    double pad_radius=tube->GetRmin()+(row+0.5)*getRowHeight(0);
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double pad_angle=(getPadNumber(pad)+0.5)*module_width/getNPadsInRow(row);
    //local center coordinates in module system
    double pad_x=pad_radius*cos(pad_angle*M_PI/180.);
    double pad_y=pad_radius*sin(pad_angle*M_PI/180.);
 
    //trafo to global coordinates
    Double_t point_local[3];
    Double_t point_global[3];
    Double_t point_global_m[3];
    point_local[0]=pad_x;
    point_local[1]=pad_y;
    point_local[2]=getModuleZPosition();
    //this will give coordinates in the system of the mother (=endplate) not the world
    placements()[0]->LocalToMaster(point_local, point_global);
    //need to loop the whole tree back to world
    TGeoManager *geom=placements()[0]->GetMotherVolume()->GetGeoManager();
    geom->LocalToMaster(point_global, point_global_m);
//     std::cout<<"Local: "<<point_local[0]<<" "<<point_local[1]<<" "<<point_local[2]<<std::endl;
//     std::cout<<"Mother: "<<point_global[0]<<" "<<point_global[1]<<" "<<point_global[2]<<std::endl;
//     std::cout<<"Mother 2: "<<point_global_m[0]<<" "<<point_global_m[1]<<" "<<point_global_m[2]<<std::endl;

    std::vector<double> center;
    center.push_back(point_global_m[0]);
    center.push_back(point_global_m[1]);
    return center;
  }
  
  int DDTPCModule::getNearestPad(double c0,double c1)const {
    //trafo to local coordinates
    Double_t point_local[3];
    Double_t point_global[3];
    point_global[0]=c0;
    point_global[1]=c1;
    point_global[2]=getModuleZPosition();
    placements()[0]->MasterToLocal(point_global, point_local);
    //check if it is on that module
    bool onMod=volume().solid()->Contains(point_local);
    if(!onMod)
      throw OutsideGeometryException("Requested point not on module querried!");
    Tube tube=volume().solid();
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double radius=sqrt(point_local[0]*point_local[0]+point_local[1]*point_local[1]);
    int row=(radius-tube->GetRmin())/getRowHeight(0);
    double pad_width=module_width/getNPadsInRow(row);    
    double angle=atan2(point_local[1],point_local[0])/M_PI*180;
    int padNr=static_cast<int>(angle/pad_width);
    return getPadIndex(row,padNr);
  }
 
  double DDTPCModule::getModuleZPosition() const {
    TGeoMatrix *nm=placements()[0]->GetMatrix();
    const Double_t *trans=nm->GetTranslation();
    return trans[2];
  }
}
