//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : A. Sailer
//
//==========================================================================

/** \addtogroup Geant4PhysicsConstructor
 *
 * @{
 * \package Geant4SetKETolerance
 *
 * \brief PhysicsConstructor to change the KETolerance and its warning severity for dynamic particles
 *
 * @}
 */

#ifndef DDG4_Geant4SetKETolerance_h
#define DDG4_Geant4SetKETolerance_h 1

// Framework include files
#include <DDG4/Geant4PhysicsList.h>

// geant4
#include <G4Version.hh>
#if G4VERSION_NUMBER >= 1130
#include "G4ExceptionSeverity.hh"
#include "G4PrimaryTransformer.hh"
#endif

#include <CLHEP/Units/SystemOfUnits.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4 physics list action to set KETolerance
    /**
     *  \author  A.Sailer
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SetKETolerance : public Geant4PhysicsList    {
    public:
      Geant4SetKETolerance(Geant4Context* ctxt, const std::string& nam)
        : Geant4PhysicsList(ctxt, nam), m_kETolerance(1.0*CLHEP::MeV)
#if G4VERSION_NUMBER >= 1130
          , m_kESeverity(G4ExceptionSeverity::JustWarning)
#endif
      {
        declareProperty("Tolerance", m_kETolerance);
#if G4VERSION_NUMBER >= 1130
        declareProperty("Severity", m_kESeverity);
#endif
      }
      virtual ~Geant4SetKETolerance()   {
      }
      /// Callback to call the KETolerance setter
      virtual void constructProcesses(G4VUserPhysicsList*) {
#if G4VERSION_NUMBER >= 1130
        G4PrimaryTransformer::SetKETolerance(m_kETolerance, G4ExceptionSeverity(m_kESeverity));
        printout(INFO, name(), "setting tolerance to %f MeV", m_kETolerance / CLHEP::MeV);
        printout(INFO, name(), "setting severity to %s", m_sevMap[m_kESeverity].c_str());
#else
        printout(DEBUG, name(), "SetKETolerance not implemented for this version of Geant4");
#endif
      }

    private:
      double m_kETolerance;
#if G4VERSION_NUMBER >= 1130
      int m_kESeverity;
      std::map<int, std::string> m_sevMap = {
        {0,   "FatalException"},
        {1,   "FatalErrorInArgument"},
        {2,   "RunMustBeAborted"},
        {3,   "EventMustBeAborted"},
        {4,   "JustWarning"},
        {5,   "IgnoreTheIssue"}};
#endif
    };
  }
}
#endif   // DDG4_Geant4SetKETolerance_h

#include <DDG4/Factories.h>
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4SetKETolerance)
