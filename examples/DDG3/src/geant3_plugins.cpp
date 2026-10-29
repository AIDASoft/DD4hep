//==========================================================================
//  ALEPH software suite
//--------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
//
//  For the licensing terms see OnlineSys/LICENSE.
//
//--------------------------------------------------------------------------
//
//  Author     : Markus Frank
//==========================================================================

/// Framework include files
#include <dd4hep/geant3_geometry.h>

/// DD4hep include files
#include <XML/DetectorStructure.h>
#include <XML/Conversions.h>

/// C/C++ include files
#include <iomanip>
#include <memory>

//==========================================================================
namespace  {
  dd4hep::geant3_geometry& geometry( dd4hep::Detector& dsc )  {
    dd4hep::geant3_geometry* ext = dsc.extension<dd4hep::geant3_geometry>(false);
    if( nullptr == ext )  {
      auto* e = new dd4hep::geant3_geometry_imp(dsc);
      dsc.addExtension<dd4hep::geant3_geometry>(e);
      ext = e;
    }
    return *ext;
  }
}

//==========================================================================
static dd4hep::Ref_t
geant3_detector_create( dd4hep::Detector& description, xml_h e, dd4hep::Ref_t /* sens */ )  {
  using namespace dd4hep;
  geant3_geometry& geo = geometry( description );
  xml_det_t      x_det = e;
  std::string   volume = x_det.attr<std::string>(_U(volume));

  geo.handle_divisions();
  geo.processing_step = geant3_geometry::PROCESSING_MATERIALS;
  geo.resolve_materials = 0;
  geo.handle_materials();
  geo.resolve_materials = 1;
  geo.handle_materials();
  geo.resolve_materials = 2;
  geo.handle_materials();
  geo.handle_media();
  geo.processing_step = geant3_geometry::PROCESSING_MATRICES;
  geo.handle_rotations();
  geo.processing_step = geant3_geometry::PROCESSING_VOLUMES;
  geo.handle_volumes();
  geo.processing_step = geant3_geometry::PROCESSING_DIVISIONS;
  geo.handle_divisions();
  geo.processing_step = geant3_geometry::PROCESSING_PLACEMENTS;
  geo.handle_placements();
  geo.processing_step = geant3_geometry::PROCESSING_FINISHED;
  
  auto im = geo.volumes.find( volume );
  if( im != geo.volumes.end() )  {
    Volume     vol = im->second;
    DetElement det( x_det.nameStr(), x_det.id() );
    Volume     mother = description.pickMotherVolume( det );

    //geo.description.manager().SetTopVolume( vol.ptr() );
    //geo.description.init();
    //geo.description.endDocument( "close" );
    PlacedVolume phv = mother.placeVolume( vol, 1 );
    vol.setVisAttributes( description, x_det.visStr() );
    vol.setLimitSet( description, x_det.limitsStr() );
    vol.setRegion( description, x_det.regionStr() );
    det.setPlacement( phv );
    return det;
  }
  except( "geant3_geometry",
          "Invalid world volume for this detector: %s",
          volume.c_str() );
  return {};
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(DD4hep_Geant3Detector,geant3_detector_create)

//==========================================================================

namespace {
  struct sensitives;
  struct properties;
  struct material;
  struct detector;
  struct debug;
}
namespace dd4hep {
  template <> void Converter<sensitives>::operator()( xml_h e) const;
  template <> void Converter<properties>::operator()( xml_h e) const;
  template <> void Converter<material>::operator()( xml_h e) const;
  template <> void Converter<detector>::operator()( xml_h e) const;
  template <> void Converter<debug>::operator()( xml_h e) const;
}

/// Converter for material aliases
template <> void dd4hep::Converter<material>::operator()( xml_h element ) const {
  xml_comp_t c = element;
  auto& geo = this->_object<geant3_geometry>();
  geo.material_aliases.emplace( c.nameStr(), c.valueStr() );
  printout( geo.debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
            "+++ add_material_alias: %s --> %s",
            c.nameStr().c_str(), c.valueStr().c_str() );
}

/// Converter to declare sensitive elements
template <> void dd4hep::Converter<sensitives>::operator()( xml_h element ) const  {
  xml_comp_t  c = element;
  std::string nam = c.nameStr();
  std::string det = c.attr<std::string>(_U(sensitive_detector));
  auto& geo = this->_object<geant3_geometry>();
  geo.sensitive_volumes.emplace( nam, det );
  printout( geo.debug_sensitive ? ALWAYS : DEBUG, "geant3_geometry",
            "+++ add_sensitive_volume:  volume: %s --> sd: %s", nam.c_str(), det.c_str() );
}

/// Converter for DetElement pathes/volumes
template <> void dd4hep::Converter<detector>::operator()( xml_h element ) const {
  xml_comp_t c = element;
  auto& geo = this->_object<geant3_geometry>();
  auto par  = c.hasAttr(_U(parent))    ? c.attr<std::string>(_U(parent))    : std::string();
  auto sens = c.hasAttr(_U(sensitive)) ? c.attr<std::string>(_U(sensitive)) : std::string();
  auto ro   = c.hasAttr(_U(readout))   ? c.attr<std::string>(_U(readout))   : std::string();
  auto vid  = c.hasAttr(_U(physvolid)) ? c.attr<std::string>(_U(physvolid)) : std::string();
  auto nam  = c.nameStr();
  int  id   = c.id();

  if( c.hasAttr(_U(placement)) )  {
    auto place = c.attr<std::string>(_U(placement));
    geo.detelement_placements[place] = { nam, par, sens, place, vid, ro, id };
    printout( geo.debug_enhancements ? ALWAYS : DEBUG, "geant3_geometry",
              "+++ add_detelement_place:  detelement:%16s.%-8s id:%2d vid:%-8s --> place:%s",
              par.c_str(), nam.c_str(), id, vid.c_str(), place.c_str() );
  }
  else if( c.hasAttr(_U(volume)) )  {
    auto vol = c.attr<std::string>(_U(volume));
    geo.detelement_volumes[vol] = { nam, par, sens, vol, vid, ro, id };
    printout( geo.debug_enhancements ? ALWAYS : DEBUG, "geant3_geometry",
              "+++ add_detelement_volume: detelement:%16s.%-8s id:%2d vid:%-8s --> volume:%s",
              par.c_str(), nam.c_str(), id, vid.c_str(), vol.c_str() );
  }
  else  {
    except("Converter<detector>", "+++ Unknown DetElement selector.");
  }
}

/// Converter to setup debugging and printouts
template <> void dd4hep::Converter<debug>::operator()( xml_h element ) const {
  auto geo = this->_param<geant3_geometry>();
  for( xml_coll_t coll(element, _U(star)); coll; ++coll )  {
    xml_comp_t  c = coll;
    std::string tag = coll.tag();
    
    if( tag == "detector" )  {
      auto dets = c.attr<std::string>(_U(name));
      if( dets.length() > 0 )  {
        geo->debug_detector += char(::toupper(dets[0]));
      }
    }
    else if( tag == "materials" )  {
      geo->debug_materials    = c.attr<bool>(_U(value));
    }
    else if( tag == "medium" )  {
      geo->debug_media        = c.attr<bool>(_U(value));
    }
    else if( tag == "volume" )  {
      geo->debug_volumes      = c.attr<bool>(_U(value));
    }
    else if( tag == "placement" )  {
      geo->debug_placements   = c.attr<bool>(_U(value));
    }
    else if( tag == "division" )  {
      geo->debug_divisions    = c.attr<bool>(_U(value));
    }
    else if( tag == "structure" )  {
      geo->debug_structure    = c.attr<bool>(_U(value));
    }
    else if( tag == "sensitive" )  {
      geo->debug_sensitive   = c.attr<bool>(_U(value));
    }
    else if( tag == "enhancements" )  {
      geo->debug_enhancements = c.attr<bool>(_U(value));
    }
    else  {
      except("Converter<detector>", "+++ Unknown debug information selector: "+tag);
    }
  }
}

/// Properties converter
template <> void dd4hep::Converter<properties>::operator()( xml_h element ) const {
  xml_elt_t compact(element);
  auto geo = this->_param<geant3_geometry>();
  geo->debug_detector = "";
  xml_coll_t( compact, _U(debug) ).for_each( Converter<debug>(description, geo) );
  xml_coll_t( compact, _U(materials) ).for_each( _U(material), Converter<material>(description, geo) );
  xml_coll_t( compact, _U(detectors) ).for_each( _U(detector), Converter<detector>(description, geo) );
  xml_coll_t( compact, _U(sensitives) ).for_each( _U(volume), Converter<sensitives>(description, geo) );
}

static long geant3_geo_enhancements(dd4hep::Detector& description, const xml_h& element) {
  auto& geo = geometry(description);
  (dd4hep::Converter <properties> ( description, &geo ))(element);
  return 1;
}
DECLARE_XML_DOC_READER(geant3_geometry_enhancements, geant3_geo_enhancements)

//==========================================================================
static long geant3_proc_enhancements(dd4hep::Detector& description, int /* argc */, char** /* argv */) {
  using namespace dd4hep;
  struct Actor {
  public:
    Detector&        description;
    geant3_geometry& geometry;
  public:
    Actor(Detector& desc, geant3_geometry& geo) : description(desc), geometry(geo)  { }
    void dump(std::string prefix, PlacedVolume node)  const {
      std::string path = std::move(prefix) + "/" + node.name();
      this->geometry.enhance(path, node);
      for( Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau )
        dump(path, node->GetDaughter(idau));
    }
  };

  Actor(description, geometry(description)).dump("", description.world().placement());
  return 1;
}
DECLARE_APPLY(geant3_process_enhancements,geant3_proc_enhancements)
//==========================================================================
