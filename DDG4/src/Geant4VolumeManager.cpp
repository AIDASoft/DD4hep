// $Id: Geant4VolumeManager.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"
#include "DD4hep/DetectorTools.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4Mapping.h"

// Geant4 include files
#include "G4VTouchable.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

// C/C++ include files
#include <sstream>

using namespace DD4hep::Simulation;
using namespace DD4hep::Simulation::Geant4GeometryMaps;
using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

#include "DDG4/Geant4AssemblyVolume.h"

namespace {

  /// Helper class to populate the Geant4 volume manager
  struct Populator {
    typedef vector<const TGeoNode*> Chain;
    typedef DD4hep::Geometry::LCDD LCDD;
    typedef DD4hep::Geometry::Readout Readout;
    typedef DD4hep::Geometry::DetElement DetElement;
    typedef map<VolumeID,Geant4PlacementPath> Registries;
    /// Reference to the LCDD instance
    LCDD& m_lcdd;
    /// Set of already added entries
    Registries m_entries;
    /// Reference to Geant4 translation information
    Geant4GeometryInfo& m_geo;

    /// Default constructor
    Populator(LCDD& lcdd, Geant4GeometryInfo& g)
      : m_lcdd(lcdd), m_geo(g) {
    }

    /// Populate the Volume manager
    void populate(DetElement e) {
      const DetElement::Children& c = e.children();
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
        DetElement de = (*i).second;
        PlacedVolume pv = de.placement();
        if (pv.isValid()) {
          Chain chain;
          SensitiveDetector sd;
          PlacedVolume::VolIDs ids;
          m_entries.clear();
          chain.push_back(m_lcdd.world().placement().ptr());
          scanPhysicalVolume(pv.ptr(), ids, sd, chain);
          continue;
        }
        printout(WARNING, "Geant4VolumeManager", "++ Detector element %s of type %s has no placement.", de.name(), de.type().c_str());
      }
    }

    /// Scan a single physical volume and look for sensitive elements below
    void scanPhysicalVolume(const TGeoNode* node, PlacedVolume::VolIDs ids, SensitiveDetector& sd, Chain& chain) {
      PlacedVolume pv = Ref_t(node);
      Volume vol = pv.volume();
      PlacedVolume::VolIDs pv_ids = pv.volIDs();

      chain.push_back(node);
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

    void add_entry(SensitiveDetector sd, const TGeoNode* /* n */, const PlacedVolume::VolIDs& ids, const Chain& nodes) {
      Chain control;
      const TGeoNode* node;
      Volume vol;
      Geant4PlacementPath path;
      Readout ro = sd.readout();
      IDDescriptor iddesc = ro.idSpec();
      VolumeID code = iddesc.encode(ids);
      Registries::const_iterator i = m_entries.find(code);
      PrintLevel print_action = VERBOSE;
      PrintLevel print_chain = VERBOSE;
      PrintLevel print_res = VERBOSE;

      printout(print_action,"Geant4VolumeManager","+++ Add path:%s vid:%016X",
               DetectorTools::placementPath(nodes,false).c_str(),code);

      if (i == m_entries.end()) {
        path.reserve(nodes.size());
        for (Chain::const_reverse_iterator k = nodes.rbegin(), kend=nodes.rend(); k != kend; ++k) {
          node = *(k);
          PlacementMap::const_iterator g4pit = m_geo.g4Placements.find(node);
          if (g4pit != m_geo.g4Placements.end()) {
            path.push_back((*g4pit).second);
            printout(print_chain, "Geant4VolumeManager", "+++     Chain: Node OK: %s [%s]",
                     node->GetName(), (*g4pit).second->GetName().c_str());
            continue;
          }
          control.insert(control.begin(),node);
          vol = Volume(node->GetVolume());
          VolumeImprintMap::const_iterator iVolImp = m_geo.g4VolumeImprints.find(vol);
          if ( iVolImp != m_geo.g4VolumeImprints.end() )   {
            const Imprints& imprints = (*iVolImp).second;
            //size_t len = kend-k;
            for(Imprints::const_iterator iImp=imprints.begin(); iImp != imprints.end(); ++iImp)  {
              const VolumeChain& c = (*iImp).first;
              if ( c.size() <= control.size() && control == c )   {
                path.push_back((*iImp).second);
                printout(print_chain, "Geant4VolumeManager", "+++     Chain: Node OK: %s %s -> %s",
                         node->GetName(), DetectorTools::placementPath(c,false).c_str(),
                         (*iImp).second->GetName().c_str());
                control.clear();
                break;
              }
            }
          }
        }
        if ( control.empty() )   {
          printout(print_res, "Geant4VolumeManager", "+++     Volume  IDs:%s",
                   DetectorTools::toString(ro.idSpec(),ids,code).c_str());
          path.erase(path.begin()+path.size()-1);
          printout(print_res, "Geant4VolumeManager", "+++     Map %016X to Geant4 Path:%s",
                   (void*)code, placementPath(path).c_str());
          if (m_geo.g4Paths.find(path) == m_geo.g4Paths.end()) {
            m_geo.g4Paths[path] = code;
            m_entries.insert(make_pair(code,path));
            return;
          }
          printout(ERROR, "Geant4VolumeManager", "populate: Severe error: Duplicated Geant4 path!!!! %s %s",
                   " [THIS SHOULD NEVER HAPPEN]",placementPath(path).c_str());
          goto Err;
        }
        printout(INFO, "Geant4VolumeManager", "Control block has still %d entries:%s",
                 int(control.size()),DetectorTools::placementPath(control,true).c_str());
        goto Err;
      }
      printout(ERROR, "Geant4VolumeManager", "populate: Severe error: Duplicated Volume entry: %X"
               " [THIS SHOULD NEVER HAPPEN]", code);

    Err:
      if ( i != m_entries.end() )
        printout(ERROR,"Geant4VolumeManager"," Known G4 path: %s",placementPath((*i).second).c_str());
      if ( !path.empty() )
        printout(ERROR,"Geant4VolumeManager"," New   G4 path: %s",placementPath(path).c_str());
      if ( !nodes.empty() )
        printout(ERROR,"Geant4VolumeManager","     TGeo path: %s",DetectorTools::placementPath(nodes,false).c_str());
      printout(ERROR,"Geant4VolumeManager",  " Offend.VolIDs: %s",DetectorTools::toString(ro.idSpec(),ids,code).c_str());
      throw runtime_error("Failed to populate Geant4 volume manager!");
    }
  };
}

