// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Random.h"

#include "TRandom1.h"

// C/C++ include files
#include <cmath>

using namespace std;
using namespace DD4hep::Simulation;

/// Default constructor
Geant4Random::Geant4Random()   {
  if ( 0 == gRandom ) gRandom = new TRandom1();
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Random::~Geant4Random()  {
  InstanceCount::decrement(this);
}

void   Geant4Random::circle(double &x, double &y, double r)  {
  gRandom->Circle(x,y,r);
}

//
double Geant4Random::exp(double tau)  {
  return gRandom->Exp(tau);
}

//
double Geant4Random::gauss(double mean, double sigma)  {
  return gRandom->Gaus(mean,sigma);
}

//
double Geant4Random::landau(double mean, double sigma)  {
  return gRandom->Landau(mean,sigma);
}

//
double Geant4Random::rndm(int i)  {
  return gRandom->Rndm(i);
}

//
void   Geant4Random::rndmArray(int n, float *array)  {
  gRandom->RndmArray(n,array);
}

//
void   Geant4Random::rndmArray(int n, double *array)  {
  gRandom->RndmArray(n,array);
}

//
void   Geant4Random::sphere(double &x, double &y, double &z, double r)  {
  gRandom->Sphere(x,y,z,r);
}

//
double Geant4Random::uniform(double x1)  {
  return gRandom->Uniform(x1);
}

//
double Geant4Random::uniform(double x1, double x2)  {
  return gRandom->Uniform(x1,x2);
}

