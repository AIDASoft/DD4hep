// $Id: XMLDetector.cpp 1479 2014-12-18 16:00:45Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "XML/Utilities.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Create a solid shape using the plugin mechanism from the attributes of the XML element
Geometry::Solid 
DD4hep::XML::createShape(Geometry::LCDD& lcdd, const std::string& shape_type, XML::Element element)   {
  string fac  = shape_type + "__shape_constructor";
  XML::Handle_t solid_elt = element;
  Geometry::Solid solid = Geometry::Ref_t(PluginService::Create<NamedObject*>(fac, &lcdd, &solid_elt));
  if ( !solid.isValid() )  {
    PluginDebug dbg;
    PluginService::Create<NamedObject*>(shape_type, &lcdd, &solid_elt);
    except("XML::createShape","Failed to create solid of type %s [%s]", 
	   shape_type.c_str(),dbg.missingFactory(shape_type).c_str());
  }
  return solid;
}




Geometry::Volume DD4hep::XML::createPlacedEnvelope( DD4hep::Geometry::LCDD& lcdd, DD4hep::XML::Handle_t e , 
						    DD4hep::Geometry::DetElement sdet ){
  
  xml_det_t     x_det     = e;
  string        det_name  = x_det.nameStr();
  
  xml_comp_t    x_env     =  x_det.child( DD4hep::XML::Strng_t("envelope") ) ;
  xml_comp_t    x_shape   =  x_env.child( _U(shape) ); 
  
  Material      env_mat   = lcdd.material( x_shape.materialStr() );
  

  bool useRot = false ;
  bool usePos = false ; 
  Position    pos ;
  RotationZYX rot ;

  if( x_env.hasChild( _U(position) ) ) {
    usePos = true ;
    xml_comp_t env_pos = x_env.position();
    pos = Position( env_pos.x(),env_pos.y(),env_pos.z() );  
  }
  if( x_env.hasChild( _U(rotation) ) ) {
    useRot = true ;
    xml_comp_t    env_rot     = x_env.rotation();
    rot = RotationZYX( env_rot.z(),env_rot.y(),env_rot.x() ) ;
  }


  // ---- create a shape from the specified xml element --------
  Box  env_solid = xml_comp_t( x_shape ).createShape();
  
  if( !env_solid.isValid() ){
    
    throw std::runtime_error( std::string(" Cannot create envelope volume : ") + x_shape.typeStr() + 
			      std::string(" for detector " ) + det_name ) ;
  }
  
  Volume        envelope  ( det_name+"_envelope", env_solid, env_mat );
  
  PlacedVolume  env_pv  ; 

  Volume        mother = lcdd.pickMotherVolume(sdet);


  // ---- place the envelope into the mother volume 
  //      only specify transformations given in xml
  //      to allow for optimization 

  if( useRot && usePos ){
    env_pv =  mother.placeVolume( envelope , Transform3D( rot, pos )  ) ;

  } else if( useRot ){
    env_pv =  mother.placeVolume( envelope , rot  ) ;

  } else if( usePos ){
    env_pv =  mother.placeVolume( envelope , pos  ) ;

  } else {
    env_pv = mother.placeVolume( envelope );
  }

  // ----------------------------------------------

  env_pv.addPhysVolID("system", sdet.id());

  sdet.setPlacement( env_pv ) ;

  envelope.setAttributes( lcdd,x_det.regionStr(),x_det.limitsStr(),x_env.visStr());

  return envelope ;
}
