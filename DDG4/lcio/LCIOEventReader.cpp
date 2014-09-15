// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
// @author  P.Kostka (main author)
// @author  M.Frank  (code reshuffeling into new DDG4 scheme)
//
//====================================================================

// Framework include files
#include "LCIOEventReader.h"

/// Initializing constructor
DD4hep::Simulation::LCIOEventReader::LCIOEventReader(const std::string& nam) : m_name(nam)   {
}

/// Default destructor
DD4hep::Simulation::LCIOEventReader::~LCIOEventReader()   {
}
