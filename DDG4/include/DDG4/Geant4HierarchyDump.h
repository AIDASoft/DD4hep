//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H
#define DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H

// Geant 4 include files
#include "G4VPhysicalVolume.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
  }

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    class Geant4HierarchyDump {
    public:
      typedef Geometry::LCDD LCDD;
      LCDD& m_lcdd;

    public:
      /// Initializing Constructor
      Geant4HierarchyDump(LCDD& lcdd);
      /// Standard destructor
      virtual ~Geant4HierarchyDump();
      /// Dump the volume hierarchy as it is known to geant 4
      virtual void dump(const std::string& indent, const G4VPhysicalVolume* vol) const;
    };
  }
}

#endif  // DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H
