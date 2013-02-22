#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/FieldTypes.h"
#include "XML/DocumentHandler.h"
#include "XML/Conversions.h"
#include "xercesc/util/XMLURL.hpp"
#include "VersatileDiskRowLayoutSeg.h"

using namespace DD4hep::Geometry;    

//factory to fill object from compact xml
static Ref_t create_VersatileDiskRowLayoutSeg(lcdd_t& /* lcdd */, const xml_h& e)  {
  VersatileDiskRowLayoutSeg obj;
  VersatileDiskRowLayoutData *dataExt=new VersatileDiskRowLayoutData();
  if ( e.hasAttr(_U(rmin))   ) dataExt->setRMin(e.attr<int>(_U(rmin)));
  //loop rows to fill VersatileDiskRowLayoutSeg
  for(xml_coll_t r(e,_U(row)); r; ++r)  {
    dataExt->addRow(r.attr<int>(Unicode("nPads")),
		    r.attr<double>(Unicode("padPitch")),
		    r.attr<double>(Unicode("rowHeight")),
		    r.attr<double>(_U(offset)));
   }
  obj.setExtension<VersatileDiskRowLayoutData>(dataExt);
  return obj;
}
DECLARE_XMLELEMENT(VersatileDiskRowLayoutSeg,create_VersatileDiskRowLayoutSeg);
