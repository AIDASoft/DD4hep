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
//
// Please note: This code comes straight from the TRandom class of ROOT
// See for details:  https://root.cern.ch/root/html534/TRandom.html
//
// The basic [0...1] generator is a std::function object to allow users
// to plug their own implementations
//
// I know this is not nice, but I did not see any other way to overcome
// the virtualization mechanism
//
// M.Frank
//==========================================================================

// Framework include files
#include "DDDigi/DigiRandomGenerator.h"
#include <Math/ProbFuncMathCore.h>
#include <Math/SpecFuncMathCore.h>
#include "Math/QuantFuncMathCore.h"
#include <cmath>


using namespace dd4hep::digi;

static constexpr double PIOVER2 = M_PI / 2.0;
static constexpr double TWOPI   = M_PI * 2.0;

double DigiRandomGenerator::random()  const   {
  return engine();
}

double DigiRandomGenerator::uniform(double x1)   const   {
  double ans = engine();
  return x1*ans;
}

double DigiRandomGenerator::uniform(double x1, double x2)   const   {
  double ans= engine();
  return x1 + (x2-x1)*ans;
}

int    DigiRandomGenerator::binomial(int ntot, double prob)  const   {
  if (prob < 0 || prob > 1) return 0;
  int n = 0;
  for (int i=0;i<ntot;i++) {
    if (engine() > prob) continue;
    n++;
  }
  return n;
}

double DigiRandomGenerator::exponential(double tau)  const   {
  double x = engine();              // uniform on ] 0, 1 ]
  double t = -tau * std::log( x ); // convert to exponential distribution
  return t;
}

double DigiRandomGenerator::gaussian(double mean, double sigma)  const   {
  const double kC1 = 1.448242853;
  const double kC2 = 3.307147487;
  const double kC3 = 1.46754004;
  const double kD1 = 1.036467755;
  const double kD2 = 5.295844968;
  const double kD3 = 3.631288474;
  const double kHm = 0.483941449;
  const double kZm = 0.107981933;
  const double kHp = 4.132731354;
  const double kZp = 18.52161694;
  const double kPhln = 0.4515827053;
  const double kHm1 = 0.516058551;
  const double kHp1 = 3.132731354;
  const double kHzm = 0.375959516;
  const double kHzmp = 0.591923442;
  /*zhm 0.967882898*/

  const double kAs = 0.8853395638;
  const double kBs = 0.2452635696;
  const double kCs = 0.2770276848;
  const double kB  = 0.5029324303;
  const double kX0 = 0.4571828819;
  const double kYm = 0.187308492 ;
  const double kS  = 0.7270572718 ;
  const double kT  = 0.03895759111;

  double result;
  double rn,x,y,z;

  do {
    y = engine();

    if (y>kHm1) {
      result = kHp*y-kHp1; break; }

    else if (y<kZm) {
      rn = kZp*y-1;
      result = (rn>0) ? (1+rn) : (-1+rn);
      break;
    }

    else if (y<kHm) {
      rn = engine();
      rn = rn-1+rn;
      z = (rn>0) ? 2-rn : -2-rn;
      if ((kC1-y)*(kC3+std::abs(z))<kC2) {
        result = z; break; }
      else {
        x = rn*rn;
        if ((y+kD1)*(kD3+x)<kD2) {
          result = rn; break; }
        else if (kHzmp-y<exp(-(z*z+kPhln)/2)) {
          result = z; break; }
        else if (y+kHzm<exp(-(x+kPhln)/2)) {
          result = rn; break; }
      }
    }

    while (1) {
      x = engine();
      y = kYm * engine();
      z = kX0 - kS*x - y;
      if (z>0)
        rn = 2+y/x;
      else {
        x = 1-x;
        y = kYm-y;
        rn = -(2+y/x);
      }
      if ((y-kAs+x)*(kCs+x)+kBs<0) {
        result = rn; break; }
      else if (y<x+kT)
        if (rn*rn<4*(kB-log(x))) {
          result = rn; break; }
    }
  } while(0);

  return mean + sigma * result;
}

double DigiRandomGenerator::landau  (double mu, double sigma)  const   {
  if (sigma <= 0) return 0;
  double x = engine();
  double res = mu + ROOT::Math::landau_quantile(x, sigma);
  return res;
}

double DigiRandomGenerator::breitWigner(double mean, double gamma)  const   {
  double rval = 2*engine() - 1;
  double displ = 0.5*gamma*std::tan(rval*PIOVER2);
  return (mean+displ);
}

double DigiRandomGenerator::poisson(double mean)  const   {
  int n;
  if (mean <= 0) return 0;
  if (mean < 25) {
    double expmean = std::exp(-mean);
    double pir = 1;
    n = -1;
    while(1) {
      n++;
      pir *= engine();
      if (pir <= expmean) break;
    }
    return static_cast<double>(n);
  }
  // for large value we use inversion method
  else if (mean < 1E9) {
    double em, t, y;
    double sq, alxm, g;
    double pi = M_PI;

    sq = std::sqrt(2.0*mean);
    alxm = std::log(mean);
    g = mean*alxm - ::ROOT::Math::lgamma(mean + 1.0);

    do {
      do {
        y = std::tan(pi*engine());
        em = sq*y + mean;
      } while( em < 0.0 );

      em = std::floor(em);
      t = 0.9*(1.0 + y*y)* std::exp(em*alxm - ::ROOT::Math::lgamma(em + 1.0) - g);
    } while( engine() > t );

    return em;

  } else {
    // use Gaussian approximation vor very large values
    return gaussian(0,1)*std::sqrt(mean) + mean +0.5;
  }
}

void	 DigiRandomGenerator::rannor(float& a, float& b)   const   {
  double r, x, y, z;

  y = engine();
  z = engine();
  x = z * 6.28318530717958623;
  r = std::sqrt(-2*std::log(y));
  a = (float)(r * std::sin(x));
  b = (float)(r * std::cos(x));
}

void	 DigiRandomGenerator::rannor(double& a, double& b)   const   {
  double r, x, y, z;

  y = engine();
  z = engine();
  x = z * 6.28318530717958623;
  r = std::sqrt(-2*std::log(y));
  a = (float)(r * std::sin(x));
  b = (float)(r * std::cos(x));
}

void   DigiRandomGenerator::sphere(double& x, double& y, double& z, double r)   const   {
  double a=0,b=0,r2=1;
  while (r2 > 0.25) {
    a  = engine() - 0.5;
    b  = engine() - 0.5;
    r2 =  a*a + b*b;
  }
  z = r* ( -1. + 8.0 * r2 );

  double scale = 8.0 * r * std::sqrt(0.25 - r2);
  x = a*scale;
  y = b*scale;
}

void   DigiRandomGenerator::circle(double &x, double &y, double r)   const  {
  double phi = uniform(0,TWOPI);
  x = r*std::cos(phi);
  y = r*std::sin(phi);
}
