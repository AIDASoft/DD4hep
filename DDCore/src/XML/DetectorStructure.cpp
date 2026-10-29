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
#include <XML/DetectorStructure.h>

/// C/C++ include files
#include <iomanip>
#include <memory>
#include <regex>

namespace  {
  /// Reset cached pathes
  void _reset_cache(dd4hep::DetElement de)  {
    de->path = "";
    de->placementPath = "";
    for( const auto& c : de->children )
      _reset_cache( c.second );
  }
}

/// dd4hep namespace declaration
namespace dd4hep  {
  
  /// tool sub-namespace declaration
  namespace tools  {

    static std::regex_constants::syntax_option_type regex_flags()  {
      return std::regex_constants::icase | std::regex_constants::ECMAScript;
    }
    
    struct placement_scan  {
      const std::string& _place;
      /// Default constructor
      placement_scan(const std::string& pl) : _place(pl)  {
      }
      std::pair<std::string, PlacedVolume>
      scan( std::string p, PlacedVolume node )  const {
        std::string path = std::move(p) + "/" + node.name();
        if( path == this->_place ) return { path, node };
        for( Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau )  {
          auto ret = this->scan( path, node->GetDaughter(idau) );
          if( ret.second.isValid() ) return ret;
        }
        return { {}, {} };
      }
    };

    struct volume_scan  {
      std::regex  _regex;
      /// Default constructor
      volume_scan(const std::string& v) : _regex(v.c_str(), regex_flags())  {    }
      std::pair<std::string, PlacedVolume>
      scan( std::string p, PlacedVolume node )  const {
        std::string path = std::move(p) + "/" + node.name();
        std::string voln  = node.volume().name();
        std::smatch sm;
        bool stat = std::regex_match( voln, sm, this->_regex );
        if( stat )  {
          return { path, node };
        }
        for( Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau )  {
          auto ret = this->scan( path, node->GetDaughter(idau) );
          if( ret.second.isValid() ) return ret;
        }
        return { {}, {} };
      }
    };

    struct volume_collect  {
      std::regex  _regex;
      std::map<std::string, PlacedVolume> placements;

      /// Default constructor
      volume_collect(const std::string& v) : _regex(v.c_str(), regex_flags())  {    }
      void collect( std::string p, PlacedVolume node ) {
        std::string path  = std::move(p) + "/" + node.name();
        std::string voln  = node.volume().name();
        std::smatch sm;
        bool stat = std::regex_match( voln, sm, this->_regex );
        if( stat )  {
          this->placements.insert( { path, node } );
        }
        for( Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau )
          this->collect( path, { node->GetDaughter(idau) } );
      }
    };
  }
}

/// Default constructor
dd4hep::DetectorStructure::DetectorStructure( Detector& dsc, xml_h e, SensitiveDetector sd )
  : description(dsc), sensitive(sd), xd(e)  {
  if( xd.hasAttr(_U(debug)) )  {
    int value = xd.attr<int>(_U(debug));
    this->print = PrintLevel(value);
  }
}

/// Assign volume ID to placement
void dd4hep::DetectorStructure::_set_volid( PlacedVolume pv, const std::string& volid, int value )  {
  const auto& vids = pv.volIDs();
  for( const auto& vid : vids )  {
    if( vid.first == volid )  {
      if( value == vid.second )  {
        return;
      }
      printout(ERROR, this->name,
               "+++ Invalid VOLID: %s: %s %d <> %d",
               name.c_str(), volid.c_str(), vid.second, value);
    }
  }
  pv.addPhysVolID( volid, value );
}

/// Set properties for all volumes
void dd4hep::DetectorStructure::_set_attr(Volume volume, xml_det_t e)  const  {
  if( e.hasAttr(_U(vis)) )  {
    auto vis = e.attr<std::string>(_U(vis));
    volume.setVisAttributes( this->description, vis );
  }
  if( e.hasAttr(_U(limits)) )  {
    auto lim = e.attr<std::string>(_U(limits));
    volume.setLimitSet( this->description, lim );
  }
  if(  e.hasAttr(_U(region)) )  {
    auto reg = e.attr<std::string>(_U(region));
    volume.setRegion( this->description, reg );
  }
  if( this->sensitive.isValid() && xd.ptr() != e.ptr() && e.hasAttr(_U(sensitive)) )  {
    volume.setSensitiveDetector( this->sensitive );
  }
}

