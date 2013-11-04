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
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Geant4Mapping.h"

// Geant4 include files
#include "G4VTouchable.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

using namespace DD4hep::Simulation;
using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

typedef Geant4GeometryInfo::PathMap      PathMap;
typedef Geant4GeometryInfo::PlacementMap PlacementMap;

namespace {

  struct Populator  {
    typedef vector<const TGeoNode*>      Chain;
    typedef DD4hep::Geometry::LCDD       LCDD;
    typedef DD4hep::Geometry::Readout    Readout;
    typedef DD4hep::Geometry::DetElement DetElement;

    /// Reference to the LCDD instance
    LCDD&               m_lcdd;
    /// Set of already added entries
    set<VolumeID>       m_entries;
    /// Reference to Geant4 translation information
    Geant4GeometryInfo& m_geo;

    /// Default constructor
    Populator(LCDD& lcdd, Geant4GeometryInfo& g) : m_lcdd(lcdd), m_geo(g) {
    }

    /// Populate the Volume manager
    void populate(DetElement e)   {
      const DetElement::Children& c = e.children();
      for(DetElement::Children::const_iterator i=c.begin(); i!=c.end(); ++i)   {
	DetElement   de = (*i).second;
	PlacedVolume pv = de.placement();
	if ( pv.isValid() )  {
	  Chain                chain;
	  SensitiveDetector    sd;
	  PlacedVolume::VolIDs ids;
	  m_entries.clear();
	  scanPhysicalVolume(pv.ptr(), ids, sd, chain);
	  continue;
	}
	printout(WARNING,"VolumeManager","++ Detector element %s of type %s has no placement.",
		 de.name(),de.type().c_str());
      }
    }

    /// Scan a single physical volume and look for sensitive elements below
    void scanPhysicalVolume(const TGeoNode* node, PlacedVolume::VolIDs ids, SensitiveDetector& sd, Chain& chain)    {
      PlacedVolume pv  = Ref_t(node);
      Volume       vol = pv.volume();
      PlacedVolume::VolIDs pv_ids = pv.volIDs();

      chain.push_back(node);
      ids.PlacedVolume::VolIDs::Base::insert(ids.end(),pv_ids.begin(),pv_ids.end());
      if ( vol.isSensitive() )  {
	sd = vol.sensitiveDetector();
	if ( sd.readout().isValid() )  {
	  add_entry(sd, node, ids, chain);
	}
	else  {
	  printout(WARNING,"VolumeManager",
		   "populate: Strange constellation volume %s is sensitive, but has no readout! sd:%p",
		   pv.volume().name(), sd.ptr());
	}
      }
      for(Int_t idau=0, ndau=node->GetNdaughters(); idau<ndau; ++idau)  {
	TGeoNode* daughter = node->GetDaughter(idau);
	if ( dynamic_cast<const PlacedVolume::Object*>(daughter) ) {
	  scanPhysicalVolume(daughter, ids, sd, chain);
	}
      }
      chain.pop_back();
    }

    void add_entry(SensitiveDetector sd, const TGeoNode* /* n */, const PlacedVolume::VolIDs& ids, const Chain& nodes)  {
      Readout      ro      = sd.readout();
      IDDescriptor iddesc  = ro.idSpec();
      VolumeID     code    = iddesc.encode(ids);
      if ( m_entries.find(code) == m_entries.end() )  {
	Geant4Mapping::PlacementPath path;
	path.reserve(nodes.size());
	for(Chain::const_reverse_iterator i=nodes.rbegin(); i != nodes.rend(); ++i)   {
	  const TGeoNode* node = *i;
	  PlacementMap::const_iterator g4pit = m_geo.g4Placements.find(node);
	  if ( g4pit != m_geo.g4Placements.end() )  {
	    path.push_back((*g4pit).second);
	  }
	}
	if ( m_geo.g4Paths.find(path) != m_geo.g4Paths.end() )  {
	  printout(ERROR,"VolumeManager","populate: Severe error: Duplicated Geant4 path!!!!");
	}
	m_geo.g4Paths[path] = code;
	m_entries.insert(code);
      }
      else   {
	printout(ERROR,"VolumeManager","populate: Severe error: Duplicated Volume entry: %X",code);
      }
    }
  };
}

