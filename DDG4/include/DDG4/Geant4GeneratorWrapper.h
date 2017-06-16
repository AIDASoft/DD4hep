// $Id$
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
#ifndef DD4HEP_DDG4_GEANT4GENERATORWRAPPER_H
#define DD4HEP_DDG4_GEANT4GENERATORWRAPPER_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

// Forward declarations
class G4VPrimaryGenerator;


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4Action to merge several independent interaction to one
    /** Wrap native G4 particle ganerators like the generic particle source etc.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GeneratorWrapper : public Geant4GeneratorAction    {
    protected:
      /// Property: Type name of the implementation instance. name: "Uses"
      std::string m_generatorType;
      /// Property: interaction identifier mask. name: "Mask"
      int m_mask;

      /// Reference to the implementation instance
      G4VPrimaryGenerator* m_generator;
      
    public:
      /// Standard constructor
      Geant4GeneratorWrapper(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4GeneratorWrapper();
      /// Access the G4VPrimaryGenerator instance
      G4VPrimaryGenerator* generator();
      /// Event generation action callback
      virtual void operator()(G4Event* event);
    };
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4GENERATORWRAPPER_H
