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
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/AlignmentsNominalMap.h"
#include "DD4hep/detail/VolumeManagerInterna.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

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
    typedef detail::tools::PlacementPath     Chain;
    typedef PlacedVolume::VolIDs             VolIDs;

    /// Helper to scan volume ids
    struct FND {
      const string& test;
      FND(const string& c) : test(c) {}
      bool operator()(const VolIDs::value_type& c) const { return c.first == test; }
    };
    AlignmentsNominalMap m_mapping;
    IDDescriptor  m_iddesc;
    VolumeManager m_mgr;
    DetElement    m_det;
    size_t        m_errors = 0;
    size_t        m_elements = 0;
    /// Initializing constructor
    VolIDTest(Detector& description, DetElement sdet, size_t depth);
    /// Default destructor
    virtual ~VolIDTest();
    /// Check volume integrity
    void checkVolume(DetElement e, PlacedVolume pv, const VolIDs& child_ids, const Chain& chain);
    /// Walk through tree of detector elements
    //void walk(DetElement de, VolIDs ids, const Chain& chain, size_t depth, size_t mx_depth)  const;
    /// Walk through tree of volume placements
    void walkVolume(DetElement e, PlacedVolume pv, VolIDs ids, const Chain& chain, size_t depth, size_t mx_depth);

    /// Action routine to execute the test
    static long run(Detector& description,int argc,char** argv);
  };
}

/// Initializing constructor
VolIDTest::VolIDTest(Detector& description, DetElement sdet, size_t depth) : m_mapping(description.world()), m_det(sdet) {
  m_mgr    = description.volumeManager();
  if ( !m_det.isValid() )   {
    stringstream err;
    err << "The subdetector " << m_det.name() << " is not known to the geometry.";
    printout(INFO,"VolIDTest",err.str().c_str());
    throw runtime_error(err.str());
  }
  if ( !description.sensitiveDetector(m_det.name()).isValid() )   {
    stringstream err;
    err << "The sensitive detector of subdetector " << m_det.name()
        << " is not known to the geometry.";
    printout(INFO,"VolIDTest",err.str().c_str());
    //throw runtime_error(err.str());
    return;
  }
  m_iddesc = description.sensitiveDetector(m_det.name()).readout().idSpec();
  //walk(m_det,VolIDs(),Chain(),0,depth);
  PlacedVolume pv  = sdet.placement();
  VolIDs       ids = pv.volIDs();
  Chain        chain;
  chain.push_back(pv);
  checkVolume(sdet, pv, ids, chain);
  walkVolume(sdet, pv, ids, chain, 1, depth);
}

/// Default destructor
VolIDTest::~VolIDTest()   {
  if ( m_errors == 0 )
    printout(ALWAYS,"VolIDTest","+++ PASSED: Checked %ld objects. Num.Errors:%ld",
             m_elements, m_errors);
  else
    printout(ALWAYS,"VolIDTest","+++ FAILED: Checked %ld objects. Num.Errors:%ld",
             m_elements, m_errors);
}

