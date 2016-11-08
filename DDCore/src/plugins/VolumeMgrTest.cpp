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

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace  {
  /** @class VolIDTest
   *
   *  Test the volume manager by scanning the sensitive
   *  volumes of one or several subdetectors.
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct VolIDTest  {
    typedef vector<PlacedVolume::VolID> VolIDs;
    /// Helper to scan volume ids
    struct FND {
      const string& test;
      FND(const string& c) : test(c) {}
      bool operator()(const VolIDs::value_type& c) const { return c.first == test; }
    };
    IDDescriptor  m_iddesc;
    VolumeManager m_mgr;
    DetElement    m_det;

    /// Initializing constructor
    VolIDTest(LCDD& lcdd, DetElement sdet, size_t depth);
    /// Default destructor
    virtual ~VolIDTest() {}
    /// Check volume integrity
    void checkVolume(DetElement e, PlacedVolume pv, const VolIDs& child_ids)  const;
    /// Walk through tree of detector elements
    void walk(DetElement de, VolIDs ids, size_t depth, size_t mx_depth)  const;
    /// Walk through tree of volume placements
    void walkVolume(DetElement e, PlacedVolume pv, VolIDs ids, size_t depth, size_t mx_depth)  const;

    /// Action routine to execute the test
    static long run(LCDD& lcdd,int argc,char** argv);
  };
}

/// Initializing constructor
VolIDTest::VolIDTest(LCDD& lcdd, DetElement sdet, size_t depth) : m_det(sdet) {
  m_mgr    = lcdd.volumeManager();
  if ( !m_det.isValid() )   {
    stringstream err;
    err << "The subdetector " << m_det.name() << " is not known to the geometry.";
    printout(INFO,"VolIDTest",err.str().c_str());
    throw runtime_error(err.str());
  }
  if ( !lcdd.sensitiveDetector(m_det.name()).isValid() )   {
    stringstream err;
    err << "The sensitive detector of subdetector " << m_det.name()
        << " is not known to the geometry.";
    printout(INFO,"VolIDTest",err.str().c_str());
    throw runtime_error(err.str());
  }
  m_iddesc = lcdd.sensitiveDetector(m_det.name()).readout().idSpec();
  walk(m_det,VolIDs(),0,depth);
}

/// Check volume integrity
void VolIDTest::checkVolume(DetElement e, PlacedVolume pv, const VolIDs& child_ids)  const {
  stringstream err, log;
  VolumeID vid = m_iddesc.encode(child_ids);
  try {
    DetElement   det       = m_mgr.lookupDetector(vid);
    DetElement   det_elem  = m_mgr.lookupDetElement(vid);
    PlacedVolume det_place = m_mgr.lookupPlacement(vid);
    if ( pv.ptr() != det_place.ptr() )   {
      err << "Wrong placement "
          << " got "        << det_place.name() << " (" << (void*)det_place.ptr() << ")"
          << " instead of " << pv.name()        << " (" << (void*)pv.ptr()        << ") "
          << " vid:" << (void*)vid;
    }
    else if ( det_elem.ptr() != e.ptr() )   {
      err << "Wrong associated detector element vid="  << (void*)vid
          << " got "        << det_elem.path() << " (" << (void*)det_elem.ptr() << ") "
          << " instead of " << e.path()        << " (" << (void*)e.ptr()        << ")"
          << " vid:" << (void*)vid;
    }
    else if ( det.ptr() != m_det.ptr() )   {
      err << "Wrong associated detector "
          << " vid:" << (void*)vid;
    }
  }
  catch(const exception& ex) {
    err << "Lookup " << pv.name() << " id:" << (void*)vid << " path:" << e.path() << " error:" << ex.what();
  }
  const IDDescriptor::FieldMap& m = m_iddesc.fields();
  log << "IDS(" << pv.name() << ") ";
  log << " vid:" << setw(16) << hex << setfill('0') << vid << dec << setfill(' ') << " ";
  for ( size_t fi=0; fi<m.size(); ++fi )    {
    IDDescriptor::Field fld = m_iddesc.field(fi);
    long val = fld->value(vid);
    if ( find_if(child_ids.begin(),child_ids.end(),FND(fld->name())) == child_ids.end() ) continue;
    log << fld->name() << (val>=0?": ":":") << val << "  ";
  }
  if ( !err.str().empty() )   {
    printout(ERROR,m_det.name(),err.str()+" "+log.str());
    throw runtime_error(err.str());
  }
  printout(INFO,m_det.name(),"OK "+log.str());
}

/// Walk through tree of detector elements
void VolIDTest::walkVolume(DetElement e, PlacedVolume pv, VolIDs ids, size_t depth, size_t mx_depth)  const   {
  if ( depth <= mx_depth )  {
    const TGeoNode* current = pv.ptr();
    TObjArray*  nodes  = current->GetNodes();
    int   num_children = nodes ? nodes->GetEntriesFast() : 0;
    for(int i=0; i<num_children; ++i)   {
      TGeoNode* node = (TGeoNode*)nodes->At(i);
      PlacedVolume place(node);
      VolIDs child_ids(ids);

      child_ids.insert(child_ids.end(),place.volIDs().begin(),place.volIDs().end());
      bool is_sensitive = place.volume().isSensitive();
      if ( is_sensitive )  {
        checkVolume(e,place,child_ids);
      }
      walkVolume(e,place,child_ids,depth+1,mx_depth);
    }
  }
}

/// Walk through tree of volume placements
void VolIDTest::walk(DetElement e, VolIDs ids, size_t depth, size_t mx_depth)  const   {
  if ( depth <= mx_depth )  {
    DetElement::Children::const_iterator i;
    const DetElement::Children& children = e.children();
    PlacedVolume pv = e.placement();
    VolIDs child_ids(ids);
    bool is_sensitive = pv.volume().isSensitive();
    child_ids.insert(child_ids.end(),pv.volIDs().begin(),pv.volIDs().end());
    for (i=children.begin(); i!=children.end(); ++i)  {
      walk((*i).second,child_ids,depth+1,mx_depth);
    }
    if ( !is_sensitive && children.empty() )  {
      walkVolume(e,pv,child_ids,depth+1,mx_depth);
    }
    else if ( is_sensitive )  {
      checkVolume(e, pv,child_ids);
    }
  }
}

/// Action routine to execute the test
long VolIDTest::run(LCDD& lcdd,int argc,char** argv)    {
  printout(ALWAYS,"DD4hepVolumeMgrTest","++ Processing plugin...");
  for(int iarg=0; iarg<argc;++iarg)  {
    if ( argv[iarg] == 0 ) break;
    string name = argv[iarg];
    if ( name == "all" || name == "All" || name == "ALL" )  {
      const DetElement::Children& children = lcdd.world().children();
      for (DetElement::Children::const_iterator i=children.begin(); i!=children.end(); ++i)  {
        DetElement sdet = (*i).second;
        printout(INFO,"DD4hepVolumeMgrTest","++ Processing subdetector: %s",sdet.name());
        VolIDTest test(lcdd,sdet,99);
      }
      return 1;
    }
    printout(INFO,"DD4hepVolumeMgrTest","++ Processing subdetector: %s",name.c_str());
    VolIDTest test(lcdd,lcdd.detector(name),99);
  }
  return 1;
}

DECLARE_APPLY(DD4hepVolumeMgrTest,VolIDTest::run)
