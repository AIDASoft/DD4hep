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

/** \addtogroup Geant4GeneratorAction
 *
 @{
  \package Geant4PrimaryHandler
 * \brief Geant4Action to convert the particle information to Geant4
 *
 *
@}
 */

#ifndef DDG4_GEANT4PRIMARYHANDLER_H
#define DDG4_GEANT4PRIMARYHANDLER_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    /// Geant4PrimaryConfig to hold configuration for PrimaryHandlers
    class Geant4PrimaryConfig {
    public:
      /// particles with these PDG IDs are not passed to geant for simulation
      std::set<int> m_rejectPDGs = {1, 2, 3, 4, 5, 6, 21, 23, 24, 25};
      /// particles with these PDG IDs are not passed to geant for simulation if their properTime is zero
      std::set<int> m_zeroTimePDGs = {11, 13, 15, 17};

      std::string toString() const {
        std::stringstream str;
        str << "\nRejectPDGs: ";
        for (int i: m_rejectPDGs) { str << i << ", "; }
        str << "\nzeroTimePDGs: ";
        for (int i: m_zeroTimePDGs) { str << i << ", "; }
        return str.str();
      }
    };

    /// Geant4Action to convert the particle information to Geant4
    /** Convert the primary interaction (object \tt{Geant4PrimaryInteraction} object
     *  attached to the \tt{Geant4Event} event context) and pass the result
     *  to Geant4 for simulation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PrimaryHandler : public Geant4GeneratorAction    {
    public:
      /// Standard constructor
      Geant4PrimaryHandler(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PrimaryHandler();
      /// Event generation action callback
      virtual void operator()(G4Event* event);

    public:
      Geant4PrimaryConfig m_primaryConfig{};

    };
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4PRIMARYHANDLER_H
