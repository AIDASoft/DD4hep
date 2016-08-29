// $Id: $
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
#include "DDAlign/DetectorAlignment.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/Printout.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/Handle.inl"

// ROOT include files
#include "TGeoMatrix.h"
#include "TGeoManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    class GlobalAlignmentData : public NamedObject  {
    public:
      GlobalAlignment global;
      std::vector<GlobalAlignment> volume_alignments;
    public:
      GlobalAlignmentData() : NamedObject("","global-alignment") {}
      virtual ~GlobalAlignmentData() {}
    };
  } /* End namespace Aligments               */
} /* End namespace DD4hep                    */


using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;

namespace DetectorTools = DD4hep::Geometry::DetectorTools;
typedef vector<pair<int,DetElement> > LevelElements;

namespace {

  GlobalAlignment _align(const GlobalAlignment& a, TGeoHMatrix* transform, bool check, double overlap) {
    TGeoPhysicalNode* n = a.ptr();
    if ( n )  {
      TGeoMatrix* mm = n->GetNode()->GetMatrix();
      printout(INFO,"Alignment","DELTA matrix of %s",n->GetName());
      transform->Print();
      /*
        printout(INFO,"Alignment","OLD matrix of %s",n->GetName());
        mm->Print();
      */
      transform->MultiplyLeft(mm); // orig * delta
      n->Align(transform, 0, check, overlap);
      /*
        printout(INFO,"Alignment","Apply new relative matrix  mother to daughter:");
        transform->Print();
        transform->MultiplyLeft(mm); // orig * delta
        printout(INFO,"Alignment","With deltas %s ....",n->GetName());
        transform->Print();
        n->Align(transform, 0, check, overlap);
        printout(INFO,"Alignment","NEW matrix of %s",n->GetName());
        n->GetNode()->GetMatrix()->Print();

        Position local, global = a.toGlobal(local);
        cout << "Local:" << local << " Global: " << global
        << " and back:" << a.globalToLocal(global) << endl;
      */
      return GlobalAlignment(n);
    }
    throw runtime_error("DD4hep: Cannot align non existing physical node. [Invalid Handle]");
  }

  GlobalAlignment _alignment(const DetectorAlignment& det)  {
    Ref_t gbl = det._data().global_alignment;
    if ( gbl.isValid() )  {
      Handle<GlobalAlignmentData> h(gbl);
      return h->global;
    }
    throw runtime_error("DD4hep: Cannot access global alignment data. [Invalid Handle]");
#if 0
    DetElement::Object& e = det._data();
    if ( !e.alignment.isValid() )  {
      string path = DetectorTools::placementPath(det);
      //cout << "Align path:" << path << endl;
      e.alignment = GlobalAlignment(path);
    }
    return e.alignment;
#endif    
  }

  void _dumpParentElements(DetectorAlignment& det, LevelElements& elements)   {
    int level = 0;
    DetectorTools::PlacementPath nodes;
    DetectorTools::ElementPath   det_nodes;
    DetectorTools::placementPath(det,nodes);
    DetectorTools::elementPath(det,det_nodes);
    ///    cout << "Placement path:";
    DetectorTools::PlacementPath::const_reverse_iterator j=nodes.rbegin();
    DetectorTools::ElementPath::const_reverse_iterator   k=det_nodes.rbegin();
    for(; j!=nodes.rend(); ++j, ++level)  {
      //cout << "(" << level << ") " << (void*)((*j).ptr())
      //           << " " << string((*j)->GetName()) << " ";
      if ( ::strcmp((*j).ptr()->GetName(),(*k).placement().ptr()->GetName()) )  {
        //cout << "[DE]";
        elements.push_back(make_pair(level,*k));
        ++k;
      }
      else  {
        //elements.push_back(make_pair(level,DetElement()));
      }
      //cout << " ";
    }
    //cout << endl;
  }
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(GlobalAlignmentData);

/// Initializing constructor
DetectorAlignment::DetectorAlignment(DetElement e)
  : DetElement(e)
{
}

/// Collect all placements from the detector element up to the world volume
void DetectorAlignment::collectNodes(vector<PlacedVolume>& nodes)   {
  DetectorTools::placementPath(*this,nodes);
}

/// Access to the alignment block
GlobalAlignment DetectorAlignment::alignment() const   {
  return _alignment(*this);
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
vector<GlobalAlignment>& DetectorAlignment::volumeAlignments()  {
  Handle<GlobalAlignmentData> h(_data().global_alignment);
  return h->volume_alignments;
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
const vector<GlobalAlignment>& DetectorAlignment::volumeAlignments() const   {
  Handle<GlobalAlignmentData> h(_data().global_alignment);
  return h->volume_alignments;
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
GlobalAlignment DetectorAlignment::align(const Position& pos, bool chk, double overlap) {
  return align(Geometry::_transform(pos),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
GlobalAlignment DetectorAlignment::align(const RotationZYX& rot, bool chk, double overlap) {
  return align(Geometry::_transform(rot),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
GlobalAlignment DetectorAlignment::align(const Position& pos, const RotationZYX& rot, bool chk, double overlap) {
  return align(Geometry::_transform(pos,rot),chk,overlap);
}

/// Align the physical node according to a generic Transform3D
GlobalAlignment DetectorAlignment::align(const Transform3D& transform, bool chk, double overlap)  {
  return align(Geometry::_transform(transform),chk,overlap);
}

/// Align the physical node according to a generic TGeo matrix
GlobalAlignment DetectorAlignment::align(TGeoHMatrix* matrix, bool chk, double overlap)  {
  return _align(_alignment(*this),matrix,chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
GlobalAlignment DetectorAlignment::align(const string& elt_path, const Position& pos, bool chk, double overlap) {
  return align(elt_path,Geometry::_transform(pos),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
GlobalAlignment DetectorAlignment::align(const string& elt_path, const RotationZYX& rot, bool chk, double overlap) {
  return align(elt_path,Geometry::_transform(rot),chk,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
GlobalAlignment 
DetectorAlignment::align(const string& elt_path, const Position& pos, const RotationZYX& rot, bool chk, double overlap) {
  return align(elt_path,Geometry::_transform(pos,rot),chk,overlap);
}

/// Align the physical node according to a generic Transform3D
GlobalAlignment DetectorAlignment::align(const string& elt_path, const Transform3D& transform, bool chk, double overlap)  {
  return align(elt_path,Geometry::_transform(transform),chk,overlap);
}

/// Align the physical node according to a generic TGeo matrix
GlobalAlignment DetectorAlignment::align(const string& elt_path, TGeoHMatrix* matrix, bool chk, double overlap)  {
  if ( elt_path.empty() )
    return _align(_alignment(*this),matrix,chk,overlap);
  else if ( elt_path == placementPath() )
    return _align(_alignment(*this),matrix,chk,overlap);
  else if ( elt_path[0] == '/' )   {
    GlobalAlignment a(elt_path);
    volumeAlignments().push_back(a);
    return _align(a,matrix,chk,overlap);
  }
  GlobalAlignment a(placementPath()+'/'+elt_path);
  volumeAlignments().push_back(a);
  return _align(a,matrix,chk,overlap);
}