/// Check volume integrity
void VolIDTest::checkVolume(DetElement detector, PlacedVolume pv, const VolIDs& child_ids, const Chain& chain)   {
  stringstream err, log;
  VolumeID     det_vol_id = detector.volumeID();
  VolumeID     vid = det_vol_id;
  DetElement   top_sdet, det_elem;
  VolumeManagerContext* mgr_ctxt = 0;
  
  ++m_elements;

  try {
    vid       = m_iddesc.encode(child_ids);
    top_sdet  = m_mgr.lookupDetector(vid);
    det_elem  = m_mgr.lookupDetElement(vid);
    mgr_ctxt  = m_mgr.lookupContext(vid);

    if ( pv.volume().isSensitive() )  {
      PlacedVolume det_place = m_mgr.lookupDetElementPlacement(vid);
      if ( pv.ptr() != det_place.ptr() )   {
        err << "VolumeMgrTest: Wrong placement "
            << " got "        << det_place.name() << " (" << (void*)det_place.ptr() << ")"
            << " instead of " << pv.name()        << " (" << (void*)pv.ptr()        << ") "
            << " vid:" << volumeID(vid);
        ++m_errors;
      }
      else if ( top_sdet.ptr() != detector.ptr() )   {
        err << "VolumeMgrTest: Wrong associated sub-detector element vid="  << volumeID(vid)
            << " got "        << top_sdet.path() << " (" << (void*)top_sdet.ptr() << ") "
            << " instead of " << detector.path() << " (" << (void*)detector.ptr() << ")"
            << " vid:" << volumeID(vid);
        ++m_errors;
      }
      else if ( !detail::tools::isParentElement(detector,det_elem) )   {
      // This is sort of a bit wischi-waschi.... 
        err << "VolumeMgrTest: Wrong associated detector element vid="  << volumeID(vid)
            << " got "        << det_elem.path() << " (" << (void*)det_elem.ptr() << ") "
            << " instead of " << detector.path() << " (" << (void*)detector.ptr() << ")"
            << " vid:" << volumeID(vid);
        ++m_errors;
      }
      else if ( top_sdet.ptr() != m_det.ptr() )   {
        err << "VolumeMgrTest: Wrong associated detector "
            << " vid:" << volumeID(vid);
        ++m_errors;
      }
    }
  }
  catch(const exception& ex) {
    err << "Lookup " << pv.name() << " id:" << volumeID(vid)
        << " path:" << detector.path() << " error:" << ex.what();
    ++m_errors;
  }

  if ( pv.volume().isSensitive() || (0 != det_vol_id) )  {
    string id_desc;
    log << "Volume:"  << setw(50) << left << pv.name();
    if ( pv.volume().isSensitive() )  {
      IDDescriptor dsc = SensitiveDetector(pv.volume().sensitiveDetector()).readout().idSpec();
      log << " IDDesc:" << (char*)(dsc.ptr() == m_iddesc.ptr() ? "OK " : "BAD");
      if ( dsc.ptr() != m_iddesc.ptr() ) ++m_errors;
    }
    else  {
      log << setw(11) << " ";
    }
    id_desc = m_iddesc.str(vid);
    log << " [" << char(pv.volume().isSensitive() ? 'S' : 'N') << "] " << right
        << " vid:" << volumeID(vid)
        << " " << id_desc;
    if ( !err.str().empty() )   {
      printout(ERROR,m_det.name(),err.str()+" "+log.str());
      //throw runtime_error(err.str());
      return;
    }
    id_desc = m_iddesc.str(det_elem.volumeID());
    printout(INFO,m_det.name(),log.str());
    printout(INFO,m_det.name(),"  Elt:%-64s    vid:%s %s Parent-OK:%3s",
             det_elem.path().c_str(),volumeID(det_elem.volumeID()).c_str(),
             id_desc.c_str(),
             yes_no(detail::tools::isParentElement(detector,det_elem)));

    try  {
      if ( pv.volume().isSensitive() )  {
        TGeoHMatrix trafo;
        for (size_t i = chain.size()-1; i > 0; --i)  {
          //for (size_t i = 0; i<chain.size(); ++i )  {
          const TGeoMatrix* mat = chain[i]->GetMatrix();
          trafo.MultiplyLeft(mat);
        }
        for (size_t i = chain.size(); i > 0; --i)  {
          const TGeoMatrix* mat = chain[i-1]->GetMatrix();
          if ( printLevel() <= INFO )  {
            ::printf("Placement [%d]  VolID:%s\t\t",int(i),chain[i-1].volIDs().str().c_str());
            mat->Print();
          }
        }
        det_elem  = m_mgr.lookupDetElement(vid);
        if ( printLevel() <= INFO )  {
          ::printf("Computed Trafo (from placements):\t\t");
          trafo.Print();
          ::printf("DetElement Trafo: %s [%s]\t\t",
                   det_elem.path().c_str(),volumeID(det_elem.volumeID()).c_str());
          det_elem.nominal().worldTransformation().Print();
          ::printf("VolumeMgr  Trafo: %s [%s]\t\t",det_elem.path().c_str(),volumeID(vid).c_str());
          m_mgr.worldTransformation(m_mapping,vid).Print();
        }
        
        /// Check volume manager context
        if ( 0 == mgr_ctxt )  {
          printout(ERROR,m_det.name(),"VOLUME_MANAGER FAILED: Could not find entry for vid:%s.",
                   volumeID(vid).c_str());
          ++m_errors;
        }

        /// Check nominal and DetElement trafos for pointer equality:
        if ( &det_elem.nominal().worldTransformation() != &m_mgr.worldTransformation(m_mapping,det_elem.volumeID()) )
        {
            printout(ERROR,m_det.name(),"DETELEMENT_PERSISTENCY FAILED: World transformation have DIFFERET pointer!");
          ++m_errors;
        }
        
        if ( pv.ptr() == det_elem.placement().ptr() )   {
          // The computed transformation 'trafo' MUST be equal to:
          // m_mgr.worldTransformation(vid) AND det_elem.nominal().worldTransformation()
          int res1 = detail::matrix::_matrixEqual(trafo, det_elem.nominal().worldTransformation());
          int res2 = detail::matrix::_matrixEqual(trafo, m_mgr.worldTransformation(m_mapping,vid));
          if ( res1 != detail::matrix::MATRICES_EQUAL || res2 != detail::matrix::MATRICES_EQUAL )  {
            printout(ERROR,m_det.name(),"DETELEMENT_PLACEMENT FAILED: World transformation DIFFER.");
            ++m_errors;
          }
          else  {
            printout(INFO,m_det.name(),"DETELEMENT_PLACEMENT: PASSED. All matrices equal: %s",
                     volumeID(vid).c_str());
          }
        }
        else  {
          // The computed transformation 'trafo' MUST be equal to:
          // m_mgr.worldTransformation(vid)
          // The det_elem.nominal().worldTransformation() however is DIFFERENT!
          int res2 = detail::matrix::_matrixEqual(trafo, m_mgr.worldTransformation(m_mapping,vid));
          if ( res2 != detail::matrix::MATRICES_EQUAL )  {
            printout(ERROR,m_det.name(),"VOLUME_PLACEMENT FAILED: World transformation DIFFER.");
            ++m_errors;
          }
          else  {
            printout(INFO,m_det.name(),"VOLUME_PLACEMENT: PASSED. All matrices equal: %s",
                     volumeID(vid).c_str());
          }
        }
      }
    }
    catch(const exception& ex) {
      err << "Matrix " << pv.name() << " id:" << volumeID(vid)
          << " path:" << detector.path() << " error:" << ex.what();
      ++m_errors;
    }
    
  }
}

