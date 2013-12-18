// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
// @author  P.Kostka (main author)
// @author  M.Frank  (code reshuffeling into new DDG4 scheme)
//
//====================================================================

// Framework include files
#include "EventReader.h"
#include "DD4hep/Plugins.h"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "Randomize.hh"

using namespace std;

/// Initializing constructor
DD4hep::lcio::EventReader::EventReader(const string& nam) : m_name(nam)   {
}

/// Default destructor
DD4hep::lcio::EventReader::~EventReader()   {
}

static void setCharge(IMPL::MCParticleImpl* p)   {
  // Initialise the particle charge looking at the actual 
  // Particle Table, or set it to -1000 to flag missing 
  // information, if PDG unknown in the actual physics list..
  G4ParticleTable*      tab  = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* def = tab->FindParticle(p->getPDG());
  double charge = def ? def->GetPDGCharge() : -1000;
  p->setCharge(charge);
}

/// Standard constructor
DD4hep::lcio::LcioGeneratorAction::LcioGeneratorAction(Simulation::Geant4Context* context, const string& name)
  : Simulation::Geant4GeneratorAction(context,name)
{
  declareProperty("Input",       m_input);
  declareProperty("Sync",        m_SYNCEVT=0);
  declareProperty("zSpread",     m_primaryVertexSpreadZ=0.0);
  declareProperty("lorentzAngle",m_lorentzTransformationAngle=0.0);
}

/// Default destructor
DD4hep::lcio::LcioGeneratorAction::~LcioGeneratorAction()   {
}

static string issue(int i)  {
  stringstream str;
  str << "lcio::LcioGeneratorAction: Event " << i;
  return str.str();
}

/// Read an event and return a LCCollectionVec of MCParticles.
DD4hep::lcio::LcioGeneratorAction::Particles *DD4hep::lcio::LcioGeneratorAction::readEvent(int which)  {
  int evid = which + m_SYNCEVT;
  if ( 0 == m_reader )  {
    if ( m_input.empty() )  {
      throw runtime_error("LcioGeneratorAction: No inoput file declared!");
    }
    string err;
    Simulation::TypeName tn = Simulation::TypeName::split(m_input,"|");
    try  {
      m_reader = PluginService::Create<DD4hep::lcio::EventReader*>(tn.first,tn.second,int(0));
      if ( 0 == m_reader )   {
	abortRun(issue(evid),"Error creating reader plugin.",
		 "Failed to create file reader of type %s. Cannot open dataset %s",
		 tn.first.c_str(),tn.second.c_str());
	return 0;
      }
    }
    catch(const std::exception& e)  {
      err = e.what();
    }
    if ( !err.empty() )  {
      abortRun(issue(evid),err,"Error when reading file %s",m_input.c_str());
      return 0;
    }
  }
  Particles* parts = m_reader->readEvent();
  if ( 0 == parts )  {
    abortRun(issue(evid),"Error when reading file - may be end of file.",
	     "Error when reading file %s",m_input.c_str());
  }
  return parts;
}

