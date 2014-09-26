// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4DATADUMP_H
#define DD4HEP_DDG4_GEANT4DATADUMP_H

// Framework include files
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Particle.h"

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Class to dump the records of the intrinsic Geant4 event model.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DataDump  {
    public:
      typedef Geant4Particle                       Particle;
      typedef std::vector<Particle*>               Particles;

      typedef SimpleTracker::Hit                   TrackerHit;
      typedef std::vector<SimpleTracker::Hit*>     TrackerHits;

      typedef SimpleCalorimeter::Hit               CalorimeterHit;
      typedef std::vector<SimpleCalorimeter::Hit*> CalorimeterHits;

    protected:
      /// Tag variable
      std::string m_tag;

    public:
      /// Default constructor
      Geant4DataDump(const std::string& tag);
      /// Standard destructor
      virtual ~Geant4DataDump();

      /// Print a single particle to the output logging using the specified print level
      void print(PrintLevel level, Geant4ParticleHandle p)  const;
      /// Print the particle container to the output logging using the specified print level
      void print(PrintLevel level, const std::string& container, const Particles* parts);

      /// Print a single tracker hit to the output logging using the specified print level
      void print(PrintLevel level, const TrackerHit* h);
      /// Print the tracker hits container to the output logging using the specified print level
      void print(PrintLevel level, const std::string& container, const TrackerHits* hits);

      /// Print a calorimeter tracker hit to the output logging using the specified print level
      void print(PrintLevel level, const CalorimeterHit* h);
      /// Print the calorimeter hits container to the output logging using the specified print level
      void print(PrintLevel level, const std::string& container, const CalorimeterHits* hits);

    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif /* DD4HEP_DDG4_GEANT4DATADUMP_H */
