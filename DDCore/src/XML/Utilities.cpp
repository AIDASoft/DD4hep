//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_NONE

// Framework include files
#include "XML/Utilities.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Detector.h"
#include "DD4hep/DetFactoryHelper.h"
#include "Math/Polar2D.h"

class TObject;

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::xml::tools;

namespace {
  static constexpr const char* TAG_LIMITSETREF     = "limitsetref";
  static constexpr const char* TAG_REGIONREF       = "regionref";
  static constexpr const char* TAG_VISREF          = "visref";

  static constexpr const char* TAG_COMBINE_HITS    = "combine_hits";
  static constexpr const char* TAG_VERBOSE         = "verbose";
  static constexpr const char* TAG_TYPE            = "type";
  static constexpr const char* TAG_ECUT            = "ecut";
  static constexpr const char* TAG_HITS_COLLECTION = "hits_collection";
}

/// Create layered transformation from xml information
Transform3D dd4hep::xml::createTransformation(xml::Element e)   {
  int flag = 0;
  Transform3D position, rotation, result;
  for( xml_coll_t c(e,_U(star)); c; ++c )    {
    string tag = c.tag();
    xml_dim_t x_elt = c;
    if ( tag == "positionRPhiZ" )   {
      if      ( flag == 1 ) result = position  * result;
      else if ( flag == 2 ) result = (position * rotation) * result;
      PositionRhoZPhi pos(x_elt.r(0), x_elt.z(0), x_elt.phi(0));
      position = Transform3D(pos);
      rotation = Transform3D();
      flag = 1;
    }
    else if ( tag == "position" )   {
      if      ( flag == 1 ) result = position  * result;
      else if ( flag == 2 ) result = (position * rotation) * result;
      Position pos(x_elt.x(0), x_elt.y(0), x_elt.z(0));
      position = Transform3D(pos);
      rotation = Transform3D();
      flag = 1;
    }
    else if ( tag == "transformation" )   {
      if      ( flag == 1 ) result = position  * result;
      else if ( flag == 2 ) result = (position * rotation) * result;
      Transform3D trafo = createTransformation(c);
      result   = trafo * result;
      position = Transform3D();
      rotation = Transform3D();
      flag = 0;
    }
    else if ( tag == "rotation" )   {
      rotation = Transform3D(RotationZYX(x_elt.z(0), x_elt.y(0), x_elt.x(0)));
      flag = 2;
    }
  }
  if      ( flag == 1 ) result = position  * result;
  else if ( flag == 2 ) result = (position * rotation) * result;
  return result;
}

/// Create a solid shape using the plugin mechanism from the attributes of the XML element
Solid dd4hep::xml::createShape(Detector& description,
                               const std::string& shape_type,
                               xml::Element element)   {
  string fac  = shape_type + "__shape_constructor";
  xml::Handle_t solid_elt = element;
  Solid solid = Solid(PluginService::Create<TObject*>(fac, &description, &solid_elt));
  if ( !solid.isValid() )  {
    PluginDebug dbg;
    PluginService::Create<TObject*>(fac, &description, &solid_elt);
    except("xml::createShape","Failed to create solid of type %s [%s]", 
           shape_type.c_str(),dbg.missingFactory(shape_type).c_str());
  }
  return solid;
}

/// Create a volume using the plugin mechanism from the attributes of the XML element
Volume dd4hep::xml::createStdVolume(Detector& description, xml::Element element)    {
  xml_dim_t e(element);
  if ( e.hasAttr(_U(material)) )   {
    xml_dim_t x_s = e.child(_U(shape));
    string    typ = x_s.typeStr();
    Material  mat = description.material(e.attr<string>(_U(material)));
    Solid     sol = createShape(description, typ, x_s);
    Volume    vol("volume", sol, mat);
    if ( e.hasAttr(_U(name)) ) vol->SetName(e.attr<string>(_U(name)).c_str());
    vol.setAttributes(description,e.regionStr(),e.limitsStr(),e.visStr());
    return vol;
  }
  xml_h h = e;
  xml_attr_t a = h.attr_nothrow(_U(type));
  if ( a )   {
    string typ = h.attr<string>(a);
    if ( typ.substr(1) == "ssembly" )  {
      Assembly vol("assembly");
      if ( e.hasAttr(_U(name)) ) vol->SetName(e.attr<string>(_U(name)).c_str());
      vol.setAttributes(description,e.regionStr(),e.limitsStr(),e.visStr());
      return vol;
    }
  }
  except("xml::createVolume","Failed to create volume. No material specified!");
  return Volume();
}

