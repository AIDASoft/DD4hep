// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================

// Framework include files
#include "DD4hep/IoStreams.h"
#include "DDG4/Geant4InputAction.h"

// C/C++ include files
#include <fstream>
#include <boost/iostreams/stream.hpp>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    namespace HepMC {  class EventStream;  }
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
    class Geant4EventReaderHepMC : public Geant4EventReader  {
      typedef boost::iostreams::stream<dd4hep_file_source<int> > in_stream;
      //typedef boost::iostreams::stream<dd4hep_file_source<TFile*> > in_stream;
      typedef HepMC::EventStream EventStream;
    protected:
      in_stream m_input;
      EventStream* m_events;
    public:
      /// Initializing constructor
      Geant4EventReaderHepMC(const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventReaderHepMC();
      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticles(int event_number, std::vector<Particle*>& particles);
    };
  }     /* End namespace Simulation   */
}       /* End namespace DD4hep       */

// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
// #include "DDG4/Geant4EventReaderHepMC"

#include "DDG4/Factories.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Primary.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cerrno>
#include <algorithm>

using namespace std;
using namespace CLHEP;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

// Factory entry
DECLARE_GEANT4_EVENT_READER(Geant4EventReaderHepMC)

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    namespace HepMC {

      class EventHeader  {
      public:
	int   id;
	int   num_vertices;
	int   bp1, bp2;
	int   signal_process_id;
	int   signal_process_vertex;
	float scale;
	float alpha_qcd;
	float alpha_qed;
	vector<float>      weights;
	vector<long>       random;
	EventHeader() {}
      };

      /// The known_io enum is used to track which type of input is being read
      enum known_io { gen=1, ascii, extascii, ascii_pdt, extascii_pdt };

      class EventStream {
      public:
	istream& instream;
	// io information
	string key;
	double mom_unit, pos_unit;
	int    io_type;
	float xsection, xsection_err;
	//WeightContainer weights;
	typedef std::map<int,Geant4Vertex*> Vertices;
	Vertices m_vertices;
	typedef std::map<int,Geant4Particle*> Particles;
	Particles m_particles;

	EventHeader header;

	EventStream(istream& in) : instream(in), io_type(0)
	{ use_default_units();                       }
	Particles& particles() { return m_particles; }
	Vertices&  vertices()  { return m_vertices;  }
	void add_vertex(int id, Geant4Vertex* v)
	{ m_vertices.insert(make_pair(id,v));        }
	void add_particle(Geant4Particle* p);
	void set_io(int typ, const string& k) 
	{ io_type = typ;    key = k;                 }
	void use_default_units()  
	{ mom_unit = MeV; pos_unit = mm;             }
	Geant4Vertex* vertex(int i);
	void fix_particles();
	bool read();
	void clear();
      };

      char get_input(istream& is, istringstream& iline);
      int find_event_end(istream & is);
      int read_weight_names(EventStream &, istringstream& iline);
      int read_particle(EventStream &info, istringstream& iline, Geant4Particle * p);
      int read_vertex(EventStream &info, istream& is, istringstream & iline);
      int read_event_header(EventStream &info, istringstream & input, EventHeader& header);
      int read_cross_section(EventStream &info, istringstream & input);
      int read_units(EventStream &info, istringstream & input);
      int read_heavy_ion(EventStream &, istringstream & input);
      int read_pdf(EventStream &, istringstream & input);
    }
  }
}

/// Initializing constructor
Geant4EventReaderHepMC::Geant4EventReaderHepMC(const string& nam)
: Geant4EventReader(nam), m_input(), m_events(0)
{
  // Now open the input file:
  m_input.open(nam.c_str(),BOOST_IOS::in|BOOST_IOS::binary);
  if ( m_input < 0 )   {
    string err = "+++ Geant4EventReaderHepMC: Failed to open input stream:"+nam+
      " Error:"+string(strerror(errno));
    throw runtime_error(err);
  }
  m_events = new HepMC::EventStream(m_input);
}

