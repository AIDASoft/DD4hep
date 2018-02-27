//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// @author  R.Ete (main author)
//
//====================================================================

// Framework include files
#include "LCIOEventParameters.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Initializing constructor
LCIOEventParameters::LCIOEventParameters()
  : m_runNumber(0), m_eventNumber(0)
{
}

/// Default destructor
LCIOEventParameters::~LCIOEventParameters()  {
}

/// Set the event parameters
void LCIOEventParameters::setParameters(int rNumber, 
                                        int evtNumber, 
                                        const EVENT::LCParameters& parameters)
{
  m_runNumber = rNumber;
  m_eventNumber = evtNumber;
  copyLCParameters(parameters, m_eventParameters);
}

/// Get the run number
int LCIOEventParameters::runNumber() const  {
  return m_runNumber;
}

/// Get the event number
int LCIOEventParameters::eventNumber() const  {
  return m_eventNumber;
}

/// Get the event parameters
const IMPL::LCParametersImpl& LCIOEventParameters::eventParameters() const  {
  return m_eventParameters;
}

/// Copy the paramaters from source to destination
void LCIOEventParameters::copyLCParameters(const EVENT::LCParameters& source, 
                                           EVENT::LCParameters& destination)  
{
  EVENT::StringVec intKeys; source.getIntKeys(intKeys);
  EVENT::StringVec floatKeys; source.getFloatKeys(floatKeys);
  EVENT::StringVec stringKeys; source.getStringKeys(stringKeys);
  
  for(unsigned int i=0; i<intKeys.size(); ++i )  {
    EVENT::IntVec intVec;
    source.getIntVals(intKeys.at(i),intVec);
    destination.setValues(intKeys.at(i),intVec);
  }
  
  for(unsigned int i=0; i<floatKeys.size(); ++i )  {
    EVENT::FloatVec floatVec;
    source.getFloatVals(floatKeys.at(i),floatVec);
    destination.setValues(floatKeys.at(i),floatVec);
  }
  
  for(unsigned int i=0; i<stringKeys.size(); ++i )  {
    EVENT::StringVec stringVec;
    source.getStringVals(stringKeys.at(i),stringVec);
    destination.setValues(stringKeys.at(i),stringVec);
  }
}


