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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Random.h"

#include "CLHEP/Random/EngineFactory.h"
#include "CLHEP/Random/Random.h"

// ROOT include files
#include "TRandom1.h"

// C/C++ include files
#include <cmath>

using namespace std;
using namespace DD4hep::Simulation;

namespace CLHEP   {
  unsigned long crc32ul(const std::string& s);
}

namespace    {

  /// Helper class to redirect calls to gRandon to Geant4Random
  class RNDM : public TRandom  {
    /// Reference to the generator object
    Geant4Random* m_generator;
    /// Reference to HepRandomEngine
    CLHEP::HepRandomEngine* m_engine;
    
  public:
    /// Initializing constructor
    RNDM(Geant4Random* r) : TRandom(), m_generator(r)    {
      m_engine = m_generator->engine();
    }
    /// Default destructor
    virtual ~RNDM() {      }
    /// Set new seed
    virtual  void SetSeed(UInt_t seed=0)    {
      fSeed = seed;
      m_generator->setSeed((long)seed);
    }
    /// Set new seed
    virtual  void SetSeed(ULong_t seed=0)    {
      fSeed = seed;
      m_generator->setSeed((long)seed);
    }
    /// Single shot random number creation
    virtual Double_t Rndm(Int_t=0)  {
      return m_engine->flat();
    }
    /// Return an array of n random numbers uniformly distributed in ]0,1].
    virtual void RndmArray(Int_t size, Float_t *array)    {
      for (Int_t i=0;i<size;i++) array[i] = m_engine->flat();
    }
    /// Return an array of n random numbers uniformly distributed in ]0,1].
    virtual void RndmArray(Int_t size, Double_t *array)    {
      m_engine->flatArray(size,array);
    }
  };
  static Geant4Random* s_instance = 0;
}