/// Walk through tree of detector elements
void VolIDTest::walkVolume(DetElement detector, PlacedVolume pv, VolIDs ids, const Chain& chain,
                           size_t depth, size_t mx_depth)  
{
  if ( depth <= mx_depth )  {
    const TGeoNode* current  = pv.ptr();
    TObjArray*  nodes        = current->GetNodes();
    int         num_children = nodes ? nodes->GetEntriesFast() : 0;

    for(int i=0; i<num_children; ++i)   {
      TGeoNode* node = (TGeoNode*)nodes->At(i);
      PlacedVolume place(node);
      VolIDs child_ids(ids);
      Chain  child_chain(chain);

      place.access(); // Test validity
      child_chain.push_back(place);
      child_ids.insert(child_ids.end(), place.volIDs().begin(), place.volIDs().end());
      //bool is_sensitive = place.volume().isSensitive();
      //if ( is_sensitive || !child_ids.empty() )  {
      checkVolume(detector, place, child_ids, child_chain);
      //}
      walkVolume(detector, place, child_ids, child_chain, depth+1, mx_depth);
    }
  }
}

/// Action routine to execute the test
long VolIDTest::run(Detector& description,int argc,char** argv)    {
  printout(ALWAYS,"DD4hepVolumeMgrTest","++ Processing plugin...");
  for(int iarg=0; iarg<argc;++iarg)  {
    if ( argv[iarg] == 0 ) break;
    string name = argv[iarg];
    if ( name == "all" || name == "All" || name == "ALL" )  {
      const DetElement::Children& children = description.world().children();
      for (DetElement::Children::const_iterator i=children.begin(); i!=children.end(); ++i)  {
        DetElement sdet = (*i).second;
        printout(INFO,"DD4hepVolumeMgrTest","++ Processing subdetector: %s",sdet.name());
        VolIDTest test(description,sdet,99);
      }
      return 1;
    }
    printout(INFO,"DD4hepVolumeMgrTest","++ Processing subdetector: %s",name.c_str());
    VolIDTest test(description,description.detector(name),99);
  }
  return 1;
}

DECLARE_APPLY(DD4hepVolumeMgrTest,VolIDTest::run)
