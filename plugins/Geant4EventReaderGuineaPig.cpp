// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDG4/Geant4InputAction.h"

// C/C++ include files
#include <fstream>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /**
     *  Reader for ascii files with e+e- pairs created from GuineaPig.
     *  Will read complete the file into one event - unless skip N events is
     *  called, then N particles are compiled into one event.
     * 
     *  \author  F.Gaede, DESY
     *  \author  A. Perez Perez IPHC
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4EventReaderGuineaPig : public Geant4EventReader  {

    protected:
      std::ifstream m_input;
      int m_part_num ;
      
    public:
      /// Initializing constructor
      explicit Geant4EventReaderGuineaPig(const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventReaderGuineaPig();
      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticles(int event_number,
                                              Vertices& vertices,
                                              std::vector<Particle*>& particles) override ;
      virtual EventReaderStatus moveToEvent(int event_number) override ;
      virtual EventReaderStatus skipEvent() override  { return EVENT_READER_OK; }
      virtual EventReaderStatus setParameters( std::map< std::string, std::string > & parameters ) override ;
    };
  }     /* End namespace Simulation   */
}       /* End namespace DD4hep       */


// Framework include files
#include "DDG4/Factories.h"
#include "DD4hep/Printout.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// C/C++ include files
#include <cerrno>

using namespace std;
using namespace CLHEP;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

// Factory entry
DECLARE_GEANT4_EVENT_READER(Geant4EventReaderGuineaPig)

/// Initializing constructor
Geant4EventReaderGuineaPig::Geant4EventReaderGuineaPig(const string& nam)
: Geant4EventReader(nam), m_input(), m_part_num(-1) 
{
  // Now open the input file:
  m_input.open(nam.c_str(),ifstream::in);
  if ( !m_input.good() )   {
    string err = "+++ Geant4EventReaderGuineaPig: Failed to open input stream:"+nam+
      " Error:"+string(strerror(errno));
    throw runtime_error(err);
  }
}

/// Default destructor
Geant4EventReaderGuineaPig::~Geant4EventReaderGuineaPig()    {
  m_input.close();
}


Geant4EventReader::EventReaderStatus
Geant4EventReaderGuineaPig::setParameters( std::map< std::string, std::string > & parameters ) {

  _getParameterValue( parameters, "ParticlesPerEvent", m_part_num, -1);
  
  if( m_part_num <  0 ) 
    printout(INFO,"EventReaderGuineaPig::setParameters"," --- Will read all particles in pairs file into one event " );
  else
    printout(INFO,"EventReaderGuineaPig::setParameters"," --- Will read %d particles per event from pairs file ", m_part_num );
    
  return EVENT_READER_OK;
}

Geant4EventReader::EventReaderStatus
Geant4EventReaderGuineaPig::moveToEvent(int event_number) {
  
  printout(DEBUG,"EventReaderGuineaPig::moveToEvent"," event_number: %d , m_currEvent %d",
	   event_number,m_currEvent ) ;
  
  if( m_currEvent == 0 && event_number > 0 ){

    printout(INFO,"EventReaderGuineaPig::moveToEvent"," --- Cannot skip to event %d in GuineaPig files - ignored ! ", event_number ); 
  }
  // else: nothing to do ...

  return EVENT_READER_OK;
}

/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
Geant4EventReaderGuineaPig::readParticles(int /* event_number */, 
					  Vertices& vertices,
					  vector<Particle*>& particles)   {


  // if no number of particles per event set, we will read the whole file
  if ( m_part_num < 0 )
    m_part_num = std::numeric_limits<int>::max() ; 



  // First check the input file status
  if ( !m_input.good() || m_input.eof() )   {
    return EVENT_READER_IO_ERROR;
  }
  
  double Energy;
  double betaX;
  double betaY;
  double betaZ;
  double posX;
  double posY;
  double posZ;

  //  Loop over particles
  for( int counter = 0; counter < m_part_num ; ++counter ){      

    
    m_input >> Energy
	    >> betaX   >> betaY >> betaZ
	    >> posX    >> posY  >> posZ ;

    if( m_input.eof() ) {
      
      if( counter==0 ) { 
	return EVENT_READER_IO_ERROR ;  // reading first particle of event failed 

      } else{

	++m_currEvent;
	return EVENT_READER_OK ; // simply EOF
      }
    }
    
    //
    //  Create a MCParticle and fill it from stdhep info
    Particle* p = new Particle(counter);
    PropertyMask status(p->status);

    //  PDGID: If Energy positive (negative) particle is electron (positron)
    p->pdgID  = 11;
    p->charge = -1;
    if(Energy < 0.0) {
      p->pdgID = -11;
      p->charge = +1;
    }

    //  Momentum vector
    p->pex = p->psx = betaX*TMath::Abs(Energy)*GeV;
    p->pey = p->psy = betaY*TMath::Abs(Energy)*GeV;
    p->pez = p->psz = betaZ*TMath::Abs(Energy)*GeV;

    //  Mass
    p->mass = 0.0005109989461*GeV;
    //


    //  Creation time (note the units [1/c_light])
    // ( not information in GuineaPig files )
    p->time       = 0.0;
    p->properTime = 0.0;


    //  Vertex
    p->vsx = posX*nm;
    p->vsy = posY*nm;
    p->vsz = posZ*nm;

    Vertex* vtx = new Vertex ;
    vtx->x = p->vsx ;
    vtx->y = p->vsy ;
    vtx->z = p->vsz ;
    vtx->time = p->time ;

    vtx->out.insert( p->id ); 

    //
    //  Generator status
    //  Simulator status 0 until simulator acts on it
    p->status = 0;
    status.set(G4PARTICLE_GEN_STABLE);


    //  Add the particle to the collection vector
    particles.push_back(p);

    // create a new vertex for this particle
    vertices.push_back( vtx) ;

    //    counter++;
  } // End loop over particles

  ++m_currEvent;

  return EVENT_READER_OK;

}

