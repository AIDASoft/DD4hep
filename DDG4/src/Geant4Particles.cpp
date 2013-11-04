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

// ---- Adjoint particles
#include "G4AdjointAlpha.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointAlpha)
#include "G4AdjointDeuteron.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointDeuteron)
#include "G4AdjointElectron.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointElectron)
#include "G4AdjointGamma.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointGamma)
#include "G4AdjointHe3.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointHe3)
#include "G4AdjointPositron.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointPositron)
#include "G4AdjointProton.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointProton)
#include "G4AdjointTriton.hh"
DECLARE_GEANT4_PARTICLE(G4AdjointTriton)

// ---- bosons
#include "G4Geantino.hh"
DECLARE_GEANT4_PARTICLE(G4Geantino)
#include "G4ChargedGeantino.hh"
DECLARE_GEANT4_PARTICLE(G4ChargedGeantino)
#include "G4Gamma.hh"
DECLARE_GEANT4_PARTICLE(G4Gamma)
#include "G4OpticalPhoton.hh"
DECLARE_GEANT4_PARTICLE(G4OpticalPhoton)
#include "G4BosonConstructor.hh"
DECLARE_GEANT4_PARTICLEGROUP(G4BosonConstructor)

// ---- barions
#include "G4XibZero.hh"
DECLARE_GEANT4_PARTICLE(G4XibZero)
#include "G4SigmabPlus.hh"
DECLARE_GEANT4_PARTICLE(G4SigmabPlus)
#include "G4Lambda.hh"
DECLARE_GEANT4_PARTICLE(G4Lambda)
#include "G4Proton.hh"
DECLARE_GEANT4_PARTICLE(G4Proton)
#include "G4SigmabMinus.hh"
DECLARE_GEANT4_PARTICLE(G4SigmabMinus)
#include "G4AntiXiMinus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiXiMinus)
#include "G4AntiSigmabMinus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmabMinus)
#include "G4XiMinus.hh"
DECLARE_GEANT4_PARTICLE(G4XiMinus)
#include "G4LambdacPlus.hh"
DECLARE_GEANT4_PARTICLE(G4LambdacPlus)
#include "G4AntiOmegabMinus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiOmegabMinus)
#include "G4Neutron.hh"
DECLARE_GEANT4_PARTICLE(G4Neutron)
#include "G4AntiXibZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiXibZero)
#include "G4OmegacZero.hh"
DECLARE_GEANT4_PARTICLE(G4OmegacZero)
#include "G4SigmacPlus.hh"
DECLARE_GEANT4_PARTICLE(G4SigmacPlus)
#include "G4AntiXibMinus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiXibMinus)
#include "G4Lambdab.hh"
DECLARE_GEANT4_PARTICLE(G4Lambdab)
#include "G4AntiXicZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiXicZero)
#include "G4AntiOmegacZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiOmegacZero)
#include "G4AntiLambdacPlus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiLambdacPlus)
#include "G4AntiSigmaZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmaZero)
#include "G4AntiSigmaPlus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmaPlus)
#include "G4SigmaZero.hh"
DECLARE_GEANT4_PARTICLE(G4SigmaZero)
#include "G4AntiLambda.hh"
DECLARE_GEANT4_PARTICLE(G4AntiLambda)
#include "G4XiZero.hh"
DECLARE_GEANT4_PARTICLE(G4XiZero)
#include "G4AntiSigmabZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmabZero)
#include "G4AntiSigmaMinus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmaMinus)
#include "G4AntiProton.hh"
DECLARE_GEANT4_PARTICLE(G4AntiProton)
#include "G4XicPlus.hh"
DECLARE_GEANT4_PARTICLE(G4XicPlus)
#include "G4BaryonConstructor.hh"
DECLARE_GEANT4_PARTICLEGROUP(G4BaryonConstructor)
#include "G4AntiSigmacPlusPlus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmacPlusPlus)
#include "G4XibMinus.hh"
DECLARE_GEANT4_PARTICLE(G4XibMinus)
#include "G4AntiSigmabPlus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmabPlus)
#include "G4AntiSigmacPlus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmacPlus)
#include "G4SigmaMinus.hh"
DECLARE_GEANT4_PARTICLE(G4SigmaMinus)
#include "G4SigmacPlusPlus.hh"
DECLARE_GEANT4_PARTICLE(G4SigmacPlusPlus)
#include "G4AntiXicPlus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiXicPlus)
#include "G4SigmabZero.hh"
DECLARE_GEANT4_PARTICLE(G4SigmabZero)
#include "G4AntiXiZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiXiZero)
#include "G4AntiOmegaMinus.hh"
DECLARE_GEANT4_PARTICLE(G4AntiOmegaMinus)
#include "G4OmegabMinus.hh"
DECLARE_GEANT4_PARTICLE(G4OmegabMinus)
#include "G4OmegaMinus.hh"
DECLARE_GEANT4_PARTICLE(G4OmegaMinus)
#include "G4AntiSigmacZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiSigmacZero)
#include "G4AntiNeutron.hh"
DECLARE_GEANT4_PARTICLE(G4AntiNeutron)
#include "G4SigmaPlus.hh"
DECLARE_GEANT4_PARTICLE(G4SigmaPlus)
#include "G4AntiLambdab.hh"
DECLARE_GEANT4_PARTICLE(G4AntiLambdab)
#include "G4XicZero.hh"
DECLARE_GEANT4_PARTICLE(G4XicZero)
#include "G4SigmacZero.hh"
DECLARE_GEANT4_PARTICLE(G4SigmacZero)