/// Default destructor
Geant4EventReaderHepMC::~Geant4EventReaderHepMC()    {
  delete m_events;
  m_events = 0;
  m_input.close();
}
namespace DD4hep{
  template <typename M> ReferenceObjects<typename M::value_type> __reference2nd(M&) {
    return ReferenceObjects<typename M::value_type>();
  }
}
/// Read an event and fill a vector of MCParticles.
Geant4EventReaderHepMC::EventReaderStatus
Geant4EventReaderHepMC::readParticles(int /* ev_id */, Particles& output) {
  // make sure the stream is good
  if ( m_input.eof() || m_input.fail() )  {
    cerr << "streaming input: end of stream found setting badbit." << endl;
    m_input.clear(ios::badbit); 
    return EVENT_READER_IO_ERROR;
  }

  if ( m_events->read() )  {
    EventStream::Particles& p = m_events->particles();
    output.reserve(p.size());
    transform(p.begin(),p.end(),back_inserter(output),reference2nd(p));
    m_events->clear();
    for(Particles::const_iterator k=output.begin(); k != output.end(); ++k) {
      Geant4ParticleHandle p(*k);
      printout(INFO,m_name,
	       "+++ %s ID:%3d status:%08X typ:%9d Mom:(%+.2e,%+.2e,%+.2e)[MeV] "
	       "time: %+.2e [ns] #Dau:%3d #Par:%1d",
	       "",p->id,p->status,p->pdgID,
	       p->psx/MeV,p->psy/MeV,p->psz/MeV,p->time/ns,
	       p->daughters.size(),
	       p->parents.size());
      //output.push_back(p);
    }
    return EVENT_READER_OK;
  }
  return EVENT_READER_IO_ERROR;
}


void HepMC::EventStream::fix_particles()  {
  EventStream::Particles::iterator i;
  std::set<int>::const_iterator id, ip;
  for(i=m_particles.begin(); i != m_particles.end(); ++i)  {
    Geant4ParticleHandle p((*i).second);
    int end_vtx_id = p->secondaries;
    p->secondaries = 0;
    Geant4Vertex* v = vertex(end_vtx_id);
    if ( v )   {
      p->vex = v->x;
      p->vey = v->y;
      p->vez = v->z;
      v->in.insert(p->id);
      for(id=v->out.begin(); id!=v->out.end();++id)
	p->daughters.insert(*id);
    }
  }
  EventStream::Vertices::iterator j;
  for(j=m_vertices.begin(); j != m_vertices.end(); ++j)  {
    Geant4Vertex* v = (*j).second;
    for (id=v->in.begin(); id!=v->in.end();++id)  {
      for (ip=v->out.begin(); ip!=v->out.end();++ip)   {
	EventStream::Particles::iterator ipp = m_particles.find(*ip);
	(*ipp).second->parents.insert(*id);
      }
    }
  }
}

char HepMC::get_input(istream& is, istringstream& iline)  {
  char value = is.peek();
  if ( !is ) {        // make sure the stream is valid
    cerr << "StreamHelpers: setting badbit." << endl;
    is.clear(ios::badbit); 
    return -1;
  }
  string line, firstc;
  getline(is,line);
  if ( !is ) {        // make sure the stream is valid
    cerr << "StreamHelpers: setting badbit." << endl;
    is.clear(ios::badbit); 
    return -1;
  }
  iline.str(line);
  if ( line.length()>0 && line[1] == ' ' ) iline >> firstc;
  return iline ? value : -1;
}

int HepMC::find_event_end(istream & is) {
  string line;
  while ( is ) { 
    char val = is.peek();
    if( val == 'E' ) {	// next event
      return 1;
    }
    getline(is,line);
    if ( !is.good() ) return 0;
  }
  return 0;
}