/// Create a volume using the plugin mechanism from the attributes of the XML element
Volume dd4hep::xml::createVolume(Detector& description,
                                 const std::string& typ,
                                 xml::Element element)   {
  if ( !typ.empty() )   {
    xml_dim_t e(element);
    string fac = typ + "__volume_constructor";
    xml::Handle_t elt = element;
    TObject* obj = PluginService::Create<TObject*>(fac, &description, &elt);
    Volume vol = Volume(dynamic_cast<TGeoVolume*>(obj));
    if ( !vol.isValid() )  {
      PluginDebug dbg;
      PluginService::Create<TObject*>(fac, &description, &elt);
      except("xml::createShape","Failed to create volume of type %s [%s]", 
             typ.c_str(),dbg.missingFactory(typ).c_str());
    }
    if ( e.hasAttr(_U(name)) ) vol->SetName(e.attr<string>(_U(name)).c_str());
    vol.setAttributes(description,e.regionStr(),e.limitsStr(),e.visStr());
    return vol;
  }
  except("xml::createVolume","Failed to create volume. No materiaWNo type specified!");
  return Volume();
}

Volume dd4hep::xml::createPlacedEnvelope( dd4hep::Detector& description,
                                          dd4hep::xml::Handle_t e, 
                                          dd4hep::DetElement sdet)
{  
  xml_det_t     x_det     = e;
  string        det_name  = x_det.nameStr();
  
  xml_comp_t    x_env     =  x_det.child( dd4hep::xml::Strng_t("envelope") ) ;
  xml_comp_t    x_shape   =  x_env.child( _U(shape) ); 
  
  
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

  Volume  envelope  ;

  if(  x_shape.typeStr() == "Assembly" ){
    envelope = Assembly( det_name+"_assembly" ) ;
  } else { 
    // ---- create a shape from the specified xml element --------
    Box  env_solid = xml_comp_t( x_shape ).createShape();
    
    if( !env_solid.isValid() ){
      
      throw std::runtime_error( std::string(" Cannot create envelope volume : ") + x_shape.typeStr() + 
                                std::string(" for detector " ) + det_name ) ;
    }

    Material      env_mat   = description.material( x_shape.materialStr() );
  
    envelope = Volume( det_name+"_envelope", env_solid, env_mat );
  }


  PlacedVolume  env_pv  ; 


  Volume        mother = description.pickMotherVolume(sdet);


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
  envelope.setAttributes( description,x_det.regionStr(),x_det.limitsStr(),x_env.visStr());
  return envelope;
}


void  dd4hep::xml::setDetectorTypeFlag( dd4hep::xml::Handle_t e, dd4hep::DetElement sdet )  {
  xml_det_t     x_det     = e;
  string        det_name  = x_det.nameStr();
  
  try{
    xml_comp_t  x_dettype = x_det.child( dd4hep::xml::Strng_t("type_flags") ) ;
    unsigned int typeFlag = x_dettype.type() ;
    printout(DEBUG,"Utilities","+++ setDetectorTypeFlags for detector: %s set to 0x%x", det_name.c_str(), typeFlag ) ;
    sdet.setTypeFlag( typeFlag ) ;
  }
  catch(const std::runtime_error& err)   {
    printout(INFO,"Utilities",
             "+++ setDetectorTypeFlags for detector: %s not set.",
             det_name.c_str() );
    printout(DEBUG, "Utilities",
             "+++ setDetectorTypeFlags encountered an error:\n%s", err.what());
  }
}

namespace   {
  template <typename TYPE>
  std::size_t _propagate(bool      debug,
			 bool      apply_to_children, 
			 Volume    vol,
			 TYPE      item, 
			 const Volume& (Volume::*apply)(const TYPE&)  const)   {
    std::size_t count = 0;
    if ( !vol->IsAssembly() )  {
      printout(debug ? ALWAYS : DEBUG,"VolumeConfig", "++ Volume: %s apply setting %s", vol.name(), item.name());
      (vol.*apply)(item);
      ++count;
    }
    if ( apply_to_children )   {
      std::set<Volume> handled; 
      for (Int_t idau = 0, ndau = vol->GetNdaughters(); idau < ndau; ++idau)   {
	Volume v = vol->GetNode(idau)->GetVolume();
	if ( handled.find(v) == handled.end() )   {
	  handled.insert(v);
	  count += _propagate(debug, apply_to_children, v, item, apply);
	}
      }
    }
    return count;
  }

}

