//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
//
// This code is originally written by Sampo Niskanen in java.
// Translated to C++ by M.Frank
//
// See original copyright notice below
//==========================================================================
//
// PinkNoise.java  -  a pink noise generator
//
// Copyright (c) 2008, Sampo Niskanen <sampo.niskanen@iki.fi>
// All rights reserved.
// Source:  http://www.iki.fi/sampo.niskanen/PinkNoise/
//
// Distrubuted under the BSD license:
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  - Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// 
//  - Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials provided
// with the distribution.
//
//  - Neither the name of the copyright owners nor contributors may be
// used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//==========================================================================

#ifndef DD4HEP_DDDIGI_FALPHANOISE_H
#define DD4HEP_DDDIGI_FALPHANOISE_H

/// C/C++ include files
#include <vector>
#include <random>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace detail {

    /// Source of pink noise with a power spectrum proportional to 1/f^alpha.
    /**
     * A class that provides a source of pink noise with a power spectrum
     * density (PSD) proportional to 1/f^alpha.  "Regular" pink noise has a
     * PSD proportional to 1/f, i.e. alpha=1.  However, many natural
     * systems may require a different PSD proportionality.  The value of
     * alpha may be from 0 to 2, inclusive.  The special case alpha=0
     * results in white noise (directly generated random numbers) and
     * alpha=2 results in brown noise (integrated white noise).
     * <p>
     * The values are computed by applying an IIR filter to generated
     * Gaussian random numbers.  The number of poles used in the filter
     * may be specified.  For each number of poles there is a limiting
     * frequency below which the PSD becomes constant.  Values as low as
     * 1-3 poles produce relatively good results, however these values
     * will be concentrated near zero.  Using a larger number of poles
     * will allow more low frequency components to be included, leading to
     * more variation from zero.  However, the sequence is stationary,
     * that is, it will always return to zero even with a large number of
     * poles.
     * <p>
     * The distribution of values is very close to Gaussian with mean
     * zero, but the variance depends on the number of poles used.
     * The resulting distribution is almost Gaussian, but has a relatively 
     * larger amount of large values. To re-normalize the variance a
     * calibration method normalize() is provided.
     * <p>
     * The IIR filter used by this class is presented by N. Jeremy Kasdin,
     * Proceedings of the IEEE, Vol. 83, No. 5, May 1995, p. 822.
     * 
     *  \author  Sampo Niskanen <sampo.niskanen@iki.fi>
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class FalphaNoise    {
    public:
      /// Minimal class to mimic a std::random_engine
      struct random_engine_wrapper  {
        typedef unsigned int result_type;
        static constexpr result_type min()      { return std::default_random_engine::min(); }
        static constexpr result_type max()      { return std::default_random_engine::max(); }
        static constexpr double      range()    { return double(max()) - double(min());     }
        virtual result_type operator()()  const = 0;
      };
      /// Concrete wrapper of a user defined random engine
      template <typename ENGINE> struct random_engine : public random_engine_wrapper {
        ENGINE& engine;
        random_engine(ENGINE& e) : engine(e) {}
        virtual result_type operator()()   const  {
          static constexpr double norm = random_engine_wrapper::range() / (double(ENGINE::max() - ENGINE::min()));
          return result_type(double(engine()) * norm);
        }
      };

    protected:
#ifdef  __GSL_FALPHA_NOISE
      long ptr = -1;
      double * arr = nullptr;
#endif
      /// Number of poles used for the approximation
      size_t                  m_poles    = 0;
      /// The variance of the pink noise distribution
      double                  m_variance = -1e0;
      /// The alpha coefficient for the 1/f**alpha generated noise
      double                  m_alpha    = -1e0;
      /// Array of multipliers
      std::vector<double>     m_multipliers {};
      /// Array of buffered values
      std::vector<double>     m_values {};
      /// Normal distribution with variance for the white noise generation
      std::normal_distribution<double> m_distribution;

      /// Internal usage for the transformation from the normal distruted white noise to pink noise
      double compute(double gaussian);
      /// Internal implementation of the normalization of the variance
      void normalizeVariance(const random_engine_wrapper& engine, size_t shots);

    public:
      /// Default constructor. Requires a later call to init(...)
      FalphaNoise() = default;
      /// Initializing constructor. Leaves the enerator full functional
      FalphaNoise(size_t poles, double alpha, double variance);
      /// Initializing constructor with 5 poles. Leaves the enerator full functional
      FalphaNoise(double alpha, double variance);
      /// Default destructor
      virtual ~FalphaNoise() = default;
      /// Access variance value
      double variance()   const   {  return m_variance;  }
      /// Access alpha value
      double alpha()      const   {  return m_alpha;     }
      /// Initialize the 1/f**alpha random generator. If already called reconfigures.
      void init(size_t poles, double alpha, double variance);
      /// Approximatively compute proper variance and apply computed value
      void normalize(size_t shots=10000);
      /// Approximatively compute proper variance using external ranfom engine and apply computed value
      template <typename ENGINE> void normalize(ENGINE& engine, size_t shots=10000);
      /// Retrieve the next random number of the sequence
      template <typename ENGINE> double operator()(ENGINE& engine);
    };

    /// Retrieve the next random number of the sequence
    template <typename ENGINE> inline double FalphaNoise::operator()(ENGINE& engine)   {
      return compute(m_distribution(engine));
    }
    /// Approximatively compute proper variance using external ranfom engine and apply computed value
    template <typename ENGINE> void FalphaNoise::normalize(ENGINE& engine, size_t shots)   {
      normalizeVariance(random_engine<ENGINE>(engine), shots);
    }
  }    // End namespace detail
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_FALPHANOISE_H