/// Callback to generate primary particles
void DD4hep::lcio::LcioGeneratorAction::operator()(G4Event* evt)   {
  //**********************************************************
  // Read in the event
  //**********************************************************
  m_g4ParticleMap.clear();
  m_convertedParticles.clear();
  Particles* primaries = readEvent(evt->GetEventID());
  if ( 0 == primaries ) return;

  vector<MCParticle*> col(primaries->size()) ;
  int NHEP = col.size();

  // parameters of the Lorentz transformation matrix
  const double zspreadparameter = m_primaryVertexSpreadZ;
  const double alpha            = m_lorentzTransformationAngle;
  const double gamma            = sqrt(1 + sqr(tan(alpha)));
  const double betagamma        = tan(alpha);

  double zspread = ( zspreadparameter == 0.0 ?  0.0 : G4RandGauss::shoot(0,zspreadparameter/mm ) );
  //cout << " PrimaryVertexSpreadZ=" << zspreadparameter/mm  <<"mm, zspread=" << zspread << "mm" << endl;
  G4ThreeVector particle_position = G4ThreeVector(0,0,zspread);

  // build collection of MCParticles
  for(int IHEP=0; IHEP<NHEP; IHEP++ ) {
    Particle* mcp =  dynamic_cast<Particle*> ( primaries->getElementAt( IHEP ) );
    setCharge(mcp);
    //Boost to lab frame with crossing angle alpha
    if (alpha != 0) {

      double c_light = 299.792;
      const double* p = mcp->getMomentum();
      const double m  = mcp->getMass();
      //double e = mcp->getEnergy();
      // after the transformation (boost in x-direction)
      double pPrime[3];
      pPrime[0] = betagamma * sqrt(sqr(p[0])+sqr(p[1])+sqr(p[2])+sqr(m)) + gamma * p[0];
      pPrime[1] = p[1];
      pPrime[2] = p[2];

      const double* v = mcp->getVertex();
      const double t = mcp->getTime();
      double vPrime[3];
      double tPrime;
      tPrime = gamma * t + betagamma * v[0] / c_light;
      vPrime[0] = gamma * v[0] + betagamma * c_light * t;
      vPrime[1] = v[1];
      vPrime[2] = v[2];

      // py and pz remain the same, E changes implicitly with px
      mcp->setMomentum( pPrime );
      mcp->setVertex( vPrime );
      mcp->setTime( tPrime );
      //cout << IHEP  << " alpha=" << alpha  <<" gamma=" << gamma << " betagamma=" << betagamma  << endl;
      //	  cout << IHEP  << " unboosted momentum: ("  << e                << "," << p[0]      << "," << p[1]      << ","  <<  p[2]      << ")" << endl;
      //	  cout << IHEP  << "   boosted momentum: ("  << mcp->getEnergy() << "," <<  betagamma * sqrt(sqr(p[0])+sqr(p[1])+sqr(p[2])+sqr(m)) + gamma * p[0] << ","  << pPrime[1] << "," <<  pPrime[2] << ")" << endl;
      //	  cout << IHEP  << "                     ("  << mcp->getEnergy()-e << ","  << ( betagamma * sqrt(sqr(p[0])+sqr(p[1])+sqr(p[2])+sqr(m)) + gamma * p[0])-p[0] << ","  << pPrime[1]-p[1] << ","  <<  pPrime[2]-p[2] << ")" << endl;
      //cout << IHEP  << "   unboosted vertex: (" << t      << "," << v[0]      << "," << v[1]      << "," <<  v[2]      << ")" << endl;
      //	  cout << IHEP  << "     boosted vertex: (" << tPrime << "," << vPrime[0] << "," << vPrime[1] << "," <<  vPrime[2] << ")" << endl;
      //	  cout << IHEP  << "     boosted vertex: (" << tPrime-t << "," << vPrime[0]-v[0] << "," << vPrime[1]-v[1] << "," <<  vPrime[2]-v[2] << ")" << endl;
    }
    // apply z spread
    if (zspreadparameter != 0){
      const double* v = mcp->getVertex();
      double vPrime[3];
      vPrime[0] = v[0];
      vPrime[1] = v[1];
      vPrime[2] = v[2] + zspread;
      mcp->setVertex(vPrime);
    }
    col[IHEP] = dynamic_cast<MCParticle*>( mcp );
  }

  // check if there is at least one particle
  if( NHEP == 0 ) return;
  
  // create G4PrimaryVertex object
  double particle_time = 0.0;
  G4PrimaryVertex* vertex = new G4PrimaryVertex(particle_position,particle_time);
  // put initial particles to the vertex
  for( size_t i=0; i< col.size(); i++ ){
    MCParticle* mcp = col[i];
    if ( mcp->getParents().size()==0 )   {
      G4PrimarySet g4set = getRelevantParticles(mcp);
      for (G4PrimarySet::iterator setit=g4set.begin(); setit != g4set.end(); setit++ ){
	vertex->SetPrimary(*setit);
	//cout << "G4PrimaryParticle ("<< (*setit)->GetPDGcode() << ") added to G4PrimaryVertex." << endl;
      }
    }
  }
  // Put the vertex to G4Event object
  evt->AddPrimaryVertex(vertex);
}

