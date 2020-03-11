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

/// Framework include files
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/Printout.h>
#include <XML/Utilities.h>
#include <DDCAD/ASSIMPReader.h>

// C/C++ include files

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Handle<TObject> create_CAD_Shape(Detector&, xml_h e)   {
  xml_elt_t elt(e);
  string fname = elt.attr<string>(_U(ref));
  double unit  = elt.hasAttr(_U(unit)) ? elt.attr<double>(_U(unit)) : dd4hep::cm;
  auto shapes = cad::ASSIMPReader().read(fname, unit);
  if ( shapes.empty() )   {
    except("CAD_Shape","+++ CAD file: %s does not contain any "
           "understandable tessellated shapes.", fname.c_str());
  }
  Solid solid;
  size_t count = shapes.size();
  if ( count == 1 )   {
    solid = shapes[0].release();
  }
  else   {
    if ( elt.hasAttr(_U(item)) )  {
      size_t which = elt.attr<int>(_U(item));
      solid = shapes[which].release();
    }
    else if ( elt.hasAttr(_U(mesh)) )  {
      size_t which = elt.attr<int>(_U(mesh));
      solid = shapes[which].release();
    }
    else  {
      except("CAD_Shape","+++ CAD file: %s does contains %ld tessellated shapes. "
             "You need to add a selector.", fname.c_str(), shapes.size());
    }
  }
  if ( elt.hasAttr(_U(name)) ) solid->SetName(elt.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(CAD_Shape__shape_constructor,create_CAD_Shape)


static Handle<TObject> create_CAD_MultiShape_Assembly(Detector&, xml_h e)   {
  xml_elt_t elt(e);
  string fname = elt.attr<string>(_U(ref));
  double unit  = elt.hasAttr(_U(unit)) ? elt.attr<double>(_U(unit)) : dd4hep::cm;
  auto shapes = cad::ASSIMPReader().read(fname, unit);
  if ( shapes.empty() )   {
    except("CAD_Shape","+++ CAD file: %s does not contain any "
           "understandable tessellated shapes.", fname.c_str());
  }
  Assembly assembly("assembly");
  for(size_t i=0; i < shapes.size(); ++i)   {
    Solid solid = shapes[i].release();
    if ( solid.isValid() )   {
      Volume vol(_toString(int(i),"vol_%d"), solid, Detector::getInstance().material("Air"));
      assembly.placeVolume(vol);
    }
  }
  if ( elt.hasAttr(_U(name)) ) assembly->SetName(elt.attr<string>(_U(name)).c_str());
  return assembly;
}
DECLARE_XML_VOLUME(CAD_Assembly__volume_constructor,create_CAD_MultiShape_Assembly)


/// CAD volume importer plugin
/**
 *
 * The CAD volume plugin allows to embed valumes and shapes originating from
 * Computer Aided Design drawings using multiple formats as they are supported
 * by the open asset importer library (http://assimp.org ).
 * The plugin can be used whenever the xmnl fragment matches the following pattern:
 *
 *   <XXX ref="file-name"  material="material-name">   
 *     <material name="material-name"/>                        <!-- alternative: child or attr -->
 *
 *     Envelope:  Use special envelop shape (default: assembly)
 *                The envelope tag must match the expected pattern of the utility
 *                dd4hep::xml::createStdVolume(Detector& desc, xml::Element e)
 *     <envelope name="volume-name" material="material-name">
 *       <shape name="shape-name" type="shape-type" args....>
 *       </shape>
 *     </envelope>
 *
 *     Option 1:  No additional children. use default material 
 *                and place all children in the origin of the envelope
 *
 *     Option 2:  Volume with default material
 *     <volume name="vol-name"/>
 *
 *     Option 3:  Volume with non-default material
 *     <volume name="vol-name" material="material-name"/>
 *
 *     Option 4:  Volume with optional placement. No position = (0,0,0), No rotation = (0,0,0)
 *     <volume name="vol-name" material="material-name"/>
 *       <position x="0" y="0" z="5*cm"/>
 *       <rotation x="0" y="0" z="0.5*pi*rad"/>
 *     </volume>
 *
 *     For sensitive volumes: add physical volume IDs:
 *     <volume name="vol-name" material="material-name"/>
 *       <physvolid name="layer" value="1"/>
 *       <physvolid name="slice" value="10"/>
 *     </volume>
 *
 *   </XXX>
 */
static Handle<TObject> create_CAD_Volume(Detector& dsc, xml_h e)   {
  xml_elt_t elt(e);
  string fname = elt.attr<string>(_U(ref));
  double unit  = elt.attr<double>(_U(unit));
  auto shapes = cad::ASSIMPReader().read(fname, unit);
  if ( shapes.empty() )   {
    except("CAD_Volume","+++ CAD file: %s does not contain any "
           "understandable tessellated shapes.", fname.c_str());
  }
  Volume envelope;
  if ( elt.hasChild(_U(envelope)) )   {
    string   typ   = "DD4hep_StdVolume";
    xml_h    x_env = elt.child(_U(envelope));
    TObject* pvol  = PluginService::Create<TObject*>(typ, &dsc, &x_env);
    envelope = dynamic_cast<TGeoVolume*>(pvol);
    if ( !envelope.isValid() )   {
      except("CAD_Volume",
             "+++ Unable to determine envelope to CAD shape: %s",fname.c_str());
    }
  }
  else   {
    envelope = Assembly("envelope");
  }
  xml_dim_t x_envpos = elt.child(_U(position),false);
  xml_dim_t x_envrot = elt.child(_U(rotation),false);
  Position env_pos;
  RotationZYX env_rot;
  if ( x_envpos && x_envrot )   {
    env_rot = RotationZYX(x_envrot.z(0), x_envrot.y(0), x_envrot.x(0));
    env_pos = Position(x_envpos.x(0), x_envpos.y(0), x_envpos.z(0));
  }
  else if ( x_envpos )
    env_pos = Position(x_envpos.x(0), x_envpos.y(0), x_envpos.z(0));
  else if ( x_envrot )
    env_rot = RotationZYX(x_envrot.z(0), x_envrot.y(0), x_envrot.x(0));

  Transform3D env_trafo(env_rot, env_pos);
  Material default_material;
  xml_dim_t x_mat = elt.child(_U(material),false);
  if      ( x_mat.ptr() ) default_material = dsc.material(x_mat.nameStr());
  else if ( elt.hasAttr(_U(material)) ) default_material = dsc.material(elt.attr<string>(_U(material)));

  if ( elt.hasChild(_U(volume)) )   {
    map<int, xml_h> shape_map;
    for (xml_coll_t c(elt,_U(volume)); c; ++c )
      shape_map.emplace(xml_dim_t(c).id(),c);

    for (size_t i=0; i < shapes.size(); ++i)   {
      Solid       sol = shapes[i].release();
      Material    mat = default_material;
      auto is = shape_map.find(i);
      if ( is == shape_map.end() )   {
        Volume vol(_toString(int(i),"vol_%d"), sol, mat);
        envelope.placeVolume(vol);
      }
      else   {
        xml_dim_t x_vol = (*is).second;
        xml_dim_t x_pos = x_vol.child(_U(position),false);
        xml_dim_t x_rot = x_vol.child(_U(rotation),false);
        string     vnam = x_vol.hasAttr(_U(name)) ? x_vol.attr<string>(_U(name)) : _toString(int(i),"vol_%d");

        if ( x_vol.hasAttr(_U(material)) )  {
          mat = dsc.material(x_vol.attr<string>(_U(material)));
        }
        Position    pos;
        RotationZYX rot;
        if ( x_pos && x_rot )   {
          rot = RotationZYX(x_rot.z(0), x_rot.y(0), x_rot.x(0));
          pos = Position(x_pos.x(0), x_pos.y(0), x_pos.z(0));
        }
        else if ( x_pos )
          pos = Position(x_pos.x(0), x_pos.y(0), x_pos.z(0));
        else if ( x_rot )
          rot = RotationZYX(x_rot.z(0), x_rot.y(0), x_rot.x(0));
      
        Volume vol(vnam, sol, mat);
        PlacedVolume pv = envelope.placeVolume(vol,env_trafo*Transform3D(rot, pos));
        vol.setAttributes(dsc, x_vol.regionStr(), x_vol.limitsStr(), x_vol.visStr());
        for (xml_coll_t cc(x_vol,_U(physvolid)); cc; ++cc )   {
          xml_dim_t vid = cc;
          pv.addPhysVolID(vid.nameStr(), vid.attr<int>(_U(value)));
        }
      }
    }
  }
  else   {
    for(size_t i=0; i < shapes.size(); ++i)   {
      Solid solid = shapes[i].release();
      if ( solid.isValid() )   {
        Volume vol(_toString(int(i),"vol_%d"), solid, default_material);
        envelope.placeVolume(vol);
      }
    }
  }
  if ( elt.hasAttr(_U(name)) ) envelope->SetName(elt.attr<string>(_U(name)).c_str());
  return envelope;
}
DECLARE_XML_VOLUME(CAD_MultiVolume__volume_constructor,create_CAD_Volume)
