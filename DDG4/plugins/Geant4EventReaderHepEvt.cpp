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

/** \addtogroup Geant4EventReader
 *
 @{
  \package Geant4EventReaderHepEvt
 * \brief Plugin to read HepEvt ASCII files
 *
 *
@}
 */


// Framework include files
#include "DDG4/Geant4InputAction.h"

// C/C++ include files
#include <fstream>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to populate Geant4 primaries from StdHep files.
    /**
     * Class to populate Geant4 primary particles and vertices from a
     * file in HEPEvt format (ASCII)
     *
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4EventReaderHepEvt : public Geant4EventReader  {

    protected:
      std::ifstream m_input;
      int           m_format;

    public:
      /// Initializing constructor
      explicit Geant4EventReaderHepEvt(const std::string& nam, int format);
      /// Default destructor
      virtual ~Geant4EventReaderHepEvt();
      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticles(int event_number,
                                              Vertices& vertices,
                                              std::vector<Particle*>& particles);
      virtual EventReaderStatus moveToEvent(int event_number);
      virtual EventReaderStatus skipEvent() { return EVENT_READER_OK; }
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep       */

//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//====================================================================
// #include "DDG4/Geant4EventReaderHepEvt"

// Framework include files
#include "DDG4/Factories.h"
#include "DD4hep/Printout.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// C/C++ include files
#include <cerrno>

using namespace std;
using namespace CLHEP;
using namespace dd4hep::sim;
typedef dd4hep::detail::ReferenceBitMask<int> PropertyMask;

#define HEPEvtShort 1
#define HEPEvtLong  2

// Local declarations in anaonymous namespace
namespace {
  class Geant4EventReaderHepEvtShort : public Geant4EventReaderHepEvt  {
  public:
    /// Initializing constructor
    explicit Geant4EventReaderHepEvtShort(const string& nam) : Geant4EventReaderHepEvt(nam,HEPEvtShort) {}
    /// Default destructor
    virtual ~Geant4EventReaderHepEvtShort() {}
  };
  class Geant4EventReaderHepEvtLong : public Geant4EventReaderHepEvt  {
  public:
    /// Initializing constructor
    explicit Geant4EventReaderHepEvtLong(const string& nam) : Geant4EventReaderHepEvt(nam,HEPEvtLong) {}
    /// Default destructor
    virtual ~Geant4EventReaderHepEvtLong() {}
  };
}

// Factory entry
DECLARE_GEANT4_EVENT_READER(Geant4EventReaderHepEvtShort)
// Factory entry
DECLARE_GEANT4_EVENT_READER(Geant4EventReaderHepEvtLong)


/// Initializing constructor
Geant4EventReaderHepEvt::Geant4EventReaderHepEvt(const string& nam, int format)
: Geant4EventReader(nam), m_input(), m_format(format)
{
  // Now open the input file:
  m_input.open(nam.c_str(),ifstream::in);
  if ( !m_input.good() )   {
    string err = "+++ Geant4EventReaderHepEvt: Failed to open input stream:"+nam+
      " Error:"+string(strerror(errno));
    throw runtime_error(err);
  }
}

/// Default destructor
Geant4EventReaderHepEvt::~Geant4EventReaderHepEvt()    {
  m_input.close();
}

/// skipEvents if required
Geant4EventReader::EventReaderStatus
Geant4EventReaderHepEvt::moveToEvent(int event_number) {
  if( m_currEvent == 0 && event_number != 0 ) {
    printout(INFO,"EventReaderHepEvt::moveToEvent","Skipping the first %d events ", event_number );
    printout(INFO,"EventReaderHepEvt::moveToEvent","Event number before skipping: %d", m_currEvent );
    while ( m_currEvent < event_number ) {
      std::vector<Particle*> particles;
      Vertices vertices ;
      EventReaderStatus sc = readParticles(m_currEvent,vertices,particles);
      for_each(vertices.begin(),vertices.end(),detail::deleteObject<Vertex>);
      for_each(particles.begin(),particles.end(),detail::deleteObject<Particle>);
      if ( sc != EVENT_READER_OK ) return sc;
      //Current event is increased in readParticles already!
      // ++m_currEvent;
    }
  }
  printout(INFO,"EventReaderHepEvt::moveToEvent","Event number after skipping: %d", m_currEvent );
  return EVENT_READER_OK;
}

