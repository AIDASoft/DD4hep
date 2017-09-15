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

#ifndef DD4HEP_LCIOParticleExtension_H
#define DD4HEP_LCIOParticleExtension_H

#include "DDG4/Geant4Particle.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {


   /** Simple helper class for additional MCParticle information
    *  used with LCIO readers and writers.
    * 
    *  @author  F.Gaede, DESY
    *  @version 1.0
    *  @ingroup DD4HEP_SIMULATION
    */
    class LCIOParticleExtension : public ParticleExtension{

    public:

      /// original generator status
      int generatorStatus ;

      /// Default constructor
      LCIOParticleExtension() {}

      /// Default destructor
      virtual ~LCIOParticleExtension() {} ;
   
    };
  }
}

#endif
