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

// Framework include files
#include "DDG4/Factories.h"

// Geant 4 include files
#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4ConstRK4.hh"
#include "G4CashKarpRKF45.hh"
#include "G4DormandPrince745.hh"
#include "G4DormandPrinceRK56.hh"
#include "G4DormandPrinceRK78.hh"
#include "G4BogackiShampine23.hh"

DECLARE_GEANT4_STEPPER(ImplicitEuler)
DECLARE_GEANT4_STEPPER(ExplicitEuler)
DECLARE_GEANT4_STEPPER(SimpleRunge)
DECLARE_GEANT4_STEPPER(SimpleHeum)
DECLARE_GEANT4_MAGSTEPPER(ConstRK4)
DECLARE_GEANT4_STEPPER(ClassicalRK4)
DECLARE_GEANT4_STEPPER(CashKarpRKF45)
DECLARE_GEANT4_STEPPER(DormandPrince745)
DECLARE_GEANT4_STEPPER(DormandPrinceRK56)
DECLARE_GEANT4_STEPPER(DormandPrinceRK78)
DECLARE_GEANT4_STEPPER(BogackiShampine23)

#if G4VERSION_NUMBER>=1003
#include "G4DoLoMcPriRK34.hh"
#include "G4BogackiShampine45.hh"
DECLARE_GEANT4_STEPPER(BogackiShampine45)
DECLARE_GEANT4_STEPPER(DoLoMcPriRK34)
#endif

// Geant 4 include files
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixMixedStepper.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4HelixHeum.hh"

DECLARE_GEANT4_MAGSTEPPER(HelixImplicitEuler)
DECLARE_GEANT4_MAGSTEPPER(HelixExplicitEuler)
DECLARE_GEANT4_MAGSTEPPER(HelixMixedStepper)
DECLARE_GEANT4_MAGSTEPPER(HelixSimpleRunge)
DECLARE_GEANT4_MAGSTEPPER(HelixHeum)


// Geant 4 include files
#include "G4Mag_EqRhs.hh"
#include "G4Mag_SpinEqRhs.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4KM_OpticalEqRhs.hh"
#include "G4KM_NucleonEqRhs.hh"
//Virtual: DECLARE_GEANT4_MAGMOTION(Mag_EqRhs);
DECLARE_GEANT4_MAGMOTION(Mag_SpinEqRhs)
DECLARE_GEANT4_MAGMOTION(Mag_UsualEqRhs)
//DECLARE_GEANT4_MAGMOTION(KM_OpticalEqRhs);
//DECLARE_GEANT4_MAGMOTION(KM_NucleonEqRhs);
