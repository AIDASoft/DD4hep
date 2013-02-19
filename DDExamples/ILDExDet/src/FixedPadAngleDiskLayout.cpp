// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//  This is a special implementation for a FixedPadAngleDiskLayout!
//  The number of pads is distrubuted on a tube shaped segment starting 
//  on the lower left corner. All rows have the same height,
//  starting at rmin for the lower edge of the first row to rmax for the
//  upper edge of the last row. The row height is calculated from the number of pads.
//
//  Row height and pad height are identical, and pad gap is 0 
//  (the only thing that matters is the effective pitch).
//====================================================================
#include "DD4hep/LCDD.h"

#include "FixedPadAngleDiskLayout.h"
#include "TGeoTube.h"
#include <math.h>
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  /// Standard constructor with arguments
  FixedPadAngleDiskLayout::FixedPadAngleDiskLayout(const Geometry::DetElement& d) : module(d) {
    pads = module.readout().segmentation();
    tube = module.volume().solid();
  }
  
  /// Standard extension constructor. det is the NEW detector element e.g. when reflecting a detector
  FixedPadAngleDiskLayout::FixedPadAngleDiskLayout(const FixedPadAngleDiskLayout& /* c */, const Geometry::DetElement& det)
    : module(det)
  {
    pads = module.readout().segmentation();
    tube = module.volume().solid();
  }

  std::string FixedPadAngleDiskLayout::getPadType()const {
    return pads.type();
  }
 
  int FixedPadAngleDiskLayout::getNPads()const {
    return pads.thetaBins()* pads.phiBins();
  }
  
  int FixedPadAngleDiskLayout::getNRows()const {
    return pads.thetaBins();
  }

  int FixedPadAngleDiskLayout::getNPadsInRow(int row)const {
    return pads.phiBins();
  }
  
  double FixedPadAngleDiskLayout::getRowHeight(int row)const {
    if(row>getNRows() || row<0)
      throw OutsideGeometryException("getRowHeight: Requested row not on module querried!");
    //all rows are the same for FixedPadAngleDiskLayout=ProjectiveCylinder 
    double module_height= tube->GetRmax()-tube->GetRmin();
    return module_height/getNRows();
  }
  
  int FixedPadAngleDiskLayout::getRowNumber(int padIndex)const {
    if(padIndex>getNPads() || padIndex<0)
      throw OutsideGeometryException("getRowNumber: Requested pad not on module querried!");
    return padIndex/getNPadsInRow(0);
  }
  
  double FixedPadAngleDiskLayout::getPadPitch(int padIndex)const {
    if(padIndex>getNPads() || padIndex<0)
      throw OutsideGeometryException("getPadPitch: Requested pad not on module querried!");
    int row=getRowNumber(padIndex);
    double pad_radius=tube->GetRmin()+(row+0.5)*getRowHeight(0);
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double pad_angle=module_width/getNPadsInRow(row);
    return pad_radius*pad_angle*M_PI/180.;
  }
  
  int FixedPadAngleDiskLayout::getPadNumber(int padIndex)const {
   if(padIndex>getNPads() || padIndex<0)
      throw OutsideGeometryException("getPadNumber: Requested pad not on module querried!");
    return padIndex % getNPadsInRow(0);
  }

  int FixedPadAngleDiskLayout::getPadIndex(int row,int padNr)const {
    if(padNr>=getNPadsInRow(row) || padNr<0)
      throw OutsideGeometryException("getPadIndex: Requested pad not on module querried!");
    if(row>=getNRows() || padNr<0 )
      throw OutsideGeometryException("getPadIndex: Requested row not on module querried!");
    return padNr + row*getNPadsInRow(row);
  }

  int FixedPadAngleDiskLayout::getRightNeighbour(int padIndex)const {
    //what is left and what is right is a matter of definition
    //if on edge their is no neighbour, should throw an exception
    int row=getRowNumber(padIndex);
    if(getPadNumber(padIndex)==getNPadsInRow(row)-1)
      throw OutsideGeometryException("getRightNeighbour: Requested pad is on right edge and has no right neighbour!");
    // if not on edge
    return padIndex + 1;
  }

  int FixedPadAngleDiskLayout::getLeftNeighbour(int padIndex)const {
    //if on edge their is no neighbour, should throw an exception
    if(getPadNumber(padIndex)==0)
      throw OutsideGeometryException("getLeftNeighbour: Requested pad is on left edge and has no left neighbour!");
    // if not on edge
    return padIndex - 1;
  }


  std::vector<double>  FixedPadAngleDiskLayout::getPadCenter (int padIndex) const {
    if(padIndex>getNPads())
      throw OutsideGeometryException("getPadCenter: Requested pad not on module querried!");
    int row=getRowNumber(padIndex);
    double pad_radius=tube->GetRmin()+(row+0.5)*getRowHeight(0);
    double module_width= tube->GetPhi2()-tube->GetPhi1();
    double pad_angle=(getPadNumber(padIndex)+0.5)*module_width/getNPadsInRow(row);
    //local center coordinates in module system
    double pad_x = pad_radius*cos(pad_angle*M_PI/180.);
    double pad_y = pad_radius*sin(pad_angle*M_PI/180.);
 
    //trafo to global coordinates
    Position global_w, local(pad_x,pad_y,0); 
    module.localToWorld(local,global_w);

    vector<double> center;
    center.push_back(global_w.X());
    center.push_back(global_w.Y());
    return center;
  }
  
  int FixedPadAngleDiskLayout::getNearestPad(double c0,double c1)const {
    //input coordinates are global
    //find z position of module in world coordinates
    Position fake_local(0,0,0);
    Position fake_global;
    module.localToWorld(fake_local,fake_global);
    // trafo to local coordinates
    Position global(c0,c1,fake_global.Z()), local;
    module.worldToLocal(global,local);
    Double_t point_local[3]={local.X(),local.Y(),local.Z()};
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
