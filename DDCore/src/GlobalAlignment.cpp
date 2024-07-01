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
#include <DD4hep/GlobalAlignment.h>
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/Printout.h>

// ROOT include files
#include <TGeoManager.h>

namespace  {
  struct CheckHandle  {
    CheckHandle(const dd4hep::align::GlobalAlignment& a)  {
      if ( a.isValid() ) return;
      dd4hep::except("dd4hep:GlobalAlignment", "Attempt to access invalid alignment object. [Invalid Handle]");
    }
    ~CheckHandle() {}
  };
}

/// Initializing constructor to create a new object
dd4hep::align::GlobalAlignment::GlobalAlignment(const std::string& path) {
  //cout << "GlobalAlignment: path=" << path << endl;
  m_element = new TGeoPhysicalNode(path.c_str());
}

/// Number of nodes in this branch
int dd4hep::align::GlobalAlignment::numNodes() const  {
  CheckHandle verify_handle(*this);
  return ptr()->GetLevel();
}

/// Access the placement of this node
dd4hep::PlacedVolume dd4hep::align::GlobalAlignment::placement()   const   {
  CheckHandle verify_handle(*this);
  return ptr()->GetNode(0);
}

/// Access the placement of a node in the chain of placements for this branch
dd4hep::PlacedVolume dd4hep::align::GlobalAlignment::nodePlacement(int level)   const   {
  CheckHandle verify_handle(*this);
  TGeoNode* n = ptr()->GetNode(level);
  if ( n ) return n;
  except("dd4hep:GlobalAlignment",
         "The object chain of %s is too short. [Invalid index]", placement().name());
  return {};
}

/// Access the placement of the mother of this node
dd4hep::PlacedVolume dd4hep::align::GlobalAlignment::motherPlacement(int level_up)   const    {
  CheckHandle verify_handle(*this);
  Int_t ind = ptr()->GetLevel()-level_up;
  if ( ind >= 0 )  {
    return ptr()->GetMother(level_up);
  }
  except("dd4hep:GlobalAlignment",
         "This object %s has not enough mothers. [Invalid index]", placement().name());
  return {};
}

/// Access the currently applied alignment/placement matrix
dd4hep::Transform3D dd4hep::align::GlobalAlignment::toGlobal(int level) const   {
  CheckHandle verify_handle(*this);
  return detail::matrix::_transform(ptr()->GetMatrix(level));
}

/// Transform a point from local coordinates of a given level to global coordinates
dd4hep::Position dd4hep::align::GlobalAlignment::toGlobal(const Position& localPoint, int level) const   {
  CheckHandle verify_handle(*this);
  Position result;
  TGeoHMatrix* matrix = ptr()->GetMatrix(level);
  matrix->LocalToMaster((Double_t*)&localPoint,(Double_t*)&result);
  return result;
}

/// Transform a point from local coordinates of a given level to global coordinates
dd4hep::Position dd4hep::align::GlobalAlignment::globalToLocal(const Position& globalPoint, int level) const   {
  CheckHandle verify_handle(*this);
  Position result;
  TGeoHMatrix* matrix = ptr()->GetMatrix(level);
  matrix->MasterToLocal((Double_t*)&globalPoint,(Double_t*)&result);
  return result;
}

/// Access the currently applied alignment/placement matrix
dd4hep::Transform3D dd4hep::align::GlobalAlignment::toMother(int level) const   {
  CheckHandle verify_handle(*this);
  return detail::matrix::_transform(ptr()->GetNode(level)->GetMatrix());
}

/// Access the currently applied alignment/placement matrix
dd4hep::Transform3D dd4hep::align::GlobalAlignment::nominal() const   {
  CheckHandle verify_handle(*this);
  return detail::matrix::_transform(ptr()->GetOriginalMatrix());
}

/// Access the currently applied correction matrix (delta)
dd4hep::Transform3D dd4hep::align::GlobalAlignment::delta() const   {
  // It may be useful at some point to cache some of these matrices....
  CheckHandle verify_handle(*this);
  TGeoPhysicalNode* n = ptr();
  // T = T_0 * Delta -> Delta = T_0^-1 * T
  TGeoHMatrix matrix(n->GetOriginalMatrix()->Inverse());
  matrix.Multiply(n->GetNode()->GetMatrix());
  return detail::matrix::_transform(&matrix);
}

/// Access the inverse of the currently applied correction matrix (delta) (mother to daughter)
dd4hep::Transform3D dd4hep::align::GlobalAlignment::invDelta() const   {
  // It may be useful at some point to cache some of these matrices....
  CheckHandle verify_handle(*this);
  TGeoPhysicalNode* n = ptr();
  // T = T_0 * Delta -> Delta^-1 = T^-1 * T_0
  TGeoHMatrix matrix(n->GetNode()->GetMatrix()->Inverse());
  matrix.Multiply(n->GetOriginalMatrix());
  return detail::matrix::_transform(&matrix);
}
