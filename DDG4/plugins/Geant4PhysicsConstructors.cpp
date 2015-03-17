// $Id: Factories.h 797 2013-10-03 19:20:32Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//-----------------------------------------------------------------------------------
//
//  Author     : M.Frank
//
//===================================================================================

// Framework include files
#include "DDG4/Factories.h"

// ==================================================================================
// Predefined physics constructors
//
//  Usage:
//
//  <physicslist name="Geant4PhysicsList/MyPhysics.0">
//    <physics>
//      <construct name="G4EmStandardPhysics"/>
//      <construct name="HadronPhysicsQGSP"/>
//    </physics>
//  </physicslist>
//
// ==================================================================================

// Geant4 physics lists
#include "G4Version.hh"
#include "G4DecayPhysics.hh"

// ==================================================================================
#if G4VERSION_NUMBER>=960 and G4VERSION_NUMBER<1000
  #include "Geant4.9.PhysicsConstructors.h"
// ==================================================================================
#elif G4VERSION_NUMBER>=1000
  #include "Geant4.10.PhysicsConstructors.h"
// ==================================================================================
#else
  #error "Unsupported Geant4 version "
#endif  /*  G4VERSION_NUMBER>=1000  */
