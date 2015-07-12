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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/DetectorTools.h"
//#include "DD4hep/DetectorAlignment.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

typedef DetectorTools::ElementPath   ElementPath;
typedef DetectorTools::PlacementPath PlacementPath;

namespace  {
  /** @class GeometryWalk
   *
   *  Test the volume manager by scanning the sensitive
   *  volumes of one or several subdetectors.
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct GeometryWalk  {
    typedef vector<PlacedVolume::VolID> VolIDs;
    /// Helper to scan volume ids
    struct FND {
      const string& test;
      FND(const string& c) : test(c) {}
      bool operator()(const VolIDs::value_type& c) const { return c.first == test; }
    };
    VolumeManager m_mgr;
    DetElement    m_det;

    /// Initializing constructor
    GeometryWalk(LCDD& lcdd, DetElement sdet);
    /// Default destructor
    virtual ~GeometryWalk() {}
    /// Walk through tree of detector elements
    void walk(DetElement de, VolIDs ids)  const;
    /// Printout volume information
    void print(DetElement e, PlacedVolume pv, const VolIDs& child_ids)  const;
    /// Action routine to execute the test
    static long run(LCDD& lcdd,int argc,char** argv);
  };
}

typedef DetElement::Children _C;

/// Initializing constructor
GeometryWalk::GeometryWalk(LCDD& lcdd, DetElement sdet) : m_det(sdet) {
  m_mgr = lcdd.volumeManager();
  if ( !m_det.isValid() )   {
    stringstream err;
    err << "The subdetector " << m_det.name() << " is not known to the geometry.";
    printout(INFO,"GeometryWalk",err.str().c_str());
    throw runtime_error(err.str());
  }
  walk(m_det,VolIDs());
}

/// Printout volume information
void GeometryWalk::print(DetElement e, PlacedVolume pv, const VolIDs& /* child_ids */)  const {
  stringstream log;
  PlacementPath all_nodes;
  ElementPath   det_elts;
  DetectorTools::elementPath(e,det_elts);
  DetectorTools::placementPath(e,all_nodes);
  string elt_path  = DetectorTools::elementPath(det_elts);
  string node_path = DetectorTools::placementPath(all_nodes);
  log << "Lookup " << left << setw(32) << pv.name() << " Detector[" << det_elts.size() << "]: " << elt_path;
  printout(INFO,m_det.name(),log.str());
  log.str("");
  log << "       " << left << setw(32) << "       " << " Places[" <<  all_nodes.size()  << "]:   " << node_path;
  printout(INFO,m_det.name(),log.str());
  log.str("");
  log << "       " << left << setw(32) << "       " << " Matrices[" <<  all_nodes.size()  << "]: ";
  for(PlacementPath::const_iterator i=all_nodes.begin(); i!=all_nodes.end(); ++i)  {
    log << (void*)((*i)->GetMatrix()) << "  ";
    if ( i+1 == all_nodes.end() ) log << "( -> " << (*i)->GetName() << ")";
  }
  printout(INFO,m_det.name(),log.str());
}

/// Walk through tree of volume placements
void GeometryWalk::walk(DetElement e, VolIDs ids)  const   {
  const _C& children = e.children();
  PlacedVolume pv = e.placement();
  VolIDs child_ids(ids);
  print(e,pv,ids);
  child_ids.insert(child_ids.end(),pv.volIDs().begin(),pv.volIDs().end());
  for (_C::const_iterator i=children.begin(); i!=children.end(); ++i)  {
    walk((*i).second,child_ids);
  }
}

/// Action routine to execute the test
long GeometryWalk::run(LCDD& lcdd,int argc,char** argv)    {
  cout << "++ Processing plugin....GeometryWalker.." << endl;
  for(int in=1; in < argc; ++in)  {
    string name = argv[in]+1;
    if ( name == "all" || name == "All" || name == "ALL" )  {
      const _C& children = lcdd.world().children();
      for (_C::const_iterator i=children.begin(); i!=children.end(); ++i)  {
        DetElement sdet = (*i).second;
        cout << "++ Processing subdetector: " << sdet.name() << endl;
        GeometryWalk test(lcdd,sdet);
      }
      return 1;
    }
    cout << "++ Processing subdetector: " << name << endl;
    GeometryWalk test(lcdd,lcdd.detector(name));
  }
  return 1;
}


namespace DD4hep {
  namespace Geometry {
    using ::GeometryWalk;
  }
}
DECLARE_APPLY(GeometryWalker,GeometryWalk::run)
