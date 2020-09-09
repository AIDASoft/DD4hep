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
#ifndef DDDIGI_DIGIRANDOMENGINE_H
#define DDDIGI_DIGIRANDOMENGINE_H
#if 0
/// Framework include files


/// C/C++ include files
#include <random>
#include <functional>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Generic engine source with a random distribution
    /**
     *  Generate random engine as it appears e.g. from electronic engine
     *  with a given mean and a given sigma
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiRandomEngine {
    public:
      typedef unsigned int result_type;
      std::function<double()> m_generator;
    public:
      /// Initializing constructor
      template <typename ENGINE> DigiRandomEngine(ENGINE* engine);
      /// Default destructor
      virtual ~DigiRandomEngine();
      static constexpr result_type min()   { return std::default_random_engine::min(); }
      static constexpr result_type max()   { return std::default_random_engine::max(); }
      static constexpr double      range() { return double(max()) - double(min());     }
      /// Callback to read event randomengine
      unsigned int operator()()  const     { return m_generator();                     }
    };
    /// Initializing constructor
    template <typename ENGINE> DigiRandomEngine::DigiRandomEngine(ENGINE* engine)   {
      m_generator = [this,engine]()  {
        static constexpr double norm =
          DigiRandomEngine::range() / (double(ENGINE::max() - ENGINE::min()));
        return result_type(double((*engine())) * norm);
      };
    }
  }    // End namespace digi
}      // End namespace dd4hep
#endif
#endif // DDDIGI_DIGIRANDOMENGINE_H
