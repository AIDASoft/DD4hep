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
#include "XML/VolumeBuilder.h"
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

/// Initializing constructor
VolumeBuilder::VolumeBuilder(Detector& dsc, xml_h x_parent, SensitiveDetector sd)
  : description(dsc), x_det(x_parent), sensitive(sd)
{
  if ( x_det )   {
    name     = x_det.nameStr();
    id       = x_det.id();
    detector = DetElement(name, id);
  }
  buildType = description.buildType();
  debug = true;
}

/// Collect a set of materials from the leafs of an xml tag
size_t VolumeBuilder::collectMaterials(xml_h element)   {
  size_t len = materials.size();
  for( xml_coll_t c(element,_U(material)); c; ++c )   {
    xml_comp_t x_c = c;
    string   nam = x_c.nameStr();
    string   val = x_c.valueStr();
    Material mat = description.material(val);
    materials[nam] = mat;
  }
  return materials.size()-len;
}

/// Register shape to map
void VolumeBuilder::registerShape(const std::string& nam, Solid shape)   {
  auto is = shapes.find(nam);
  if ( is == shapes.end() )  {
    shapes[nam] = make_pair(xml_h(0), shape);
    return;
  }
  except("VolumeBuilder","+++ Shape %s is already known to this builder unit. ",nam.c_str());
}

/// Register volume to map
void VolumeBuilder::registerVolume(const std::string& nam, Volume volume)   {
  auto is = volumes.find(nam);
  if ( is == volumes.end() )  {
    volumes[nam] = make_pair(xml_h(0), volume);
    return;
  }
  except("VolumeBuilder","+++ Volume %s is already known to this builder unit. ",nam.c_str());
}