/// Construct subdetector from Geant3 geometry
dd4hep::DetElement dd4hep::DetectorStructure::construct()  {
  std::pair<std::string, PlacedVolume> ret;

  this->id   = this->xd.id();
  this->name = this->xd.nameStr();
  
  if( this->xd.hasAttr(_U(placement)) )  {
    auto place = this->xd.attr<std::string>(_U(placement));
    ret = tools::placement_scan(place).scan( {}, this->description.world().placement() );
    if( !ret.second.isValid() )  {
      except( this->name, "+++ [id:%d] %s: Failed to find top level placement: '%s'",
              this->id, this->name.c_str(), place.c_str() );
    }
  }
  else if( this->xd.hasAttr(_U(volume)) )  {
    auto vol = this->xd.attr<std::string>(_U(volume));
    ret = tools::volume_scan(vol).scan( {}, this->description.world().placement() );
    if( !ret.second.isValid() )  {
      except( this->name, "+++ [id:%d] %s: Failed to find top level volume: '%s'",
              this->id, this->name.c_str(), vol.c_str() );
    }
  }
  else  {
    except( this->name, "+++ [id:%d] %s Unknown DetElement selector.",
            this->id, this->name.c_str() );
  }

  if( xd.hasAttr(_U(sensitive)) )  {
    auto sens = xd.attr<std::string>(_U(sensitive));
    if( !this->sensitive.isValid() )  {
      this->sensitive = this->description.sensitiveDetector( this->name );
    }
    if( !this->sensitive.isValid() )  {
      except( this->name, "+++ [id:%d] %s No suitable sensitive detector available",
              this->id, this->name.c_str() );
    }
    this->sensitive.setType( sens );
  }

  DetElement detector( this->name, this->id );
  auto pv  = ret.second;
  auto vol = pv.volume();
  detector->volumeID = this->id;
  detector.setPlacement( pv );
  _set_attr( vol, xd );

  printout( this->print, this->name,
            "+++ [id:%d] %-40s Creating detector element path:%s",
            detector.id(), detector.path().c_str(), detector.placementPath().c_str());
  if( xd.hasAttr(_U(sensitive)) )  {
    std::string readout_name = "----";

    pv.addPhysVolID( "system", this->id );
    if( xd.hasAttr(_U(readout)) )  {
      readout_name = xd.readoutStr();
      Readout rdout = this->description.readout( readout_name );
      if( !rdout.isValid() )  {
        except(this->name, "+++ [id:%d] %s Invalid readout object: %s.",
               this->id, this->name.c_str(), readout_name.c_str() );
      }
      this->sensitive.setReadout( rdout );
    }
    printout( this->print, this->name,
              "+++ [id:%d] %-40s Assign readout '%s' to sensitive detector: '%s' [%s]",
              this->id, this->name.c_str(), readout_name.c_str(),
              this->sensitive.name(), this->sensitive.type().c_str() );
  }
  if( pv.num_daughters() > 0 )  {
    for( xml_coll_t coll(this->xd, _U(daughter)); coll; ++coll )  {
      this->add_children( detector, coll );
    }
  }
  // We printed the detelement pathes. Need to reset them before returning.
  _reset_cache( detector );
  return detector;
}

/// Add sub-branch of DetElements to subdetector
void dd4hep::DetectorStructure::add_children(DetElement parent, xml_det_t e)  {
  auto pv  = parent.placement();
  auto vol = e.attr<std::string>(_U(volume));
  tools::volume_collect helper(vol);

  helper.collect( {}, pv );
  if( helper.placements.empty() )  {
    return;
  }
  for( auto& place : helper.placements )  {
    PlacedVolume& dpv = place.second;
    int           vid = e.hasAttr(_U(id)) ? e.id() : parent.children().size();
    VisAttr       vis = dpv.volume().visAttributes();
    std::string   nam = e.nameStr() + _toString(vid, "_%d");
    DetElement    de( parent, nam, parent.id() );
    Volume        volume = dpv.volume();
        
    _set_attr( volume, e );
    if( !this->sensitive.isValid() )  {
      if( e.hasAttr(_U(sensitive)) )  {
        volume.setSensitiveDetector( this->sensitive );
      }
    }
    if( e.hasAttr(_U(physvolid)) )  {
      auto volid = e.attr<std::string>( _U(physvolid) );
      this->_set_volid( dpv, volid, vid );
    }
    de->volumeID = vid;
    de.setPlacement( dpv );

    printout( this->print, this->name,
              "+++ [id:%d] %-40s sub-element %-8s [id:%d] vis:%-10s %3d --> place %s",
              parent.id(), de.path().c_str(), de.name(), de.id(),
              vis.isValid() ? vis.name() : "---", dpv.copyNumber(), place.first.c_str() );
    if( dpv.num_daughters() > 0 )  {
      for( xml_coll_t coll(e, _U(daughter)); coll; ++coll )  {
        this->add_children( de, coll );
      }
    }
  }
}
