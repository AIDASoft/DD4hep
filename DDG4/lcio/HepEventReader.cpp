// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================

// Framework include files
#include "LCIOEventReader.h"

// C/C++ include files
#include <fstream>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation  {

    /// File reader to load StdHep format (ASCII)
    /**
     * 
     * Class to populate Geant4 primary particles and vertices from a 
     * file in HepEvent format (ASCII)
     *
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class HepEventReader : public LCIOEventReader  {
    protected:
      std::ifstream m_input;
      int m_format;
    public:
      /// Initializing constructor
      HepEventReader(const std::string& nam, int fmt);
      /// Default destructor
      virtual ~HepEventReader();
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual EVENT::LCCollection *readParticles();
    };
  }     /* End namespace lcio   */
}       /* End namespace DD4hep */


#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"
#include "EVENT/LCIO.h"
#include "lcio.h"
using namespace IMPL;
using namespace EVENT;
using namespace DD4hep::Simulation;
#define HEPEvt 1

// Factory entry
DECLARE_LCIO_EVENT_READER(HepEventReader)

/// Initializing constructor
HepEventReader::HepEventReader(const std::string& nam, int fmt) 
: LCIOEventReader(nam), m_format(fmt)
{
}

/// Default destructor
HepEventReader::~HepEventReader()    {
  m_input.close();
}

/// Helper function
MCParticleImpl* cast_particle(LCObject* obj)  {
  return dynamic_cast<MCParticleImpl*>(obj);
}

/// Read an event and return a LCCollectionVec of MCParticles.
EVENT::LCCollection *HepEventReader::readParticles()   {
  static const double c_light = 299.792;// mm/ns
  //
  //  Read the event, check for errors
  //
  int NHEP;  // number of entries
  m_input >> NHEP;
  if( m_input.eof() )   {
    //
    // End of File :: ??? Exception ???
    //   -> FG:   EOF is not an exception as it happens for every file at the end !
    return 0;
  }
    
  //
  //  Create a Collection Vector
  IMPL::LCCollectionVec* mcVec = new IMPL::LCCollectionVec(LCIO::MCPARTICLE);
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
 
  //???? Memory leak ?
  std::vector<int> *daughter1 = new std::vector<int> ();
  std::vector<int> *daughter2 =  new std::vector<int> ();
 
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
      return 0;	
    //
    //  Create a MCParticle and fill it from stdhep info
    MCParticleImpl* mcp = new MCParticleImpl();
    //
    //  PDGID
    mcp->setPDG(IDHEP);
    //
    //  Momentum vector
    float p0[3] = {PHEP1,PHEP2,PHEP3};
    mcp->setMomentum(p0);
    //
    //  Mass
    mcp->setMass(PHEP5);
    //
    //  Vertex 
    // (missing information in HEPEvt files)
    double v0[3] = {0.,0.,0.};
    mcp->setVertex(v0);
    //
    //  Generator status
    mcp->setGeneratorStatus(ISTHEP);
    //
    //  Simulator status 0 until simulator acts on it
    mcp->setSimulatorStatus(0);
    //
    //  Creation time (note the units)
    // (No information in HEPEvt files)
    mcp->setTime(0./c_light);
    //
    //  Add the particle to the collection vector
    mcVec->push_back(mcp);
    //
    // Keep daughters information for later
    daughter1->push_back(JDAHEP1);
    daughter2->push_back(JDAHEP2);
  }// End loop over particles
  //
  //  Now make a second loop over the particles, checking the daughter
  //  information. This is not always consistent with parent 
  //  information, and this utility assumes all parents listed are
  //  parents and all daughters listed are daughters
  //
  for( int IHEP=0; IHEP<NHEP; IHEP++ )    {
    struct Particle  {
      MCParticleImpl* m_part;
      Particle(MCParticleImpl* p) : m_part(p) {}
      void addParent(MCParticleImpl* p)  {   m_part->addParent(p);	  }
      MCParticleImpl* findParent(MCParticleImpl* p)  {
	int np = m_part->getParents().size();
	for(int ip=0;ip < np;ip++)	      {
	  MCParticleImpl* q = cast_particle(p->getParents()[ip]);  // cast necessary ? only if virt inheritance 
	  if ( q == p ) return p;
	}
	return 0;
      }
    };

    //
    //  Get the MCParticle
    //
    MCParticleImpl* mcp = cast_particle(mcVec->getElementAt(IHEP));
    //
    //  Get the daughter information, discarding extra information
    //  sometimes stored in daughter variables.
    // 
    int fd = daughter1->operator[](IHEP) - 1;
    int ld = daughter2->operator[](IHEP) - 1;
 
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
	  Particle part(cast_particle(mcVec->getElementAt(id)));
	  if ( !part.findParent(mcp) ) part.addParent(mcp);
	}
      }
      else  {   //  Same logic, discrete cases
	Particle part_fd(cast_particle(mcVec->getElementAt(fd)));
	if ( !part_fd.findParent(mcp) ) part_fd.addParent(mcp);

	Particle part_ld(cast_particle(mcVec->getElementAt(ld)));
	if ( !part_ld.findParent(mcp) ) part_ld.addParent(mcp);
      }
    }
    else if(fd > -1)      {
      Particle part(cast_particle(mcVec->getElementAt(fd)));
      if ( !part.findParent(mcp) ) part.addParent(mcp);
    }
    else if(ld > -1)      {
      Particle part(cast_particle(mcVec->getElementAt(ld)));
      if ( !part.findParent(mcp) ) part.addParent(mcp);
    }
  }  // End second loop over particles

  return mcVec;    
}

