// $Id: SubdetectorAssembly_geo.cpp 1306 2014-08-22 12:29:38Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "XML/Utilities.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, Ref_t)  {
  xml_det_t  x_det  (e);
  string     det_name = x_det.nameStr();
  DetElement sdet(det_name, x_det.id());
  Volume     vol;

  bool useRot = false ;
  bool usePos = false ; 
  Position    pos ;
  RotationZYX rot ;

  if ( x_det.hasChild(_U(shape)) )  {
    xml_comp_t x_shape = x_det.child(_U(shape));
    string     type  = x_shape.typeStr();
    Solid      solid = XML::createShape(lcdd, type, x_shape);
    Material   mat   = lcdd.material(x_shape.materialStr());
    printout(DEBUG,det_name,"+++ Creating detector assembly with shape of type:%s",type.c_str());
    vol = Volume(det_name,solid,mat);
    
    if( x_shape.hasChild( _U(position) ) ) {
      usePos = true ;
      pos = Position(   x_shape.position().x() , x_shape.position().y(),   x_shape.position().z()    ) ;
    }
    if( x_shape.hasChild( _U(rotation) ) ) {
      useRot = true ;
      rot = RotationZYX(   x_shape.rotation().x() , x_shape.rotation().y(),   x_shape.rotation().z()    ) ;
    }
  }
  else  {
    printout(DEBUG,det_name,"+++ Creating detector assembly without shape");
    vol = Assembly(det_name);
  }

  for(xml_coll_t c(x_det,_U(composite)); c; ++c)  {
    xml_dim_t component = c;
    string nam = component.nameStr();
    lcdd.declareMotherVolume(nam, vol);
  }

  vol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  Volume mother = lcdd.pickMotherVolume(sdet) ;
  PlacedVolume pv ;

  if( useRot && usePos ){

    pv =  mother.placeVolume( vol , Transform3D( rot, pos )  ) ;

  } else if( useRot ){

    pv =  mother.placeVolume( vol , rot  ) ;

  } else if( usePos ){

    pv =  mother.placeVolume( vol , pos  ) ;

  } else {

    pv = mother.placeVolume( vol );
  }

  pv.addPhysVolID("system", sdet.id() );
  
  sdet.setPlacement(pv);
  
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_SubdetectorAssembly,create_element)