int HepMC::read_weight_names(EventStream&, istringstream&)   {
#if 0
  int HepMC::read_weight_names(EventStream& info, istringstream& iline)
    size_t name_size = 0;
  iline >> name_size;
  info.weights.names.clear();
  info.weights.weights.clear();

  string name;
  WeightContainer namedWeight;
  string::size_type i1 = line.find("\""), i2, len = line.size();
  for(size_t ii = 0; ii < name_size; ++ii) {
    // weight names may contain blanks
    if(i1 >= len) {
      cout << "debug: attempting to read past the end of the named weight line " << endl;
      cout << "debug: We should never get here" << endl;
      cout << "debug: Looking for the end of this event" << endl;
      find_event_end(is);
    }
    i2 = line.find("\"",i1+1);
    name = line.substr(i1+1,i2-i1-1);
    if ( namedWeight.names.find(name) == namedWeight.names.end() )
      namedWeight.names[name] = namedWeight.names.size();
    namedWeight.weights[ii] = info.weights.weights[ii];
    i1 = line.find("\"",i2+1);
  }

  info.weights.weights = namedWeight.weights;
  info.weights.names   = namedWeight.names;
#endif
  return 1;
}

int HepMC::read_particle(EventStream &info, istringstream& iline, Geant4Particle * p)   {
  float ene = 0., theta = 0., phi = 0;
  int   size = 0;

  // check that the input stream is still OK after reading item
  iline >> p->id >> p->pdgID >> p->psx >> p->psy >> p->psz >> ene;
  if ( !iline ) 
    return 0;
  else if( info.io_type != ascii )
    iline >> p->mass;
  else 
    p->mass = sqrt(fabs(ene*ene - p->psx*p->psx + p->psy*p->psy + p->psz*p->psz));

  // Reuse here the secondaries to store the end-vertex ID
  iline >> p->status >> theta >> phi >> p->secondaries >> size;
  if(!iline) 
    return 0;

  // read flow patterns if any exist
  for (int i = 0; i < size; ++i ) {
    iline >> p->colorFlow[0] >> p->colorFlow[1];
    if(!iline) return 0;
  }
  return 1;
}

int HepMC::read_vertex(EventStream &info, istream& is, istringstream & iline)    {
  int id=0, dummy=0, num_orphans_in=0, num_particles_out=0, weights_size=0;
  vector<float> weights;
  Geant4Vertex* v = new Geant4Vertex();
  Geant4Particle* p;

  iline >> id >> dummy >> v->x >> v->y >> v->z >> v->time 
	>> num_orphans_in >> num_particles_out >> weights_size;
  if(!iline) 
    return 0;
  weights.resize(weights_size);
  for (int i1 = 0; i1 < weights_size; ++i1) {
    iline >> weights[i1];
    if(!iline)
      return 0;
  }
  info.add_vertex(id,v);
  //cout << "Add Vertex:" << id << endl;

  for(char value = is.peek(); value=='P'; value=is.peek())  {
    value = get_input(is,iline);
    if( !iline || value < 0 )
      return 0;

    read_particle(info, iline,p = new Geant4Particle());
    if(!iline)   {
      cerr << "Failed to read particle!" << endl;
      delete p;
      return 0;
    }
    info.add_particle(p);
    p->pex = p->psx;
    p->pey = p->psy;
    p->pez = p->psz;
    if ( --num_orphans_in >= 0 )   {
      p->vex = v->x;
      p->vey = v->y;
      p->vez = v->z;
      v->in.insert(p->id);
      //cout << "Add INGOING  Particle:" << p->id << endl;
    }
    else if ( num_particles_out >= 0 )   {
      v->out.insert(p->id);
      p->vsx = v->x;
      p->vsy = v->y;
      p->vsz = v->z;
      //cout << "Add OUTGOING Particle:" << p->id << endl;
    }
    else  {
      delete p;
      throw runtime_error("Invalid number of particles....");
    }
  }
  return 1;
}
	
int HepMC::read_event_header(EventStream &info, istringstream & input, EventHeader& header)   {
  // read values into temp variables, then fill GenEvent
  int random_states_size = 0, nmpi = -1;
  input >> header.id;
  if( info.io_type == gen || info.io_type == extascii ) {
    input >> nmpi;
    if( input.fail() ) return 0;
    //MSF set_mpi( nmpi );
  }
  input >> header.scale;
  input >> header.alpha_qcd;
  input >> header.alpha_qed;
  input >> header.signal_process_id;
  input >> header.signal_process_vertex;
  input >> header.num_vertices;
  if( info.io_type == gen || info.io_type == extascii )
    input >> header.bp1 >> header.bp2;

  input >> random_states_size;
  cout << input.str() << endl;
  input.clear();
  if( input.fail() ) return 0;

  header.random.resize(random_states_size);
  for(int i = 0; i < random_states_size; ++i )
    input >> header.random[i];

  size_t weights_size = 0;
  input >> weights_size;
  if( input.fail() ) return 0;

  vector<float> wgt(weights_size);
  for(size_t ii = 0; ii < weights_size; ++ii )
    input >> wgt[ii];
  if( input.fail() ) return 0;

  // weight names will be added later if they exist
  if( weights_size > 0 ) header.weights = wgt;
  return 1;
}

