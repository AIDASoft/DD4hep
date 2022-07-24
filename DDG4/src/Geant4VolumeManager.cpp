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
#include <DDG4/Geant4VolumeManager.h>
#include <DDG4/Geant4TouchableHandler.h>
#include <DDG4/Geant4Mapping.h>

// Geant4 include files
#include <G4VTouchable.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>

// C/C++ include files
#include <sstream>

using namespace dd4hep::sim::Geant4GeometryMaps;
using namespace dd4hep::detail::tools;
using namespace dd4hep::detail;
using namespace dd4hep::sim;
using namespace dd4hep;
using namespace std;

#include <DDG4/Geant4AssemblyVolume.h>
typedef pair<VolumeID,vector<pair<const BitFieldElement*, VolumeID> > > VolIDDescriptor;
namespace {

  /// Helper class to populate the Geant4 volume manager
  struct Populator {
    typedef vector<const TGeoNode*> Chain;
    typedef map<VolumeID,Geant4GeometryInfo::Geant4PlacementPath> Registries;
    /// Reference to the Detector instance
    const Detector& m_detDesc;
    /// Set of already added entries
    Registries m_entries;
    /// Reference to Geant4 translation information
    Geant4GeometryInfo& m_geo;

    /// Default constructor
    Populator(const Detector& description, Geant4GeometryInfo& g)
      : m_detDesc(description), m_geo(g) {
    }

    /// Populate the Volume manager
    void populate(DetElement e) {
      const DetElement::Children& c = e.children();
      for (const auto& i : c)  {
        DetElement de = i.second;
        PlacedVolume pv = de.placement();
        if (pv.isValid()) {
          Chain chain;
          SensitiveDetector sd;
          PlacedVolume::VolIDs ids;
          m_entries.clear();
          chain.emplace_back(m_detDesc.world().placement().ptr());
          scanPhysicalVolume(pv.ptr(), ids, sd, chain);
          continue;
        }
        printout(WARNING, "Geant4VolumeManager", "++ Detector element %s of type %s has no placement.", de.name(), de.type().c_str());
      }
      /// Needed to compute the cellID of parameterized volumes
      for( const auto& pv : m_geo.g4Placements )   {
	if ( pv.second->IsParameterised() )
	  m_geo.g4Parameterised[pv.second] = pv.first;
	if ( pv.second->IsReplicated() )
	  m_geo.g4Replicated[pv.second] = pv.first;
      }
    }

    /// Scan a single physical volume and look for sensitive elements below
    void scanPhysicalVolume(const TGeoNode* node, PlacedVolume::VolIDs ids, SensitiveDetector& sd, Chain& chain) {
      PlacedVolume pv = node;
      Volume vol = pv.volume();
      PlacedVolume::VolIDs pv_ids = pv.volIDs();

      chain.emplace_back(node);
      ids.PlacedVolume::VolIDs::Base::insert(ids.end(), pv_ids.begin(), pv_ids.end());
      if (vol.isSensitive()) {
        sd = vol.sensitiveDetector();
        if (sd.readout().isValid()) {
          add_entry(sd, node, ids, chain);
        }
        else {
          printout(WARNING, "Geant4VolumeManager",
                   "populate: Strange constellation volume %s is sensitive, but has no readout! sd:%p", pv.volume().name(),
                   sd.ptr());
        }
      }
      for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
        TGeoNode* daughter = node->GetDaughter(idau);
        PlacedVolume placement(daughter);
        if ( placement.data() ) {
          scanPhysicalVolume(daughter, ids, sd, chain);
        }
      }
      chain.pop_back();
    }

