// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A. Muennich
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/FieldTypes.h"
#include "XML/DocumentHandler.h"
#include "XML/Conversions.h"
#include "xercesc/util/XMLURL.hpp"
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
    
int  VersatileDiskRowLayoutData::setRMin(int rmin)
{
  _rMin=rmin;
}

//factory to fill object from compact xml
static Ref_t create_VersatileDiskRowLayoutSeg(lcdd_t& /* lcdd */, const xml_h& e)  {
  VersatileDiskRowLayoutSeg obj;
  VersatileDiskRowLayoutData *dataExt=new VersatileDiskRowLayoutData();
  if ( e.hasAttr(_A(rmin))   ) dataExt->setRMin(e.attr<int>(_A(rmin)));
  //loop rows to fill VersatileDiskRowLayoutSeg
  for(xml_coll_t r(e,_X(row)); r; ++r)  {
    dataExt->addRow(r.attr<int>(Unicode("nPads")),
		    r.attr<double>(Unicode("padPitch")),
		    r.attr<double>(Unicode("rowHeight")),
		    r.attr<double>(_A(offset)));
   }
  obj.setExtension<VersatileDiskRowLayoutData>(dataExt);
  return obj;
}
DECLARE_XMLELEMENT(VersatileDiskRowLayoutSeg,create_VersatileDiskRowLayoutSeg);