// ---- ions
#include "G4AntiHe3.hh"
DECLARE_GEANT4_PARTICLE(G4AntiHe3)
#include "G4Deuteron.hh"
DECLARE_GEANT4_PARTICLE(G4Deuteron)
#include "G4He3.hh"
DECLARE_GEANT4_PARTICLE(G4He3)
#include "G4IonConstructor.hh"
DECLARE_GEANT4_PARTICLEGROUP(G4IonConstructor)
#include "G4Alpha.hh"
DECLARE_GEANT4_PARTICLE(G4Alpha)
#include "G4AntiAlpha.hh"
DECLARE_GEANT4_PARTICLE(G4AntiAlpha)
#include "G4AntiTriton.hh"
DECLARE_GEANT4_PARTICLE(G4AntiTriton)
#include "G4GenericIon.hh"
DECLARE_GEANT4_PARTICLE(G4GenericIon)
#include "G4AntiDeuteron.hh"
DECLARE_GEANT4_PARTICLE(G4AntiDeuteron)
#include "G4Triton.hh"
DECLARE_GEANT4_PARTICLE(G4Triton)

// ---- mesons
#include "G4Eta.hh"
DECLARE_GEANT4_PARTICLE(G4Eta)
#include "G4KaonMinus.hh"
DECLARE_GEANT4_PARTICLE(G4KaonMinus)
#include "G4KaonZeroLong.hh"
DECLARE_GEANT4_PARTICLE(G4KaonZeroLong)
#include "G4DsMesonPlus.hh"
DECLARE_GEANT4_PARTICLE(G4DsMesonPlus)
#include "G4BMesonPlus.hh"
DECLARE_GEANT4_PARTICLE(G4BMesonPlus)
#include "G4PionPlus.hh"
DECLARE_GEANT4_PARTICLE(G4PionPlus)
#include "G4KaonZeroShort.hh"
DECLARE_GEANT4_PARTICLE(G4KaonZeroShort)
#include "G4MesonConstructor.hh"
DECLARE_GEANT4_PARTICLEGROUP(G4MesonConstructor)
#include "G4DsMesonMinus.hh"
DECLARE_GEANT4_PARTICLE(G4DsMesonMinus)
#include "G4AntiBsMesonZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiBsMesonZero)
#include "G4DMesonZero.hh"
DECLARE_GEANT4_PARTICLE(G4DMesonZero)
#include "G4AntiDMesonZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiDMesonZero)
#include "G4BsMesonZero.hh"
DECLARE_GEANT4_PARTICLE(G4BsMesonZero)
#include "G4BMesonMinus.hh"
DECLARE_GEANT4_PARTICLE(G4BMesonMinus)
#include "G4PionZero.hh"
DECLARE_GEANT4_PARTICLE(G4PionZero)
#include "G4KaonPlus.hh"
DECLARE_GEANT4_PARTICLE(G4KaonPlus)
#include "G4DMesonMinus.hh"
DECLARE_GEANT4_PARTICLE(G4DMesonMinus)
#include "G4BcMesonMinus.hh"
DECLARE_GEANT4_PARTICLE(G4BcMesonMinus)
#include "G4BMesonZero.hh"
DECLARE_GEANT4_PARTICLE(G4BMesonZero)
#include "G4AntiKaonZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiKaonZero)
#include "G4EtaPrime.hh"
DECLARE_GEANT4_PARTICLE(G4EtaPrime)
#include "G4Upsiron.hh"
DECLARE_GEANT4_PARTICLE(G4Upsiron)
#include "G4AntiBMesonZero.hh"
DECLARE_GEANT4_PARTICLE(G4AntiBMesonZero)
#include "G4JPsi.hh"
DECLARE_GEANT4_PARTICLE(G4JPsi)
#include "G4KaonZero.hh"
DECLARE_GEANT4_PARTICLE(G4KaonZero)
#include "G4DMesonPlus.hh"
DECLARE_GEANT4_PARTICLE(G4DMesonPlus)
#include "G4PionMinus.hh"
DECLARE_GEANT4_PARTICLE(G4PionMinus)
#include "G4Etac.hh"
DECLARE_GEANT4_PARTICLE(G4Etac)
#include "G4BcMesonPlus.hh"
DECLARE_GEANT4_PARTICLE(G4BcMesonPlus)

