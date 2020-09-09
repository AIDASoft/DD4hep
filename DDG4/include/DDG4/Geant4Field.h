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

#ifndef DDG4_GEANT4FIELD_H
#define DDG4_GEANT4FIELD_H

// Framework include files
#include "DD4hep/Detector.h"

// Geant 4 include files
#include "G4ElectroMagneticField.hh"
#include "G4MagneticField.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4Field;

    /// Mediator class to allow Geant4 accessing magnetic fields defined in dd4hep
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Field : public G4MagneticField {
    protected:
      /// Reference to the detector description field
      OverlayedField m_field;

    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4Field(OverlayedField field) : m_field(field) {   }
      /// Standard destructor
      virtual ~Geant4Field() {    }
      /// Access field values at a given point
      virtual void GetFieldValue(const double pos[4], double *arr) const;
      /// Does field change energy ?
      virtual G4bool DoesFieldChangeEnergy() const;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4FIELD_H
