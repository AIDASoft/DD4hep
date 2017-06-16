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

#ifndef DD4HEP_DDG4_GEANT4CALL_H
#define DD4HEP_DDG4_GEANT4CALL_H

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Callback interface class with argument
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class  Geant4Call   {
    public:
      /// Default destructor
      virtual ~Geant4Call();
      /// Default callback with argument
      virtual void operator()(void* param) = 0;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4CALL_H