// ---- leptons
#include "G4MuonPlus.hh"
DECLARE_GEANT4_PARTICLE(G4MuonPlus)
#include "G4TauPlus.hh"
DECLARE_GEANT4_PARTICLE(G4TauPlus)
#include "G4AntiNeutrinoTau.hh"
DECLARE_GEANT4_PARTICLE(G4AntiNeutrinoTau)
#include "G4MuonMinus.hh"
DECLARE_GEANT4_PARTICLE(G4MuonMinus)
#include "G4NeutrinoE.hh"
DECLARE_GEANT4_PARTICLE(G4NeutrinoE)
#include "G4TauMinus.hh"
DECLARE_GEANT4_PARTICLE(G4TauMinus)
#include "G4Positron.hh"
DECLARE_GEANT4_PARTICLE(G4Positron)
#include "G4AntiNeutrinoMu.hh"
DECLARE_GEANT4_PARTICLE(G4AntiNeutrinoMu)
#include "G4LeptonConstructor.hh"
DECLARE_GEANT4_PARTICLEGROUP(G4LeptonConstructor)
#include "G4NeutrinoMu.hh"
DECLARE_GEANT4_PARTICLE(G4NeutrinoMu)
#include "G4AntiNeutrinoE.hh"
DECLARE_GEANT4_PARTICLE(G4AntiNeutrinoE)
#include "G4Electron.hh"
DECLARE_GEANT4_PARTICLE(G4Electron)
#include "G4NeutrinoTau.hh"
DECLARE_GEANT4_PARTICLE(G4NeutrinoTau)

// ---- shortlived
#include "G4ExcitedXiConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedXiConstructor)
#include "G4ExcitedLambdaConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedLambdaConstructor)
#include "G4ShortLivedConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ShortLivedConstructor)
#include "G4ExcitedSigmaConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedSigmaConstructor)
#include "G4ExcitedBaryons.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedBaryons)
#include "G4DiQuarks.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4DiQuarks)
#include "G4VShortLivedParticle.hh"
//DECLARE_GEANT4_PARTICLE(G4VShortLivedParticle)
#include "G4Quarks.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4Quarks)
#include "G4ExcitedDeltaConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedDeltaConstructor)
#include "G4ExcitedBaryonConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedBaryonConstructor)
#include "G4ExcitedNucleonConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedNucleonConstructor)
#include "G4ExcitedMesonConstructor.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedMesonConstructor)
#include "G4ExcitedMesons.hh"
//DECLARE_GEANT4_PARTICLEGROUP(G4ExcitedMesons)

