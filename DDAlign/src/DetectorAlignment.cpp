// $Id: Readout.cpp 985 2014-01-30 13:50:10Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DDAlign/DetectorAlignment.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/Printout.h"
#include "DD4hep/objects/DetectorInterna.h"

// ROOT include files
#include "TGeoMatrix.h"
#include "TGeoManager.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

typedef DetectorTools::ElementPath   ElementPath;
typedef DetectorTools::PlacementPath PlacementPath;
typedef vector<pair<int,DetElement> > LevelElements;
namespace {

  Alignment _align(const Alignment& a, TGeoHMatrix* transform, bool check, double overlap) {
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
      return Alignment(n);
    }
    throw runtime_error("DD4hep: Cannot align non existing physical node. [Invalid Handle]");
  }

  Alignment& _alignment(const DetectorAlignment& det)  {
    DetElement::Object& e = det._data();
    if ( !e.alignment.isValid() )  {
      string path = DetectorTools::placementPath(det);
      //cout << "Align path:" << path << endl;
      e.alignment = Alignment(path);
    }
    return e.alignment;
  }
  void _dumpParentElements(DetectorAlignment& det, LevelElements& elements)   {
    PlacementPath nodes;
    ElementPath   det_nodes;
    DetectorTools::placementPath(det,nodes);
    DetectorTools::elementPath(det,det_nodes);
    ///    cout << "Placement path:";
    int level = 0;
    PlacementPath::const_reverse_iterator j=nodes.rbegin();
    ElementPath::const_reverse_iterator   k=det_nodes.rbegin();
    for(; j!=nodes.rend(); ++j, ++level)  {
      //cout << "(" << level << ") " << (void*)((*j).ptr()) 
      //	   << " " << string((*j)->GetName()) << " ";
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
Alignment DetectorAlignment::alignment() const   {
  return _alignment(*this);
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
vector<Alignment>& DetectorAlignment::volumeAlignments()  {
  return _data().volume_alignments;
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
const vector<Alignment>& DetectorAlignment::volumeAlignments() const   {
  return _data().volume_alignments;
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
Alignment DetectorAlignment::align(const Position& pos, bool check, double overlap) {
  return align(_transform(pos),check,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
Alignment DetectorAlignment::align(const RotationZYX& rot, bool check, double overlap) {
  return align(_transform(rot),check,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
Alignment DetectorAlignment::align(const Position& pos, const RotationZYX& rot, bool check, double overlap) {
  return align(_transform(pos,rot),check,overlap);
}

/// Align the physical node according to a generic Transform3D
Alignment DetectorAlignment::align(const Transform3D& transform, bool check, double overlap)  {
  return align(_transform(transform),check,overlap);
}

/// Align the physical node according to a generic TGeo matrix
Alignment DetectorAlignment::align(TGeoHMatrix* matrix, bool check, double overlap)  {
  return _align(_alignment(*this),matrix,check,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
Alignment DetectorAlignment::align(const string& path, const Position& pos, bool check, double overlap) {
  return align(path,_transform(pos),check,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
Alignment DetectorAlignment::align(const string& path, const RotationZYX& rot, bool check, double overlap) {
  return align(path,_transform(rot),check,overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
Alignment DetectorAlignment::align(const string& path, const Position& pos, const RotationZYX& rot, bool check, double overlap) {
  return align(path,_transform(pos,rot),check,overlap);
}

/// Align the physical node according to a generic Transform3D
Alignment DetectorAlignment::align(const string& path, const Transform3D& transform, bool check, double overlap)  {
  return align(path,_transform(transform),check,overlap);
}

/// Align the physical node according to a generic TGeo matrix
Alignment DetectorAlignment::align(const string& path, TGeoHMatrix* matrix, bool check, double overlap)  {
  if ( path.empty() )
    return _align(_alignment(*this),matrix,check,overlap);
  else if ( path == placementPath() )
    return _align(_alignment(*this),matrix,check,overlap);
  else if ( path[0] == '/' )   {
    Alignment a(path);
    volumeAlignments().push_back(a);
    return _align(a,matrix,check,overlap);
  }
  Alignment a(placementPath()+'/'+path);
  volumeAlignments().push_back(a);
  return _align(a,matrix,check,overlap);
}
