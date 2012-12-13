// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A. Muennich
//
//====================================================================

#include "VersatileDiskRowLayoutSeg.h"

using namespace DD4hep::Geometry;    

VersatileDiskRowLayoutSeg::VersatileDiskRowLayoutSeg() 
  : Segmentation("VersatileDiskRowLayoutSeg")   {}


VersatileDiskRowLayoutData::VersatileDiskRowLayoutData()
{
  _rMin=0;
  _nPads=0;
  _rows.clear();
}

 
void VersatileDiskRowLayoutData::addRow(int nPads, double padPitch, double rowHeight, double offset)
{
  Row new_row;
  new_row._nPads=nPads;
  new_row._padPitch=padPitch;
  new_row._rowHeight=rowHeight;
  new_row._offset=offset;
  double centre=0.;
  if( _rows.size()==0)
    centre = _rMin + rowHeight*0.5;
  else
    centre =_rows[_rows.size()-1]._rCentre+rowHeight;
  new_row._rCentre=centre;
  _rows.push_back(new_row);
  _nPads+=nPads;
}
    
void VersatileDiskRowLayoutData::setRMin(int rmin)
{
  _rMin=rmin;
}

