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
#include "DDG4/Geant4DetectorConstruction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to create Geant4 detector geometry from TGeo representation in memory
    /**
     *  On demand (ie. when calling "Construct") the dd4hep geometry is converted
     *  to Geant4 with all volumes, assemblies, shapes, materials etc.
     *  The actuak work is performed by the Geant4Converter class called by this method.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorGeometryConstruction : public Geant4DetectorConstruction   {
      /// Property: Dump geometry hierarchy
      bool m_dumpHierarchy   = false;
      /// Property: Flag to debug materials during conversion mechanism
      bool m_debugMaterials  = false;
      /// Property: Flag to debug elements during conversion mechanism
      bool m_debugElements   = false;
      /// Property: Flag to debug shapes during conversion mechanism
      bool m_debugShapes     = false;
      /// Property: Flag to debug volumes during conversion mechanism
      bool m_debugVolumes    = false;
      /// Property: Flag to debug placements during conversion mechanism
      bool m_debugPlacements = false;
      /// Property: Flag to debug regions during conversion mechanism
      bool m_debugRegions    = false;
      /// Property: Flag to debug regions during conversion mechanism
      bool m_debugSurfaces   = false;

      /// Property: Flag to dump all placements after the conversion procedure
      bool m_printPlacements = false;
      /// Property: Flag to dump all sensitives after the conversion procedure
      bool m_printSensitives = false;

      /// Property: Printout level of info object
      int  m_geoInfoPrintLevel;
      /// Property: G4 GDML dump file name (default: empty. If non empty, dump)
      std::string m_dumpGDML;
      /// Property: DD4hep path to volume to be printed (default: empty)
      std::string m_volumePath;

      /// Write GDML file
      int writeGDML();
      /// Print geant4 volume
      int printVolume();
      /// Check geant4 volume
      int checkVolume();

    public:
      /// Initializing constructor for DDG4
      Geant4DetectorGeometryConstruction(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4DetectorGeometryConstruction();
      /// Geometry construction callback. Called at "Construct()"
      void constructGeo(Geant4DetectorConstructionContext* ctxt);
      /// Install command control messenger to write GDML file from command prompt.
      virtual void installCommandMessenger()   override;
    };
  }    // End namespace sim
}      // End namespace dd4hep


// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DD4hep/DetectorTools.h>
#include "DD4hep/DD4hepUnits.h"
#include <DD4hep/Printout.h>
#include <DD4hep/Detector.h>

#include "DDG4/Geant4HierarchyDump.h"
#include "DDG4/Geant4UIMessenger.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Factories.h"

// Geant4 include files
#include <G4LogicalVolume.hh>
#include "G4PVPlacement.hh"
#include <G4Material.hh>
#include <G4VSolid.hh>

//#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
//#endif

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4DetectorGeometryConstruction)

/// Initializing constructor for other clients
Geant4DetectorGeometryConstruction::Geant4DetectorGeometryConstruction(Geant4Context* ctxt, const string& nam)
: Geant4DetectorConstruction(ctxt,nam)
{
  declareProperty("DebugMaterials",    m_debugMaterials);
  declareProperty("DebugElements",     m_debugElements);
  declareProperty("DebugShapes",       m_debugShapes);
  declareProperty("DebugVolumes",      m_debugVolumes);
  declareProperty("DebugPlacements",   m_debugPlacements);
  declareProperty("DebugRegions",      m_debugRegions);
  declareProperty("DebugSurfaces",     m_debugSurfaces);

  declareProperty("PrintPlacements",   m_printPlacements);
  declareProperty("PrintSensitives",   m_printSensitives);
  declareProperty("GeoInfoPrintLevel", m_geoInfoPrintLevel = DEBUG);

  declareProperty("DumpHierarchy",     m_dumpHierarchy);
  declareProperty("DumpGDML",          m_dumpGDML="");
  declareProperty("VolumePath",        m_volumePath="");
  InstanceCount::increment(this);
}

/// Default destructor
Geant4DetectorGeometryConstruction::~Geant4DetectorGeometryConstruction() {
  InstanceCount::decrement(this);
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorGeometryConstruction::constructGeo(Geant4DetectorConstructionContext* ctxt)   {
  Geant4Mapping&  g4map = Geant4Mapping::instance();
  DetElement      world = ctxt->description.world();
  Geant4Converter conv(ctxt->description, outputLevel());
  conv.debugMaterials  = m_debugMaterials;
  conv.debugElements   = m_debugElements;
  conv.debugShapes     = m_debugShapes;
  conv.debugVolumes    = m_debugVolumes;
  conv.debugPlacements = m_debugPlacements;
  conv.debugRegions    = m_debugRegions;
  conv.debugSurfaces   = m_debugSurfaces;

  ctxt->geometry       = conv.create(world).detach();
  ctxt->geometry->printLevel = outputLevel();
  g4map.attach(ctxt->geometry);
  G4VPhysicalVolume* w = ctxt->geometry->world();
  // Save away the reference to the world volume
  context()->kernel().setWorld(w);
  // Create Geant4 volume manager only if not yet available
  g4map.volumeManager();
  if ( m_dumpHierarchy )   {
    Geant4HierarchyDump dmp(ctxt->description);
    dmp.dump("",w);
  }
  ctxt->world = w;
  if ( !m_dumpGDML.empty() || ::getenv("DUMP_GDML") ) writeGDML();
  enableUI();
}

/// Print geant4 volume
int Geant4DetectorGeometryConstruction::printVolume()  {
  if ( !m_volumePath.empty() )   {
    Detector& det = context()->kernel().detectorDescription();
    PlacedVolume top = det.world().placement();
    PlacedVolume pv = detail::tools::findNode(top, m_volumePath);
    if ( pv.isValid() )   {
      auto& g4map = Geant4Mapping::instance().data();
      auto it = g4map.g4Volumes.find(pv.volume());
      if ( it != g4map.g4Volumes.end() )   {
        const G4LogicalVolume* vol = (*it).second;
        auto* sol = vol->GetSolid(); 
        string txt;
        stringstream str;
        str << *(vol->GetMaterial()) << endl;
        str << *sol;
        stringstream istr(str.str());
        printP2("+++  Dump of ROOT   solid: %s", m_volumePath.c_str());
        pv.volume().solid()->InspectShape();
        printP2("+++  Dump of GEANT4 solid: %s", m_volumePath.c_str());
        while(istr.good())   {
          getline(istr,txt);
          printP2(txt.c_str());
        }
        printP2("Shape: %s  cubic volume: %8.3g mm^3  area: %8.3g mm^2",
                sol->GetName().c_str(), sol->GetCubicVolume(), sol->GetSurfaceArea());
        return 1;
      }
    }
    warning("+++ printVolume: FAILED to find the volume %s from the top volume",m_volumePath.c_str());
  }
  warning("+++ printVolume: Property VolumePath not set. [Ignored]");
  return 0;
}

/// Check geant4 volume
int Geant4DetectorGeometryConstruction::checkVolume()  {
  if ( !m_volumePath.empty() )   {
    Detector& det = context()->kernel().detectorDescription();
    PlacedVolume top = det.world().placement();
    PlacedVolume pv = detail::tools::findNode(top, m_volumePath);
    if ( pv.isValid() )   {
      auto& g4map = Geant4Mapping::instance().data();
      auto it = g4map.g4Volumes.find(pv.volume());
      if ( it != g4map.g4Volumes.end() )   {
        const G4LogicalVolume* vol = (*it).second;
        auto* g4_sol = vol->GetSolid();
        Box   rt_sol = pv.volume().solid();
        G4ThreeVector pMin, pMax;
        double conv = (dd4hep::centimeter/CLHEP::centimeter)/2.0;
        g4_sol->BoundingLimits(pMin,pMax);
        printP2("Geant4 Shape: %s  cubic volume: %8.3g mm^3  area: %8.3g mm^2",
                g4_sol->GetName().c_str(), g4_sol->GetCubicVolume(), g4_sol->GetSurfaceArea());
        printP2("Geant4 Bounding box extends:    %8.3g  %8.3g %8.3g",
                (pMax.x()-pMin.x())*conv, (pMax.y()-pMin.y())*conv, (pMax.z()-pMin.z())*conv);
        printP2("ROOT   Bounding box dimensions: %8.3g  %8.3g %8.3g",
                rt_sol->GetDX(), rt_sol->GetDY(), rt_sol->GetDZ());
        
        return 1;
      }
    }
    warning("+++ checkVolume: FAILED to find the volume %s from the top volume",m_volumePath.c_str());
  }
  warning("+++ checkVolume: Property VolumePath not set. [Ignored]");
  return 0;
}

/// Write GDML file
int Geant4DetectorGeometryConstruction::writeGDML()  {
  G4VPhysicalVolume* w  = context()->world();
  //#ifdef GEANT4_HAS_GDML
  if ( !m_dumpGDML.empty() ) {
    G4GDMLParser parser;
    parser.Write(m_dumpGDML.c_str(), w);
    info("+++ writeGDML: Wrote GDML file: %s", m_dumpGDML.c_str());
    return 1;
  }
  else {
    const char* gdml_dmp = ::getenv("DUMP_GDML");
    if ( gdml_dmp )    {
      G4GDMLParser parser;
      parser.Write(gdml_dmp, w);
      info("+++ writeGDML: Wrote GDML file: %s", gdml_dmp);
      return 1;
    }
  }
  warning("+++ writeGDML: Neither property DumpGDML nor environment DUMP_GDML set. No file written!");
  //#endif
  return 0;
}

/// Install command control messenger to write GDML file from command prompt.
void Geant4DetectorGeometryConstruction::installCommandMessenger()   {
  this->Geant4DetectorConstruction::installCommandMessenger();
  m_control->addCall("writeGDML", "GDML: write geometry to file: '"+m_dumpGDML+"' [uses property DumpGDML]",
                     Callback(this).make(&Geant4DetectorGeometryConstruction::writeGDML));
  m_control->addCall("printVolume", "Print Geant4 volume properties [uses property VolumePath]",
                     Callback(this).make(&Geant4DetectorGeometryConstruction::printVolume));
  m_control->addCall("checkVolume", "Check Geant4 volume properties [uses property VolumePath]",
                     Callback(this).make(&Geant4DetectorGeometryConstruction::checkVolume));
}


