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

/** \addtogroup Geant4GeneratorAction
 *
 @{
   \package Geant4InputAction
 * \brief Basic geant4 event reader class. This interface/base-class must be implemented by concrete readers.
 *
 *
@}
 */

#ifndef DDG4_GEANT4INPUTACTION_H
#define DDG4_GEANT4INPUTACTION_H

// Framework include files
#include "DDG4/Geant4Vertex.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "Parsers/Parsers.h"

// C/C++ include files
#include <vector>
#include <memory>

// Forward declarations
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {
    
    class Geant4InputAction;

    /// Basic geant4 event reader class. This interface/base-class must be implemented by concrete readers.
    /**
     * Base class to read input files containing simulation data.
     *
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \author  R.Ete    (added context from input action)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4EventReader  {

    public:
      typedef Geant4Vertex   Vertex;
      typedef Geant4Particle Particle;
      typedef std::vector<Particle*> Particles;
      typedef std::vector<Vertex*> Vertices;
      /// Status codes of the event reader object. Anything with NOT low-bit set is an error.
      enum EventReaderStatus {
        EVENT_READER_ERROR=0,
        EVENT_READER_OK=1,
        EVENT_READER_NO_DIRECT=2,
        EVENT_READER_NO_PRIMARIES=4,
        EVENT_READER_NO_FACTORY=6,
        EVENT_READER_IO_ERROR=8,
        EVENT_READER_EOF=10
      };
    protected:
      /// File name to be opened and read
      std::string m_name;
      /// Flag if direct event access is supported. To be explicitly set by subclass constructors
      bool m_directAccess;
      /// Current event number
      int  m_currEvent;
      /// The input action context
      Geant4InputAction *m_inputAction;

      /// transform the string parameter value into the type of parameter
      /**
       * removes parameter from the parameters map
       */
      template <typename T>
      void _getParameterValue( std::map< std::string, std::string > & parameters,
                               std::string const& parameterName,
                               T& parameter, T defaultValue ) {

        if( parameters.find( parameterName ) != parameters.end() ) {
          dd4hep::Parsers::parse( parameter, parameters.at( parameterName ) );
          parameters.erase( parameterName );
        } else {
          parameter = defaultValue;
        }
      }

    public:
      /// Initializing constructor
      Geant4EventReader(const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventReader();
      /// Get the context (from the input action)
      Geant4Context* context() const;
      /// Set the input action
      void setInputAction(Geant4InputAction* action);
      /// File name
      const std::string& name()  const   {  return m_name;         }
      /// Flag if direct event access (by event sequence number) is supported (Default: false)
      bool hasDirectAccess() const       {  return m_directAccess; }
      /// return current Event Number
      int currentEventNumber() const     {  return m_currEvent;    }
      /// Move to the indicated event number.
      /** For pure sequential access, the default implementation
       *  will skip events one by one.
       *  For technologies supporting direct event access the default
       *  implementation will be empty.
       *
       *  @return
       */
      virtual EventReaderStatus moveToEvent(int event_number);
      /// Skip event. To be implemented for sequential sources
      virtual EventReaderStatus skipEvent();
      /// Read an event and fill a vector of MCParticles.
      /** The additional argument
       */
      virtual EventReaderStatus readParticles(int event_number, 
                                              Vertices&  vertices,
                                              Particles& particles) = 0;

      /// pass parameters to the event reader object
      virtual EventReaderStatus setParameters( std::map< std::string, std::string > & ) {return EVENT_READER_OK; }

      /// make sure that all parameters have been processed, otherwise throw exceptions
      virtual void checkParameters( std::map< std::string, std::string >& );
    };

    /// Generic input action capable of using the Geant4EventReader class.
    /**
     * Concrete implementation of the Geant4 generator action base class
     * populating Geant4 primaries from Geant4 and HepStd files.
     *
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4InputAction : public Geant4GeneratorAction {

    public:
      typedef Geant4Vertex   Vertex;
      typedef Geant4Particle Particle;
      typedef std::vector<Particle*> Particles;
      typedef std::vector<Vertex*> Vertices;
    protected:
      /// Property: input file
      std::string         m_input;
      /// Property: SYNCEVT
      int                 m_firstEvent;
      /// Property; interaction mask
      int                 m_mask;
      /// Property: Momentum downscaler for debugging
      double              m_momScale;
      /// Event reader object
      Geant4EventReader*  m_reader;
      /// current event number without initially skipped events
      int m_currentEventNumber;
      /// Flag to call abortEvent in case of failure (default: true)
      bool m_abort;
      /// Property: named parameters to configure file readers or input actions
      std::map< std::string, std::string> m_parameters;

    public:
      /// Read an event and return a LCCollectionVec of MCParticles.
      int readParticles(int event_number,
                        Vertices&  vertices,
                        Particles& particles);
      /// helper to report Geant4 exceptions
      std::string issue(int i) const;

      /// Standard constructor
      Geant4InputAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4InputAction();
      /// Create particle vector
      Particles* new_particles() const { return new Particles; }
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_GEANT4INPUTACTION_H
