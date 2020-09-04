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

// Framework include files
#include "DD4hep/GlobalAlignment.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/Printout.h"

// ROOT include files
#include "TGeoMatrix.h"
#include "TGeoManager.h"


using namespace std;
using namespace dd4hep;
using namespace dd4hep::align;

namespace  {
  struct CheckHandle  {
    CheckHandle(const GlobalAlignment& a)  {
      if ( a.isValid() ) return;
      throw runtime_error("dd4hep: Attempt to access invalid alignment object. [Invalid Handle]");
    }
    ~CheckHandle() {}
  };
}

/// Initializing constructor to create a new object
GlobalAlignment::GlobalAlignment(const string& path) {
  //cout << "GlobalAlignment: path=" << path << endl;
  m_element = new TGeoPhysicalNode(path.c_str());
}

/// Number of nodes in this branch
int GlobalAlignment::numNodes() const  {
  CheckHandle verify_handle(*this);
  return ptr()->GetLevel();
}

/// Access the placement of this node
PlacedVolume GlobalAlignment::placement()   const   {
  CheckHandle verify_handle(*this);
  return ptr()->GetNode(0);
}

/// Access the placement of a node in the chain of placements for this branch
PlacedVolume GlobalAlignment::nodePlacement(int level)   const   {
  CheckHandle verify_handle(*this);
  TGeoNode* n = ptr()->GetNode(level);
  if ( n ) return n;
  throw runtime_error("dd4hep: The object chain of "+string(placement().name())+
                      " is too short. [Invalid index]");
}

/// Access the placement of the mother of this node
PlacedVolume GlobalAlignment::motherPlacement(int level_up)   const    {
  CheckHandle verify_handle(*this);
  Int_t ind = ptr()->GetLevel()-level_up;
  if ( ind >= 0 )  {
    return ptr()->GetMother(level_up);
  }
  throw runtime_error("dd4hep: This object "+string(placement().name())+" has not enough mothers. [Invalid index]");
}

/// Access the currently applied alignment/placement matrix
Transform3D GlobalAlignment::toGlobal(int level) const   {
  CheckHandle verify_handle(*this);
  return detail::matrix::_transform(ptr()->GetMatrix(level));
}

/// Transform a point from local coordinates of a given level to global coordinates
Position GlobalAlignment::toGlobal(const Position& localPoint, int level) const   {
  CheckHandle verify_handle(*this);
  Position result;
  TGeoHMatrix* matrix = ptr()->GetMatrix(level);
  matrix->LocalToMaster((Double_t*)&localPoint,(Double_t*)&result);
  return result;
}

/// Transform a point from local coordinates of a given level to global coordinates
Position GlobalAlignment::globalToLocal(const Position& globalPoint, int level) const   {
  CheckHandle verify_handle(*this);
  Position result;
  TGeoHMatrix* matrix = ptr()->GetMatrix(level);
  matrix->MasterToLocal((Double_t*)&globalPoint,(Double_t*)&result);
  return result;
}

/// Access the currently applied alignment/placement matrix
Transform3D GlobalAlignment::toMother(int level) const   {
  CheckHandle verify_handle(*this);
  return detail::matrix::_transform(ptr()->GetNode(level)->GetMatrix());
}

/// Access the currently applied alignment/placement matrix
Transform3D GlobalAlignment::nominal() const   {
  CheckHandle verify_handle(*this);
  return detail::matrix::_transform(ptr()->GetOriginalMatrix());
}

/// Access the currently applied correction matrix (delta)
Transform3D GlobalAlignment::delta() const   {
  // It may be useful at some point to cache some of these matrices....
  CheckHandle verify_handle(*this);
  TGeoPhysicalNode* n = ptr();
  // T = T_0 * Delta -> Delta = T_0^-1 * T
  TGeoHMatrix matrix(n->GetOriginalMatrix()->Inverse());
  matrix.Multiply(n->GetNode()->GetMatrix());
  return detail::matrix::_transform(&matrix);
}

/// Access the inverse of the currently applied correction matrix (delta) (mother to daughter)
Transform3D GlobalAlignment::invDelta() const   {
  // It may be useful at some point to cache some of these matrices....
  CheckHandle verify_handle(*this);
  TGeoPhysicalNode* n = ptr();
  // T = T_0 * Delta -> Delta^-1 = T^-1 * T_0
  TGeoHMatrix matrix(n->GetNode()->GetMatrix()->Inverse());
  matrix.Multiply(n->GetOriginalMatrix());
  return detail::matrix::_transform(&matrix);
}
