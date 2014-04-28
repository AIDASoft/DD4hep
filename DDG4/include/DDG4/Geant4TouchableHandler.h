// $Id: Geant4TouchableHandler.h 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4TOUCHABLEHANDLER_H
#define DD4HEP_GEANT4TOUCHABLEHANDLER_H

// C/C++ include files
#include <vector>
#include <string>

// Forward declarations
class G4VPhysicalVolume;
class G4VTouchable;
class G4Step;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations;
    class Geant4TouchableHandler;

    /** @class Geant4TouchableHandler Geant4SensitiveDetector.h DDG4/Geant4SensitiveDetector.h
     *
     * Tiny helper/utility class to easily access Geant4 touchable information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TouchableHandler {
    public:
      
      typedef std::vector<const G4VPhysicalVolume*> Geant4PlacementPath;

      const G4VTouchable* touchable;

      /// Default constructor.
      Geant4TouchableHandler(const G4VTouchable* t) : touchable(t) {}
      /// Default constructor. Takes the step's pre-touchable
      Geant4TouchableHandler(const G4Step* step);

      /// Helper: Generate placement path from touchable object
      Geant4PlacementPath placementPath(bool exception=false) const;

      /// Helper: Access the placement path of a Geant4 touchable object as a string
      std::string path()  const;

    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4TOUCHABLEHANDLER_H
