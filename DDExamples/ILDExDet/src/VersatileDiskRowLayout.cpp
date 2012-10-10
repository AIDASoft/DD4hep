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
#include <sstream>
#include <cmath>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  /// Standard constructor with arguments
  VersatileDiskRowLayout::VersatileDiskRowLayout(const Geometry::DetElement& d) : module(d) {
    pads = module.readout().segmentation();
    tube = module.volume().solid();
    padData = pads.extension<VersatileDiskRowLayoutData>();
    //needs some check if segmentation actually fits on module
  }
  
  /// Standard extension constructor. det is the NEW detector element e.g. when reflecting a detector
  VersatileDiskRowLayout::VersatileDiskRowLayout(const VersatileDiskRowLayout& /* c */, const Geometry::DetElement& det)
    : module(det)
  {
    pads = module.readout().segmentation();
    tube = module.volume().solid();
    padData = pads.extension<VersatileDiskRowLayoutData>();
  }


  std::string VersatileDiskRowLayout::getPadType()const {
    return pads.type();
  }
 
  int VersatileDiskRowLayout::getNPads()const {
    return padData->_nPads;
  }
  
  int VersatileDiskRowLayout::getNRows()const {
    return padData->_rows.size();
  }

  int VersatileDiskRowLayout::getNPadsInRow(int row)const {
    if( (unsigned) row >  padData->_rows.size() - 1 ) {
      
      throw OutsideGeometryException(" VersatileDiskRowLayout::row number too large !");
    }
    return padData->_rows[row]._nPads;
  }
  
  double VersatileDiskRowLayout::getRowHeight(int row)const {
    if(row>getNRows() || row<0)
      throw OutsideGeometryException("getRowHeight: Requested row not on module querried!");
    return padData->_rows[row]._rowHeight;
  }
  
  int VersatileDiskRowLayout::getRowNumber(int padIndex)const {
    int rn = ( 0xffff0000 & padIndex ) >> 16 ;
    
    if( rn < 0 ||  rn > int( padData->_rows.size() - 1) ){
      
      std::stringstream sstr ;
      
      sstr << "VersatileDiskRowLayout::getRowNumber: illegal rownumber: "
           << rn << " for padIndex  0x" << std::hex << padIndex << " nRows: " << padData->_rows.size() << std::dec ;
      
      throw  OutsideGeometryException( sstr.str() ) ;
    }
    
    return rn ;
  }
  
  double VersatileDiskRowLayout::getPadPitch(int padIndex)const {
    int row =  getRowNumber( padIndex ) ;
    return padData->_rows[row]._padPitch;
  }
  
  int VersatileDiskRowLayout::getPadNumber(int padIndex)const {
    return ( 0x0000ffff & padIndex ) ;
  }

  int VersatileDiskRowLayout::getPadIndex(int rowNr,int padNr)const {
   if( (unsigned) rowNr >  padData->_rows.size() - 1 ) {
     
      throw OutsideGeometryException(" VersatileDiskRowLayout::getPadIndex row number too large !");
    }
   
    if( padNr > getNPadsInRow(rowNr) - 1 ) {
     
      std::stringstream sstr ;
     
      sstr << "VersatileDiskRowLayout::getPadIndex: pad number too large: "
           << padNr << " only " <<   getNPadsInRow(rowNr) << " pads in row " << rowNr ;

      throw OutsideGeometryException( sstr.str() );
    }
   
    return  (rowNr << 16 ) | ( 0x0000ffff & padNr ) ;
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


  std::vector<double>  VersatileDiskRowLayout::getPadCenter (int padIndex) const {
    int rowNum =  getRowNumber( padIndex ) ;
    int padNum =  getPadNumber( padIndex ) ;
    
    const Row& row =  padData->_rows[ rowNum ] ;
    //pad pitch and offset come in in mm, need to be converted to angles in rad
    double r = row._rCentre;
    double phi_mm = row._offset + (static_cast<double>(padNum) + 0.5) * row._padPitch;
    double phi=phi_mm/r;
    
    //local center coordinates in module system
    double pad_x = r*cos(phi);//*M_PI/180.);
    double pad_y = r*sin(phi);//*M_PI/180.);
    
    //trafo to global coordinates
    Position global_w, local(pad_x,pad_y,0); 
    module.localToWorld(local,global_w);
   
    vector<double> center;
    center.push_back(global_w.x);
    center.push_back(global_w.y);

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
    double radius=sqrt(point_local[0]*point_local[0]+point_local[1]*point_local[1]);
    double row_radius=radius-padData->_rMin;
    //determine row
    double row_step=0;
    int row=0;
    for (int r=0;r<getNRows();r++)
      {
	row_step+=getRowHeight(r);
	if(row_radius<row_step)
	  {
	    row=r;
	    break;
	  }
      }
    //outer edge of last row belongs to last row, same for the pad
    if(row==getNRows())
      row=getNRows()-1;
    
    double r= padData->_rows[row]._rCentre;
    double pad_width=padData->_rows[row]._padPitch;
    double angle=atan2(point_local[1],point_local[0]);

    int padNr=static_cast<int>((angle*r-padData->_rows[row]._offset)/pad_width-0.5);
    //outside active area of the module
    if(padNr>=getNPadsInRow(row))
      padNr=getNPadsInRow(row)-1;

    return getPadIndex(row,padNr);
  }
 
}