/// Configure volume properties from XML element
std::size_t dd4hep::xml::configVolume( dd4hep::Detector&     detector,
				       dd4hep::xml::Handle_t element,
				       dd4hep::Volume        vol,
				       bool                  propagate,
				       bool                  ignore_unknown)
{
  std::size_t count = 0;
  if ( element )   {
    xml::Attribute x_dbg = element.attr_nothrow(_U(debug));
    bool           debug = x_dbg ? element.attr<bool>(x_dbg) : false;
    PrintLevel     lvl   = debug ? ALWAYS : DEBUG;
    for( xml_coll_t coll(element, "*"); coll; coll++ )   {
      xml_dim_t      itm = coll;
      std::string    nam   = itm.nameStr("UN-NAMED");

      if ( itm.tag() == TAG_REGIONREF )   {
	Region region = detector.region(nam);
	count += _propagate(debug, propagate, vol.ptr(), region, &Volume::setRegion);
	printout(lvl, "VolumeConfig", "++ %-12s: %-10s REGIONs   named '%s'",
		 vol.name(), region.isValid() ? "Set" : "Invalidate", nam.c_str());
      }
      else if ( itm.tag() == TAG_LIMITSETREF )   {
	LimitSet limitset = detector.limitSet(nam);      
	count += _propagate(debug, propagate, vol.ptr(), limitset, &Volume::setLimitSet);
	printout(lvl, "VolumeConfig", "++ %-12s: %-10s LIMITSETs named '%s'",
		 vol.name(), limitset.isValid() ? "Set" : "Invalidate", nam.c_str());
      }
      else if ( itm.tag() == TAG_VISREF )   {
	VisAttr attrs = detector.visAttributes(nam);
	count += _propagate(debug, propagate, vol.ptr(), attrs, &Volume::setVisAttributes);
	printout(lvl, "VolumeConfig", "++ %-12s: %-10s VISATTRs  named '%s'",
		 vol.name(), attrs.isValid() ? "Set" : "Invalidate", nam.c_str());
      }
      else if ( !ignore_unknown )  {
	except("VolumeConfig", "++ Unknown Volume property: %s [Ignored]", itm.tag().c_str());
      }
      else  {
	printout(DEBUG, "VolumeConfig", "++ Unknown Volume property: %s [Ignored]", itm.tag().c_str());
      }
    }
    return count;
  }
  except("VolumeConfig","++ Invalid XML handle to configure DetElement!");
  return count;
}

/// Configure sensitive detector from XML element
std::size_t dd4hep::xml::configSensitiveDetector( dd4hep::Detector&        /* detector */,
						  dd4hep::SensitiveDetector   sensitive,
						  dd4hep::xml::Handle_t       element)
{
  std::size_t count = 0;
  if ( sensitive.isValid() && element )    {
    xml::Attribute x_dbg = element.attr_nothrow(_U(debug));
    bool           debug = x_dbg ? element.attr<bool>(x_dbg) : false;
    PrintLevel     lvl   = debug ? ALWAYS : DEBUG;

    for( xml_coll_t coll(element, "*"); coll; coll++ )   {
      xml_dim_t itm = coll;
      if ( itm.tag() == TAG_COMBINE_HITS )   {
	bool value = itm.attr<bool>(_U(value));
	sensitive.setCombineHits(value);
	++count;
	printout(lvl, "SensDetConfig", "++ %s Set property 'combine_hits' to %s",
		 sensitive.name(), true_false(value));
      }
      else if ( itm.tag() == TAG_VERBOSE )   {
	bool value = itm.attr<bool>(_U(value));
	sensitive.setVerbose(value);
	++count;
	printout(lvl, "SensDetConfig", "++ %s Set property 'verbose' to %s",
		 sensitive.name(), true_false(value));
      }
      else if ( itm.tag() == TAG_TYPE )   {
	std::string value = itm.valueStr();
	sensitive.setType(value);
	++count;
	printout(lvl, "SensDetConfig", "++ %s Set property 'type' to %s",
		 sensitive.name(), value.c_str());
      }
      else if ( itm.tag() == TAG_ECUT )   {
	double value = itm.attr<double>(_U(value));
	sensitive.setEnergyCutoff(value);
	++count;
	printout(lvl, "SensDetConfig", "++ %s Set property 'ecut' to %f",
		 sensitive.name(), value);
      }
      else if ( itm.tag() == TAG_HITS_COLLECTION )   {
	sensitive.setHitsCollection(itm.valueStr());
	++count;
	printout(lvl, "SensDetConfig", "++ %s Set property 'hits_collection' to %s",
		 sensitive.name(), itm.valueStr().c_str());
      }
      else   {
	except("SensDetConfig",
	       "++ Unknown Sensitive Detector property: %s [Failure]",
	       itm.tag().c_str());
      }
    }
    return count;
  }
  except("SensDetConfig",
	 "FAILED: No valid sensitive detector. Configuration could not be applied!");
  return count;
}

#endif