/// Initializing constructor. The tree will automatically be built if possible
Geant4VolumeManager::Geant4VolumeManager(LCDD& lcdd, Geant4GeometryInfo* info) 
: Base(info), m_isValid(false)
{
  if ( info && info->valid && info->g4Paths.empty() )   {
    Populator p(lcdd,*info);
    p.populate(lcdd.world());
    return;
  }
  throw runtime_error(format("Geant4VolumeManager","Attempt populate from invalid Geant4 geometry info [Invalid-Info]"));
}

/// Helper: Generate placement path from touchable object
Geant4VolumeManager::PlacementPath 
Geant4VolumeManager::placementPath(const G4VTouchable* touchable, bool exception) const   
{
  Geant4Mapping::PlacementPath path;
  if ( touchable )   {
    for(int i=0, n=touchable->GetHistoryDepth(); i<n; ++i)  {
      G4VPhysicalVolume* pv = touchable->GetVolume(i);
      path.push_back(pv);
    }
  }
  else if ( exception )  {
    throw runtime_error(format("Geant4VolumeManager","Attempt to use invalid Geant4 touchable [Invalid-Touchable]"));
  }
  return path;
}

/// Check the validity of the information before accessing it.
bool Geant4VolumeManager::checkValidity() const   {
  if ( m_isValid )  {
    return true;
  }
  else if ( !isValid() )   {
    throw runtime_error(format("Geant4VolumeManager","Attempt to use invalid Geant4 volume manager [Invalid-Handle]"));
  }
  else if ( !ptr()->valid )  {
    throw runtime_error(format("Geant4VolumeManager","Attempt to use invalid Geant4 geometry info [Invalid-Info]"));
  }
  m_isValid = true;
  return m_isValid;
}

/// Accessor to resolve G4 placements
G4VPhysicalVolume* Geant4VolumeManager::placement(const TGeoNode* node)  const   {
  if ( node && checkValidity() )  {
    const PlacementMap& m = ptr()->g4Placements;
    PlacementMap::const_iterator i = m.find(node);
    if ( i !=  m.end() ) return (*i).second;
    return 0;
  }
  throw runtime_error(format("Geant4VolumeManager","Attempt to use invalid Geant4 volume manager [Invalid-Handle]"));
}

/// Accessor to resolve geometry placements
PlacedVolume Geant4VolumeManager::placement(const G4VPhysicalVolume* node)  const   {
  if ( node && checkValidity() )  {
    const PlacementMap& m = ptr()->g4Placements;
    for(PlacementMap::const_iterator i = m.begin(); i != m.end(); ++i)   {
      if ( (*i).second == node ) return PlacedVolume((*i).first);
    }
    return PlacedVolume(0);
  }
  throw runtime_error(format("Geant4VolumeManager","Attempt to lookup invalid TGeo placement [Null-Pointer]"));
}

/// Access CELLID by placement path
VolumeID Geant4VolumeManager::volumeID(const PlacementPath& path) const    {
  if ( !path.empty() && checkValidity() )  {
    const PathMap& m = ptr()->g4Paths;
    PathMap::const_iterator i=m.find(path);
    if ( i != m.end() ) return (*i).second;
    if ( !path[0] )
      return InvalidPath;
    else if ( !path[0]->GetLogicalVolume()->GetSensitiveDetector() )
      return Insensitive;
    return NonExisting;
  }
  return NonExisting;
}

/// Access CELLID by Geant4 touchable object
VolumeID Geant4VolumeManager::volumeID(const G4VTouchable* touchable) const   {
  return volumeID(placementPath(touchable));
}

/// Accessfully decoded volume fields  by placement path
void Geant4VolumeManager::volumeDescriptor(const PlacementPath& path, VolIDDescriptor& vol_desc) const    {
  vol_desc.second.clear();
  vol_desc.first = NonExisting;
  if ( !path.empty() && checkValidity() )  {
    const PathMap& m = ptr()->g4Paths;
    PathMap::const_iterator i=m.find(path);
    if ( i != m.end() )   {
      VolumeID vid = (*i).second;
      G4LogicalVolume* lvol = path[0]->GetLogicalVolume();
      if ( lvol->GetSensitiveDetector() )  {
	PlacedVolume pv = placement(path[0]);
	Geometry::SensitiveDetector sd  = pv.volume().sensitiveDetector();
	Geometry::IDDescriptor      dsc = sd.readout().idSpec();
	vol_desc.first = vid;
	dsc.decodeFields(vid,vol_desc.second);
	return;
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
void Geant4VolumeManager::volumeDescriptor(const G4VTouchable* touchable, VolIDDescriptor& vol_desc) const    {
  volumeDescriptor(placementPath(touchable),vol_desc);
}

