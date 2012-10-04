// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#include "DD4hep/LCDD.h"

#include "VersatileDiskRowLayout.h"
#include "TGeoTube.h"
#include <math.h>
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  /// Standard constructor with arguments
  VersatileDiskRowLayout::VersatileDiskRowLayout(const Geometry::DetElement& d) : module(d) {
    pads = module.readout().segmentation();
    tube = module.volume().solid();
  }
  
  /// Standard extension constructor. det is the NEW detector element e.g. when reflecting a detector
  VersatileDiskRowLayout::VersatileDiskRowLayout(const VersatileDiskRowLayout& /* c */, const Geometry::DetElement& det)
    : module(det)
  {
    pads = module.readout().segmentation();
    tube = module.volume().solid();
  }

  std::string VersatileDiskRowLayout::getPadType()const {
    return pads.type();
  }
 
  int VersatileDiskRowLayout::getNPads()const {
    return pads._nPads;
  }
  
  int VersatileDiskRowLayout::getNRows()const {
    return pads._rows.size();
  }

  int VersatileDiskRowLayout::getNPadsInRow(int row)const {
    return pads._rows[row]._nPads;
  }
  
  double VersatileDiskRowLayout::getRowHeight(int row)const {
    if(row>getNRows() || row<0)
      throw OutsideGeometryException("getRowHeight: Requested row not on module querried!");
    return 0;
  }
  
  int VersatileDiskRowLayout::getRowNumber(int pad)const {
    if(pad>getNPads() || pad<0)
      throw OutsideGeometryException("getRowNumber: Requested pad not on module querried!");
    return 0;
  }
  
  double VersatileDiskRowLayout::getPadPitch(int pad)const {
    if(pad>getNPads() || pad<0)
      throw OutsideGeometryException("getPadPitch: Requested pad not on module querried!");
    return 0;
  }
  
  int VersatileDiskRowLayout::getPadNumber(int pad)const {
   if(pad>getNPads() || pad<0)
      throw OutsideGeometryException("getPadNumber: Requested pad not on module querried!");
   return 0;
  }

  int VersatileDiskRowLayout::getPadIndex(int row,int padNr)const {
    if(padNr>=getNPadsInRow(row) || padNr<0)
      throw OutsideGeometryException("getPadIndex: Requested pad not on module querried!");
    if(row>=getNRows() || padNr<0 )
      throw OutsideGeometryException("getPadIndex: Requested row not on module querried!");
    return 0;
  }

  int VersatileDiskRowLayout::getRightNeighbour(int pad)const {
    //what is left and what is right is a matter of definition
    //if on edge their is no neighbour, should throw an exception
    int row=getRowNumber(pad);
    if(getPadNumber(pad)==getNPadsInRow(row)-1)
      throw OutsideGeometryException("getRightNeighbour: Requested pad is on right edge and has no right neighbour!");
    // if not on edge
    return pad + 1;
  }

  int VersatileDiskRowLayout::getLeftNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    if(getPadNumber(pad)==0)
      throw OutsideGeometryException("getLeftNeighbour: Requested pad is on left edge and has no left neighbour!");
    // if not on edge
    return pad - 1;
  }


  std::vector<double>  VersatileDiskRowLayout::getPadCenter (int pad) const {
    if(pad>getNPads())
      throw OutsideGeometryException("getPadCenter: Requested pad not on module querried!");
    vector<double> center;
    //center.push_back(global_w.x);
    //center.push_back(global_w.y);
    return center;
  }
  
  int VersatileDiskRowLayout::getNearestPad(double c0,double c1)const {
    //input coordinates are global
    //find z position of module in world coordinates
    Position fake_local(0,0,0);
    Position fake_global;
    module.localToWorld(fake_local,fake_global);
    // trafo to local coordinates
    Position global(c0,c1,fake_global.z), local;
    module.worldToLocal(global,local);
    Double_t point_local[3]={local.x,local.y,local.z};
    //check if it is on that module
    bool onMod=tube->Contains(point_local);
    if(!onMod)
      throw OutsideGeometryException("getNearestPad: Requested point not on module querried!");
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double radius=sqrt(point_local[0]*point_local[0]+point_local[1]*point_local[1]);
    int row=(radius-tube->GetRmin())/getRowHeight(0);
    //outer edge of last row belongs to last row, same for the pad
    if(row==getNRows())
      row=getNRows()-1;
    double pad_width=module_width/getNPadsInRow(row);    
    double angle=atan2(point_local[1],point_local[0])/M_PI*180;
    int padNr=static_cast<int>(angle/pad_width);
    if(padNr==getNPadsInRow(row))
      padNr=padNr-1;

    return getPadIndex(row,padNr);
  }
 
}
