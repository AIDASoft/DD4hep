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

/// Create layered transformation from xml information
Transform3D dd4hep::xml::createTransformation(xml::Element e)   {
  int cnt = 0;
  Position pos;
  RotationZYX rot;
  Transform3D result;
  for( xml_coll_t c(e,_U(star)); c; ++c )    {
    string tag = c.tag();
    xml_dim_t x_elt = c;
    if ( tag == "positionRPhiZ" )   {
      Transform3D trafo = Transform3D(pos);
      if ( cnt > 1 ) trafo *= Transform3D(rot);
      result = trafo * result;
      ROOT::Math::Polar2D<double> dim(x_elt.r(0.0), x_elt.phi(0.0));
      pos.SetXYZ(dim.X(), dim.Y(), x_elt.z(0.0));
      rot.SetComponents(0,0,0);
      cnt = 1;
    }
    else if ( tag == "position" )   {
      Transform3D trafo = Transform3D(pos);
      if ( cnt > 1 ) trafo *= Transform3D(rot);
      result = trafo * result;
      pos.SetXYZ(x_elt.x(0), x_elt.y(0), x_elt.z(0));
      rot.SetComponents(0,0,0);
      cnt = 1;
    }
    else if ( tag == "rotation" )   {
      rot = RotationZYX(x_elt.z(0), x_elt.y(0), x_elt.x(0));
      cnt = 2;
    }
  }
  if ( cnt > 0 )   {
    Transform3D trafo = Transform3D(pos);
    if ( cnt > 1 ) trafo *= Transform3D(rot);
    result = trafo * result;
  }
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
    PluginService::Create<TObject*>(shape_type, &description, &solid_elt);
    except("xml::createShape","Failed to create solid of type %s [%s]", 
           shape_type.c_str(),dbg.missingFactory(shape_type).c_str());
  }
  return solid;
}

/// Create a volume using the plugin mechanism from the attributes of the XML element
Volume dd4hep::xml::createVolume(Detector& description, xml::Element element)    {
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
  except("xml::createVolume","Failed to create volume. No material specified!");
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
    printout(DEBUG,"Utilities","+++ setDetectorTypeFlags for detector :%s set to 0x%x", det_name.c_str(), typeFlag ) ; 
    sdet.setTypeFlag( typeFlag ) ;
  }
  catch(std::runtime_error)   {
    printout(INFO,"Utilities",
             "+++ setDetectorTypeFlags for detector :%s no xml element <type_flags/> found - nothing to set ",
             det_name.c_str() ) ; 
  }
}

/// Initializing constructor
VolumeBuilder::VolumeBuilder(Detector& dsc, xml_h x_parent, SensitiveDetector sd)
  : description(dsc), x_det(x_parent), sensitive(sd)
{
  if ( x_det )   {
    xml_comp_t c(x_det);
    string name = c.nameStr();
    detector = DetElement(name, c.id());
  }
  buildType = description.buildType();
}

/// Access element from shape cache by name. Invalid returns means 'veto'. Otherwise exception
Solid VolumeBuilder::getShape(const string& nam)  const   {
  auto is = shapes.find(nam);
  if ( is == shapes.end() )  {
    auto ib = shape_veto.find(nam);
    if ( ib != shape_veto.end() )  {
      // Veto'ed shape. Ignore it.
      return Solid();
    }
    except("VolumeBuilder","+++ Shape %s is not known to this builder unit. ",nam.c_str());
  }
  Solid solid = (*is).second.second;
  if ( !solid.isValid() )   {
    except("VolumeBuilder","+++ Failed to access shape %s from the local cache.",nam.c_str());
  }
  return solid;
}

/// Create a new shape from the information given in the xml handle
Solid VolumeBuilder::makeShape(xml_h handle)   {
  xml_comp_t x = handle;
  string     nam;
  xml_attr_t a = handle.attr_nothrow(_U(name));
  if ( a )   {
    nam = handle.attr<string>(a);
    auto is = shapes.find(nam);
    if ( is == shapes.end() )  {
      except("VolumeBuilder","+++ The named shape %s is already known to this builder unit. "
             "Cannot be overridded.",nam.c_str());
    }
  }
  /// Was it veto'ed before ?
  if ( !nam.empty() )   {
    auto iv = shape_veto.find(nam);
    if ( iv == shape_veto.end() )  {
      return Solid();
    }
  }
  /// Check if this volume is part of the volumes to be built for this description type
  a = handle.attr_nothrow(_U(build));
  if ( a )   {
    string build = handle.attr<string>(a);
    if ( !buildMatch(build,buildType) )  {
      printout(INFO,"VolumeBuilder",
               "+++ Shape %s does NOT match build requirements. [Ignored]",nam.c_str());
      if ( !nam.empty() ) shape_veto.insert(nam);
      return Solid();
    }
  }
  /// Now we create the shape....
  string type = x.attr<string>(_U(type));
  Solid solid = xml::createShape(description, type, x);
  if ( !solid.isValid() )   {
    except("VolumeBuilder","+++ Failed to create shape %s of type: %s",
           nam.c_str(), type.c_str());
  }
  /// And register it if it is not anonymous
  if ( !nam.empty() )   {
    solid.setName(nam);
    shapes.insert(make_pair(nam,make_pair(handle,solid)));
  }
  return solid;
}

