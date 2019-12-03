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
#ifndef DD4HEP_DDDIGI_DIGIRANDOMGENERATOR_H
#define DD4HEP_DDDIGI_DIGIRANDOMGENERATOR_H

/// Framework include files

/// C/C++ include files
#include <functional>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Generic generator source with a random distribution
    /**
     *  Generate random numbers according to a given distribution
     *
     *  Attention:
     *  ================================================================ 
     *  Before this distribution generator is usable, the 
     *  function object 'engine' must be validated!
     *  ================================================================ 
     *
     *  Please note:
     *  ============
     *  This code comes straight from the TRandom class of ROOT
     *  See for details:  https:* root.cern.ch/root/html534/TRandom.html
     * 
     *  The basic [0...1] generator is a std::function object to allow users
     *  to plug their own implementations
     * 
     *  I know this is not nice, but I did not see any other way to overcome
     *  the virtualization mechanism
     * 
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiRandomGenerator {
    public:
      std::function<double()>  engine;
    public:
      /// Initializing constructor
      DigiRandomGenerator() = default;
      /// Default destructor
      virtual ~DigiRandomGenerator() = default;
      double random()  const;
      double uniform(double x1 = 1.0)   const;
      double uniform(double x1, double x2)   const;
      int    binomial(int ntotal, double probabaility)  const;
      double exponential(double tau)  const;
      double gaussian(double mean = 0.0, double sigma = 1.0)  const;
      double landau  (double mean = 0.0, double sigma = 1.0)  const;
      double breitWigner(double mean = 0.0, double gamma = 1.0)  const;
      double poisson(double mean)  const;
      void	 rannor(float& a, float& b)   const;
      void	 rannor(double& a, double& b)   const;
      void   sphere(double& x, double& y, double& z, double r)   const;
      void   circle(double &x, double &y, double r)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIRANDOMGENERATOR_H
