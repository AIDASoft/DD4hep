// $Id: Geant4Mapping.cpp 588 2013-06-03 11:41:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DDG4/Geant4Mapping.h"
#include "G4PVPlacement.hh"
#include <stdexcept>

using namespace DD4hep::Simulation;
using namespace DD4hep::Geometry;
using namespace std;

/// Initializing Constructor
Geant4Mapping::Geant4Mapping( LCDD& lcdd, G4GeometryInfo* data) 
  : m_lcdd(lcdd), m_dataPtr(data)
{
}

/// Standard destructor
Geant4Mapping::~Geant4Mapping()  {
  if ( m_dataPtr ) delete m_dataPtr;
  m_dataPtr = 0;
}

/// Possibility to define a singleton instance
Geant4Mapping& Geant4Mapping::instance()  {
  static Geant4Mapping inst(LCDD::getInstance(),0);
  return inst;
}

/// When resolving pointers, we must check for the validity of the data block
void Geant4Mapping::checkValidity() const   {
  if ( m_dataPtr ) return;
  throw runtime_error("Geant4Mapping: Attempt to access an invalid data block!");
}

/// Release data and pass over the ownership
Geant4Mapping::G4GeometryInfo* Geant4Mapping::detach()   {
  G4GeometryInfo* p = m_dataPtr;
  m_dataPtr = 0;
  return p;
}

/// Set a new data block
void Geant4Mapping::attach(G4GeometryInfo* data)   {
  m_dataPtr = data;
}

/// Accessor to resolve G4 placements
G4PVPlacement* Geant4Mapping::g4Placement(const TGeoNode* node)  const   {
  checkValidity();
  const PlacementMap& m = m_dataPtr->g4Placements;
  PlacementMap::const_iterator i = m.find(node);
  if ( i !=  m.end() ) return (*i).second;
  return 0;
}

/// Accessor to resolve geometry placements
PlacedVolume Geant4Mapping::placement(const G4VPhysicalVolume* node)  const   {
  checkValidity();
  const PlacementMap& m = m_dataPtr->g4Placements;
  for(PlacementMap::const_iterator i = m.begin(); i != m.end(); ++i)
    if ( (*i).second == node ) return PlacedVolume((*i).first);
  return PlacedVolume(0);
}
