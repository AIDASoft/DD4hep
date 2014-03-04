// $Id: Readout.cpp 985 2014-01-30 13:50:10Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Alignment.h"
#include "DD4hep/InstanceCount.h"
#include "MatrixHelpers.h"

// ROOT include files
#include "TGeoMatrix.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace  {
  int _align(TGeoPhysicalNode* n, TGeoMatrix* transform, bool check, double overlap) {
    if ( n )  {
      n->Align(transform, 0, check, overlap);
      return 1;
    }
    throw runtime_error("DD4hep: Cannot align non existing physical node.");
  }
}

/// Default constructor
Alignment::Alignment() : Handle<TGeoPhysicalNode>() {
}

/// Copy constructor
Alignment::Alignment(const Alignment& c) : Handle<TGeoPhysicalNode>(c) {
}

/// Initializing constructor to create a new object
Alignment::Alignment(const string& path) {
  //assign(new TGeoPhysicalNode(path.c_str()), path, "*");
  m_element = new TGeoPhysicalNode(path.c_str());
}

/// Assignment operator
Alignment& Alignment::operator=(const Alignment& c)  {
  if ( &c != this )  {
    m_element = c.ptr();
  }
  return *this;
}

/// Number of nodes in this branch
int Alignment::numNodes() const  {
  if ( isValid() )  {
    return ptr()->GetLevel();
  }
  throw runtime_error("DD4hep: Attempt to access invalid aignment object. [Invalid Handle]");
}

/// Access the placement of this node
PlacedVolume Alignment::placement()   const   {
  if ( isValid() )  {
    return PlacedVolume(ptr()->GetNode(0));
  }
  throw runtime_error("DD4hep: Attempt to access invalid aignment object. [Invalid Handle]");
}

/// Access the placement of a node in the chain of placements for this branch
PlacedVolume Alignment::nodePlacement(int level)   const   {
  if ( isValid() )  {
    PlacedVolume pv = PlacedVolume(ptr()->GetNode(level));
    if ( pv.isValid() ) return pv;
    throw runtime_error("DD4hep: The object chain of "+string(placement().name())+
			" is too short. [Invalid index]");
  }
  throw runtime_error("DD4hep: Attempt to access invalid aignment object. [Invalid Handle]");
}

/// Access the placement of the mother of this node
PlacedVolume Alignment::motherPlacement(int level_up)   const    {
  if ( isValid() )  {
    Int_t ind = ptr()->GetLevel()-level_up;
    if ( ind >= 0 )  {
      return PlacedVolume(ptr()->GetMother(level_up));
    }
    throw runtime_error("DD4hep: This object "+string(placement().name())+" has not enough mothers. [Invalid index]");
  }
  throw runtime_error("DD4hep: Attempt to access invalid aignment object. [Invalid Handle]");
}

/// Align the PhysicalNode (translation only)
int Alignment::align(const Position& pos, bool check, double overlap) {
  return _align(ptr(),_translation(pos), check, overlap);
}

/// Align the PhysicalNode (rotation only)
int Alignment::align(const RotationZYX& rot, bool check, double overlap) {
  return _align(ptr(),_rotationZYX(rot), check, overlap);
}

/// Align the PhysicalNode (translation + rotation)
int Alignment::align(const Position& pos, const RotationZYX& rot, bool check, double overlap) {
  return _align(ptr(),_transform(pos,rot), check, overlap);
}
