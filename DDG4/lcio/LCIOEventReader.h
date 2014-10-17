// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_LCIOEVENTREADER_H
#define DD4HEP_DDG4_LCIOEVENTREADER_H

// Framework include files
#include "DDG4/Geant4InputAction.h"

// Forward declarations
namespace EVENT { class LCCollection;    }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation  {

    /// Base class to read lcio files.
    /** 
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOEventReader : public Geant4EventReader  {
    protected:
      /// Event counter
      int m_numEvent;
    public:
      /// Initializing constructor
      LCIOEventReader(const std::string& nam);
      /// Default destructor
      virtual ~LCIOEventReader();
      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticles(int event_number, std::vector<Particle*>& particles);
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual EventReaderStatus readParticles(int event_number, EVENT::LCCollection** particles) = 0;
    };

  }     /* End namespace Simulation   */
}       /* End namespace DD4hep */
#endif  /* DD4HEP_DDG4_LCIOEVENTREADER_H  */
