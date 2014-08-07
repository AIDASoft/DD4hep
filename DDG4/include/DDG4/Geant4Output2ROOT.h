// $Id: Geant4Field.cpp 875 2013-11-04 16:15:14Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4OUTPUT2ROOT_H
#define DD4HEP_DDG4_GEANT4OUTPUT2ROOT_H

// Framework include files
#include "DDG4/Geant4OutputAction.h"

class TFile;
class TTree;
class TBranch;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  class ComponentCast;

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4Output2ROOT Geant4Output2ROOT.h DDG4/Geant4Output2ROOT.h
     *
     * Base class to output Geant4 event data to media
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Output2ROOT: public Geant4OutputAction {
    protected:
      typedef std::map<std::string, TBranch*> Branches;
      typedef std::map<std::string, TTree*> Sections;
      /// Known file sections
      Sections m_sections;
      /// Branches in the event tree
      Branches m_branches;
      /// name of the event tree
      std::string m_section;
      /// Reference to the ROOT file to open
      TFile* m_file;
      /// Reference to the event data tree
      TTree* m_tree;
    public:
      /// Standard constructor
      Geant4Output2ROOT(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4Output2ROOT();
      /// Create/access tree by name for non collection user data
      TTree* section(const std::string& nam);
      /// Fill single EVENT branch entry (Geant4 collection data)
      int fill(const std::string& nam, const ComponentCast& type, void* ptr);

      /// Callback to store the Geant4 run information
      virtual void beginRun(const G4Run* run);
      /// Callback to store each Geant4 hit collection
      virtual void saveCollection(OutputContext<G4Event>& ctxt, G4VHitsCollection* collection);
      /// Callback to store the Geant4 event
      virtual void saveEvent(OutputContext<G4Event>& ctxt);

      /// Commit data at end of filling procedure
      virtual void commit(OutputContext<G4Event>& ctxt);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4OUTPUT2ROOT_H