/// Build all <shape/> identifiers in the passed parent xml element
size_t VolumeBuilder::buildShapes(xml_h handle)    {
  size_t len = shapes.size();
  for( xml_coll_t c(handle,_U(shape)); c; ++c )   {
    xml_elt_t x = c;
    string nam = x.attr<string>(_U(name));
    auto is = shapes.find(nam);
    if ( is == shapes.end() )  {
      /// Check if this volume is part of the volumes to be built for this description type
      xml_attr_t x_build = c.attr_nothrow(_U(build));
      if ( x_build )   {
        string build = c.attr<string>(x_build);
        if ( !buildMatch(build,buildType) )  {
          printout(INFO,"VolumeBuilder",
                   "+++ Shape %s does NOT match build requirements. [Ignored]",nam.c_str());
          shape_veto.insert(nam);
          continue;
        }
      }
      string type  = x.attr<string>(_U(type));
      Solid  solid = xml::createShape(description, type,c);
      if ( solid.isValid() )   {
        except("VolumeBuilder","+++ Failed to create shape %s of type: %s",
               nam.c_str(), type.c_str());
      }
      shapes.insert(make_pair(nam,make_pair(c,solid)));
      continue;
    }
    except("VolumeBuilder","+++ Shape %s is already known to this builder unit. "
           "Cannot be overridded.",nam.c_str());
  }
  return shapes.size()-len;
}

/// Build all <volume/> identifiers in the passed parent xml element
size_t VolumeBuilder::buildVolumes(xml_h handle)    {
  size_t len = volumes.size();
  xml_elt_t  x_comp(0);
  for( xml_coll_t c(handle,_U(volume)); c; ++c )   {
    Solid solid;
    xml_comp_t x    = c;
    string     nam  = x.attr<string>(_U(name));
    xml_attr_t attr = c.attr_nothrow(_U(build));
    /// Check if this volume is part of the volumes to be built for this description type
    if ( attr )   {
      string build = c.attr<string>(attr);
      if ( !buildMatch(build,buildType) )  {
        printout(INFO,"VolumeBuilder",
                 "+++ Volume %s does NOT match build requirements. [Ignored]",nam.c_str());
        continue;
      }
    }
    /// Check if the volume has a shape attribute --> shape reference
    if ( (attr=c.attr_nothrow(_U(shape))) )   {
      string ref = c.attr<string>(attr);
      if ( !(solid=getShape(ref)).isValid() ) continue;
    }
    /// Else use anonymous shape embedded in volume
    else if ( (x_comp=x.child(_U(shape),false)) )  {
      if ( !(solid=makeShape(x_comp)).isValid() ) continue;
    }

    /// We have a real volume here with a concrete shape:
    if ( solid.isValid() )   {
      Material  mat = description.material(x.attr<string>(_U(material)));
      Volume    vol(nam, solid, mat);
      placeDaughters(detector, vol, x);
      vol.setAttributes(description,x.regionStr(),x.limitsStr(),x.visStr());
      volumes.insert(make_pair(nam,make_pair(c,vol)));
      /// Check if the volume is sensitive
      if ( c.attr_nothrow(_U(sensitive)) )   {
        vol.setSensitiveDetector(sensitive);
      }
      continue;
    }
    bool is_assembly = true;
    is_assembly |= x.child(_U(assembly),false) != 0;
    is_assembly |= c.attr_nothrow(_U(assembly)) != 0;
    if ( is_assembly )   {
      Assembly vol(nam);
      placeDaughters(detector, vol, x);
      vol.setAttributes(description,x.regionStr(),x.limitsStr(),x.visStr());
      volumes.insert(make_pair(nam,make_pair(c,vol)));
      continue;
    }
    except("VolumeBuilder","+++ Failed to create volume %s. "
           "It is neither Volume nor assembly....", nam.c_str());
  }
  return volumes.size()-len;
}

