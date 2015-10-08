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
#ifndef DD4HEP_GEANT4RANDOM_H
#define DD4HEP_GEANT4RANDOM_H

// Framework include files
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <string>

// Forward declarations
class TRandom;

/// CLHEP namespace 
namespace CLHEP   {  class HepRandomEngine;   }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Exec;

    /// Mini interface to THE random generator of the application
    /**
     *  Mini interface to THE random generator of the application.
     *  Necessary, that on every object creates its own instance, but accesses
     *  the main instance avaible throu the Geant4Context.
     *
     *  This is mandatory to ensure reproducability of the event generation
     *  process. Particular objects may use a dependent generator from
     *  an experiment framework like GAUDI.
     *
     *  This main interface is supposed to be stable. Unclear however is
     *  if the generation functions will have to become virtual....
     *  Future will tell us.
     *
     *  The first instance of the random action is automatically set
     *  to be the Geant4 instance. If another instance should be used by 
     *  Geant4, use setMainInstance(Geant4Random* ptr).
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Random : public Geant4Action   {
      friend class Geant4Exec;

    protected:
      /// Property: File name if initialized from file. If set, engine name and seeds are ignored
      std::string  m_file;
      /// Property: Engine type. default: "HepJamesRandom"
      std::string  m_engineType;
      /// Property: Initial random seed. Default: 123456789
      long         m_seed, m_luxury;
      /// Property: Indicator to replace the ROOT gRandom instance
      bool         m_replace;
      
      /// Reference to the CLHEP random number engine (valid only after initialization)
      CLHEP::HepRandomEngine* m_engine;

      /// Reference to ROOT random instance
      TRandom *m_rootRandom, *m_rootOLD;
      /// Flag to remember initialization
      bool m_inited;
      
    public:
      /// Standard constructor
      /** Please Note:
       *  Should be used only for initialization of the main instance.
       *  Subsequent usage should be invoked using the class member
       *  Geant4Random::instance().
       *
       *  @param   context     Geant4 context for this action.
       *  @param   name        Name of the action object
       */
      Geant4Random(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4Random();

      /// Access the main Geant4 random generator instance. Must be created before used!
      static Geant4Random* instance(bool throw_exception=true);
      /// Make this random generator instance the one used by Geant4.
      /** Returns reference to previous instance. It is up to the user
       *  to manage the reference.
       *  Caveat: other code may hold references to this instance.
       *
       *  @param   ptr         Reference to main random number generator
       *  @return              Reference to previous random number generator instance
       */
      static Geant4Random* setMainInstance(Geant4Random* ptr);

      /// Initialize the instance. 
      /** Called either by user or on request of the first random number.
       *  To propagate the engine to Geant4, initialize MUST be called by 
       *  the user.
       */
      void initialize();
      
      /** Access to the CLHEP random number engine. For further doc see CLHEP/Random/RandomEngine.h  */

      /// CLHEP random number engine (valid after initialization only)
      CLHEP::HepRandomEngine* engine()      {  return m_engine;   }
      
      /// Should initialise the status of the algorithm according to seed.
      virtual void setSeed(long seed);
      /// Should initialise the status of the algorithm
      /** Initialization according to the zero terminated
       *  array of seeds. It is allowed to ignore one or 
       *  many seeds in this array.
       */
      virtual void setSeeds(const long * seeds, int size);
      /// Should save on a file specific to the instantiated engine in use the current status.
      virtual void saveStatus( const char filename[] = "Config.conf") const;
      /// Should read from a file and restore the last saved engine configuration.
      virtual void restoreStatus( const char filename[] = "Config.conf" );
      /// Should dump the current engine status on the screen.
      virtual void showStatus() const;

      /** Basic random generator functions  */

      /// Create flat distributed random numbers in the interval ]0,1]
      double rndm(int i=0);
      /// Create a float array of flat distributed random numbers in the interval ]0,1]
      void   rndmArray(int n, float *array);
      /// Create a double array of flat distributed random numbers in the interval ]0,1]
      void   rndmArray(int n, double *array);
      /// Create uniformly disributed random numbers in the interval ]0,x1]
      double uniform(double x1=1);
      /// Create uniformly disributed random numbers in the interval ]x1,x2]
      double uniform(double x1, double x2);

      /// Create exponentially distributed random numbers
      double exp(double tau);
      /// Create gaussian distributed random numbers
      double gauss(double mean=0, double sigma=1);
      /// Create landau distributed random numbers
      double landau(double mean=0, double sigma=1);
      /// Create tuple of randum number around a circle with radius r
      void   circle(double &x, double &y, double r);
      /// Create tuple of randum number on a sphere with radius r
      void   sphere(double &x, double &y, double &z, double r);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_GEANT4RANDOM_H