int HepMC::read_cross_section(EventStream &info, istringstream & input)   {
  input >> info.xsection >> info.xsection_err;
  return input.fail() ? 0 : 1;
}

int HepMC::read_units(EventStream &info, istringstream & input)   {
  if( info.io_type == gen )  {
    string mom, pos;
    input >> mom >> pos;
    if ( !input.fail() )  {
      if ( mom == "KEV" ) info.mom_unit = keV;
      else if ( mom == "MEV" ) info.mom_unit = MeV;
      else if ( mom == "GEV" ) info.mom_unit = GeV;
      else if ( mom == "TEV" ) info.mom_unit = TeV;
      
      if ( pos == "MM" ) info.pos_unit = mm;
      else if ( pos == "CM" ) info.pos_unit = cm;
      else if ( pos == "M"  ) info.pos_unit = m;
    }
  }
  return input.fail() ? 0 : 1;
}

int HepMC::read_heavy_ion(EventStream &, istringstream & input)  {
  // read values into temp variables, then create a new HeavyIon object
  int nh =0, np =0, nt =0, nc =0, 
    neut = 0, prot = 0, nw =0, nwn =0, nwnw =0;
  float impact = 0., plane = 0., xcen = 0., inel = 0.; 
  input >> nh >> np >> nt >> nc >> neut >> prot >> nw >> nwn >> nwnw;
  input >> impact >> plane >> xcen >> inel;
  cerr << "Reading heavy ion, but igoring data!" << endl;
  /*
    ion->set_Ncoll_hard(nh);
    ion->set_Npart_proj(np);
    ion->set_Npart_targ(nt);
    ion->set_Ncoll(nc);
    ion->set_spectator_neutrons(neut);
    ion->set_spectator_protons(prot);
    ion->set_N_Nwounded_collisions(nw);
    ion->set_Nwounded_N_collisions(nwn);
    ion->set_Nwounded_Nwounded_collisions(nwnw);
    ion->set_impact_parameter(impact);
    ion->set_event_plane_angle(plane);
    ion->set_eccentricity(xcen);
    ion->set_sigma_inel_NN(inel);
  */
  return input.fail() ? 0 : 1;
}

int HepMC::read_pdf(EventStream &, istringstream & input)  {
  // read values into temp variables, then create a new PdfInfo object
  int id1 =0, id2 =0, pdf_id1=0, pdf_id2=0;
  double  x1 = 0., x2 = 0., scale = 0., pdf1 = 0., pdf2 = 0.; 
  input >> id1 ;
  if ( input.fail()  )
    return 0;
  // check now for empty PdfInfo line
  if( id1 == 0 )
    return 0;
  // continue reading
  input >> id2 >> x1 >> x2 >> scale >> pdf1 >> pdf2;
  if ( input.fail()  )
    return 0;
  // check to see if we are at the end of the line
  if( !input.eof() )  {
    input >> pdf_id1 >> pdf_id2;
  }
  cerr << "Reading pdf, but igoring data!" << endl;
  /*
    pdf->set_id1( id1 );
    pdf->set_id2( id2 );
    pdf->set_pdf_id1( pdf_id1 );
    pdf->set_pdf_id2( pdf_id2 );
    pdf->set_x1( x1 );
    pdf->set_x2( x2 );
    pdf->set_scalePDF( scale );
    pdf->set_pdf1( pdf1 );
    pdf->set_pdf2( pdf2 );
  */
  return input.fail() ? 0 : 1;
}

void HepMC::EventStream::add_particle(Geant4Particle* p)   {
  p->id = m_particles.size();
  m_particles.insert(make_pair(p->id,p));
}

