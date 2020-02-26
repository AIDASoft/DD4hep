//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//==========================================================================
#ifndef DD4HEP_DDG4_HEPMC3EVENTREADER_H
#define DD4HEP_DDG4_HEPMC3EVENTREADER_H

// Framework include files
#include "DDG4/Geant4InputAction.h"

namespace HepMC3{ class GenEvent; }

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Base class to read hepmc3 files.
    /**
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     * @property: Parameters.Flow1
     * @property: Parameters.Flow2
     */
    class HEPMC3EventReader : public Geant4EventReader  {
    public:
      /// Initializing constructor
      explicit HEPMC3EventReader(const std::string& fileName);
      /// Default destructor
      virtual ~HEPMC3EventReader() = default;

      /// Read an event and fill a vector of Particles and vertices
      virtual EventReaderStatus readParticles(int event_number, Vertices& vertices, Particles& particles);
      /// Read an event
      virtual EventReaderStatus readGenEvent(int event_number, HepMC3::GenEvent& genEvent) = 0;

    protected:
      /// name of the GenEvent Attribute storing the color flow1
      std::string m_flow1 = "flow1";
      /// name of the GenEvent Attribute storing the color flow2
      std::string m_flow2 = "flow2";

    };

  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif  /* DD4HEP_DDG4_HEPMC3EVENTREADER_H  */
