//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-09
//
//==========================================================================

// Framework include files
#include <DDG4/Geant4DetectorConstruction.h>

// C/C++ include files
#include <set>
#include <regex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to create Geant4 detector geometry from TGeo representation in memory
    /**
     *  On demand the sensitive detectors are created and attached to all sensitive
     *  volumes. The relevant  callback is executed when the call to 
     *  ConstructSDandField() of the corresponding G4VUserDetectorConstruction
     *  instance is called. The call is thread-local!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4RegexSensitivesConstruction : public Geant4DetectorConstruction   {
    public:
      std::string detector_name;
      std::vector<std::string> regex_values;
      // Cached result from the first call: TGeo geometry is shared across worker
      // threads so the matching volume set is identical every call. Subsequent
      // calls skip the tree traversal and reuse this directly.
      std::set<Volume> m_cached_volumes;
      bool             m_volumes_cached {false};
      std::size_t collect_volumes(std::set<Volume>&               volumes,
                                  std::set<Volume>&               visited,
                                  PlacedVolume                    pv,
                                  std::string&                    path,
                                  const std::vector<std::regex>&  matches);
    public:
      /// Initializing constructor for DDG4
      Geant4RegexSensitivesConstruction(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RegexSensitivesConstruction();
      /// Sensitives construction callback. Called at "ConstructSDandField()"
      void constructSensitives(Geant4DetectorConstructionContext* ctxt);
    };
  }    // End namespace sim
}      // End namespace dd4hep


// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/Detector.h>
#include <DD4hep/DetectorTools.h>

#include <DDG4/Geant4Mapping.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Factories.h>

// ROOT include files
#include <TTimeStamp.h>
#include <TGeoManager.h>
// Geant4 include files
#include <G4PVPlacement.hh>
#include <G4VSensitiveDetector.hh>

using namespace dd4hep::sim;

DECLARE_GEANT4ACTION(Geant4RegexSensitivesConstruction)

/// Initializing constructor for other clients
Geant4RegexSensitivesConstruction::Geant4RegexSensitivesConstruction(Geant4Context* ctxt, const std::string& nam)
: Geant4DetectorConstruction(ctxt,nam)
{
  declareProperty("Detector", detector_name);
  declareProperty("Match",    regex_values);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4RegexSensitivesConstruction::~Geant4RegexSensitivesConstruction() {
  InstanceCount::decrement(this);
}

std::size_t
Geant4RegexSensitivesConstruction::collect_volumes(std::set<Volume>&               volumes,
                                                   std::set<Volume>&               visited,
                                                   PlacedVolume                    pv,
                                                   std::string&                    path,
                                                   const std::vector<std::regex>&  matches)
{
  std::size_t count = 0;
  // visited guards on logical volume: each unique Volume is walked exactly once
  // regardless of how many times it is placed in the geometry tree.
  if ( visited.insert(pv.volume()).second )  {
    for( const auto& match : matches )  {
      std::smatch sm;
      if( std::regex_search(path, sm, match) )  {
        volumes.insert(pv.volume());
        ++count;
        break;
      }
    }
    // Recurse into daughters, reusing the path string in-place.
    const std::size_t base_len = path.size();
    for( int i=0, num = pv->GetNdaughters(); i < num; ++i )  {
      PlacedVolume daughter = pv->GetDaughter(i);
      path += '/';
      path += daughter.name();
      count += this->collect_volumes(volumes, visited, daughter, path, matches);
      path.resize(base_len);
    }
  }
  return count;
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4RegexSensitivesConstruction::constructSensitives(Geant4DetectorConstructionContext* ctxt)   {
  Geant4GeometryInfo* g4info = Geant4Mapping::instance().ptr();
  const Geant4Kernel& kernel = context()->kernel();
  const auto&         types  = kernel.sensitiveDetectorTypes();
  const std::string&  dflt   = kernel.defaultSensitiveDetectorType();
  const char*         det    = detector_name.c_str();
  
  DetElement de = detail::tools::findElement(ctxt->description, detector_name);
  if( !de.isValid() )  {
    except("Failed to locate subdetector DetElement %s to manage Geant4 energy deposits.", det);
  }
  SensitiveDetector sd = ctxt->description.sensitiveDetector(detector_name);
  if( !sd.isValid() )  {
    except("Failed to locate sensitive detector %s to manage Geant4 energy deposits.", det);
  }
  std::string nam  = sd.name();
  auto        iter = types.find(nam);
  std::string typ  = (iter != types.end()) ? (*iter).second : dflt;
  G4VSensitiveDetector* g4sd = this->createSensitiveDetector(typ, nam);

  TTimeStamp start;
  std::size_t num_nodes = 0;
  if( !m_volumes_cached )  {
    int flags = std::regex_constants::icase | std::regex_constants::ECMAScript;
    std::vector<std::regex> expressions;
    for( const auto& val : regex_values )  {
      std::regex e(val, (std::regex_constants::syntax_option_type)flags);
      expressions.emplace_back(e);
    }
    info("%s Starting to scan volume....", det);
    std::set<Volume> visited;
    std::string placement_path = de.placementPath();
    num_nodes = this->collect_volumes(m_cached_volumes, visited, de.placement(), placement_path, expressions);
    m_volumes_cached = true;
  }
  else  {
    info("%s Reusing cached volume set (%zu volumes).", det, m_cached_volumes.size());
  }
  const std::set<Volume>& volumes = m_cached_volumes;
  for( const auto& vol : volumes )  {
    G4LogicalVolume* g4vol = g4info->g4Volumes[vol];
    if( !g4vol )  {
      except("+++ Failed to access G4LogicalVolume for SD %s of type %s", nam.c_str(), typ.c_str());
    }
    debug("%s Assign sensitive detector [%s] to volume: %s.",
          nam.c_str(), typ.c_str(), vol.name());
    ctxt->setSensitiveDetector(g4vol, g4sd);
  }
  TTimeStamp stop;
  info("%s Handled %ld nodes with %ld sensitive volume type(s). Total of %7.3f seconds.",
       det, num_nodes, volumes.size(), stop.AsDouble()-start.AsDouble() );
}
