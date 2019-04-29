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
#ifndef DD4HEP_DDG4_GEANT4GDMLWRITEACTION_H
#define DD4HEP_DDG4_GEANT4GDMLWRITEACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to measure the energy of escaping tracks
    /** Class to dump Geant4 geometry to GDML
     *
     *  Please note: 
     *  The Geant4 physics list must be initialized BEFORE
     *  invoking the writer with options. Otherwise the particle definitions
     *  are missing!
     *  If you ONLY want to dump the geometry to GDML you must call
     *  /run/beamOn 0
     *  before writing the GDML file!
     *  You also need to setup a minimal generation action like:
     *  sid.geant4.setupGun('Gun','pi-',10*GeV,Standalone=True)
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GDMLWriteAction : public Geant4Action {
    public:
      /// Property: collection names to be dumped 
      std::string m_output;
      /// Poprerty: Flag to overwrite existing files
      int         m_overWrite;
      /// Property: Export region information to the GDML
      int         m_exportRegions;
      /// Property: Export energy cut information to the GDML
      int         m_exportEnergyCuts;
      /// Property: Export sensitive detector information to the GDML
      int         m_exportSensitiveDetectors;

    public:
      /// Standard constructor
      Geant4GDMLWriteAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4GDMLWriteAction();
      /// Install command control messenger if wanted
      virtual void installCommandMessenger()  override;
      /// Write geometry to GDML
      virtual void writeGDML();
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif /* DD4HEP_DDG4_GEANT4GDMLWRITEACTION_H */

//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4UIMessenger.h"

// Geant 4 includes
#include "G4GDMLParser.hh"
#include "G4Version.hh"

// C/C++ include files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Standard constructor
Geant4GDMLWriteAction::Geant4GDMLWriteAction(Geant4Context* ctxt, const string& nam)
  : Geant4Action(ctxt, nam)
{
  m_needsControl = true;
  declareProperty("Output",                   m_output = "");
  declareProperty("OverWrite",                m_overWrite = 1);
  declareProperty("exportRegions",            m_exportRegions = 1);
  declareProperty("exportEnergyCuts",         m_exportEnergyCuts = 1);
  declareProperty("exportSensitiveDetectors", m_exportSensitiveDetectors = 1);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4GDMLWriteAction::~Geant4GDMLWriteAction() {
  InstanceCount::decrement(this);
}

/// Install command control messenger if wanted
void Geant4GDMLWriteAction::installCommandMessenger()   {
  Callback cb = Callback(this).make(&Geant4GDMLWriteAction::writeGDML);
  m_control->addCall("write", "Write geometry to GDML file",cb);
}

/// Write geometry to GDML
void Geant4GDMLWriteAction::writeGDML()   {
  struct stat buff;
  if ( m_output.empty() )   {
    error("+++ No GDML file name given. Please set the output file (property Output)");
    return;
  }
  if ( 0 == ::stat(m_output.c_str(), &buff) && !m_overWrite )  {
    error("+++ GDML file elready exists. Please set another output file (property Output)");
    return;
  }
  if ( 0 == ::stat(m_output.c_str(), &buff) && m_overWrite )  {
    warning("+++ GDML file %s already exists. Overwriting existing file.", m_output.c_str());
    ::unlink(m_output.c_str());
  }
  unique_ptr<G4GDMLParser> parser(new G4GDMLParser());
  parser->SetRegionExport(m_exportRegions != 0);
  parser->SetEnergyCutsExport(m_exportEnergyCuts != 0);
#if G4VERSION_NUMBER>=1020
  parser->SetSDExport(m_exportSensitiveDetectors != 0);
#endif
  info("+++ Writing GDML file: %s", m_output.c_str());
  parser->Write(m_output, context()->world());
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4GDMLWriteAction)
