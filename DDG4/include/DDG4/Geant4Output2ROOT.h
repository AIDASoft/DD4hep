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
#ifndef DDG4_GEANT4OUTPUT2ROOT_H
#define DDG4_GEANT4OUTPUT2ROOT_H

// Framework include files
#include <DDG4/Geant4OutputAction.h>

#include <memory>

class TFile;
class TTree;
class TBranch;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class ComponentCast;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to output Geant4 event data to ROOT files
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Output2ROOT: public Geant4OutputAction {
    protected:
      using Branches = std::map<std::string, TBranch*>;
      using Sections = std::map<std::string, TTree*>;
      /// Known file sections
      Sections m_sections;
      /// Branches in the event tree
      Branches m_branches;
      /// Reference to the ROOT file to open
      std::unique_ptr<TFile> m_file;
      /// Reference to the event data tree (owned by m_file)
      TTree* m_tree = nullptr;
      /// Property: name of the event tree
      std::string m_section;
      /// Property: vector with disabled collections
      std::vector<std::string> m_disabledCollections;
      /// Property: vector with disabled collections
      bool  m_disableParticles = false;
      /// Property: Flag if Monte-Carlo truth should be followed and checked
      bool m_handleMCTruth = true;
      /// Property: Flag to create a new output file for each run
      bool m_filesByRun = false;

    public:
      /// Standard constructor
      Geant4Output2ROOT(Geant4Context* context, const std::string& nam);
      /// Default destructor
      ~Geant4Output2ROOT() override;
      /// Create/access tree by name for non collection user data
      TTree* section(const std::string& nam);
      /// Fill single EVENT branch entry (Geant4 collection data)
      int fill(const std::string& nam, const ComponentCast& type, void* ptr);

      /// Close current output file
      virtual void closeOutput();
      /// Callback to store the Geant4 run information
      void beginRun(const G4Run* run)  override;
      /// Callback to store each Geant4 hit collection
      void saveCollection(OutputContext<G4Event>& ctxt, G4VHitsCollection* collection)  override;
      /// Callback to store the Geant4 event
      void saveEvent(OutputContext<G4Event>& ctxt)  override;

      /// Commit data at end of filling procedure
      void commit(OutputContext<G4Event>& ctxt)  override;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4OUTPUT2ROOT_H
