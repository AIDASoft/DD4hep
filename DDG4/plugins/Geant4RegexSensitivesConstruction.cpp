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
      std::size_t collect_volumes(std::set<Volume>&  volumes,
                                  PlacedVolume       pv,
                                  const std::string& path,
                                  const std::vector<std::regex>& matches);
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
Geant4RegexSensitivesConstruction::collect_volumes(std::set<Volume>&  volumes,
                                                   PlacedVolume       pv,
                                                   const std::string& path,
                                                   const std::vector<std::regex>& matches)
{
  std::size_t count = 0;
  // Try to minimize a bit the number of regex matches.
  if ( volumes.find(pv.volume()) == volumes.end() )  {
    if( !path.empty() )  {
      for( const auto& match : matches )  {
        std::smatch sm;
        bool stat = std::regex_match(path, sm, match);
        if( stat )  {
          volumes.insert(pv.volume());
          ++count;
          break;
        }
      }
    }
    // Now recurse down the daughters
    for( int i=0, num = pv->GetNdaughters(); i < num; ++i )  {
      PlacedVolume daughter = pv->GetDaughter(i);
      std::string  daughter_path = path + "/" + daughter.name();
      count += this->collect_volumes(volumes, daughter, daughter_path, matches);
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

  std::set<Volume> volumes;
  int flags = std::regex_constants::icase | std::regex_constants::ECMAScript;
  std::vector<std::regex> expressions;
  for( const auto& val : regex_values )  {
    std::regex e(val, (std::regex_constants::syntax_option_type)flags);
    expressions.emplace_back(e);
  }
  TTimeStamp start;
  print("+++ Detector: %s Starting to scan volume....", det);
  std::size_t num_nodes = this->collect_volumes(volumes, de.placement(), de.placementPath(), expressions);
  for( const auto& vol : volumes )  {
    G4LogicalVolume* g4vol = g4info->g4Volumes[vol];
    if( !g4vol )  {
      except("+++ Failed to access G4LogicalVolume for SD %s of type %s", nam.c_str(), typ.c_str());
    }
    print("+++ Detector: %s Assign sensitive detector [%s] to volume: %s.",
          nam.c_str(), typ.c_str(), vol.name());
    ctxt->setSensitiveDetector(g4vol, g4sd);
  }
  TTimeStamp stop;
  print("+++ Detector: %s Handled %ld nodes with %ld sensitive volume type(s). Total of %7.3f seconds.",
        det, num_nodes, volumes.size(), stop.AsDouble()-start.AsDouble() );
}
