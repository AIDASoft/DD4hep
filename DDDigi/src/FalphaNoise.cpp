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

/// Framework include files
#include <DDDigi/FalphaNoise.h>

/// C/C++ include files
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace dd4hep::detail;

#ifdef  __GSL_FALPHA_NOISE
#ifndef __CNOISE_C
#define __CNOISE_C

/*
* This code is distributed under GPLv3
* 
* This code generates correlated or colored noise with a 1/f^alpha power 
* spectrum distribution. 
*
* It uses the algorithm by:
* 
* Jeremy Kasdin
* Discrete Simulation of Colored Noise and Stochastic Processes and $ 1/f^\alpha $ Power Law Noise Generation
* Proceedings of the IEEE
* Volume 83, Number 5, 1995, pages 802-827.
* 
* This code uses GSL fast Fourier transform gsl_fft_complex_forward(...) 
* and the GCC rand() functions
* 
* Code Author: Miroslav Stoyanov, Jan 2011
* 
* Copyright (C) 2011  Miroslav Stoyanov
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* Since the GNU General Public License is longer than this entire code, 
* a copy of it can be obtained separately at <http://www.gnu.org/licenses/>
* 
* updated June 2011: fixed a small bug causing "nan" to be returned sometimes
* 
*/

//#include "cnoise.h"
#ifndef __CNOISE_H
#define __CNOISE_H

/*
* This code isdistrubuted under GPLv3
* 
* This code generates corelated or colored noise with 1/f^alpha power spectrum distribution. 
* It uses the algorithm by:
* 
* Jeremy Kasdin
* Discrete Simulation of Colored Noise and Stochastic Processes and $ 1/f^\alpha $ Power Law Noise Generation
* Proceedings of the IEEE
* Volume 83, Number 5, 1995, pages 802-827.
* 
* This code uses GSL fast Fourier transform gsl_fft_complex_forward(...) and the GCC rand() functions
* 
* Code Author: Miroslav Stoyanov, Jan 2011
* 
* Copyright (C) 2011  Miroslav Stoyanov
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* Since the GNU General Public License is longer than this entire code, 
* a copy of it can be obtained separately at <http://www.gnu.org/licenses/>
* 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

double cnoise_uniform01 ( );

void cnoise_two_gaussian_truncated ( double *a, double *b, double std, double range );

void cnoise_generate_colored_noise ( double *x, int N, double alpha, double std );

// generates a vector x of size N with a 1/f^alpha frequency distribution
// std is the standard deviation of the underlying gaussian distribution
// Variables: double *x - Input: allocated vector of size N
//                        Output: a realization of 1/f^alpha noise vector of size N, using an undelying Gaussian (0,std)
//             int N    - Input: the size of the vector
//            double alpha - Input: the decay rate for the power spectrum (i.e. 1/f^alpha
//            double std - Input: the standard deviation of the underlying Gaussian distribution
// NOTE: you should call srand( seed ) before you call dcnoise_generate_colored_noise

void cnoise_generate_colored_noise_uniform( double *x, int N, double alpha, double range );
// same as cnoise_generate_colored_noise_uniform, except the white noise vector comes from
// uniform distribution on (-range,+range)

void cnoise_generate_colored_noise_truncated( double *x, int N, double alpha, double std, double range );
// same as cnoise_generate_colored_noise_uniform, except the white noise vector comes from
// truncated Gaussian distribution with mean 0, standard deviation std and truncated to (-range,range)



#endif


#define _CNOISE_PI 3.14159265358979323846 // pi

/******************************************************************************/

double cnoise_uniform01 ( )

/******************************************************************************/
{
	return ( ( (double) rand() + 1.0 ) / ( (double) RAND_MAX + 1.0 ) );
};
/******************************************************************************/

void cnoise_two_gaussian_truncated ( double *a, double *b, double std, double range )

/******************************************************************************/
{
	double gauss_u = cnoise_uniform01();
	double gauss_v = cnoise_uniform01();
	*a = std * sqrt( - 2 * log( gauss_u ) ) * cos( 2 * _CNOISE_PI * gauss_v );
	*b = std * sqrt( - 2 * log( gauss_u ) ) * sin( 2 * _CNOISE_PI * gauss_v );
	while ( (*a < -range) || (*a > range) ){
		gauss_u = cnoise_uniform01(); gauss_v = cnoise_uniform01();
		*a = std * sqrt( - 2 * log( gauss_u ) ) * cos( 2 * _CNOISE_PI * gauss_v );
	};
	while ( (*b < -range) || (*b > range) ){
		gauss_u = cnoise_uniform01(); gauss_v = cnoise_uniform01();
		*b = std * sqrt( - 2 * log( gauss_u ) ) * cos( 2 * _CNOISE_PI * gauss_v );
	};
};
/******************************************************************************/

void cnoise_generate_colored_noise ( double *x, int N, double alpha, double std )

