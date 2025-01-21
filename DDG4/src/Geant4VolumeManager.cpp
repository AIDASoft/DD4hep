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

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/Volumes.h>
#include <DD4hep/DetElement.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/VolumeManager.h>
#include <DD4hep/detail/VolumeManagerInterna.h>
#include <DDG4/Geant4VolumeManager.h>
#include <DDG4/Geant4TouchableHandler.h>
#include <DDG4/Geant4Mapping.h>

// Geant4 include files
#include <G4VTouchable.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>

// C/C++ include files
#include <sstream>

using namespace dd4hep::sim;
using namespace dd4hep;

#include <DDG4/Geant4AssemblyVolume.h>
using VolIDDescriptor = std::pair<VolumeID,std::vector<std::pair<const BitFieldElement*, VolumeID> > >;

namespace  {

  /// Helper class to populate the Geant4 volume manager
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  struct Populator {

    typedef std::vector<const TGeoNode*> Chain;
    // typedef std::map<VolumeID,Geant4TouchableHandler::Geant4PlacementPath> Registries;
    typedef std::set<VolumeID> Registries;

    /// Reference to the Detector instance
    const Detector&     m_detDesc;
    /// Set of already added entries
    Registries          m_entries;
    /// Reference to Geant4 translation information
    Geant4GeometryInfo& m_geo;

    /// Default constructor
    Populator(const Detector& description, Geant4GeometryInfo& g)
      : m_detDesc(description), m_geo(g)
    {
    }

    typedef std::pair<VolumeID, VolumeID> Encoding;
    /// Compute the encoding for a set of VolIDs within a readout descriptor
    static Encoding encoding(const IDDescriptor iddesc, const PlacedVolume::VolIDs& ids)  {
      VolumeID volume_id = 0, mask = 0;
      for( const auto& id : ids )  {
        const BitFieldElement* f = iddesc.field(id.first);
        VolumeID msk = f->mask();
        int      off = f->offset();
        VolumeID val = id.second;    // Necessary to extend volume IDs > 32 bit
        volume_id |= ((f->value(val << off) << off)&msk);
        mask      |= msk;
      }
      return std::make_pair(volume_id, mask);
    }

    /// Populate the Volume manager
    void populate(DetElement e)  {
      const DetElement::Children& c = e.children();
      for( const auto& i : c )  {
        DetElement de = i.second;
        PlacedVolume pv = de.placement();
        if( pv.isValid() )  {
          Chain chain;
          SensitiveDetector sd;
          PlacedVolume::VolIDs ids;
          m_entries.clear();
          chain.emplace_back(m_detDesc.world().placement().ptr());
          scanPhysicalVolume(pv.ptr(), std::move(ids), sd, chain);
          continue;
        }
        printout(WARNING, "Geant4VolumeManager",
                 "++ Detector element %s of type %s has no placement.",
                 de.name(), de.type().c_str());
      }
      /// Needed to compute the cellID of parameterized volumes
      for( const auto& pv : m_geo.g4Placements )  {
        if( pv.second->IsParameterised() )
          m_geo.g4Parameterised[pv.second] = pv.first;
        if( pv.second->IsReplicated() )
          m_geo.g4Replicated[pv.second] = pv.first;
      }
      m_entries.clear();
    }

    /// Scan a single physical volume and look for sensitive elements below
    void scanPhysicalVolume(const TGeoNode* node, PlacedVolume::VolIDs ids, SensitiveDetector& sd, Chain& chain) {
      PlacedVolume pv = node;
      Volume vol = pv.volume();
      PlacedVolume::VolIDs pv_ids = pv.volIDs();

      chain.emplace_back(node);
      ids.PlacedVolume::VolIDs::Base::insert(ids.end(), pv_ids.begin(), pv_ids.end());
      if( vol.isSensitive() )  {
        sd = vol.sensitiveDetector();
        if( sd.readout().isValid() )  {
          add_entry(sd, node, ids, chain);
        }
        else  {
          printout(WARNING, "Geant4VolumeManager",
                   "populate: Strange constellation volume %s is sensitive, but has no readout! sd:%p", pv.volume().name(),
                   sd.ptr());
        }
      }
      for( Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau )  {
        TGeoNode* daughter = node->GetDaughter(idau);
        PlacedVolume placement(daughter);
        if( placement.data() ) {
          scanPhysicalVolume(daughter, ids, sd, chain);
        }
      }
      chain.pop_back();
    }

    void add_entry(SensitiveDetector sd, const TGeoNode* n, const PlacedVolume::VolIDs& ids, const Chain& nodes) {
      Chain           control;
      Volume          vol;
      Readout         rdout   = sd.readout();
      IDDescriptor    iddesc  = rdout.idSpec();
      VolumeID        code    = iddesc.encode(ids);
      PrintLevel print_level  = m_geo.printLevel;
      PrintLevel print_action = print_level;
      PrintLevel print_chain  = print_level;
      PrintLevel print_res    = print_level;
      Geant4TouchableHandler::Geant4PlacementPath path;
      Registries::const_iterator i = m_entries.find(code);

      printout(print_action,"Geant4VolumeManager","+++ Add path:%s vid:%016X",
               detail::tools::placementPath(nodes, false).c_str(), code);

      if( i == m_entries.end() ) {
        path.reserve(nodes.size());
        for( Chain::const_reverse_iterator k = nodes.rbegin(), kend=nodes.rend(); k != kend; ++k ) {
          const TGeoNode* node = *(k);
          auto g4pit = m_geo.g4Placements.find(node);
          if( g4pit != m_geo.g4Placements.end() )  {
            G4VPhysicalVolume* phys = g4pit->second;
            if( phys->IsParameterised() )  {
              PlacedVolume pv(n);
              PlacedVolumeExtension* ext = pv.data();
              if( nullptr == ext->params->field )  {
                ext->params->field = iddesc.field(ext->volIDs.at(0).first);
              }
            }
            path.emplace_back(phys);
            printout(print_chain, "Geant4VolumeManager",
                     "+++     Chain: Node OK: %s [%s]", node->GetName(), phys->GetName().c_str());
            continue;
          }
          control.insert(control.begin(),node);
          vol = Volume(node->GetVolume());
          auto iVolImp = m_geo.g4VolumeImprints.find(vol);
          if ( iVolImp != m_geo.g4VolumeImprints.end() )  {
            for(const auto& imp : iVolImp->second )  {
              const auto& c = imp.first;
              if ( c.size() <= control.size() && control == c )  {
                path.emplace_back(imp.second);
                printout(print_chain, "Geant4VolumeManager", "+++     Chain: Node OK: %s %s -> %s",
                         node->GetName(), detail::tools::placementPath(c,false).c_str(),
                         imp.second->GetName().c_str());
                control.clear();
                break;
              }
            }
          }
        }
        if ( control.empty() )  {
          printout(print_res, "Geant4VolumeManager", "+++     Volume  IDs:%s",
                   detail::tools::toString(rdout.idSpec(),ids,code).c_str());
          path.erase(path.begin()+path.size()-1);
          printout(print_res, "Geant4VolumeManager", "+++     Map %016X to Geant4 Path:%s",
                   (void*)code, Geant4TouchableHandler::placementPath(path).c_str());
          auto hash = detail::hash64(&path[0], path.size()*sizeof(path[0]));
          if ( m_geo.g4Paths.find(hash) == m_geo.g4Paths.end() ) {
            Geant4GeometryInfo::PlacementFlags opt;
            opt.flags.parametrised = path.front()->IsParameterised() ? 1 : 0;
            opt.flags.replicated   = path.front()->IsReplicated()    ? 1 : 0;
            m_geo.g4Paths[hash]    = { code, opt.value };
            m_entries.emplace(code);
            return;
          }
          /// This is a normal case for parametrized volumes and no error
          if ( !path.empty() && (path.front()->IsParameterised() || path.front()->IsReplicated()) )  {
            return;
          }
          printout(ERROR, "Geant4VolumeManager", "populate: Severe error: Duplicated Geant4 path!!!! %s %s",
                   " [THIS SHOULD NEVER HAPPEN]", Geant4TouchableHandler::placementPath(path).c_str());
          goto Err;
        }
        printout(INFO, "Geant4VolumeManager", "Control block has still %d entries:%s",
                 int(control.size()), detail::tools::placementPath(control,true).c_str());
        goto Err;
      }
      else  {
        /// This is a normal case for parametrized volumes and no error
        if ( !path.empty() && (path.front()->IsParameterised() || path.front()->IsReplicated()) )  {
          return;
        }
      }
      printout(ERROR, "Geant4VolumeManager", "populate: Severe error: Duplicated Volume entry: 0x%X"
               " [THIS SHOULD NEVER HAPPEN]", code);

    Err:
      if ( i != m_entries.end() )
        printout(ERROR,"Geant4VolumeManager"," Known G4 path: %s", Geant4TouchableHandler::placementPath(path).c_str());
      if ( !path.empty() )
        printout(ERROR,"Geant4VolumeManager"," New   G4 path: %s", Geant4TouchableHandler::placementPath(path).c_str());
      if ( !nodes.empty() )
        printout(ERROR,"Geant4VolumeManager","     TGeo path: %s", detail::tools::placementPath(nodes,false).c_str());
      printout(ERROR,"Geant4VolumeManager",  " Offend.VolIDs: %s", detail::tools::toString(rdout.idSpec(),ids,code).c_str());
      throw std::runtime_error("Failed to populate Geant4 volume manager!");
    }
  };
}

