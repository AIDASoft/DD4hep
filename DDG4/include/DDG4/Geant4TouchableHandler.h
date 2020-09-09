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
#ifndef DDG4_GEANT4TOUCHABLEHANDLER_H
#define DDG4_GEANT4TOUCHABLEHANDLER_H

// C/C++ include files
#include <vector>
#include <string>

// Forward declarations
class G4VPhysicalVolume;
class G4VTouchable;
class G4Step;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations;
    class Geant4TouchableHandler;

    /// Helper class to ease the extraction of information from a G4Touchable object.
    /**
     * Tiny helper/utility class to easily access Geant4 touchable information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4TouchableHandler {
    public:

      typedef std::vector<const G4VPhysicalVolume*> Geant4PlacementPath;
      /// Data member of the helper objects
      const G4VTouchable* touchable;

      /// Default constructor.
      Geant4TouchableHandler(const G4VTouchable* t) : touchable(t) {}
      /// Default constructor. Takes the step's pre-touchable
      Geant4TouchableHandler(const G4Step* step);
      /// Default constructor. Takes the step's pre-touchable
      Geant4TouchableHandler(const G4Step* step, bool use_post_step_point);

      /// Helper: Generate placement path from touchable object
      Geant4PlacementPath placementPath(bool exception=false) const;

      /// Helper: Access the placement path of a Geant4 touchable object as a string
      std::string path()  const;
      /// Touchable history depth
      int depth() const;
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4TOUCHABLEHANDLER_H