/// Initializing constructor. The tree will automatically be built if possible
Geant4VolumeManager::Geant4VolumeManager(LCDD& lcdd, Geant4GeometryInfo* info)
: Base(info), m_isValid(false) {
  if (info && info->valid && info->g4Paths.empty()) {
    Populator p(lcdd, *info);
    p.populate(lcdd.world());
    return;
  }
  throw runtime_error(format("Geant4VolumeManager", "Attempt populate from invalid Geant4 geometry info [Invalid-Info]"));
}

/// Helper: Generate placement path from touchable object
Geant4PlacementPath Geant4VolumeManager::placementPath(const G4VTouchable* touchable, bool exception) const {
  Geant4TouchableHandler handler(touchable);
  return handler.placementPath(exception);
}

/// Check the validity of the information before accessing it.
bool Geant4VolumeManager::checkValidity() const {
  if (m_isValid) {
    return true;
  }
  else if (!isValid()) {
    throw runtime_error(format("Geant4VolumeManager", "Attempt to use invalid Geant4 volume manager [Invalid-Handle]"));
  }
  else if (!ptr()->valid) {
    throw runtime_error(format("Geant4VolumeManager", "Attempt to use invalid Geant4 geometry info [Invalid-Info]"));
  }
  m_isValid = true;
  return m_isValid;
}

/// Access CELLID by placement path
VolumeID Geant4VolumeManager::volumeID(const PlacementPath& path) const {
  if (!path.empty() && checkValidity()) {
    const Geant4PathMap& m = ptr()->g4Paths;
    Geant4PathMap::const_iterator i = m.find(path);
    if (i != m.end())
      return (*i).second;
    if (!path[0])
      return InvalidPath;
    else if (!path[0]->GetLogicalVolume()->GetSensitiveDetector())
      return Insensitive;
  }
  printout(INFO, "Geant4VolumeManager","+++   Bad volume Geant4 Path: %s",
           Geant4GeometryMaps::placementPath(path).c_str());
  return NonExisting;
}

/// Access CELLID by Geant4 touchable object
VolumeID Geant4VolumeManager::volumeID(const G4VTouchable* touchable) const {
  Geant4TouchableHandler handler(touchable);
  return volumeID(handler.placementPath());
}

/// Accessfully decoded volume fields  by placement path
void Geant4VolumeManager::volumeDescriptor(const PlacementPath& path, VolIDDescriptor& vol_desc) const {
  vol_desc.second.clear();
  vol_desc.first = NonExisting;
  if (!path.empty() && checkValidity()) {
    const Geant4PathMap& m = ptr()->g4Paths;
    Geant4PathMap::const_iterator i = m.find(path);
    if (i != m.end()) {
      VolumeID vid = (*i).second;
      G4LogicalVolume* lvol = path[0]->GetLogicalVolume();
      if (lvol->GetSensitiveDetector()) {
        const G4VPhysicalVolume* node = path[0];
        const PlacementMap& pm = ptr()->g4Placements;
        for (PlacementMap::const_iterator ipm = pm.begin(); ipm != pm.end(); ++ipm) {
          if ((*ipm).second == node)  {
            PlacedVolume pv = (*ipm).first;
            Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
            Geometry::IDDescriptor dsc = sd.readout().idSpec();
            vol_desc.first = vid;
            dsc.decodeFields(vid, vol_desc.second);
            return;
          }
        }
      }
      vol_desc.first = Insensitive;
      return;
    }
    if (!path[0])
      vol_desc.first = InvalidPath;
    else if (!path[0]->GetLogicalVolume()->GetSensitiveDetector())
      vol_desc.first = Insensitive;
    else
      vol_desc.first = NonExisting;
  }
}

/// Access fully decoded volume fields by Geant4 touchable object
void Geant4VolumeManager::volumeDescriptor(const G4VTouchable* touchable, VolIDDescriptor& vol_desc) const {
  volumeDescriptor(placementPath(touchable), vol_desc);
}

