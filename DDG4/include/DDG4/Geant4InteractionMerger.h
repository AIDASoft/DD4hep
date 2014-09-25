// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4INTERACTIONMERGER_H
#define DD4HEP_DDG4_GEANT4INTERACTIONMERGER_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4PrimaryInteraction;

    /// Geant4Action to merge several independent interaction to one
    /** Geant4Action to convert the particle information to Geant4
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class Geant4InteractionMerger : public Geant4GeneratorAction    {
      /// Append input interaction to global output
      void appendInteraction(Geant4PrimaryInteraction* output, Geant4PrimaryInteraction* input);
    public:
      /// Standard constructor
      Geant4InteractionMerger(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4InteractionMerger();
      /// Event generation action callback
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4INTERACTIONMERGER_H