    void add_entry(SensitiveDetector sd, const TGeoNode* n, const PlacedVolume::VolIDs& ids, const Chain& nodes) {
      Chain control;
      const TGeoNode* node;
      Volume vol;
      Geant4GeometryInfo::Geant4PlacementPath path;
      Readout ro = sd.readout();
      IDDescriptor iddesc = ro.idSpec();
      VolumeID code = iddesc.encode(ids);
      Registries::const_iterator i = m_entries.find(code);
      PrintLevel print_level  = m_geo.printLevel;
      PrintLevel print_action = print_level;
      PrintLevel print_chain  = print_level;
      PrintLevel print_res    = print_level;

      printout(print_action,"Geant4VolumeManager","+++ Add path:%s vid:%016X",
               detail::tools::placementPath(nodes,false).c_str(),code);

      if (i == m_entries.end()) {
        path.reserve(nodes.size());
        for (Chain::const_reverse_iterator k = nodes.rbegin(), kend=nodes.rend(); k != kend; ++k) {
          node = *(k);
          PlacementMap::const_iterator g4pit = m_geo.g4Placements.find(node);
          if (g4pit != m_geo.g4Placements.end()) {
	    G4VPhysicalVolume* phys = (*g4pit).second;
	    if ( phys->IsParameterised() )   {
	      PlacedVolume pv(n);
	      PlacedVolumeExtension* ext = pv.data();
	      if ( nullptr == ext->params->field )   {
		ext->params->field = iddesc.field(ext->volIDs.at(0).first);
	      }
	    }
            path.emplace_back(phys);
            printout(print_chain, "Geant4VolumeManager", "+++     Chain: Node OK: %s [%s]",
                     node->GetName(), phys->GetName().c_str());
            continue;
          }
          control.insert(control.begin(),node);
          vol = Volume(node->GetVolume());
          VolumeImprintMap::const_iterator iVolImp = m_geo.g4VolumeImprints.find(vol);
          if ( iVolImp != m_geo.g4VolumeImprints.end() )   {
            const Imprints& imprints = (*iVolImp).second;
            for(const auto& imp : imprints )   {
              const VolumeChain& c = imp.first;
              if ( c.size() <= control.size() && control == c )   {
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
        if ( control.empty() )   {
          printout(print_res, "Geant4VolumeManager", "+++     Volume  IDs:%s",
                   detail::tools::toString(ro.idSpec(),ids,code).c_str());
          path.erase(path.begin()+path.size()-1);
          printout(print_res, "Geant4VolumeManager", "+++     Map %016X to Geant4 Path:%s",
                   (void*)code, Geant4GeometryInfo::placementPath(path).c_str());
          if ( m_geo.g4Paths.find(path) == m_geo.g4Paths.end() ) {
	    Geant4GeometryInfo::PlacementFlags opt;
	    for(const auto* phys : path)   {
	      opt.flags.path_has_parametrised = phys->IsParameterised() ? 1 : 0;
	      opt.flags.path_has_replicated   = phys->IsReplicated()    ? 1 : 0;
	    }
	    opt.flags.parametrised = path.front()->IsParameterised() ? 1 : 0;
	    opt.flags.replicated   = path.front()->IsReplicated()    ? 1 : 0;
            m_geo.g4Paths[path] = { code, opt.value };
            m_entries.emplace(code,path);
            return;
          }
	  /// This is a normal case for parametrized volumes and no error
	  if ( !path.empty() && (path.front()->IsParameterised() || path.front()->IsReplicated()) )   {
	    return;
	  }
          printout(ERROR, "Geant4VolumeManager", "populate: Severe error: Duplicated Geant4 path!!!! %s %s",
                   " [THIS SHOULD NEVER HAPPEN]",Geant4GeometryInfo::placementPath(path).c_str());
          goto Err;
        }
        printout(INFO, "Geant4VolumeManager", "Control block has still %d entries:%s",
                 int(control.size()),detail::tools::placementPath(control,true).c_str());
        goto Err;
      }
      else  {
	/// This is a normal case for parametrized volumes and no error
	if ( !path.empty() && (path.front()->IsParameterised() || path.front()->IsReplicated()) )   {
	  return;
	}
      }
      printout(ERROR, "Geant4VolumeManager", "populate: Severe error: Duplicated Volume entry: 0x%X"
               " [THIS SHOULD NEVER HAPPEN]", code);

    Err:
      if ( i != m_entries.end() )
        printout(ERROR,"Geant4VolumeManager"," Known G4 path: %s",Geant4GeometryInfo::placementPath((*i).second).c_str());
      if ( !path.empty() )
        printout(ERROR,"Geant4VolumeManager"," New   G4 path: %s",Geant4GeometryInfo::placementPath(path).c_str());
      if ( !nodes.empty() )
        printout(ERROR,"Geant4VolumeManager","     TGeo path: %s",detail::tools::placementPath(nodes,false).c_str());
      printout(ERROR,"Geant4VolumeManager",  " Offend.VolIDs: %s",detail::tools::toString(ro.idSpec(),ids,code).c_str());
      throw runtime_error("Failed to populate Geant4 volume manager!");
    }
  };
}

/// Initializing constructor. The tree will automatically be built if possible
Geant4VolumeManager::Geant4VolumeManager(const Detector& description, Geant4GeometryInfo* info)
  : Handle<Geant4GeometryInfo>(info)   {
  if (info && info->valid && info->g4Paths.empty()) {
    Populator p(description, *info);
    p.populate(description.world());
    return;
  }
  throw runtime_error(format("Geant4VolumeManager", "Attempt populate from invalid Geant4 geometry info [Invalid-Info]"));
}

/// Helper: Generate placement path from touchable object
vector<const G4VPhysicalVolume*>
Geant4VolumeManager::placementPath(const G4VTouchable* touchable, bool exception) const {
  Geant4TouchableHandler handler(touchable);
  return handler.placementPath(exception);
}

/// Check the validity of the information before accessing it.
bool Geant4VolumeManager::checkValidity() const {
  if (!isValid()) {
    throw runtime_error(format("Geant4VolumeManager", "Attempt to use invalid Geant4 volume manager [Invalid-Handle]"));
  }
  else if (!ptr()->valid) {
    throw runtime_error(format("Geant4VolumeManager", "Attempt to use invalid Geant4 geometry info [Invalid-Info]"));
  }
  return true;
}

#if 0
/// Access CELLID by placement path
VolumeID Geant4VolumeManager::volumeID(const vector<const G4VPhysicalVolume*>& path) const {
  if (!path.empty() && checkValidity()) {
    const auto& mapping = ptr()->g4Paths;
    auto i = mapping.find(path);
    if ( i != mapping.end() )   {
      return (*i).second.first;
    }
    if (!path[0])
      return InvalidPath;
    else if (!path[0]->GetLogicalVolume()->GetSensitiveDetector())
      return Insensitive;
  }
  printout(INFO, "Geant4VolumeManager","+++   Bad volume Geant4 Path: %s",
           Geant4GeometryInfo::placementPath(path).c_str());
  return NonExisting;
}
#endif

/// Access CELLID by Geant4 touchable object
VolumeID Geant4VolumeManager::volumeID(const G4VTouchable* touchable) const {
  Geant4TouchableHandler handler(touchable);
  vector<const G4VPhysicalVolume*> path = handler.placementPath();
  if (!path.empty() && checkValidity()) {
    const auto& mapping = ptr()->g4Paths;
    auto i = mapping.find(path);
    if ( i != mapping.end() )   {
      const auto& e = (*i).second;
      /// No parametrization or replication.
      if ( e.flags == 0 )  {
	return e.volumeID;
      }
      VolumeID volid = e.volumeID;
      const auto& paramterised = ptr()->g4Parameterised;
      const auto& replicated   = ptr()->g4Replicated;
      /// This is incredibly slow .... but what can I do ? Need a better idea.
      for ( std::size_t j=0; j < path.size(); ++j )   {
	const auto* phys = path[j];
	if ( phys->IsParameterised() )   {
	  int copy_no = touchable->GetCopyNumber(j);
	  const auto it = paramterised.find(phys);
	  if ( it != paramterised.end() )   {
	    //printout(INFO,"Geant4VolumeManager",
	    //         "Copy number:   %ld  <--> %ld", copy_no, long(phys->GetCopyNo()));
	    const auto* field = (*it).second.data()->params->field;
	    volid |= IDDescriptor::encode(field, copy_no);
	    continue;
	  }
	  except("Geant4VolumeManager","Error  Geant4VolumeManager::volumeID(const G4VTouchable* touchable)");
	}
	else if ( phys->IsReplicated() )    {
	  int copy_no = touchable->GetCopyNumber(j);
	  const auto it = replicated.find(phys);
	  if ( it != replicated.end() )   {
	    const auto* field = (*it).second.data()->params->field;
	    volid |= IDDescriptor::encode(field, copy_no);
	    continue;
	  }
	  except("Geant4VolumeManager","Error  Geant4VolumeManager::volumeID(const G4VTouchable* touchable)");
	}
      }
      return volid;
    }
    if (!path[0])
      return InvalidPath;
    else if (!path[0]->GetLogicalVolume()->GetSensitiveDetector())
      return Insensitive;
  }
  printout(INFO, "Geant4VolumeManager","+++   Bad volume Geant4 Path: %s",
           Geant4GeometryInfo::placementPath(path).c_str());
  return NonExisting;
}

/// Accessfully decoded volume fields  by placement path
void Geant4VolumeManager::volumeDescriptor(const vector<const G4VPhysicalVolume*>& path,
                                           VolIDDescriptor& vol_desc) const
{
  vol_desc.second.clear();
  vol_desc.first = NonExisting;
  if ( !path.empty() && checkValidity() )  {
    const auto& mapping = ptr()->g4Paths;
    auto i = mapping.find(path);
    if (i != mapping.end()) {
      VolumeID vid = (*i).second.volumeID;
      G4LogicalVolume* lvol = path[0]->GetLogicalVolume();
      if ( lvol->GetSensitiveDetector() ) {
        const G4VPhysicalVolume* node = path[0];
        const PlacementMap& pm = ptr()->g4Placements;
        for (PlacementMap::const_iterator ipm = pm.begin(); ipm != pm.end(); ++ipm) {
          if ( (*ipm).second == node )  {
            PlacedVolume pv = (*ipm).first;
            SensitiveDetector sd = pv.volume().sensitiveDetector();
            IDDescriptor dsc = sd.readout().idSpec();
            vol_desc.first = vid;
            dsc.decodeFields(vid, vol_desc.second);
            return;
          }
        }
      }
      vol_desc.first = Insensitive;
      return;
    }
    if ( !path[0] )
      vol_desc.first = InvalidPath;
    else if ( !path[0]->GetLogicalVolume()->GetSensitiveDetector() )
      vol_desc.first = Insensitive;
    else
      vol_desc.first = NonExisting;
  }
}

/// Access fully decoded volume fields by Geant4 touchable object
void Geant4VolumeManager::volumeDescriptor(const G4VTouchable* touchable,
                                           VolIDDescriptor&    vol_desc) const {
  volumeDescriptor(placementPath(touchable), vol_desc);
}