DD4hep::lcio::LcioGeneratorAction::G4PrimarySet 
DD4hep::lcio::LcioGeneratorAction::getRelevantParticles(MCParticle* p)      {
  //    log each particle which has been called, to avoid double counting and increase efficiency
  m_convertedParticles.insert(p);
  G4ParticleMap::iterator mcpIT;
  G4PrimarySet relevantParticlesSet; //holds all relevant decay particles of p
  
  // logic starts here:
  // CASE1: if p is a stable particle: search for it in G4ParticleMap
  // if it is already there: get G4PrimaryParticle version of p from G4ParticleMap
  // else: create G4PrimaryParticle version of p and write it to G4ParticleMap
  // in the end: insert this single G4PrimaryParticle verion of p to the 
  // relevant particle list and return this "list".
  if (p->getGeneratorStatus() == 1)    {
    G4PrimaryParticle* g4p;
    mcpIT = m_g4ParticleMap.find( p );
    if( mcpIT != m_g4ParticleMap.end() ){
      g4p =  mcpIT->second;
    }
    else    {
      int    IDHEP = p->getPDG();
      double PHEP1 = p->getMomentum()[0];
      double PHEP2 = p->getMomentum()[1];
      double PHEP3 = p->getMomentum()[2];
      double PHEP5 = p->getMass();
      // create G4PrimaryParticle object
      g4p = new G4PrimaryParticle(IDHEP, PHEP1*GeV, PHEP2*GeV, PHEP3*GeV);
      g4p->SetMass(PHEP5*GeV);
      m_g4ParticleMap[ p ] = g4p;  
    }
    relevantParticlesSet.insert(g4p);
    return relevantParticlesSet;
  }

  // CASE2: if p is not stable: get first decay daughter and calculate the proper time of p
  // if proper time is not zero: particle is "relevant", since it carries vertex information
  // if p is already in G4ParticleMap: take it
  // otherwise: create G4PrimaryParticle version of p and write it to G4ParticleMap
  // now collect all relevant particles of all daughters and setup "relevant mother-
  // daughter-relations" between relevant decay particles and G4PrimaryParticle version of p
  // in the end: insert only the G4PrimaryParticle version of p to the
  // relevant particle list and return this "list". The added particle has now the correct pre-assigned
  // decay products and (hopefully) the right lifetime.
  else if(  p->getDaughters().size() > 0  ) {  //fg: require that there is at least one daughter - otherwise forget the particle
    cout << "case 2" <<  endl;
    // calculate proper time
    MCParticle* dp = p->getDaughters()[0];
    double proper_time = fabs((dp->getTime()-p->getTime()) * p->getMass()) / p->getEnergy();
    //  fix by S.Morozov for real != 0
    double aPrecision = dp->getTime() * p->getMass() / p->getEnergy();
    double bPrecision =  p->getTime() * p->getMass() / p->getEnergy();
    double ten = 10;
    double proper_time_Precision =  pow(ten,-DBL_DIG)*fmax(fabs(aPrecision),fabs(bPrecision));

    bool isProperTimeZero = ( proper_time <= proper_time_Precision );

    // -- remove original --- if (proper_time != 0) {
    if ( isProperTimeZero == false ) {
      G4PrimaryParticle* g4p;
      mcpIT = m_g4ParticleMap.find( p );
      if( mcpIT != m_g4ParticleMap.end() ){
	g4p =  mcpIT->second;
      }
      else   {
	int    IDHEP = p->getPDG();    
	double PHEP1 = p->getMomentum()[0];
	double PHEP2 = p->getMomentum()[1];
	double PHEP3 = p->getMomentum()[2];	
	double PHEP5 = p->getMass();
	// create G4PrimaryParticle object
	g4p = new G4PrimaryParticle( IDHEP, PHEP1*GeV, PHEP2*GeV, PHEP3*GeV );	
	g4p->SetMass( PHEP5*GeV );
	g4p->SetProperTime( proper_time*ns );
	m_g4ParticleMap[ p ] = g4p;
	G4PrimarySet vec3;
	for (size_t i=0; i<p->getDaughters().size(); i++){
	  if (m_convertedParticles.count(p->getDaughters()[i])==0)  {
	    G4PrimarySet vec2 = getRelevantParticles(p->getDaughters()[i]);
	    G4PrimarySet::iterator setit;
	    for (setit=vec2.begin(); setit != vec2.end(); setit++ ){
	      vec3.insert(*setit);
	    }
	  }
	}
	G4PrimarySet::iterator setit;
	for (setit=vec3.begin(); setit != vec3.end(); setit++ ){
	  g4p->SetDaughter(*setit);
	}
      }
      relevantParticlesSet.insert(g4p);
      return relevantParticlesSet;
    }

    // CASE3: if p is not stable AND has zero lifetime: forget about p and retrieve all relevant
    // decay particles of all daughters of p. In this case this step of recursion is just there for
    // collecting all relevant decay products of daughters (and return them).
    else   {
      for (size_t i=0; i<p->getDaughters().size(); i++){
	if (m_convertedParticles.count(p->getDaughters()[i])==0){
	  G4PrimarySet vec2 = getRelevantParticles(p->getDaughters()[i]);
	  G4PrimarySet::iterator setit;
	  for (setit=vec2.begin(); setit != vec2.end(); setit++ ){
	    relevantParticlesSet.insert(*setit);
	  }
	}
      }
      return relevantParticlesSet;
    }
  }
  //fg: add a default return
  return relevantParticlesSet;
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(DD4hep::lcio,LcioGeneratorAction)
