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
#ifndef EXAMPLES_DDG4_MYSENSDET_SRC_MYTRACKERHIT_H
#define EXAMPLES_DDG4_MYSENSDET_SRC_MYTRACKERHIT_H

/// Framework include files
#include "DDG4/Geant4Data.h"

namespace SomeExperiment {

  /// This is the hit definition.
  /** I took here the same definition of the default Geant4Tracker class,
   *  (see DDG4/Geant4Data.h)  but it could be anything else as well.
   *
   *  Please note:
   *  ============
   *  The MC truth handling as implemented in the Geant4ParticleHandler
   *  will not work with this class if the object(s) are saved with 
   *  the standard Geant4Output2ROOT event action. If the hit is 
   *  specialized, the output writing also must be specialized if
   *  MC truth handling should be supported.
   *  Otherwise it is sufficient to provide a ROOT dictionary as long as the
   *  base class dd4hep::sim::Geant4HitData is kept.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class MyTrackerHit : public dd4hep::sim::Geant4Tracker::Hit   {

  public:
    dd4hep::Position prePos;
    dd4hep::Position postPos;
    double step_length;

  public:
    /// Default constructor
    MyTrackerHit() = default;
    /// Initializing constructor
    MyTrackerHit(int track_id, int pdg_id, double deposit, double time_stamp, double len,
		 const dd4hep::Position& pos, const dd4hep::Direction& mom)
      : dd4hep::sim::Geant4Tracker::Hit(track_id,pdg_id,deposit,time_stamp, len, pos, mom) {}
    /// Default destructor
    virtual ~MyTrackerHit() = default;
    /// Assignment operator
    void copyFrom(const MyTrackerHit& c);
  };

  /// Helper to dump data file
  /**
   *  Usage:  
   *  $> root.exe
   *  ....
   *  root [0] gSystem->Load("libDDG4Plugins.so");
   *  root [1] gSystem->Load("libDDG4_MySensDet.so");
   *  root [2] SomeExperiment::Dump::dumpData(<num-ebents>,<file-name>);
   *
   */
  class Dump   {
  public:
    /// Standalone function to dump data from a root file
    static int dumpData(int num_evts, const char* file_name);
  };
}

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

/// Define namespaces
#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::sim;
#pragma link C++ namespace SomeExperiment;
#pragma link C++ class     SomeExperiment::MyTrackerHit+;
#pragma link C++ class     SomeExperiment::Dump;
#endif

#endif // EXAMPLES_DDG4_MYSENSDET_SRC_MYTRACKERHIT_H