/// Default constructor
Geant4Random::Geant4Random(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt,nam), m_engine(0), m_rootRandom(0), m_rootOLD(0), 
    m_inited(false)
{
  declareProperty("File",   m_file="");
  declareProperty("Type",   m_engineType="");
  declareProperty("Seed",   m_seed = 123456789);
  declareProperty("Luxury", m_luxury = 1);
  declareProperty("Replace_gRandom",  m_replace = true);
  // Default: static Geant4 random engine.
  m_engine = CLHEP::HepRandom::getTheEngine();
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Random::~Geant4Random()  {
  // Only delete the engine if it is NOT the CLEP default one
  // BUT: Just cannot delete the engine. Causes havoc with static destructors!
  //CLHEP::HepRandomEngine* curr = CLHEP::HepRandom::getTheEngine();
  //if ( !m_engineType.empty() && m_engine != curr ) deletePtr(m_engine);

  // Set gRandom to the old value
  if (  m_rootRandom == gRandom ) gRandom = m_rootOLD;
  // Reset instance pointer
  if (  s_instance == this ) s_instance = 0;
  // Finally delete the TRandom instance wrapper
  deletePtr(m_rootRandom);
  InstanceCount::decrement(this);
}

/// Access the main Geant4 random generator instance. Must be created before used!
Geant4Random* Geant4Random::instance(bool throw_exception)   {
  if ( !s_instance && throw_exception )  {
    throw runtime_error("No global random number generator defined!");
  }  
  return s_instance;
}

/// Make this random generator instance the one used by Geant4
Geant4Random* Geant4Random::setMainInstance(Geant4Random* ptr)   {
  if ( ptr && !ptr->m_inited )  {
    ptr->initialize();
  }
  if ( s_instance != ptr )   {
    if ( !ptr )  {
      throw runtime_error("Attempt to declare invalid Geant4Random instance.");
    }
    if ( !ptr->m_inited )  {  
      throw runtime_error("Attempt to declare uninitialized Geant4Random instance.");
    }
    Geant4Random* old = s_instance;
    CLHEP::HepRandomEngine* curr = CLHEP::HepRandom::getTheEngine();
    if ( ptr->m_engine != curr )     {       
      ptr->printP2("Moving CLHEP random instance from %p to %p",curr,ptr->m_engine); 
      CLHEP::HepRandom::setTheEngine(ptr->m_engine);
    }
    if ( ptr->m_replace )   {
      ptr->m_rootOLD = gRandom;
      gRandom = ptr->m_rootRandom;
    }
    s_instance = ptr;
    return old;
  }
  return 0;
}

#include "CLHEP/Random/DualRand.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "CLHEP/Random/RanshiEngine.h"
#include "CLHEP/Random/NonRandomEngine.h"

/// Initialize the instance. 
void Geant4Random::initialize()   {
  if ( !m_file.empty() )  {
    ifstream in(m_file.c_str(), std::ios::in);
    m_engine = CLHEP::EngineFactory::newEngine(in);
    if ( !m_engine )    {
      except("Failed to create CLHEP random engine from file:%s.",m_file.c_str());
    }
    m_seed = m_engine->getSeed();
  }
  else if ( !m_engineType.empty() )    {
    /// Create new engine if a type is specified
    if ( m_engineType == CLHEP::HepJamesRandom::engineName() )
      m_engine = new CLHEP::HepJamesRandom();
    else if ( m_engineType == CLHEP::RanecuEngine::engineName() )
      m_engine = new CLHEP::RanecuEngine();
    else if ( m_engineType == CLHEP::Ranlux64Engine::engineName() )
      m_engine = new CLHEP::Ranlux64Engine();
    else if ( m_engineType == CLHEP::MTwistEngine::engineName() )
      m_engine = new CLHEP::MTwistEngine();
    else if ( m_engineType == CLHEP::DualRand::engineName() )
      m_engine = new CLHEP::DualRand();
    else if ( m_engineType == CLHEP::RanluxEngine::engineName() )
      m_engine = new CLHEP::RanluxEngine();
    else if ( m_engineType == CLHEP::RanshiEngine::engineName() )
      m_engine = new CLHEP::RanshiEngine();
    else if ( m_engineType == CLHEP::NonRandomEngine::engineName() )
      m_engine = new CLHEP::NonRandomEngine();

    if ( !m_engine )    {
      except("Failed to create CLHEP random engine of type: %s.",m_engineType.c_str());
    }
  }
  m_engine->setSeed(m_seed,m_luxury);
  m_rootRandom = new RNDM(this);
  m_inited = true;
  if ( 0 == s_instance )   {
    setMainInstance(this);
  }
}

/// Should initialise the status of the algorithm according to seed.
void Geant4Random::setSeed(long seed)   {
  if ( !m_inited ) initialize();
  m_engine->setSeed(m_seed=seed,0);
}

/// Should initialise the status of the algorithm
/** Initialization according to the zero terminated
 *  array of seeds. It is allowed to ignore one or 
 *  many seeds in this array.
 */
void Geant4Random::setSeeds(const long* seeds, int size)   {
  if ( !m_inited ) initialize();
  m_seed = seeds[0];
  m_engine->setSeeds(seeds, size);
}

/// Should save on a file specific to the instantiated engine in use the current status.
void Geant4Random::saveStatus( const char filename[] ) const    {
  if ( !m_inited )  {
    except("Failed to save RandomGenerator status. [Not-inited]");
  }
  m_engine->saveStatus(filename);
}

/// Should read from a file and restore the last saved engine configuration.
void Geant4Random::restoreStatus( const char filename[] )    {
  if ( !m_inited ) initialize();
  m_engine->restoreStatus(filename);
}

/// Should dump the current engine status on the screen.
void Geant4Random::showStatus() const    {
  if ( !m_inited )  {
    error("Failed to show RandomGenerator status. [Not-inited]");
    return;
  }
  printP2("Random engine status of object of type Geant4Random @ 0x%p",this);
  if ( !m_file.empty() )
    printP2("   Created from file: %s",m_file.c_str());
  else if ( !m_engineType.empty() )
    printP2("   Special instance created of type:%s @ 0x%p",
            m_engineType.c_str(),m_engine);
  else
    printP2("   Reused HepRandom engine instance %s @ 0x%p",
            m_engine ? m_engine->name().c_str() : "???", m_engine);
  
  if ( m_engine == CLHEP::HepRandom::getTheEngine() )
    printP2("   Instance is identical to Geant4's HepRandom instance.");

  printP2("   Instance is %sidentical to ROOT's gRandom instance.",
          gRandom == m_rootRandom ? "" : "NOT ");

  if ( gRandom != m_rootRandom )  {
    printP2("      Local TRandom: 0x%p  gRandom: 0x%p",m_rootRandom,gRandom);
  }  
  m_engine->showStatus();
}

/// Create flat distributed random numbers in the interval ]0,1]
double Geant4Random::rndm_clhep()  {
  if ( !m_inited ) initialize();
  return m_engine->flat();
}

/// Create flat distributed random numbers in the interval ]0,1]
double Geant4Random::rndm(int i)  {
  if ( !m_inited ) initialize();
  return gRandom->Rndm(i);
}

/// Create a float array of flat distributed random numbers in the interval ]0,1]
void   Geant4Random::rndmArray(int n, float *array)  {
  if ( !m_inited ) initialize();
  gRandom->RndmArray(n,array);
}

/// Create a double array of flat distributed random numbers in the interval ]0,1]
void   Geant4Random::rndmArray(int n, double *array)  {
  if ( !m_inited ) initialize();
  gRandom->RndmArray(n,array);
}

/// Create uniformly disributed random numbers in the interval ]0,x1]
double Geant4Random::uniform(double x1)  {
  if ( !m_inited ) initialize();
  return gRandom->Uniform(x1);
}

/// Create uniformly disributed random numbers in the interval ]x1,x2]
double Geant4Random::uniform(double x1, double x2)  {
  if ( !m_inited ) initialize();
  return gRandom->Uniform(x1,x2);
}

/// Create exponentially distributed random numbers
double Geant4Random::exp(double tau)  {
  if ( !m_inited ) initialize();
  return gRandom->Exp(tau);
}

/// Generates random vectors, uniformly distributed over a circle of given radius.
double Geant4Random::gauss(double mean, double sigma)  {
  if ( !m_inited ) initialize();
  return gRandom->Gaus(mean,sigma);
}

/// Create landau distributed random numbers
double Geant4Random::landau(double mean, double sigma)  {
  if ( !m_inited ) initialize();
  return gRandom->Landau(mean,sigma);
}

/// Create tuple of randum number around a circle with radius r
void   Geant4Random::circle(double &x, double &y, double r)  {
  if ( !m_inited ) initialize();  
  gRandom->Circle(x,y,r);
}

/// Create tuple of randum number on a sphere with radius r
void   Geant4Random::sphere(double &x, double &y, double &z, double r)  {
  if ( !m_inited ) initialize();
  gRandom->Sphere(x,y,z,r);
}

