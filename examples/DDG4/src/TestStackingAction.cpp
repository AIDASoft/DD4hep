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

// Framework include files
#include "DDG4/Geant4StackingAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    
    /// Class to measure the energy of escaping tracks
    /** Class to measure the energy of escaping tracks of a detector using Geant 4
     * Measure escaping energy....
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class TestStackingAction : public Geant4StackingAction {
      std::size_t m_calls_newStage   { 0UL };
      std::size_t m_calls_prepare    { 0UL };
      std::size_t m_calls_classify   { 0UL };
    
    public:
      /// Standard constructor
      TestStackingAction(Geant4Context* context, const std::string& nam)
	: Geant4StackingAction(context, nam) 
      {
      }
      /// Default destructor
      virtual ~TestStackingAction()   {
	info("+++ Calls newStage: %ld prepare: %ld classifyNewTrack: %ld",
	     m_calls_newStage, m_calls_prepare, m_calls_classify);
      }
      /// New-stage callback
      virtual void newStage(G4StackManager* stackManager)   override final  {
	info("+++ [%ld] Calling newStage. StackManager: %p", m_calls_newStage, (void*)stackManager);
	++m_calls_newStage;
      }
      /// Preparation callback
      virtual void prepare(G4StackManager* stackManager)   override final   {
	info("+++ [%ld] Calling prepare. StackManager: %p", m_calls_prepare, (void*)stackManager);
	++m_calls_prepare;
      }
      /// Return TrackClassification with enum G4ClassificationOfNewTrack or NoTrackClassification
      virtual TrackClassification 
      classifyNewTrack(G4StackManager* stackManager, const G4Track* track)   override final   {
	info("+++ [%ld] Calling classifyNewTrack. StackManager: %p Track: %p", 
	     m_calls_classify, (void*)stackManager, (void*)track);
	++m_calls_classify;
	return { fKill };
      }
    };
  }    // End namespace sim
}      // End namespace dd4hep

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(dd4hep::sim,TestStackingAction)
