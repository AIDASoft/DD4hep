// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4FIELD_H
#define DD4HEP_DDG4_GEANT4FIELD_H

// Framework include files
#include "DD4hep/LCDD.h"

// Geant 4 include files
#include "G4ElectroMagneticField.hh"
#include "G4MagneticField.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4Field;

    /** @class Geant4Field Geant4Field.h DDG4/Geant4Field.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Field : public G4MagneticField   {
      /// Reference to the detector description field
      Geometry::OverlayedField  m_field;

      public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4Field(Geometry::OverlayedField field) : m_field(field) {}
      /// Standard destructor
      virtual ~Geant4Field() {}
      /// Access field values at a given point
      virtual void GetFieldValue(const double pos[4], double *arr) const;
      /// Does field change energy ?
      virtual G4bool DoesFieldChangeEnergy() const;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4FIELD_H
