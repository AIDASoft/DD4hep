// $Id: $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : F.Gaede
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "XML/Utilities.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/** Create an envelope for subdetectors - calls a plugin 
 *  for the volume to be used, which is specified in the type attribute
 *  of the <volume/> child element. If no volume is specified
 *  an assembly is created. The actual subdetector is specified
 *  in the <composite/> elements' name attribute. The placement
 *  of the envelope volume can be optionally specified in the 
 *  <position/> and <rotation/> elements.
 * 
 *  @author F.Gaede CERN/DESY
 *  @version $Id:$
 */ 
static Ref_t create_Envelope(LCDD& lcdd, xml_h e, Ref_t)  {

  xml_det_t  x_det  (e);
  string     det_name = x_det.nameStr();
  DetElement sdet(det_name, x_det.id());
  Volume     vol;

  double xpos = 0. ;
  double ypos = 0. ;
  double zpos = 0. ;
  double xrot = 0. ;
  double yrot = 0. ;
  double zrot = 0. ;

  if ( x_det.hasChild(_U(volume)) )  {

    xml_comp_t x_vol = x_det.child(_U(volume));

    string     type  = x_vol.typeStr();

    vol = XML::createVolume(lcdd, type, x_vol);

    if( x_vol.hasChild( _U(position) ) ){
      xpos = x_vol.position().x() ;
      ypos = x_vol.position().y() ;
      zpos = x_vol.position().z() ;
    }

    if( x_vol.hasChild( _U(rotation) ) ){
      xrot = x_vol.rotation().x() ;
      yrot = x_vol.rotation().y() ;
      zrot = x_vol.rotation().z() ;
    }

    printout(DEBUG,det_name,"+++ Creating envelope volume with of type:%s",type.c_str());

  }
  else  {
    printout(DEBUG,det_name,"+++ Creating detector assembly without shape");
    vol = Assembly(det_name);
  }

  for(xml_coll_t c(x_det,_U(composite)); c; ++c)  {
    xml_dim_t component = c;
    string nam = component.nameStr();
    lcdd.declareMotherVolume( nam, vol);
  }

  vol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());


  Position    pos( xpos, ypos, zpos ) ;
  RotationZYX rot( zrot, yrot , xrot );
  Transform3D trans( rot, pos);

  Volume mother = lcdd.pickMotherVolume(sdet) ;

  PlacedVolume pv = mother.placeVolume( vol, trans );

  sdet.setPlacement( pv );
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_Envelope,create_Envelope)