/// Initializing constructor. The tree will automatically be built if possible
Geant4VolumeManager::Geant4VolumeManager(const Detector& description, Geant4GeometryInfo* info)
  : Handle<Geant4GeometryInfo>(info)  {
  if( info && info->valid )  {
    if( !info->has_volmgr )  {
      Populator p(description, *info);
      p.populate(description.world());
      info->has_volmgr = true;
    }
    return;
  }
  except("Geant4VolumeManager", "Attempt populate from invalid Geant4 geometry info [Invalid-Info]");
}

/// Helper: Generate placement path from touchable object
std::vector<const G4VPhysicalVolume*>
Geant4VolumeManager::placementPath(const G4VTouchable* touchable, bool exception) const  {
  Geant4TouchableHandler handler(touchable);
  return handler.placementPath(exception);
}

/// Check the validity of the information before accessing it.
bool Geant4VolumeManager::checkValidity() const  {
  if( !isValid() )  {
    except("Geant4VolumeManager", "Attempt to use invalid Geant4 volume manager [Invalid-Handle]");
  }
  else if( !ptr()->valid )  {
    except("Geant4VolumeManager", "Attempt to use invalid Geant4 geometry info [Invalid-Info]");
  }
  return true;
}

/// Access CELLID by Geant4 touchable object
VolumeID Geant4VolumeManager::volumeID(const G4VTouchable* touchable) const  {
  Geant4TouchableHandler handler(touchable);
  std::vector<const G4VPhysicalVolume*> path = handler.placementPath();

  if( checkValidity() && !path.empty() )  {
    auto hash = detail::hash64(&path[0], sizeof(path[0])*path.size());
    auto i = ptr()->g4Paths.find(hash);
    if( i != ptr()->g4Paths.end() )  {
      const auto& e = (*i).second;
      VolumeID volid = e.volumeID;
      /// No parametrization or replication.
      if( e.flags == 0 )  {
        return volid;
      }
      const auto& paramterised = ptr()->g4Parameterised;
      const auto& replicated   = ptr()->g4Replicated;
      /// This is incredibly slow .... but what can I do ? Need a better idea.
      for( std::size_t j=0; j < path.size(); ++j )  {
        const auto* phys = path[j];
        if( phys->IsParameterised() )  {
          int copy_no = touchable->GetCopyNumber(j);
          const auto it = paramterised.find(phys);
          if( it != paramterised.end() )  {
            //printout(INFO,"Geant4VolumeManager",
            //         "Copy number:   %ld  <--> %ld", copy_no, long(phys->GetCopyNo()));
            const auto* field = (*it).second.data()->params->field;
            volid |= IDDescriptor::encode(field, copy_no);
            continue;
          }
          except("Geant4VolumeManager",
                 "Error  Geant4VolumeManager::volumeID(const G4VTouchable* touchable)");
        }
        else if( phys->IsReplicated() )   {
          int copy_no = touchable->GetCopyNumber(j);
          const auto it = replicated.find(phys);
          if( it != replicated.end() )  {
            const auto* field = (*it).second.data()->params->field;
            volid |= IDDescriptor::encode(field, copy_no);
            continue;
          }
          except("Geant4VolumeManager",
                 "Error  Geant4VolumeManager::volumeID(const G4VTouchable* touchable)");
        }
      }
      return volid;
    }
    if( !path[0] )
      return InvalidPath;
    else if( !path[0]->GetLogicalVolume()->GetSensitiveDetector() )
      return Insensitive;
  }
  printout(INFO, "Geant4VolumeManager", "+++   Bad volume Geant4 Path: %s",
           Geant4TouchableHandler::placementPath(path).c_str());
  return NonExisting;
}