/// Build all <physvol/> identifiers as PlaceVolume daughters. Ignores structure
VolumeBuilder& VolumeBuilder::placeDaughters(Volume vol, xml_h handle)   {
  for( xml_coll_t c(handle,_U(physvol)); c; ++c )   {
    xml_comp_t x_comp = c;
    string nam = x_comp.attr<string>(_U(logvol));
    if ( vol_veto.find(nam) == vol_veto.end() )   {
      auto iv = volumes.find(nam);
      if ( iv == volumes.end() )  {
        except("VolumeBuilder","+++ Failed to locate volume %s. [typo somewhere in the XML?]",
               nam.c_str());      
      }
      xml_attr_t attr = c.attr_nothrow(_U(transformation));
      if ( attr )   {
        string tr_nam = c.attr<string>(attr);
        auto it = transformations.find(tr_nam);
        if ( it == transformations.end() )   {
          except("VolumeBuilder",
                 "+++ Failed to locate name transformation %s. "
                 "[typo somewhere in the XML?]",
                 nam.c_str());      
        }
        const Transform3D& tr = (*it).second.second;
        vol.placeVolume((*iv).second.second, tr);
      }
      else   {
        Transform3D tr = xml::createTransformation(c);
        vol.placeVolume((*iv).second.second, tr);
      }
    }
  }
  return *this;
}

/// Build all <physvol/> identifiers as PlaceVolume daughters. Also handles structure
VolumeBuilder& VolumeBuilder::placeDaughters(DetElement parent, Volume vol, xml_h handle)
{
  for( xml_coll_t c(handle,_U(physvol)); c; ++c )   {
    xml_comp_t x_comp = c;
    string nam = x_comp.attr<string>(_U(logvol));
    if ( vol_veto.find(nam) == vol_veto.end() )   {
      auto iv = volumes.find(nam);
      if ( iv == volumes.end() )  {
        except("VolumeBuilder",
               "+++ Failed to locate volume %s. [typo somewhere in the XML?]",
               nam.c_str());      
      }
      PlacedVolume pv;
      xml_attr_t attr = c.attr_nothrow(_U(transformation));
      if ( attr )   {
        string tr_nam = c.attr<string>(attr);
        auto it = transformations.find(tr_nam);
        if ( it == transformations.end() )   {
          except("VolumeBuilder",
                 "+++ Failed to locate name transformation %s. "
                 "[typo somewhere in the XML?]",
                 nam.c_str());      
        }
        const Transform3D& tr = (*it).second.second;
        pv = vol.placeVolume((*iv).second.second, tr);
      }
      else  {
        Transform3D tr = xml::createTransformation(c);
        pv = vol.placeVolume((*iv).second.second, tr);
      }
      if ( (attr=c.attr_nothrow(_U(element))) )  {
        int id = parent.id();
        string elt = c.attr<string>(attr);
        attr = c.attr_nothrow(_U(id));
        if ( attr )   {
          id = c.attr<int>(attr);
          elt += c.attr<string>(attr);
        }
        DetElement de(parent,elt,id);
        de.setPlacement(pv);
      }
    }
  }
  return *this;
}

/// Build all <transformation/> identifiers in the passed parent xml element
size_t VolumeBuilder::buildTransformations(Handle_t handle)   {
  size_t len = transformations.size();
  for( xml_coll_t c(handle,_U(transformation)); c; ++c )   {
    string nam = xml_comp_t(c).nameStr();
    transformations.insert(make_pair(nam,make_pair(c,xml::createTransformation(c))));
  }
  return transformations.size() - len;
}

/// Place the detector object into the mother volume returned by the Detector instance
PlacedVolume VolumeBuilder::placeDetector(Volume vol)    {
  return placeDetector(vol, x_det);
}

/// Place the detector object into the mother volume returned by the Detector instance
PlacedVolume VolumeBuilder::placeDetector(Volume vol, xml_h handle)    {
  xml_comp_t x     = handle;
  xml_dim_t  x_pos = x_det.child(_U(position),false);
  xml_dim_t  x_rot = x_det.child(_U(rotation),false);
  xml_dim_t  x_tr  = x_det.child(_U(transformation),false);
  Volume       mother = description.pickMotherVolume(detector);
  PlacedVolume pv;

  if ( x_tr )   {
    Transform3D tr = createTransformation(x_tr);
    pv = mother.placeVolume(vol, tr);
  }
  else if ( x_pos && x_rot )   {
    Transform3D tr = createTransformation(x_det);
    pv = mother.placeVolume(vol, tr);
  }
  else if ( x_pos )  {
    pv = mother.placeVolume(vol, Position(x_pos.x(0),x_pos.y(0),x_pos.z(0)));
  }
  else  {
    pv = mother.placeVolume(vol);
  }
  vol.setVisAttributes(description, x.visStr());
  vol.setLimitSet(description, x.limitsStr());
  vol.setRegion(description, x.regionStr());
  if ( detector.isValid() )  {
    detector.setPlacement(pv);
  }
  return pv;
}

#endif