Geant4Vertex* HepMC::EventStream::vertex(int i)   {
  Vertices::iterator it=m_vertices.find(i);
  return (it==m_vertices.end()) ? 0 : (*it).second;
}

void HepMC::EventStream::clear()   {
  releaseObjects(m_vertices)();
  releaseObjects(m_particles)();
}

bool HepMC::EventStream::read()   {
  EventStream& info = *this;

  // OK - now ready to start reading the event, so set the header flag
  // The flag will be set to false when we reach the end of the header
  m_particles.clear();
  m_vertices.clear();

  bool event_read = false;
  while( instream.good() ) {
    char value = instream.peek();
    if ( value == 'E' && event_read ) break;
    else if ( event_read ) break;
    else if ( instream.eof() ) return false;

    istringstream input_line;
    value = get_input(instream,input_line);

    // On failure switch to end
    if( !input_line || value < 0 )
      goto Skip;

    switch( value )   {
    case 'H':  {
      int iotype = 0;
      string key;
      input_line >> key;
      // search for event listing key before first event only.
      key = key.substr(0,key.find('\r'));
      if ( key == "H" && (this->io_type == gen || this->io_type == extascii) ) {
	read_heavy_ion(info, input_line);
	break;
      }
      else if( key == "HepMC::IO_GenEvent-START_EVENT_LISTING" )
	this->set_io(gen,key);
      else if( key == "HepMC::IO_Ascii-START_EVENT_LISTING" )
	this->set_io(ascii,key);
      else if( key == "HepMC::IO_ExtendedAscii-START_EVENT_LISTING" )
	this->set_io(extascii,key);
      else if( key == "HepMC::IO_Ascii-START_PARTICLE_DATA" )
	this->set_io(ascii_pdt,key);
      else if( key == "HepMC::IO_ExtendedAscii-START_PARTICLE_DATA" )
	this->set_io(extascii_pdt,key);
      else if( key == "HepMC::IO_GenEvent-END_EVENT_LISTING" )
	iotype = gen;
      else if( key == "HepMC::IO_Ascii-END_EVENT_LISTING" ) 
	iotype = ascii;
      else if( key == "HepMC::IO_ExtendedAscii-END_EVENT_LISTING" ) 
	iotype = extascii;
      else if( key == "HepMC::IO_Ascii-END_PARTICLE_DATA" ) 
	iotype = ascii_pdt;
      else if( key == "HepMC::IO_ExtendedAscii-END_PARTICLE_DATA" ) 
	iotype = extascii_pdt;

      if( iotype != 0 && this->io_type != iotype )  {
	cerr << "GenEvent::find_end_key: iotype keys have changed. "
	     << "MALFORMED INPUT" << endl;
	instream.clear(ios::badbit); 
	return false;
      }
      continue;
    }      
    case 'E':      	// deal with the event line
      if ( !read_event_header(info, input_line, this->header) )
	goto Skip;
      event_read = true;
      continue;

    case 'N':      	// get weight names 
      if ( !read_weight_names(info, input_line) )
	goto Skip;
      continue;

    case 'U':      	// get unit information if it exists
      if ( !read_units(info, input_line) )
	goto Skip;
      continue;

    case 'C':      	// we have a GenCrossSection line
      if ( !read_cross_section(info, input_line) )
	goto Skip;
      continue;

    case 'V':           // Read vertex with particles
      if ( !read_vertex(info, instream, input_line) )
	goto Skip;
      continue;

    case 'F':           // Read PDF
      if ( !read_pdf(info, input_line) )
	goto Skip;
      continue;

    case 'P':      	// we should not find this line
      cerr << "streaming input: found unexpected Particle line." << endl;
      continue;

    default:            // ignore everything else
      continue;
    }
    continue;
  Skip:
    printout(WARNING,"HepMC::EventStream","+++ Skip event with ID: %d",this->header.id);
    releaseObjects(vertices())();
    releaseObjects(particles())();
    find_event_end(instream);
    event_read = false;
    if ( instream.eof() ) return false;
  }
  this->fix_particles();
  releaseObjects(vertices())();
  return true;
}