/******************************************************************************/
{
	int i;
	double tmp;
	double gauss_u, gauss_v;
	double * fh = (double*)malloc( 4*N*sizeof(double) );
	double * fw = (double*)malloc( 4*N*sizeof(double) );
	
	fh[0] = 1.0; fh[1] = 0.0;
	for( i=1; i<N; i++ ){
		fh[2*i] = fh[2*(i-1)] * ( 0.5 * alpha + (double)(i-1) ) / ((double) i );
		fh[2*i+1] = 0.0;
	};
	for( i=0; i<N; i+=2 ){
		gauss_u = cnoise_uniform01();
    gauss_v = cnoise_uniform01();
		fw[2*i]   = std * sqrt( - 2 * log( gauss_u ) ) * cos( 2 * _CNOISE_PI * gauss_v );
    fw[2*i+1] = 0.0;
		fw[2*i+2] = std * sqrt( - 2 * log( gauss_u ) ) * sin( 2 * _CNOISE_PI * gauss_v );
    fw[2*i+3] = 0.0;
	};
	for( i=2*N; i<4*N; i++ ){
    fh[i] = fw[i] = 0.0;
  };
	
	gsl_fft_complex_wavetable* wavetable = gsl_fft_complex_wavetable_alloc(2*N);
	gsl_fft_complex_workspace* workspace = gsl_fft_complex_workspace_alloc(2*N);

	gsl_fft_complex_forward (fh, 1, 2*N, wavetable, workspace);
	gsl_fft_complex_forward (fw, 1, 2*N, wavetable, workspace);
	
	for( i=0; i<N+1; i++ ){
		tmp = fw[2*i];
		fw[2*i]   = tmp*fh[2*i] - fw[2*i+1]*fh[2*i+1];
		fw[2*i+1] = tmp*fh[2*i+1] + fw[2*i+1]*fh[2*i];
	};

	fw[0] /= 2.0; fw[1] /= 2.0;
	fw[2*N] /= 2.0; fw[2*N+1] /= 2.0;
	
	for( i=N+1; i<2*N; i++ ){
		fw[2*i] = 0.0; fw[2*i+1] = 0.0;
	};
	
	gsl_fft_complex_inverse( fw, 1, 2*N, wavetable, workspace);
	
	for( i=0; i<N; i++ ){
		x[i] = 2.0*fw[2*i];
	};

	free( fh );
	free( fw );

	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);
};
/******************************************************************************/

void cnoise_generate_colored_noise_uniform( double *x, int N, double alpha,
  double range )

/******************************************************************************/
{
	gsl_fft_complex_wavetable * wavetable;
	gsl_fft_complex_workspace * workspace;
	
	int i;
	double tmp;
	double * fh = (double*)malloc( 4*N*sizeof(double) );
	double * fw = (double*)malloc( 4*N*sizeof(double) );
	
	fh[0] = 1.0; fh[1] = 0.0;
	fw[0] = 2.0*range*cnoise_uniform01()-range; fw[1] = 0.0;
	for( i=1; i<N; i++ ){
		fh[2*i] = fh[2*(i-1)] * ( 0.5 * alpha + (double)(i-1) ) / ((double) i );
		fh[2*i+1] = 0.0;
		fw[2*i] = 2.0*range*cnoise_uniform01()-range; fw[2*i+1] = 0.0;
	};
	for( i=2*N; i<4*N; i++ ){ fh[i] = 0.0; fw[i] = 0.0; };
	
	wavetable = gsl_fft_complex_wavetable_alloc(2*N);
	workspace = gsl_fft_complex_workspace_alloc(2*N);

	gsl_fft_complex_forward (fh, 1, 2*N, wavetable, workspace);
	gsl_fft_complex_forward (fw, 1, 2*N, wavetable, workspace);
	
	for( i=0; i<N+1; i++ ){
		tmp = fw[2*i];
		fw[2*i]   = tmp*fh[2*i] - fw[2*i+1]*fh[2*i+1];
		fw[2*i+1] = tmp*fh[2*i+1] + fw[2*i+1]*fh[2*i];
	};

	fw[0] /= 2.0; fw[1] /= 2.0;
	fw[2*N] /= 2.0; fw[2*N+1] /= 2.0;
	
	for( i=N+1; i<2*N; i++ ){
		fw[2*i] = 0.0; fw[2*i+1] = 0.0;
	};
	
	gsl_fft_complex_inverse( fw, 1, 2*N, wavetable, workspace);
	
	for( i=0; i<N; i++ ){
		x[i] = 2.0*fw[2*i];
	};

	free( fh );
	free( fw );

	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);
};
/******************************************************************************/

void cnoise_generate_colored_noise_truncated( double *x, int N, double alpha, double std, double range )

