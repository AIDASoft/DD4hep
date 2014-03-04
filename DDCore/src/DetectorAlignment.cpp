// $Id: Readout.cpp 985 2014-01-30 13:50:10Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetectorAlignment.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

typedef DetectorTools::ElementPath   ElementPath;
typedef DetectorTools::PlacementPath PlacementPath;
typedef vector<pair<int,DetElement> > LevelElements;
namespace {

  Alignment& _alignment(DetectorAlignment& det)  {
    DetElement::Object& e = det._data();
    if ( !e.alignment.isValid() )  {
      DetectorTools::PlacementPath nodes;
      DetectorTools::placementPath(det,nodes);
      string path = DetectorTools::nodePath(nodes);
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
  void _patchParentElements(DetectorAlignment& det)   {
    DetElement::Object& obj = det._data();
    Alignment a = obj.alignment;
    TGeoPhysicalNode* n = a.ptr();
    string path;
    path.reserve(1024);
    int level = -1;
    
    size_t path_len = ::strlen(a->GetName());
    for(int i = 0; i <= n->GetLevel(); ++i)  {
      path += "/";
      path += n->GetNode(i)->GetName();
      if ( path.length() == path_len && path == a->GetName() ) level = i;
    }
    //cout << "Physical node: " << path << endl;
    PlacedVolume pv(n->GetNode(level));
    det.setPlacement(pv);
#if 0
    cout << "Updated placement of DetElement[" << level << "] " << det.name() 
	 << " Placement: " << (void*)det.placement().ptr()
	 << " / " << n->GetNode(level)
	 << endl;
    cout << "Placement path:";
    PlacementPath nodes;
    DetectorTools::placementPath(det,nodes);
    for(PlacementPath::const_reverse_iterator j=nodes.rbegin(); j!=nodes.rend(); ++j)
      cout << (void*)((*j).ptr()) << "  " << (*j)->GetName() << "  ";
    cout << endl;
#endif
    cout << " Path:";
    for(int i = 0; i <= n->GetLevel(); ++i)
      cout << (void*)(n->GetNode(i)) << "  " << n->GetNode(i)->GetName() << "  ";
    cout << " / " << (void*)pv.ptr() << endl;

    TGeoNode* p = n->GetNode(n->GetLevel()-1);
    TGeoNode* d = n->GetNode(n->GetLevel());
    if ( strcmp(d->GetName(),"MuonBarrel_layer9_slice1_0")==0 || 
	 strcmp(d->GetName(),"MuonBarrel_layer9_slice9_8")==0) 
      {
      TIter next(p->GetVolume()->GetNodes());
      // Now check next layer children
      for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
	cout << p->GetName() << " / " << (void*)p << " ---> " << daughter->GetName() << "  " << (void*)daughter << endl;
      }
    }
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

/// Align the PhysicalNode of the placement of the detector element (translation only)
int DetectorAlignment::align(const Position& pos, bool check, double overlap) {
  int result = _alignment(*this).align(pos,check,overlap);
  _patchParentElements(*this);
  return result;
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
int DetectorAlignment::align(const RotationZYX& rot, bool check, double overlap) {
  int result = _alignment(*this).align(rot,check,overlap);
  _patchParentElements(*this);
  return result;
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
int DetectorAlignment::align(const Position& pos, const RotationZYX& rot, bool check, double overlap) {
  int result = _alignment(*this).align(pos,rot,check,overlap);
  _patchParentElements(*this);
  return result;
}
