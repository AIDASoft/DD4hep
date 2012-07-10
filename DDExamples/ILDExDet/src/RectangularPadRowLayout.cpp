// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//  This is a special implementation for a RectangularPadRowLayout!
//  for a rectangular row based layout where all pads in a given row are 
//  equal and have rectangular shape ditributed on a rectangular shaped module.
//
//  Row height and pad height are identical, and pad gap is 0 
//  (the only thing that matters is the effective pitch).
//====================================================================
#include "DD4hep/LCDD.h"

#include "RectangularPadRowLayout.h"
#include "TGeoBBox.h"
#include <math.h>
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  /// Standard constructor with arguments
  RectangularPadRowLayout::RectangularPadRowLayout(const Geometry::DetElement& d) : module(d) {
    pads = module.readout().segmentation();
    box = module.volume().solid();
  }
  
  /// Standard extension constructor. det is the NEW detector element e.g. when reflecting a detector
  RectangularPadRowLayout::RectangularPadRowLayout(const RectangularPadRowLayout& /* c */, const Geometry::DetElement& det)
    : module(det)
  {
    pads = module.readout().segmentation();
    box = module.volume().solid();
  }

  std::string RectangularPadRowLayout::getPadType()const {
    return pads.type();
  }
 
  int RectangularPadRowLayout::getNPads()const {
     return getNRows()*getNPadsInRow(0);
  }
  
  int RectangularPadRowLayout::getNRows()const {
    return box->GetDY()*2/pads.getGridSizeY();
  }

  int RectangularPadRowLayout::getNPadsInRow(int row)const {
    return box->GetDX()*2/pads.getGridSizeX();
  }
  
  double RectangularPadRowLayout::getRowHeight(int row)const {
    if(row>getNRows() || row<0)
      throw OutsideGeometryException("getRowHeight: Requested row not on module querried!");
    //all rows are the same for RectangularPadRowLayout=cartesian_grid
    double module_height= 2*box->GetDY();
    return module_height/getNRows();
  }
  
  int RectangularPadRowLayout::getRowNumber(int pad)const {
    if(pad>getNPads() || pad<0)
      throw OutsideGeometryException("getRowNumber: Requested pad not on module querried!");
    return pad/getNPadsInRow(0);
  }
  
  double RectangularPadRowLayout::getPadPitch(int pad)const {
    if(pad>getNPads() || pad<0)
      throw OutsideGeometryException("getPadPitch: Requested pad not on module querried!");
    int row=getRowNumber(pad);
    double module_width= 2*box->GetDX();
    return module_width/getNPadsInRow(row);
  }
  
  int RectangularPadRowLayout::getPadNumber(int pad)const {
   if(pad>getNPads() || pad<0)
      throw OutsideGeometryException("getPadNumber: Requested pad not on module querried!");
    return pad % getNPadsInRow(0);
  }

  int RectangularPadRowLayout::getPadIndex(int row,int padNr)const {
    if(padNr>=getNPadsInRow(row) || padNr<0)
      throw OutsideGeometryException("getPadIndex: Requested pad not on module querried!");
    if(row>=getNRows() || padNr<0 )
      throw OutsideGeometryException("getPadIndex: Requested row not on module querried!");
    return padNr + row*getNPadsInRow(row);
  }

  int RectangularPadRowLayout::getRightNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    int row=getRowNumber(pad);
    if(getPadNumber(pad)==getNPadsInRow(row)-1)
      throw OutsideGeometryException("getRightNeighbour: Requested pad is on right edge and has no right neighbour!");
    // if not on edge
    return pad + 1;
  }

  int RectangularPadRowLayout::getLeftNeighbour(int pad)const {
    //if on edge their is no neighbour, should throw an exception
    if(getPadNumber(pad)==0)
      throw OutsideGeometryException("getLeftNeighbour: Requested pad is on left edge and has no left neighbour!");
    // if not on edge
    return pad - 1;
  }


  std::vector<double>  RectangularPadRowLayout::getPadCenter (int pad) const {
    if(pad>getNPads())
      throw OutsideGeometryException("getPadCenter: Requested pad not on module querried!");
    int row=getRowNumber(pad);
    //shift coordinates from pad system where 0,0 is on lower left corner of module into module
    //system where 0,0 is in the middle of the module box
    double pad_y=(row+0.5)*getRowHeight(0)-box->GetDY();
    double pad_x = (getPadNumber(pad)+0.5)*getPadPitch(pad)-box->GetDX();
    //trafo to global coordinates
    Position global_w, local(pad_x,pad_y,0);
    module.localToWorld(local,global_w);

    vector<double> center;
    center.push_back(global_w.x);
    center.push_back(global_w.y);
    return center;
  }
  
  int RectangularPadRowLayout::getNearestPad(double c0,double c1)const {
    //find z position of module in world coordinates
    Position fake_local(0,0,0);
    Position fake_global;
    module.localToWorld(fake_local,fake_global);
    // trafo to local coordinates
    Position global(c0,c1,fake_global.z), local;
    module.worldToLocal(global,local);
    Double_t point_local[3]={local.x,local.y,local.z};
    //check if it is on that module
    bool onMod=box->Contains(point_local);
    if(!onMod)
      throw OutsideGeometryException("getNearestPad: Requested point not on module querried!");
    //shift coordinates into pad system where 0,0 is on lower left corner of module
    int row=static_cast<int>((point_local[1]+box->GetDY())/getRowHeight(0));
    //outer edge of last row belongs to last row, same goes for pad
    if(row==getNRows())
      row=getNRows()-1;
    int padNr=static_cast<int>((point_local[0]+box->GetDX())/getPadPitch(0));
    if(padNr==getNPadsInRow(row))
      padNr=padNr-1;
    return getPadIndex(row,padNr);
  }
 
}