/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
Geant4EventReaderHepEvt::readParticles(int /* event_number */, 
                                       Vertices& vertices,
                                       vector<Particle*>& particles)   {


  // First check the input file status
  if ( m_input.eof() )   {
    return EVENT_READER_EOF;
  }
  else if ( !m_input.good() )   {
    return EVENT_READER_IO_ERROR;
  }
  //static const double c_light = 299.792;// mm/ns
  //
  //  Read the event, check for errors
  //
  unsigned NHEP(0);  // number of entries
  m_input >> NHEP;

  if( m_input.eof() )  { return EVENT_READER_EOF; }


  //check loop variable read from input file and chack that is reasonable
  // should fix coverity issue: "Using tainted variable NHEP as a loop boundary."

  if( NHEP > 1e6 ){ 
    printout(ERROR,"EventReaderHepEvt::readParticles","Cannot read in more than million particles, but  %d requested", NHEP );
    return EVENT_READER_EOF; 
  }

  //
  //  Loop over particles
  int ISTHEP(0);   // status code
  int IDHEP(0);    // PDG code
  int JMOHEP1(0);  // first mother
  int JMOHEP2(0);  // last mother
  int JDAHEP1(0);  // first daughter
  int JDAHEP2(0);  // last daughter
  double PHEP1(0); // px in GeV/c
  double PHEP2(0); // py in GeV/c
  double PHEP3(0); // pz in GeV/c
  double PHEP4(0); // energy in GeV
  double PHEP5(0); // mass in GeV/c**2
  double VHEP1(0); // x vertex position in mm
  double VHEP2(0); // y vertex position in mm
  double VHEP3(0); // z vertex position in mm
  double VHEP4(0); // production time in mm/c

  vector<int> daughter1;
  vector<int> daughter2;

  for( unsigned IHEP=0; IHEP<NHEP; IHEP++ )    {
    if ( m_format == HEPEvtShort )
      m_input >> ISTHEP >> IDHEP >> JDAHEP1 >> JDAHEP2
              >> PHEP1 >> PHEP2 >> PHEP3 >> PHEP5;
    else
      m_input >> ISTHEP >> IDHEP
              >> JMOHEP1 >> JMOHEP2
              >> JDAHEP1 >> JDAHEP2
              >> PHEP1 >> PHEP2 >> PHEP3
              >> PHEP4 >> PHEP5
              >> VHEP1 >> VHEP2 >> VHEP3
              >> VHEP4;

    if(m_input.eof())
      return EVENT_READER_EOF;

    if(! m_input.good())
      return EVENT_READER_IO_ERROR;

    //
    //  create a MCParticle and fill it from stdhep info
    Particle* p = new Particle(IHEP);
    PropertyMask status(p->status);
    //
    //  PDGID
    p->pdgID = IDHEP;
    //
    //  Momentum vector
    p->pex = p->psx = PHEP1*CLHEP::GeV;
    p->pey = p->psy = PHEP2*CLHEP::GeV;
    p->pez = p->psz = PHEP3*CLHEP::GeV;
    //
    //  Mass
    p->mass = PHEP5*CLHEP::GeV;
    //
    //  Vertex
    p->vsx = VHEP1*CLHEP::mm;
    p->vsy = VHEP2*CLHEP::mm;
    p->vsz = VHEP3*CLHEP::mm;
    // endpoint (missing information in HEPEvt files)
    p->vex = 0.0;
    p->vey = 0.0;
    p->vez = 0.0;
    //
    //  Creation time (note the units [1/c_light])
    p->time       = VHEP4*CLHEP::ns;
    p->properTime = VHEP4*CLHEP::ns;
    //
    //  Generator status
    //  Simulator status 0 until simulator acts on it
    p->status = 0;
    if ( ISTHEP == 0 )      status.set(G4PARTICLE_GEN_EMPTY);
    else if ( ISTHEP == 1 ) status.set(G4PARTICLE_GEN_STABLE);
    else if ( ISTHEP == 2 ) status.set(G4PARTICLE_GEN_DECAYED);
    else if ( ISTHEP == 3 ) status.set(G4PARTICLE_GEN_DOCUMENTATION);
    else                    status.set(G4PARTICLE_GEN_DOCUMENTATION);
    //  RAW Generator status
    p->genStatus = ISTHEP&G4PARTICLE_GEN_STATUS_MASK;

    //
    // Keep daughters information for later
    daughter1.emplace_back(JDAHEP1);
    daughter2.emplace_back(JDAHEP2);
    //
    //  Add the particle to the collection vector
    particles.emplace_back(p);
    //
  }// End loop over particles

  //
  //  Now make a second loop over the particles, checking the daughter
  //  information. This is not always consistent with parent
  //  information, and this utility assumes all parents listed are
  //  parents and all daughters listed are daughters
  //
  for( unsigned IHEP=0; IHEP<NHEP; IHEP++ )    {
    struct ParticleHandler  {
      Particle* m_part;
      ParticleHandler(Particle* p) : m_part(p) {}
      void addParent(const Particle* p)  {
        m_part->parents.insert(p->id);
      }
      void addDaughter(const Particle* p)  {
        if(m_part->daughters.find(p->id) == m_part->daughters.end()) {
          m_part->daughters.insert(p->id);
        }
      }
      Particle* findParent(const Particle* p)  {
        return m_part->parents.find(p->id)==m_part->parents.end() ? 0 : m_part;
      }
    };

    //
    //  Get the MCParticle
    //
    Particle* mcp = particles[IHEP];
    ParticleHandler theParticle(mcp);
    //
    //  Get the daughter information, discarding extra information
    //  sometimes stored in daughter variables.
    //
    int fd = daughter1[IHEP] - 1;
    int ld = daughter2[IHEP] - 1;

    //
    //  As with the parents, look for range, 2 discreet or 1 discreet
    //  daughter.
    if( (fd > -1) && (ld > -1) )  {
      if(ld >= fd)   {
        for(int id=fd;id<ld+1;id++)   {
          //
          //  Get the daughter, and see if it already lists this particle as
          //  a parent. If not, add this particle as a parent
          //
          ParticleHandler part(particles[id]);
          if ( !part.findParent(mcp) ) part.addParent(mcp);
          theParticle.addDaughter(particles[id]);
        }
      }
      else  {   //  Same logic, discrete cases
        ParticleHandler part_fd(particles[fd]);
        if ( !part_fd.findParent(mcp) ) part_fd.addParent(mcp);
        theParticle.addDaughter(particles[fd]);

        ParticleHandler part_ld(particles[ld]);
        if ( !part_ld.findParent(mcp) ) part_ld.addParent(mcp);
        theParticle.addDaughter(particles[ld]);
      }
    }
    else if(fd > -1)      {
      ParticleHandler part(particles[fd]);
      if ( !part.findParent(mcp) ) part.addParent(mcp);
    }
    else if(ld > -1)      {
      ParticleHandler part(particles[ld]);
      if ( !part.findParent(mcp) ) part.addParent(mcp);
    }
  }  // End second loop over particles


  //fg: we simply add all particles without parents as with their own vertex.
  //    This might include the incoming beam particles, e.g. in
  //    the case of lcio files written with Whizard2, which is slightly odd,
  //    however should be treated correctly in Geant4InputHandling.cpp.
  //    We no longer make an attempt to identify the incoming particles
  //    based on the generator status, as this varies widely with different
  //    generators.

  for(size_t i=0; i<particles.size(); ++i )   {
    Geant4ParticleHandle p(particles[i]);
    if ( p->parents.size() == 0 )  {
      Geant4Vertex* vtx = new Geant4Vertex ;
      vertices.emplace_back( vtx );
      vtx->x = p->vsx;
      vtx->y = p->vsy;
      vtx->z = p->vsz;
      vtx->time = p->time;

      vtx->out.insert(p->id) ;
    }
  }

  ++m_currEvent;
  return EVENT_READER_OK;
}