/// Access a registered volume by name
Volume VolumeBuilder::volume(const std::string& nam)  const    {
  auto iv = volumes.find(nam);
  if ( iv == volumes.end() )  {
    auto ib = vol_veto.find(nam);
    if ( ib != vol_veto.end() )  {
      // Veto'ed shape. Ignore it.
      return Volume();
    }
    except("VolumeBuilder","+++ Volume %s is not known to this builder unit. ",nam.c_str());
  }
  Volume vol = (*iv).second.second;
  if ( !vol.isValid() )   {
    except("VolumeBuilder","+++ Failed to access volume %s from the local cache.",nam.c_str());
  }
  return vol;
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
    if ( is != shapes.end() )  {
      except("VolumeBuilder","+++ The named shape %s is already known to this builder unit. "
             "Cannot be overridded.",nam.c_str());
    }
  }
  /// Was it veto'ed before ?
  if ( !nam.empty() )   {
    auto iv = shape_veto.find(nam);
    if ( iv != shape_veto.end() )  {
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
  printout(debug ? ALWAYS : INFO, "VolumeBuilder",
           "+++ Created shape of type: %s name: %s",type.c_str(), nam.c_str());
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
      Solid  solid = xml::createShape(description, type, c);
      if ( !solid.isValid() )   {
        except("VolumeBuilder","+++ Failed to create shape %s of type: %s",
               nam.c_str(), type.c_str());
      }
      if ( debug )  {
        printout(ALWAYS,"VolumeBuilder","+++ Building shape  from XML: %s",nam.c_str());
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
    /// Check if the volume is implemented by a factory
    if ( (attr=c.attr_nothrow(_U(type))) )   {
      string typ = c.attr<string>(attr);
      Volume vol = xml::createVolume(description, typ, c);
      vol.setAttributes(description,x.regionStr(),x.limitsStr(),x.visStr());
      volumes.insert(make_pair(nam,make_pair(c,vol)));
      /// Check if the volume is sensitive
      if ( c.attr_nothrow(_U(sensitive)) )   {
        vol.setSensitiveDetector(sensitive);
      }
      if ( debug )  {
        printout(ALWAYS,"VolumeBuilder","+++ Building volume from XML: %s",nam.c_str());
      }
      buildVolumes(c);
      continue;
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
      if ( debug )  {
        printout(ALWAYS,"VolumeBuilder","+++ Building volume from XML: %s",nam.c_str());
      }
      buildVolumes(c);
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
      buildVolumes(c);
      continue;
    }
    except("VolumeBuilder","+++ Failed to create volume %s - "
           "It is neither Volume nor assembly....", nam.c_str());
  }
  return volumes.size()-len;
}

/// Place single volumes
void VolumeBuilder::_placeSingleVolume(DetElement parent, Volume vol, xml_h c)   {
  xml_attr_t attr = c.attr_nothrow(_U(logvol));
  if ( !attr )   {
    attr = c.attr_nothrow(_U(volume));
  }
  if ( !attr )   {
    except("VolumeBuilder","+++ The xml volume element has no 'logvol' or 'volume' attribute!");
  }
  string nam = c.attr<string>(attr);
  if ( vol_veto.find(nam) != vol_veto.end() )   {
    return;
  }
  auto iv = volumes.find(nam);
  if ( iv == volumes.end() )  {
    except("VolumeBuilder",
           "+++ Failed to locate volume %s [typo somewhere in the XML?]",
           nam.c_str());      
  }
  PlacedVolume pv;
  Volume daughter = (*iv).second.second;
  attr = c.attr_nothrow(_U(transformation));
  if ( attr )   {
    string tr_nam = c.attr<string>(attr);
    auto it = transformations.find(tr_nam);
    if ( it == transformations.end() )   {
      except("VolumeBuilder",
             "+++ Failed to locate name transformation %s "
             "[typo somewhere in the XML?]",
             nam.c_str());      
    }
    const Transform3D& tr = (*it).second.second;
    pv = vol.placeVolume(daughter, tr);
  }
  else  {
    Transform3D tr = xml::createTransformation(c);
    pv = vol.placeVolume(daughter, tr);
  }
  xml_attr_t attr_nam = c.attr_nothrow(_U(name));
  if ( attr_nam )  {
    string phys_nam = c.attr<string>(attr_nam);
    pv->SetName(phys_nam.c_str());
  }
  attr = c.attr_nothrow(_U(element));
  if ( attr && !parent.isValid() )  {
    except("VolumeBuilder",
           "+++ Failed to set DetElement placement for volume %s [Invalid parent]",
           nam.c_str());      
  }
  else if ( attr )  {
    int parent_id = parent.id();
    string elt = c.attr<string>(attr);
    attr = c.attr_nothrow(_U(id));
    if ( attr )   {
      id = c.attr<int>(attr);
      elt += c.attr<string>(attr);
    }
    DetElement de(parent,elt,parent_id);
    de.setPlacement(pv);
    placeDaughters(de, daughter, c);
  }
  else  {
    placeDaughters(parent, daughter, c);
  }
}

/// Place parametrized volumes
void VolumeBuilder::_placeParamVolumes(DetElement parent, Volume vol, xml_h c)   {
  xml_attr_t attr_tr, attr_elt, attr_nam;
  xml_h      x_phys = c.child(_U(physvol));
  xml_attr_t attr = x_phys.attr_nothrow(_U(logvol));
  if ( !attr )   {
    attr = x_phys.attr_nothrow(_U(volume));
  }
  if ( !attr )   {
    except("VolumeBuilder","+++ The xml volume element has no 'logvol' or 'volume' attribute!");
  }
  string nam = x_phys.attr<string>(attr);
  if ( vol_veto.find(nam) != vol_veto.end() )   {
    return;
  }
  auto iv = volumes.find(nam);
  if ( iv == volumes.end() )  {
    except("VolumeBuilder",
           "+++ Failed to locate volume %s [typo somewhere in the XML?]",
           nam.c_str());      
  }
  attr_elt = c.attr_nothrow(_U(element));
  if ( attr_elt && !parent.isValid() )  {
    except("VolumeBuilder",
           "+++ Failed to set DetElement placement for volume %s [Invalid parent]",
           nam.c_str());      
  }
  Volume daughter = (*iv).second.second;
  attr_tr = c.attr_nothrow(_U(transformation));
  Transform3D tr;
  if ( attr_tr )   {
    string tr_nam = c.attr<string>(attr_tr);
    auto it = transformations.find(tr_nam);
    if ( it == transformations.end() )   {
      except("VolumeBuilder",
             "+++ Failed to locate name transformation %s "
             "[typo somewhere in the XML?]",
             nam.c_str());      
    }
    tr = (*it).second.second;
  }
  else  {
    tr = xml::createTransformation(c);
  }
  Transform3D transformation(Position(0,0,0));
  int parent_id = -1;
  string elt, phys_nam;
  attr_nam = x_phys.attr_nothrow(_U(name));
  if ( attr_nam )  {
    phys_nam = x_phys.attr<string>(_U(name))+"_%d";
  }
  if ( attr_elt )  {
    parent_id = parent.id();
    elt = c.attr<string>(attr_elt);
  }
  int number = c.attr<int>(_U(number));
  printout(debug ? ALWAYS : DEBUG,"VolumeBuilder","+++ Mother:%s place volume %s  %d times.",
           vol.name(), daughter.name(), number);
  for(int i=0; i<number; ++i)    {
    PlacedVolume pv = vol.placeVolume(daughter, transformation);
    if ( attr_nam )  {
      //pv->SetName(_toString(i,phys_nam.c_str()).c_str());
    }
    if ( attr_elt )  {
      DetElement de(parent,elt,parent_id);
      de.setPlacement(pv);
      //placeDaughters(de, daughter, c);
    }
    else  {
      //placeDaughters(parent, daughter, c);
    }
    transformation *= tr;
  }
}

/// Build all <physvol/> identifiers as PlaceVolume daughters. Ignores structure
VolumeBuilder& VolumeBuilder::placeDaughters(Volume vol, xml_h handle)   {
  DetElement null_de;
  return placeDaughters(null_de, vol, handle);
}

/// Build all <physvol/> identifiers as PlaceVolume daughters. Also handles structure
VolumeBuilder& VolumeBuilder::placeDaughters(DetElement parent, Volume vol, xml_h handle)   {
  for( xml_coll_t c(handle,_U(physvol)); c; ++c )
    _placeSingleVolume(parent, vol, c);
  for( xml_coll_t c(handle,_U(paramphysvol)); c; ++c )
    _placeParamVolumes(parent, vol, c);
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
  xml_comp_t   x     = handle;
  xml_dim_t    x_pos = x_det.child(_U(position),false);
  xml_dim_t    x_rot = x_det.child(_U(rotation),false);
  xml_dim_t    x_tr  = x_det.child(_U(transformation),false);
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