/// Accessfully decoded volume fields  by placement path
void Geant4VolumeManager::volumeDescriptor(const std::vector<const G4VPhysicalVolume*>& path,
                                           VolIDDescriptor& vol_desc) const
{
  vol_desc.second.clear();
  vol_desc.first = NonExisting;
  if( !path.empty() && checkValidity() )  {
    auto hash = detail::hash64(&path[0], sizeof(path[0])*path.size());
    auto i = ptr()->g4Paths.find(hash);
    if( i != ptr()->g4Paths.end() )  {
      VolumeID vid = (*i).second.volumeID;
      G4LogicalVolume* lvol = path[0]->GetLogicalVolume();
      if( lvol->GetSensitiveDetector() ) {
        const auto* node = path[0];
        const auto& pm = ptr()->g4Placements;
        for( const auto& ipm : pm )  {
          if ( ipm.second == node )  {
            PlacedVolume      pv  = ipm.first;
            SensitiveDetector sd  = pv.volume().sensitiveDetector();
            IDDescriptor      dsc = sd.readout().idSpec();
            vol_desc.first = vid;
            dsc.decodeFields(vid, vol_desc.second);
            return;
          }
        }
      }
      vol_desc.first = Insensitive;
      return;
    }
    if( !path[0] )
      vol_desc.first = InvalidPath;
    else if( !path[0]->GetLogicalVolume()->GetSensitiveDetector() )
      vol_desc.first = Insensitive;
    else
      vol_desc.first = NonExisting;
  }
}

/// Access fully decoded volume fields by Geant4 touchable object
void Geant4VolumeManager::volumeDescriptor(const G4VTouchable* touchable,
                                           VolIDDescriptor&    vol_desc)  const  {
  volumeDescriptor(placementPath(touchable), vol_desc);
}

