//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : R.Ete
//
//==========================================================================
#ifndef DD4HEP_DDG4_LCIOEVENTPARAMETERS_H
#define DD4HEP_DDG4_LCIOEVENTPARAMETERS_H

// lcio include files
#include "lcio.h"
#include "IMPL/LCParametersImpl.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Event extension to pass input LCIO event data to output LCIO event
    /**
     *  \author  R.Ete (main author)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOEventParameters  {
    private:
      int                     m_runNumber;
      int                     m_eventNumber;
      IMPL::LCParametersImpl  m_eventParameters;
      
    public:
      /// Initializing constructor
      LCIOEventParameters();
      /// Default destructor
      ~LCIOEventParameters();

      /// Set the event parameters
      void setParameters(int runNumber, int eventNumber, const EVENT::LCParameters& parameters);
      /// Get the run number
      int runNumber() const;
      /// Get the event number
      int eventNumber() const;
      /// Get the event parameters
      const IMPL::LCParametersImpl& eventParameters() const;
      
      /// Copy the paramaters from source to destination
      static void copyLCParameters(const EVENT::LCParameters& source, EVENT::LCParameters& destination);
    };

  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif  /* DD4HEP_DDG4_LCIOEVENTPARAMETERS_H  */
