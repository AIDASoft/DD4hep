// $Id: Geant4Mapping.cpp 588 2013-06-03 11:41:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DDG4/Geant4GeometryInfo.h"
using namespace DD4hep::Simulation;

/// Default constructor
Geant4GeometryInfo::Geant4GeometryInfo() 
: TNamed("Geant4GeometryInfo","Geant4GeometryInfo"), valid(false)
{
}

/// Default destructor
Geant4GeometryInfo::~Geant4GeometryInfo()   {
}
