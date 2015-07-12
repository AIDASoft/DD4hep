// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Alignment.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/Printout.h"

// ROOT include files
#include "TGeoMatrix.h"
#include "TGeoManager.h"


using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;



namespace  {
  int _align(const Alignment& a, TGeoHMatrix* transform, bool check, double overlap) {
    TGeoPhysicalNode* n = a.ptr();
    if ( n )  {
      TGeoMatrix* mm = n->GetNode()->GetMatrix();
      transform->MultiplyLeft(mm); // orig * delta
      n->Align(transform, 0, check, overlap);
      /*
        printout(INFO,"Alignment","OLD matrix....");
        mm->Print();
        printout(INFO,"Alignment","Apply new relative matrix  mother to daughter:");
        transform->Print();
        transform->MultiplyLeft(mm); // orig * delta
        printout(INFO,"Alignment","With deltas....");
        transform->Print();
        n->Align(transform, 0, check, overlap);
        printout(INFO,"Alignment","NEW matrix....");
        n->GetNode()->GetMatrix()->Print();
      */
      /*
       */
      printout(INFO,"Alignment","NEW matrix....");
      n->GetNode()->GetMatrix()->Print();
      Position local, global = a.toGlobal(local);
      cout << "Local:" << local << " Global: " << global
           << " and back:" << a.globalToLocal(global) << endl;

      return 1;
    }
    throw runtime_error("DD4hep: Cannot align non existing physical node. [Invalid Handle]");
  }
  struct CheckHandle  {
    CheckHandle(const Alignment& a)  {
      if ( a.isValid() ) return;
      throw runtime_error("DD4hep: Attempt to access invalid alignment object. [Invalid Handle]");
    }
    ~CheckHandle() {}
  };
}

/// Default constructor
Alignment::Alignment() : Handle<TGeoPhysicalNode>() {
}

/// Copy constructor
Alignment::Alignment(const Alignment& c) : Handle<TGeoPhysicalNode>(c) {
}

/// Initializing constructor to create a new object
Alignment::Alignment(const string& path) {
  //cout << "Alignment: path=" << path << endl;
  //m_element = gGeoManager->MakePhysicalNode(path.c_str());
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
  CheckHandle verify_handle(*this);
  return ptr()->GetLevel();
}

/// Access the placement of this node
PlacedVolume Alignment::placement()   const   {
  CheckHandle verify_handle(*this);
  return PlacedVolume(ptr()->GetNode(0));
}

/// Access the placement of a node in the chain of placements for this branch
PlacedVolume Alignment::nodePlacement(int level)   const   {
  CheckHandle verify_handle(*this);
  PlacedVolume pv = PlacedVolume(ptr()->GetNode(level));
  if ( pv.isValid() ) return pv;
  throw runtime_error("DD4hep: The object chain of "+string(placement().name())+
                      " is too short. [Invalid index]");
}

/// Access the placement of the mother of this node
PlacedVolume Alignment::motherPlacement(int level_up)   const    {
  CheckHandle verify_handle(*this);
  Int_t ind = ptr()->GetLevel()-level_up;
  if ( ind >= 0 )  {
    return PlacedVolume(ptr()->GetMother(level_up));
  }
  throw runtime_error("DD4hep: This object "+string(placement().name())+" has not enough mothers. [Invalid index]");
}

/// Access the currently applied alignment/placement matrix
Transform3D Alignment::toGlobal(int level) const   {
  CheckHandle verify_handle(*this);
  return _transform(ptr()->GetMatrix(level));
}

/// Transform a point from local coordinates of a given level to global coordinates
Position Alignment::toGlobal(const Position& localPoint, int level) const   {
  CheckHandle verify_handle(*this);
  Position result;
  TGeoHMatrix* m = ptr()->GetMatrix(level);
  m->LocalToMaster((Double_t*)&localPoint,(Double_t*)&result);
  return result;
}

/// Transform a point from local coordinates of a given level to global coordinates
Position Alignment::globalToLocal(const Position& globalPoint, int level) const   {
  CheckHandle verify_handle(*this);
  Position result;
  TGeoHMatrix* m = ptr()->GetMatrix(level);
  m->MasterToLocal((Double_t*)&globalPoint,(Double_t*)&result);
  return result;
}

/// Access the currently applied alignment/placement matrix
Transform3D Alignment::toMother(int level) const   {
  CheckHandle verify_handle(*this);
  return _transform(ptr()->GetNode(level)->GetMatrix());
}

/// Access the currently applied alignment/placement matrix
Transform3D Alignment::nominal() const   {
  CheckHandle verify_handle(*this);
  return _transform(ptr()->GetOriginalMatrix());
}

/// Access the currently applied correction matrix (delta)
Transform3D Alignment::delta() const   {
  // It may be useful at some point to cache some of these matrices....
  CheckHandle verify_handle(*this);
  TGeoPhysicalNode* n = ptr();
  // T = T_0 * Delta -> Delta = T_0^-1 * T
  TGeoHMatrix mat(n->GetOriginalMatrix()->Inverse());
  mat.Multiply(n->GetNode()->GetMatrix());
  return _transform(&mat);
}

/// Access the inverse of the currently applied correction matrix (delta) (mother to daughter)
Transform3D Alignment::invDelta() const   {
  // It may be useful at some point to cache some of these matrices....
  CheckHandle verify_handle(*this);
  TGeoPhysicalNode* n = ptr();
  // T = T_0 * Delta -> Delta^-1 = T^-1 * T_0
  TGeoHMatrix mat(n->GetNode()->GetMatrix()->Inverse());
  mat.Multiply(n->GetOriginalMatrix());
  return _transform(&mat);
}
