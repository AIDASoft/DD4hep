// $Id: Factories.h 797 2013-10-03 19:20:32Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Factories.h"

// Geant4 physics lists
#include "G4Version.hh"
#include "G4DecayPhysics.hh"

#if G4VERSION_NUMBER>=960
#define GEANT4_9_6
#endif

// ======================================================================
// Predefined physics lists
//
//  Usage:
//
//  <physicslist name="Geant4PhysicsList/MyPhysics.0">
//    <list name="TQGSP_FTFP_BERT_95"/>
//  </physicslist>
//
//
// ======================================================================
#include "DDG4/Geant4UserPhysicsList.h"
namespace {
  struct EmptyPhysics : public G4VModularPhysicsList {
    EmptyPhysics(int) {}
    virtual ~EmptyPhysics() {}
    virtual void ConstructProcess()      {}
    virtual void ConstructParticle()      {}
  };
}
DECLARE_GEANT4_PHYSICS_LIST(EmptyPhysics)

// Physics constructors from source/physics_lists
#include "CHIPS.hh"
DECLARE_GEANT4_PHYSICS_LIST(CHIPS)

#ifdef GEANT4_9_6
#include "CHIPS_HP.hh"
DECLARE_GEANT4_PHYSICS_LIST(CHIPS_HP)
#include "FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT)
#include "FTFP_BERT_HP.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_HP)
#include "QGSP_BERT_95.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_95)
#include "QGSP_BERT_95XS.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_95XS)
#include "QGSP_INCLXX.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_INCLXX)

#endif

#include "FTF_BIC.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTF_BIC)
#include "FTFP_BERT_EMV.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_EMV)
#include "FTFP_BERT_EMX.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_EMX)
#include "FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT)
#include "FTFP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_TRV)
#include "LBE.hh"
//DECLARE_GEANT4_PHYSICS_LIST(LBE) takes no verbosity arg!
#include "LHEP_EMV.hh"
DECLARE_GEANT4_PHYSICS_LIST(LHEP_EMV)
#include "LHEP.hh"
DECLARE_GEANT4_PHYSICS_LIST(LHEP)
#include "QBBC.hh"
DECLARE_GEANT4_PHYSICS_LIST(QBBC)
#include "QGS_BIC.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGS_BIC)
#include "QGSC_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSC_BERT)
#include "QGSC_CHIPS.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSC_CHIPS)
#include "QGSP_BERT_CHIPS.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_CHIPS)
#include "QGSP_BERT_EMV.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_EMV)
#include "QGSP_BERT_EMX.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_EMX)
#include "QGSP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT)
#include "QGSP_BERT_NOLEP.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_NOLEP)
#include "QGSP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_TRV)
#include "QGSP_BIC_EMY.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BIC_EMY)
#include "QGSP_BIC_HP.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BIC_HP)
#include "QGSP_BIC.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BIC)
#include "QGSP_FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_FTFP_BERT)
#include "QGSP.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP)
#include "QGSP_QEL.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_QEL)
#include "HadronPhysicsCHIPS.hh"

#if 0
#include ".hh"
DECLARE_GEANT4_PHYSICS_LIST()
#endif
