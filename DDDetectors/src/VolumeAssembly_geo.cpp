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
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/Printout.h"
#include "XML/Utilities.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;


namespace {
  /// Helper to create the volume assembly
  struct Builder  {
    /// Created volumes
    map<string, Volume> volumes;
    Detector&           description;
    /// Default constructor
    Builder(Detector& dsc) : description(dsc) {}

    /// Default destructor
    ~Builder() {}

    /// Place single volume in mother
    PlacedVolume placeVolume(DetElement parent, Volume mother, xml_h c)  {
      xml_dim_t x_vol = c;
      string vnam = x_vol.attr<string>(_U(volume));
      string name = x_vol.hasAttr(_U(name)) ? x_vol.nameStr() : string("");
      xml_dim_t x_tr  = x_vol.child(_U(transformation),false);
      xml_dim_t x_pos = x_vol.child(_U(position),false);
      xml_dim_t x_rot = x_vol.child(_U(rotation),false);
      PlacedVolume pv;

      auto iv = volumes.find(vnam);
      if ( iv == volumes.end() )   {
        except("VolumeAssembly","+++ Failed to attach unknown volume: %s",vnam.c_str());
      }
      Volume vol = (*iv).second;
      for(xml_coll_t coll(c,_U(physvol)); coll; ++coll)   {
        if ( coll.hasAttr(_U(element)) )  {
          DetElement de(parent, coll.attr<string>(_U(element)), parent.id());
          pv = placeVolume(de, vol, coll);
          de.setType("compound");
          de.setPlacement(pv);
          continue;
        }
        placeVolume(parent, vol, coll);
      }

      if ( x_tr )  {
        Transform3D tr = xml::createTransformation(x_tr);
        pv = mother.placeVolume(vol, tr);
      }
      else if ( x_rot && x_pos )   {
        Position pos(x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0));
        RotationZYX rot(x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0));
        pv = mother.placeVolume(vol, Transform3D(rot, pos));
      }
      else if ( x_rot )  {
        RotationZYX rot(x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0));
        pv = mother.placeVolume(vol, rot);
      }
      else if ( x_pos )   {
        Position pos(x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0));
        pv = mother.placeVolume(vol, pos);
      }
      else   {
        pv = mother.placeVolume(vol);
      }
      return pv;
    }

    Ref_t create(xml_h e, SensitiveDetector sens)  {
      xml_det_t   x_det(e);
      xml_comp_t  x_env = x_det.child(_U(envelope));
      xml_comp_t  x_envshape = x_env.child(_U(shape),false);
      string      det_name = x_det.nameStr();
      DetElement  sdet(det_name, x_det.id());
      Volume      assembly;
      PlacedVolume pv;

      if ( !x_envshape ) x_envshape = x_env;
      if ( x_envshape.typeStr() == "Assembly" )  {
        assembly = Assembly("lv"+det_name);
      }
      else  {
        Material mat   = description.material(x_env.materialStr());
        Solid    solid = xml::createShape(description, x_envshape.typeStr(), x_envshape);
        assembly = Volume("lv"+det_name, solid, mat);
      }
      /// Set generic associations
      assembly.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
      /// If specified more direct: use these ones.
      if ( x_env.hasAttr(_U(vis)) )  {
        assembly.setVisAttributes(description, x_env.visStr());
      }
      if ( x_det.hasAttr(_U(sensitive)) )  {
        sens.setType(x_det.attr<string>(_U(sensitive)));
      }
      if ( x_env.hasAttr(_U(name)) )   {
        assembly->SetName(x_env.nameStr().c_str());
      }

      for(xml_coll_t coll(e,_U(volume)); coll; ++coll)   {
        xml_comp_t x_vol = coll;
        Volume vol = xml::createVolume(description,x_vol);
        if ( x_vol.isSensitive() )  {
          vol.setSensitiveDetector(sens);
        }
        volumes.insert(make_pair(vol.name(), vol));
      }
      for(xml_coll_t coll(e,_U(physvol)); coll; ++coll)   {
        if ( coll.hasAttr(_U(element)) )  {
          string de_name = coll.attr<string>(_U(element));
          DetElement de(sdet, de_name, x_det.id());
          pv = placeVolume(de, assembly, coll);
          de.setType("compound");
          de.setPlacement(pv);
          continue;
        }
        placeVolume(sdet, assembly, coll);
      }

      xml_dim_t x_tr  = x_det.child(_U(transformation),false);
      xml_dim_t x_pos = x_det.child(_U(position),false);
      xml_dim_t x_rot = x_det.child(_U(rotation),false);
      Volume mother   = description.pickMotherVolume(sdet);
      if ( x_tr )  {
        Transform3D tr = xml::createTransformation(x_tr);
        pv = mother.placeVolume(assembly, tr);
      }
      else if( x_rot && x_pos ){
        Position    pos(x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0));
        RotationZYX rot(x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0));
        pv =  mother.placeVolume(assembly, Transform3D(rot, pos));
      } else if( x_rot ){
        RotationZYX rot(x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0));
        pv =  mother.placeVolume(assembly, rot);
      } else if( x_pos ){
        Position    pos(x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0));
        pv =  mother.placeVolume(assembly, pos);
      } else {
        pv = mother.placeVolume(assembly);
      }
      sdet.setPlacement(pv);
      return sdet;
    }
  };
}

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector sens)  {
  Builder  b(description);
  return b.create(e, sens);
}
DECLARE_DETELEMENT(DD4hep_VolumeAssembly,create_element)
