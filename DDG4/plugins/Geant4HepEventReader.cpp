// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================

// Framework include files
#include "DDG4/Geant4InputAction.h"

// C/C++ include files
#include <fstream>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Class to populate Geant4 primaries from StdHep files.
    /** 
     * Class to populate Geant4 primary particles and vertices from a 
     * file in StdHep format (ASCII)
     *
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HepEventReader : public Geant4EventReader  {
    protected:
      std::ifstream m_input;
      int m_format;

    public:
      /// Initializing constructor
      Geant4HepEventReader(const std::string& nam);
      /// Default destructor
      virtual ~Geant4HepEventReader();
      /// Read an event and fill a vector of MCParticles.
      virtual int readParticles(int event_number, std::vector<Particle*>& particles);
    };
  }     /* End namespace Simulation   */
}       /* End namespace DD4hep       */

// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
// #include "DDG4/Geant4HepEventReader"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cerrno>

using namespace CLHEP;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

#define HEPEvt 1

// Factory entry
DECLARE_GEANT4_EVENT_READER(Geant4HepEventReader)

/// Initializing constructor
Geant4HepEventReader::Geant4HepEventReader(const std::string& nam)
: Geant4EventReader(nam), m_input(), m_format(HEPEvt)
{
  // Need to set format from input specification!!!

  // Now open the input file:
}

/// Default destructor
Geant4HepEventReader::~Geant4HepEventReader()    {
  m_input.close();
}

/// Read an event and fill a vector of MCParticles.
int Geant4HepEventReader::readParticles(int /* event_number */, std::vector<Particle*>& particles)   {
  //static const double c_light = 299.792;// mm/ns
  //
  //  Read the event, check for errors
  //
  int NHEP;  // number of entries
  m_input >> NHEP;
  if( m_input.eof() )   {
    //
    // End of File :: ??? Exception ???
    //   -> FG:   EOF is not an exception as it happens for every file at the end !
    return EIO;
  }
  //
  //  Loop over particles
  int ISTHEP;   // status code
  int IDHEP;    // PDG code
  int JMOHEP1;  // first mother
  int JMOHEP2;  // last mother
  int JDAHEP1;  // first daughter
  int JDAHEP2;  // last daughter
  double PHEP1; // px in GeV/c
  double PHEP2; // py in GeV/c
  double PHEP3; // pz in GeV/c
  double PHEP4; // energy in GeV
  double PHEP5; // mass in GeV/c**2
  double VHEP1; // x vertex position in mm
  double VHEP2; // y vertex position in mm
  double VHEP3; // z vertex position in mm
  double VHEP4; // production time in mm/c
 
  std::vector<int> daughter1;
  std::vector<int> daughter2;
 
  for( int IHEP=0; IHEP<NHEP; IHEP++ )    {
    if ( m_format == HEPEvt)
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
      return EIO;
    //
    //  Create a MCParticle and fill it from stdhep info
    Particle* p = new Particle(IHEP);
    PropertyMask status(p->status);
    //
    //  PDGID
    p->pdgID = IDHEP;
    //
    //  Momentum vector
    p->pex = p->psx = PHEP1*GeV;
    p->pey = p->psy = PHEP2*GeV;
    p->pez = p->psz = PHEP3*GeV;
    //
    //  Mass
    p->mass = PHEP5*GeV;
    //
    //  Vertex 
    // (missing information in HEPEvt files)
    p->vsx = 0.0;
    p->vsy = 0.0;
    p->vsz = 0.0;
    p->vex = 0.0;
    p->vey = 0.0;
    p->vez = 0.0;
    //
    //  Generator status
    //  Simulator status 0 until simulator acts on it
    p->status = 0;
    if ( ISTHEP == 0 ) status.set(G4PARTICLE_GEN_EMPTY);
    if ( ISTHEP == 1 ) status.set(G4PARTICLE_GEN_STABLE); 
    if ( ISTHEP == 2 ) status.set(G4PARTICLE_GEN_DECAYED);
    if ( ISTHEP == 3 ) status.set(G4PARTICLE_GEN_DOCUMENTATION);
    //
    //  Creation time (note the units [1/c_light])
    // (No information in HEPEvt files)
    p->time = 0.0;
    p->properTime = 0.0;
    //
    // Keep daughters information for later
    daughter1.push_back(JDAHEP1);
    daughter2.push_back(JDAHEP2);
    //
    //  Add the particle to the collection vector
    particles.push_back(p);
    //
  }// End loop over particles

  //
  //  Now make a second loop over the particles, checking the daughter
  //  information. This is not always consistent with parent 
  //  information, and this utility assumes all parents listed are
  //  parents and all daughters listed are daughters
  //
  for( int IHEP=0; IHEP<NHEP; IHEP++ )    {
    struct ParticleHandler  {
      Particle* m_part;
      ParticleHandler(Particle* p) : m_part(p) {}
      void addParent(const Particle* p)  {
	m_part->parents.insert(p->id);
      }
      Particle* findParent(const Particle* p)  {
	return m_part->parents.find(p->id)==m_part->parents.end() ? 0 : m_part;
      }
    };

    //
    //  Get the MCParticle
    //
    Particle* mcp = particles[IHEP];
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
	}
      }
      else  {   //  Same logic, discrete cases
	ParticleHandler part_fd(particles[fd]);
	if ( !part_fd.findParent(mcp) ) part_fd.addParent(mcp);

	ParticleHandler part_ld(particles[ld]);
	if ( !part_ld.findParent(mcp) ) part_ld.addParent(mcp);
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
  return 0;
}