/******************************************************************************/
{
	gsl_fft_complex_wavetable * wavetable;
	gsl_fft_complex_workspace * workspace;
	
	int i;
	double tmp;
	double * fh = (double*)malloc( 4*N*sizeof(double) );
	double * fw = (double*)malloc( 4*N*sizeof(double) );
	
	fh[0] = 1.0; fh[1] = 0.0;
	for( i=1; i<N; i++ ){
		fh[2*i] = fh[2*(i-1)] * ( 0.5 * alpha + (double)(i-1) ) / ((double) i );
		fh[2*i+1] = 0.0;
	};
	for( i=0; i<N; i+=2 ){
		cnoise_two_gaussian_truncated( &fw[2*i], &fw[2*i+2], std, range );
	};
	for( i=2*N; i<4*N; i++ ){ fh[i] = 0.0; fw[i] = 0.0; };
	
	wavetable = gsl_fft_complex_wavetable_alloc(2*N);
	workspace = gsl_fft_complex_workspace_alloc(2*N);

	gsl_fft_complex_forward (fh, 1, 2*N, wavetable, workspace);
	gsl_fft_complex_forward (fw, 1, 2*N, wavetable, workspace);
	
	for( i=0; i<N+1; i++ ){
		tmp = fw[2*i];
		fw[2*i]   = tmp*fh[2*i] - fw[2*i+1]*fh[2*i+1];
		fw[2*i+1] = tmp*fh[2*i+1] + fw[2*i+1]*fh[2*i];
	};

	fw[0] /= 2.0; fw[1] /= 2.0;
	fw[2*N] /= 2.0; fw[2*N+1] /= 2.0;
	
	for( i=N+1; i<2*N; i++ ){
		fw[2*i] = 0.0; fw[2*i+1] = 0.0;
	};
	
	gsl_fft_complex_inverse( fw, 1, 2*N, wavetable, workspace);
	
	for( i=0; i<N; i++ ){
		x[i] = 2.0*fw[2*i];
	};

	free ( fh );
	free ( fw );

	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);
};

#endif
#endif

/// Initializing constructor. Leaves the enerator full functional
FalphaNoise::FalphaNoise(size_t poles, double alpha, double variance)   {
  init(poles, alpha, variance);
}

/// Initializing constructor with 5 poles. Leaves the enerator full functional
FalphaNoise::FalphaNoise(double alpha, double variance)   {
  init(5, alpha, variance);
}

/// Initialize the 1/f**alpha random generator. If already called reconfigures.
void FalphaNoise::init(size_t poles, double alpha, double variance)    {
  m_poles    = poles;
  m_alpha    = alpha;
  m_variance = variance;
  if ( alpha < 0e0 || alpha > 2e0 )   {
    throw std::runtime_error("FalphaNoise: Invalid value for alpha: must be inside the interval [0,2]");
  }
  else if ( variance < 1e-10 )   {
    throw std::runtime_error("FalphaNoise: Invalid variance. Must be bigger than NULL!");
  }
  m_values.clear();
  m_multipliers.clear();
  m_distribution = std::normal_distribution<double>(0.0, m_variance);
  m_values.resize(m_poles+1,0e0);
  m_multipliers.reserve(m_poles);

  double val = 1e0;
  for ( size_t i=0; i<m_poles; ++i)   {
    val *= (double(i) - alpha/2e0) / double(i+1);
    m_multipliers.emplace_back(val);
  }
  // Fill the history with random values
  std::default_random_engine generator;
  for ( size_t i=0; i < 5*m_poles; i++)
    (*this)(generator);
}

/// Approximative compute proper variance and apply computed value
void FalphaNoise::normalize(size_t shots)    {
  std::default_random_engine engine;
  normalize(engine, shots);
}

/// Internal usage to normalize using user defined random engine
void FalphaNoise::normalizeVariance(const random_engine_wrapper& generator, size_t shots)  {
  double mean = 0e0, mean2 = 0e0, var;
  auto tmp = m_multipliers;
  double val = 1e0;
  /// We have to correct for the case of white noise: alpha=0
  for ( size_t i=0; i<m_poles; ++i)   {
    val *= (double(i) - 0.5) / double(i+1);
    m_multipliers[i] = val;
  }
  for ( size_t i=0; i < shots; i++)  {
    var = (*this)(generator);
    mean  += var;
    mean2 += var*var;
  }
  mean  /= double(shots);
  var = std::sqrt(mean2/double(shots) - mean*mean);
  m_variance *= m_variance/var;
  m_multipliers = tmp;
  m_distribution = std::normal_distribution<double>(0.0, m_variance);
}

/// Retrieve the next random number of the sequence
double FalphaNoise::compute(double rndm_value)   {
#ifdef  __GSL_FALPHA_NOISE
  if ( arr == nullptr )  {
    arr = new double[1000];
  }
  if ( ptr < 0 )  {
    cout << "Alpha: " << m_alpha << " sigma: " << m_variance << endl;
    cnoise_generate_colored_noise (arr, 1000, m_alpha, m_variance);
    ptr = 999;
  }
  --ptr;
  return arr[ptr+1];
#else
  for ( int i=m_poles-1; i >= 0; --i )   {
    rndm_value -= m_multipliers[i] * m_values[i];
    m_values[i+1] = m_values[i]; // highest index always drops off...
  }
  m_values[0] = rndm_value;
  return rndm_value;
#endif
}
