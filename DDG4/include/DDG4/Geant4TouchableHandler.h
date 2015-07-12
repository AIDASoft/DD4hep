// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_GEANT4TOUCHABLEHANDLER_H
#define DD4HEP_GEANT4TOUCHABLEHANDLER_H

// C/C++ include files
#include <vector>
#include <string>

// Forward declarations
class G4VPhysicalVolume;
class G4VTouchable;
class G4Step;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

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

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4TOUCHABLEHANDLER_H
