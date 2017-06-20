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
#include "DD4hep/Detector.h"
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
using namespace dd4hep;
using namespace dd4hep::detail;

typedef detail::tools::ElementPath   ElementPath;
typedef detail::tools::PlacementPath PlacementPath;

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
    /// Helper to scan volume ids
    struct FND {
      const string& test;
      FND(const string& c) : test(c) {}
      bool operator()(const PlacedVolume::VolIDs::value_type& c) const { return c.first == test; }
    };
    VolumeManager m_mgr;
    DetElement    m_det;

    /// Initializing constructor
    GeometryWalk(Detector& description, DetElement sdet);
    /// Default destructor
    virtual ~GeometryWalk() {}
    /// Walk through tree of detector elements
    void walk(DetElement de, PlacedVolume::VolIDs ids)  const;
    /// Printout volume information
    void print(DetElement e, PlacedVolume pv, const PlacedVolume::VolIDs& child_ids)  const;
    /// Action routine to execute the test
    static long run(Detector& description,int argc,char** argv);
  };
}

typedef DetElement::Children _C;

/// Initializing constructor
GeometryWalk::GeometryWalk(Detector& description, DetElement sdet) : m_det(sdet) {
  m_mgr = description.volumeManager();
  if ( !m_det.isValid() )   {
    stringstream err;
    err << "The subdetector " << m_det.name() << " is not known to the geometry.";
    printout(INFO,"GeometryWalk",err.str().c_str());
    throw runtime_error(err.str());
  }
  walk(m_det,PlacedVolume::VolIDs());
}

/// Printout volume information
void GeometryWalk::print(DetElement e, PlacedVolume pv, const PlacedVolume::VolIDs& /* child_ids */)  const {
  stringstream log;
  PlacementPath all_nodes;
  ElementPath   det_elts;
  detail::tools::elementPath(e,det_elts);
  detail::tools::placementPath(e,all_nodes);
  string elt_path  = detail::tools::elementPath(det_elts);
  string node_path = detail::tools::placementPath(all_nodes);
  log << "Lookup " << left << setw(32) << pv.name() << " Detector[" << det_elts.size() << "]: " << elt_path;
  printout(INFO,m_det.name(),log.str());
  log.str("");
  log << "       " << left << setw(32) << "       " << " Places[" <<  all_nodes.size()  << "]:   " << node_path;
  printout(INFO,m_det.name(),log.str());
  log.str("");
  log << "       " << left << setw(32) << "       " << " detail::matrix[" <<  all_nodes.size()  << "]: ";
  for(PlacementPath::const_iterator i=all_nodes.begin(); i!=all_nodes.end(); ++i)  {
    log << (void*)((*i)->GetMatrix()) << "  ";
    if ( i+1 == all_nodes.end() ) log << "( -> " << (*i)->GetName() << ")";
  }
  printout(INFO,m_det.name(),log.str());
}

/// Walk through tree of volume placements
void GeometryWalk::walk(DetElement e, PlacedVolume::VolIDs ids)  const   {
  const _C& children = e.children();
  PlacedVolume pv = e.placement();
  PlacedVolume::VolIDs child_ids(ids);
  print(e,pv,ids);
  child_ids.insert(child_ids.end(),pv.volIDs().begin(),pv.volIDs().end());
  for (_C::const_iterator i=children.begin(); i!=children.end(); ++i)  {
    walk((*i).second,child_ids);
  }
}

/// Action routine to execute the test
long GeometryWalk::run(Detector& description,int argc,char** argv)    {
  cout << "++ Processing plugin....GeometryWalker.." << endl;
  for(int in=1; in < argc; ++in)  {
    string name = argv[in]+1;
    if ( name == "all" || name == "All" || name == "ALL" )  {
      const _C& children = description.world().children();
      for (_C::const_iterator i=children.begin(); i!=children.end(); ++i)  {
        DetElement sdet = (*i).second;
        cout << "++ Processing subdetector: " << sdet.name() << endl;
        GeometryWalk test(description,sdet);
      }
      return 1;
    }
    cout << "++ Processing subdetector: " << name << endl;
    GeometryWalk test(description,description.detector(name));
  }
  return 1;
}


namespace dd4hep {
  namespace detail {
    using ::GeometryWalk;
  }
}
DECLARE_APPLY(GeometryWalker,GeometryWalk::run)
