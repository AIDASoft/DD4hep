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
#ifndef DD4HEP_DDG4_HITTUPLEACTION_H
#define DD4HEP_DDG4_HITTUPLEACTION_H

// Framework include files
#include "DDG4/Geant4EventAction.h"

// Forward declarations
class G4VHitsCollection;
class TFile;
class TTree;
class TBranch;

/// Namespace example name of the user
namespace myanalysis {

  // Forward declarations
  class Geant4ParticleMap;
    
  /// Class to measure the energy of escaping tracks
  /** Class to measure the energy of escaping tracks of a detector using Geant 4
   * Measure escaping energy....
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class HitTupleAction : public dd4hep::sim::Geant4EventAction {
  public:
    typedef std::vector<std::string> CollectionNames;
    /// Property: collection names to be dumped 
    CollectionNames m_containers;
    /// Property: Output file name
    std::string     m_outFileName;
    /// ROOT TFile instance
    TFile*          m_outFile = 0;
    /// Tree object within the file
    TTree*          m_outTree = 0;
    /// We want to write a separate branch for all deposits of one container(sub-detector)
    typedef std::pair<double, std::vector<double> > Data;
    /// The intermediate storage of the hit deposits to be written to ROOT
    std::map<std::string, std::pair<TBranch*, Data> > m_deposits;
    
  public:
    /// Standard constructor
    HitTupleAction(dd4hep::sim::Geant4Context* context, const std::string& nam);
    /// Default destructor
    virtual ~HitTupleAction();
    /// Begin-of-run callback: Open output file
    void beginRun(const G4Run* run);
    /// End-of-run callback: Close output file
    void endRun(const G4Run* run);
    /// Geant4EventAction interface: Begin-of-event callback
    virtual void begin(const G4Event* event)  override;
    /// Geant4EventAction interface: End-of-event callback
    virtual void end(const G4Event* event)  override;
  };
}      // End namespace dd4hep
#endif /* DD4HEP_DDG4_HITTUPLEACTION_H */

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
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4HitCollection.h"

// Geant 4 includes
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

// ROOT include files
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TClass.h"
#include "TBranch.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Standard constructor
myanalysis::HitTupleAction::HitTupleAction(Geant4Context* ctxt, const string& nam)
  : Geant4EventAction(ctxt, nam), m_containers{"*"}
{
  m_needsControl = true;
  m_containers.push_back("*");
  declareProperty("OutputFile",   m_outFileName);
  declareProperty("Collections",  m_containers);
  runAction().callAtBegin(this,  &HitTupleAction::beginRun);
  runAction().callAtEnd(this,    &HitTupleAction::endRun);
}

/// Default destructor
myanalysis::HitTupleAction::~HitTupleAction() {
}

/// Begin-of-run callback: Open output file
void myanalysis::HitTupleAction::beginRun(const G4Run* /* run */)   {
}

/// End-of-run callback: Close output file
void myanalysis::HitTupleAction::endRun(const G4Run* /* run */)    {
  if ( m_outFile )   {
    if ( m_outTree )   {
      m_outTree->Print();
    }
    m_outFile->Write();
    m_outFile->Close();
    delete m_outFile;
    m_outFile = 0;
  }
}

/// Geant4EventAction interface: Begin-of-event callback
void myanalysis::HitTupleAction::begin(const G4Event* /* event */)   {
}

/// Geant4EventAction interface: End-of-event callback
void myanalysis::HitTupleAction::end(const G4Event* event)    {
  G4HCofThisEvent* hce = event->GetHCofThisEvent();
  if ( hce )  {
    int nCol = hce->GetNumberOfCollections();
    if ( nCol <= 0 )   {
      return;
    }
    else if ( !m_outFile )   {
      //                      Name                   Option      Title
      m_outFile = TFile::Open(m_outFileName.c_str(), "RECREATE", "DDG4 User file");
      if ( m_outFile && !m_outFile->IsZombie() )   {
        m_outTree = new TTree("DDG4 User Test","DDG4 data");
        printout(ALWAYS,"HitTupleAction","+++ Successfully opened ROOT file %s and created TTree:%s",
                 m_outFile->GetName(), "DDG4 User Test");
        if ( m_containers.size() == 1 && (m_containers[0] == "*" || m_containers[0] == "ALL") )  {
          m_containers.clear();
          for (int i = 0; i < nCol; ++i)
            m_containers.push_back(hce->GetHC(i)->GetName());
        }
        // Seperate loop. We need fixed addresses when creating the branches
        for(const auto& c : m_containers)   {
          m_deposits[c].first = 0;
          m_deposits[c].second.first = 0e0;
          m_deposits[c].second.second.clear();
        }
        for(const auto& c : m_containers)   {
          std::pair<TBranch*, Data>& e = m_deposits[c];
          TClass* cl = gROOT->GetClass(typeid(Data));
          e.first = m_outTree->Branch(c.c_str(), cl->GetName(), (void*)0);
          e.first->SetAutoDelete(false);
          printout(ALWAYS,"HitTupleAction","+++ Prepare hit branch %s in root file.",c.c_str());
        }
      }
      else   {
        except("HitTupleAction","Failed to open ROOT N-tuple file: %s",m_outFileName.c_str());
      }
    }
    for ( auto& e : m_deposits )  {
      e.second.first->SetAddress(0);
    }
    for (int i = 0; i < nCol; ++i) {
      G4VHitsCollection* hc = hce->GetHC(i);
      const string& nam = hc->GetName();
      if ( find(m_containers.begin(),m_containers.end(),nam) != m_containers.end() )   {
        Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(hc);
        if ( coll )    {
          std::pair<TBranch*, Data>& e = m_deposits[nam];
          size_t nhits = coll->GetSize();
          Data* d = &e.second;

          e.second.first = 0e0;
          e.second.second.clear();
          e.first->SetAddress(&d);
          for ( size_t j=0; j<nhits; ++j )   {
            double dep = 0e0;
            Geant4HitData* h = coll->hit(j);
            Geant4Tracker::Hit* trk_hit = dynamic_cast<Geant4Tracker::Hit*>(h);
            if ( 0 != trk_hit )   {
              dep = trk_hit->truth.deposit;
            }
            else  {
              Geant4Calorimeter::Hit* cal_hit = dynamic_cast<Geant4Calorimeter::Hit*>(h);
              if ( 0 != cal_hit )
                dep = cal_hit->energyDeposit;
              else
                continue;
            }
            if ( dep > 0 )  {
              e.second.first += dep;
              e.second.second.push_back(dep);
            }
          }
        }
      }
    }
    m_outTree->Fill();
    return;
  }
  warning("+++ [Event:%d] The value of G4HCofThisEvent is NULL.",event->GetEventID());
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(myanalysis,HitTupleAction)
