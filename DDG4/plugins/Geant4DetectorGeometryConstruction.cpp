//  AIDA Detector description implementation for LCD
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
// $Id$

// Framework include files
#include "DDG4/Geant4DetectorConstruction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Class to create Geant4 detector geometry from TGeo representation in memory
    /**
     *  On demand (ie. when calling "Construct") the DD4hep geometry is converted
     *  to Geant4 with all volumes, assemblies, shapes, materials etc.
     *  The actuak work is performed by the Geant4Converter class called by this method.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorGeometryConstruction : public Geant4DetectorConstruction   {
      /// Property: Dump geometry hierarchy
      bool m_dumpHierarchy;
      /// Property: G4 GDML dump file name (default: empty. If non empty, dump)
      std::string m_dumpGDML;

    public:
      /// Initializing constructor for DDG4
      Geant4DetectorGeometryConstruction(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4DetectorGeometryConstruction();
      /// Geometry construction callback. Called at "Construct()"
      void constructGeo(Geant4DetectorConstructionContext* ctxt);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep


// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"

#include "DDG4/Geant4HierarchyDump.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Factories.h"

// Geant4 include files
#include "G4PVPlacement.hh"
#ifdef GEANT4_HAS_GDML
#include "G4GDMLParser.hh"
#endif

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
DECLARE_GEANT4ACTION(Geant4DetectorGeometryConstruction)

/// Initializing constructor for other clients
Geant4DetectorGeometryConstruction::Geant4DetectorGeometryConstruction(Geant4Context* ctxt, const string& nam)
  : Geant4DetectorConstruction(ctxt,nam)
{
  declareProperty("DumpHierarchy", m_dumpHierarchy=false);
  declareProperty("DumpGDML",      m_dumpGDML="");
  InstanceCount::increment(this);
}

/// Default destructor
Geant4DetectorGeometryConstruction::~Geant4DetectorGeometryConstruction() {
  InstanceCount::decrement(this);
}

/// Geometry construction callback. Called at "Construct()"
void Geant4DetectorGeometryConstruction::constructGeo(Geant4DetectorConstructionContext* ctxt)   {
  Geant4Mapping& g4map = Geant4Mapping::instance();
  Geometry::DetElement world = ctxt->lcdd.world();
  Geant4Converter conv(ctxt->lcdd, outputLevel());
  ctxt->geometry = conv.create(world).detach();
  g4map.attach(ctxt->geometry);
  G4VPhysicalVolume* w = ctxt->geometry->world();
  // Create Geant4 volume manager only if not yet available
  if ( not g4map.volumeManager().isValid() ) {
    ctxt->lcdd.apply("DD4hepVolumeManager", 0, 0);
  }
  // Create Geant4 volume manager
  g4map.volumeManager();
  if ( m_dumpHierarchy )   {
    Geant4HierarchyDump dmp(ctxt->lcdd);
    dmp.dump("",w);
  }
#ifdef GEANT4_HAS_GDML
  const char* gdml_dmp = ::getenv("DUMP_GDML");
  if ( !m_dumpGDML.empty() ||  ) {
    G4GDMLParser parser;
    if ( !m_dumpGDML.empty() )
      parser.Write(m_dumpGDML.c_str(), w);
    else if ( gdml_dmp )
      parser.Write(gdml_dmp, w);
  }
#endif
  ctxt->world = w;
}

