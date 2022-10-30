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
#include <cstdlib>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep { namespace detail { namespace tools  {
      /// Assemble the path of the PlacedVolume selection
      std::string elementPath(const PlacementPath& nodes, bool reverse= false);
    } } }

namespace  {

  /// Tool to check the consistency of a detector setup or individual subdetectors
  /** Tool to check the consistency of a detector setup or individual subdetectors
   *
   *  Test the detector setup:
   *
   *   - Scan the detectore structure and check DetElements and its placements
   *
   *   - Scan the geomeytrical hierarchy and check the volumes and placement
   *     + optionally the sensitive setup
   *
   *   - Scan the volume hierarchy and test the volume manager by scanning 
   *     the sensitive volumes and checking the correct setup of the
   *     PhysVolIDs of the placements against the volume manager
   *
   *  Other ideas for implementing a proper detector check tool are welcome!
   *
   *  @author  M.Frank
   *  @version 2.0
   */
  struct DetectorCheck  {
    using StructureElements = std::map<DetElement, size_t>;
    using Chain = detail::tools::PlacementPath;
    using VolIDs = PlacedVolume::VolIDs;

    /// Helper to scan volume ids
    struct FND {
      const string& test;
      FND(const string& c) : test(c) {}
      bool operator()(const VolIDs::value_type& c) const { return c.first == test; }
    };
    struct counters  {
      size_t elements { 0 };
      size_t errors { 0 };
      void reset() { elements = errors = 0; }
      counters& operator+=(const counters& c)  {
	elements += c.elements;
	errors   += c.errors;
	return *this;
      }
    };

    Detector&            description;
    AlignmentsNominalMap m_mapping;
    DetElement           m_current_detector;
    SensitiveDetector    m_current_sensitive;
    IDDescriptor         m_current_iddesc;
    VolumeManager        m_volMgr;
    DetElement           m_det;
    std::string          m_name { "GeometryCheck" };

    counters             m_place_counters, m_sens_counters, m_geo_counters, m_struct_counters;
    StructureElements    m_structure_elements;

    bool check_structure  { false };
    bool check_geometry   { false };
    bool check_placements { false };
    bool check_volmgr     { false };
    bool check_sensitive  { false };

    SensitiveDetector get_current_sensitive_detector();

    /// Initializing constructor
    DetectorCheck(Detector& description);
    /// Default destructor
    virtual ~DetectorCheck() = default;

    /// Check single volume integrity
    void checkManagerSingleVolume(DetElement e, PlacedVolume pv, const VolIDs& child_ids, const Chain& chain);
    /// Walk through tree of volume placements
    void checkManagerVolumeTree(DetElement e, PlacedVolume pv, VolIDs ids, const Chain& chain, size_t depth, size_t mx_depth);

    /// Check single volume integrity
    void checkSingleVolume(DetElement e, PlacedVolume pv);
    /// Walk through tree of volume placements
    void checkVolumeTree(DetElement e, PlacedVolume pv);

    /// Check DetElement integrity
    bool checkDetElement(const std::string& path, DetElement detector, PlacedVolume pv);
    /// Check DetElement tree for integrity
    bool checkDetElementTree(const std::string& path, DetElement detector, PlacedVolume pv);

    void execute(DetElement sdet, size_t depth);

    /// Action routine to execute the test
    static long run(Detector& description,int argc,char** argv);
    static void help(int argc,char** argv);
  };
  const char* tag_fail(size_t errs)   {
    return errs==0 ? "PASSED" : "FAILED";
  }
}


/// Initializing constructor
DetectorCheck::DetectorCheck(Detector& desc)
  : description(desc), m_mapping(desc.world())
{
}

SensitiveDetector DetectorCheck::get_current_sensitive_detector()  {
  DetElement de = m_current_detector;
  m_current_sensitive = description.sensitiveDetector(de.name());
  m_current_iddesc = IDDescriptor();
  if ( m_current_sensitive.isValid() )   {
    m_current_iddesc = m_current_sensitive.readout().idSpec();
  }
  return m_current_sensitive;
}

void DetectorCheck::execute(DetElement sdet, size_t depth)   {
  const char* line = "============================";
  struct counters count_volmgr_sens, count_volmgr_place;
  struct counters total, count_struct;
  struct counters count_geo, count_geo_sens;

  if ( !sdet.isValid() )   {
    ++m_place_counters.errors;
    except("VolumeMgrTest", "The detector element is not known to the geometry.");
    return;
  }

  m_det = sdet;
  m_current_detector = m_det;

  /// Enable sensitive volume checks
  if ( check_sensitive || check_volmgr )   {
    if ( m_det == m_det.world() )  {
      m_current_sensitive = SensitiveDetector();
      m_current_iddesc = IDDescriptor();
    }
    else   {
      m_current_sensitive = description.sensitiveDetector(m_det.name());
      if ( !m_current_sensitive.isValid() )   {
	printout(ERROR, m_name,
		 "The sensitive detector of subdetector %s "
		 "is not known to the geometry.", m_det.name());
	return;
      }
      m_current_iddesc = m_current_sensitive.readout().idSpec();
    }
  }
  /// Execute actions:
  if ( check_structure )   {
    printout(ALWAYS, m_name, "%s%s  Executing STRUCTURE      test  %s%s", line, line, line, line);
    PlacedVolume pv  = m_det.placement();
    checkDetElementTree(m_det.path(), m_det, pv);
    count_struct.elements = m_structure_elements.size();
    count_struct.errors   = m_struct_counters.errors;
    total += count_struct;
    m_structure_elements.clear();
    m_struct_counters.reset();
  }
  if ( check_geometry )   {
    printout(ALWAYS, m_name, "%s%s  Executing GEOMETRY       test  %s%s", line, line, line, line);
    PlacedVolume pv  = m_det.placement();
    checkVolumeTree(m_det, pv);
    count_geo      = m_geo_counters;
    count_geo_sens = m_sens_counters;
    total += count_geo_sens;
    total += count_geo;
    m_sens_counters.reset();
    m_geo_counters.reset();
  }

  if ( check_volmgr )   {
    Chain chain;
    PlacedVolume pv  = m_det.placement();
    VolIDs       ids;

    printout(ALWAYS, m_name, "%s%s  Executing VOLUME MANAGER test  %s%s", line, line, line, line);
    chain.emplace_back(pv);
    m_volMgr = description.volumeManager();
    if ( !m_volMgr.isValid() )    {
      printout(ERROR, m_name, "Volume manager is not instantiated. Required for test!");
      return;
    }
    if ( pv.volume() != description.worldVolume() )   {
      ids = pv.volIDs();
    }
    m_sens_counters.reset();
    m_current_detector = m_det;
    checkManagerVolumeTree(m_det, pv, ids, chain, 1, depth);
    count_volmgr_place = m_place_counters;
    count_volmgr_sens  = m_sens_counters;
    total += count_volmgr_place;
    total += count_volmgr_sens;
    m_place_counters.reset();
    m_sens_counters.reset();
  }

  if ( check_structure )   {
    printout(count_struct.errors > 0 ? ERROR : ALWAYS, 
	     m_name, "+++ %s: Checked %10ld structure elements.   Num.Errors:%6ld (structure test)",
	     tag_fail(count_struct.errors), count_struct.elements, count_struct.errors);
  }
  if ( check_geometry )   {
    if ( check_sensitive )  {
      printout(count_geo_sens.errors > 0 ? ERROR : ALWAYS,
	       m_name, "+++ %s: Checked %10ld sensitive elements.   Num.Errors:%6ld (geometry test)",
	       tag_fail(count_geo_sens.errors), count_geo_sens.elements, count_geo_sens.errors);
    }
    printout(count_geo.errors > 0 ? ERROR : ALWAYS,
	     m_name, "+++ %s: Checked %10ld placements.           Num.Errors:%6ld (geometry test)",
	     tag_fail(count_geo.errors), count_geo.elements, count_geo.errors);
  }
  if ( check_volmgr )   {
    if ( check_sensitive )  {
      printout(count_volmgr_sens.errors > 0 ? ERROR : ALWAYS,
	       m_name, "+++ %s: Checked %10ld sensitive elements.   Num.Errors:%6ld (phys.VolID test)",
	       tag_fail(count_volmgr_sens.errors), count_volmgr_sens.elements, count_volmgr_sens.errors);
    }
    printout(count_volmgr_place.errors > 0 ? ERROR : ALWAYS,
	     m_name, "+++ %s: Checked %10ld sensitive placements. Num.Errors:%6ld (phys.VolID test)",
	     tag_fail(count_volmgr_place.errors), count_volmgr_sens.elements, count_volmgr_place.errors);
  }
  printout(ALWAYS, m_name, "+++ %s: Checked a total of %11ld elements. Num.Errors:%6ld (Some elements checked twice)",
	   tag_fail(total.errors), total.elements, total.errors);
}

/// Check DetElement integrity
bool DetectorCheck::checkDetElement(const std::string& path, DetElement detector, PlacedVolume pv)   {
  bool det_valid = true;
  bool parent_valid = true;
  bool place_valid = true;
  bool det_place_valid = true;
  bool vol_valid = true;
  auto nerrs = m_struct_counters.errors;
  const char* de_path = detector.path().c_str();

  if ( !pv.isValid() )   {
    printout(ERROR, m_name, "Invalid DetElement placement: %s", de_path);
    ++m_struct_counters.errors;
    place_valid = false;
  }
  if ( detector.path() != path )    {
    printout(ERROR, m_name, "Invalid DetElement [path mismatch]: %s <> %s",
	     de_path, path.c_str());
    ++m_struct_counters.errors;
  }
  if ( !detector.parent().isValid() && detector.world() != detector )   {
    printout(ERROR, m_name, "Invalid DetElement [No parent]:     %s", de_path);
    ++m_struct_counters.errors;
    parent_valid = false;
  }
  if ( !detector.placement().isValid() )   {
    printout(ERROR, m_name, "Invalid DetElement [No placement]:  %s", de_path);
    ++m_struct_counters.errors;
    det_place_valid = false;
  }
  else if ( !detector.volume().isValid() )   {
    printout(ERROR, m_name, "Invalid DetElement [No volume]:     %s", de_path);
    ++m_struct_counters.errors;
    vol_valid = false;
  }
  if ( detector.placement().isValid() && detector.placement() != pv )   {
    printout(ERROR, m_name, "Invalid DetElement [Mismatched placement]:  %s", de_path);
    ++m_struct_counters.errors;
    det_place_valid = false;
  }
  auto count = ++m_structure_elements[detector];
  if ( count > 1 )   {
    DetElement par = detector.parent();
    printout(ERROR, m_name, "DetElement %s parent: %s is placed %ld times! Only single placement allowed.", 
	     de_path, par.isValid() ? par.path().c_str() : "", m_structure_elements[detector]);
    ++m_struct_counters.errors;
  }
  Alignment ideal = detector.nominal();
  if ( !ideal.isValid() )    {
    printout(ERROR, m_name, "Invalid DetElement [No ideal alignment]: %s", de_path);
    ++m_struct_counters.errors;
  }
  Alignment survey = detector.survey();
  if ( !survey.isValid() )    {
    printout(ERROR, m_name, "Invalid DetElement [No survey alignment]: %s", de_path);
    ++m_struct_counters.errors;
  }
  if ( ideal.isValid() )    {
    const TGeoHMatrix& matrix = ideal.worldTransformation();
    if ( matrix.IsIdentity() )  {
    }
  }
  printout(nerrs != m_struct_counters.errors ? ERROR : INFO, m_name, 
	   "DetElement %s [%s] parent: %s placement: %s [%s] volume: %s",
	   path.c_str(), yes_no(det_valid), yes_no(parent_valid), yes_no(det_place_valid),
	   yes_no(place_valid), yes_no(vol_valid));
  return nerrs == m_struct_counters.errors;
}

/// Check DetElement tree for integrity
bool DetectorCheck::checkDetElementTree(const std::string& path, DetElement detector, PlacedVolume pv)   {
  auto nerrs = m_struct_counters.errors;
  if ( !detector.isValid() )   {
    printout(ERROR, m_name, "Invalid DetElement seen: %s", path.c_str());
    ++m_struct_counters.errors;
    return false;
  }
  bool is_world = detector == detector.world();
  /// Check single DetElement object
  checkDetElement(path, detector, pv);
  /// Recurse the tree
  for ( const auto& c : detector.children() )   {
    DetElement de = c.second;
    if ( is_world )   {
      m_current_sensitive = SensitiveDetector();
      m_current_iddesc = IDDescriptor();
      m_current_detector = de;
    }
    if ( de.parent().isValid() && de.parent() != detector )    {
      printout(ERROR, m_name, "Invalid DetElement [Parent mismatch]:    %s", de.path().c_str());
      printout(ERROR, m_name, "        apparent parent: %s  structural parent: %s",
	       de.parent().path().c_str(), detector.path().c_str());
      ++m_struct_counters.errors;
    }
    /// Invalid daughter elements will be detectoed in there:
    checkDetElementTree(path + "/" + c.first, de, de.placement());
  }
  return nerrs == m_struct_counters.errors;
}

/// Check single volume integrity
void DetectorCheck::checkSingleVolume(DetElement e, PlacedVolume pv)   {

  ++m_geo_counters.elements;
  /// Check DetElement validity
  if ( !e.isValid() )   {
    printout(ERROR, m_name, "Invalid DetElement [Invalid handle]");
    ++m_geo_counters.errors;
  }
  /// Check placement validity
  if ( !pv.isValid() )   {
    printout(ERROR, m_name, "Invalid PlacedVolume [Invalid handle] DetElement: %s", e.path().c_str());
    ++m_geo_counters.errors;
  }
  Volume vol = pv.volume();
  /// Check volume validity
  if ( !vol.isValid() )   {
    printout(ERROR, m_name, "Invalid Volume [Invalid handle] DetElement: %s", e.path().c_str());
    ++m_geo_counters.errors;
    return;
  }
  /// Check sensitive settings for sensitive volumes
  if ( check_sensitive && vol.isSensitive() )    {
    SensitiveDetector sdv = vol.sensitiveDetector();
    ++m_sens_counters.elements;
    if ( !sdv.isValid() )   {
      printout(ERROR, m_name, "Invalid SensitiveDetector DetElement: %s", e.path().c_str());
      ++m_sens_counters.errors;
    } 
    SensitiveDetector sdd = get_current_sensitive_detector();
    if ( sdd != sdv )   {
      printout(ERROR, m_name, "Inconsistent sensitive detectors for DetElement: %s", e.path().c_str());
      ++m_sens_counters.errors;
    }
  }
}

/// Walk through tree of volume placements
void DetectorCheck::checkVolumeTree(DetElement detector, PlacedVolume pv)   {
  const TGeoNode* current  = pv.ptr();
  TObjArray*  nodes        = current->GetNodes();
  int         num_children = nodes ? nodes->GetEntriesFast() : 0;
  bool        is_world     = detector == description.world();

  /// Check single volume object
  checkSingleVolume(detector, pv);
  /// Recurse the tree
  for(int i=0; i < num_children; ++i)   {
    TGeoNode* node = (TGeoNode*)nodes->At(i);
    PlacedVolume place(node);
    DetElement de = detector;

    if ( is_world )   {
      m_current_detector = de;
      get_current_sensitive_detector();
    }

    /// Check if there is a new parent at the next level:
    for ( const auto& c : detector.children() )   {
      if ( c.second.placement() == place )   {
	de = c.second;
	break;
      }
    }
    checkVolumeTree(de, place);
    if ( is_world )   {
      m_current_sensitive = SensitiveDetector();
      m_current_iddesc    = IDDescriptor();
    }
  }
}

/// Check volume integrity
void DetectorCheck::checkManagerSingleVolume(DetElement detector, PlacedVolume pv, const VolIDs& child_ids, const Chain& chain)   {
  stringstream err, log;
  VolumeID     det_vol_id = detector.volumeID();
  VolumeID     vid        = det_vol_id;
  DetElement   top_sdet, det_elem;
  VolumeManagerContext* mgr_ctxt = 0;
  
  ++m_place_counters.elements;

  try {
    vid       = m_current_iddesc.encode(child_ids);
    top_sdet  = m_volMgr.lookupDetector(vid);
    det_elem  = m_volMgr.lookupDetElement(vid);
    mgr_ctxt  = m_volMgr.lookupContext(vid);

    if ( pv.volume().isSensitive() )  {
      PlacedVolume det_place = m_volMgr.lookupDetElementPlacement(vid);
      ++m_sens_counters.elements;
      if ( pv.ptr() != det_place.ptr() )   {
        err << "VolumeMgrTest: Wrong placement "
            << " got "        << det_place.name() << " (" << (void*)det_place.ptr() << ")"
            << " instead of " << pv.name()        << " (" << (void*)pv.ptr()        << ") "
            << " vid:" << volumeID(vid);
        ++m_place_counters.errors;
      }
      else if ( top_sdet.ptr() != detector.ptr() )   {
        top_sdet  = m_volMgr.lookupDetector(vid);
        err << "VolumeMgrTest: Wrong associated sub-detector element vid="  << volumeID(vid)
            << " got "        << top_sdet.path() << " (" << (void*)top_sdet.ptr() << ") "
            << " instead of " << detector.path() << " (" << (void*)detector.ptr() << ")"
            << " vid:" << volumeID(vid);
        ++m_place_counters.errors;
      }
      else if ( !detail::tools::isParentElement(detector,det_elem) )   {
      // This is sort of a bit wischi-waschi.... 
        err << "VolumeMgrTest: Wrong associated detector element vid="  << volumeID(vid)
            << " got "        << det_elem.path() << " (" << (void*)det_elem.ptr() << ") "
            << " instead of " << detector.path() << " (" << (void*)detector.ptr() << ")"
            << " vid:" << volumeID(vid);
        ++m_place_counters.errors;
      }
      else if ( top_sdet.ptr() != m_det.ptr() )   {
        err << "VolumeMgrTest: Wrong associated detector "
            << " vid:" << volumeID(vid);
        ++m_place_counters.errors;
      }
    }
  }
  catch(const exception& ex) {
    err << "Lookup " << pv.name() << " id:" << volumeID(vid)
        << " path:" << detector.path() << " error:" << ex.what();
    ++m_place_counters.errors;
  }

  if ( pv.volume().isSensitive() || (0 != det_vol_id) )  {
    string id_desc;
    log << "Volume:"  << setw(50) << left << pv.name();
    if ( pv.volume().isSensitive() )  {
      IDDescriptor dsc = SensitiveDetector(pv.volume().sensitiveDetector()).readout().idSpec();
      log << " IDDesc:" << (char*)(dsc.ptr() == m_current_iddesc.ptr() ? "OK " : "BAD");
      if ( dsc.ptr() != m_current_iddesc.ptr() ) ++m_place_counters.errors;
    }
    else  {
      log << setw(11) << " ";
    }
    id_desc = m_current_iddesc.str(vid);
    log << " [" << char(pv.volume().isSensitive() ? 'S' : 'N') << "] " << right
        << " vid:" << volumeID(vid)
        << " " << id_desc;
    if ( !err.str().empty() )   {
      printout(ERROR, m_det.name(),err.str()+" "+log.str());
      //throw runtime_error(err.str());
      return;
    }
    id_desc = m_current_iddesc.str(det_elem.volumeID());
    printout(INFO, m_det.name(),log.str());
    printout(INFO, m_det.name(), "  Elt:%-64s    vid:%s %s Parent-OK:%3s",
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
        det_elem  = m_volMgr.lookupDetElement(vid);
        if ( printLevel() <= INFO )  {
          ::printf("Computed Trafo (from placements):\t\t");
          trafo.Print();
          ::printf("DetElement Trafo: %s [%s]\t\t",
                   det_elem.path().c_str(),volumeID(det_elem.volumeID()).c_str());
          det_elem.nominal().worldTransformation().Print();
          ::printf("VolumeMgr  Trafo: %s [%s]\t\t",det_elem.path().c_str(),volumeID(vid).c_str());
          m_volMgr.worldTransformation(m_mapping,vid).Print();
        }
        
        /// Check volume manager context
        if ( 0 == mgr_ctxt )  {
          printout(ERROR, m_det.name(), "VOLUME_MANAGER FAILED: Could not find entry for vid:%s.",
                   volumeID(vid).c_str());
          ++m_place_counters.errors;
        }

        /// Check nominal and DetElement trafos for pointer equality:
        if ( &det_elem.nominal().worldTransformation() != &m_volMgr.worldTransformation(m_mapping,det_elem.volumeID()) )
        {
            printout(ERROR, m_det.name(), "DETELEMENT_PERSISTENCY FAILED: World transformation have DIFFERET pointer!");
          ++m_place_counters.errors;
        }
        
        if ( pv.ptr() == det_elem.placement().ptr() )   {
          // The computed transformation 'trafo' MUST be equal to:
          // m_volMgr.worldTransformation(vid) AND det_elem.nominal().worldTransformation()
          int res1 = detail::matrix::_matrixEqual(trafo, det_elem.nominal().worldTransformation());
          int res2 = detail::matrix::_matrixEqual(trafo, m_volMgr.worldTransformation(m_mapping,vid));
          if ( res1 != detail::matrix::MATRICES_EQUAL || res2 != detail::matrix::MATRICES_EQUAL )  {
            printout(ERROR, m_det.name(), "DETELEMENT_PLACEMENT FAILED: World transformation DIFFER.");
            ++m_place_counters.errors;
          }
          else  {
            printout(INFO, m_det.name(), "DETELEMENT_PLACEMENT: PASSED. All matrices equal: %s",
                     volumeID(vid).c_str());
          }
        }
        else  {
          // The computed transformation 'trafo' MUST be equal to:
          // m_volMgr.worldTransformation(vid)
          // The det_elem.nominal().worldTransformation() however is DIFFERENT!
          int res2 = detail::matrix::_matrixEqual(trafo, m_volMgr.worldTransformation(m_mapping,vid));
          if ( res2 != detail::matrix::MATRICES_EQUAL )  {
            printout(ERROR, m_det.name(), "VOLUME_PLACEMENT FAILED: World transformation DIFFER.");
            ++m_place_counters.errors;
          }
          else  {
            printout(INFO, m_det.name(), "VOLUME_PLACEMENT: PASSED. All matrices equal: %s",
                     volumeID(vid).c_str());
          }
        }
      }
    }
    catch(const exception& ex) {
      err << "Matrix " << pv.name() << " id:" << volumeID(vid)
          << " path:" << detector.path() << " error:" << ex.what();
      ++m_place_counters.errors;
    }
    
  }
}

/// Walk through tree of detector elements
void DetectorCheck::checkManagerVolumeTree(DetElement detector, PlacedVolume pv, VolIDs ids, const Chain& chain,
                           size_t depth, size_t mx_depth)  
{
  if ( depth <= mx_depth )  {
    const TGeoNode* current  = pv.ptr();
    TObjArray*  nodes        = current->GetNodes();
    int         num_children = nodes ? nodes->GetEntriesFast() : 0;
    bool        is_world     = detector == description.world();

    for(int i=0; i<num_children; ++i)   {
      TGeoNode* node = (TGeoNode*)nodes->At(i);
      PlacedVolume place(node);
      VolIDs child_ids(ids);
      Chain  child_chain(chain);
      DetElement de = detector;
      if ( is_world )  {
	/// Check if there is a new parent at the next level:
	for ( const auto& c : detector.children() )   {
	  if ( c.second.placement() == place )   {
	    de = c.second;
	    break;
	  }
	}
	m_current_detector = de;
	get_current_sensitive_detector();
      }
      place.access(); // Test validity
      child_chain.emplace_back(place);
      child_ids.insert(child_ids.end(), place.volIDs().begin(), place.volIDs().end());
      checkManagerSingleVolume(de, place, child_ids, child_chain);
      checkManagerVolumeTree(de, place, child_ids, child_chain, depth+1, mx_depth);
    }
  }
}

void DetectorCheck::help(int argc,char** argv)   {
  std::cout
    << 
    "DD4hep_DetectorCheck -option [-option]                                         \n"
    "  -help                        Print this help message                         \n"
    "  -name  <subdetector name>    Name of the subdetector to be checked           \n"
    "                               \"ALL\" or \"all\": loop over known subdetectors\n"
    "                               \"world\" start from the mother of all...       \n"
    "  -structure                   Check structural tree consistency               \n"
    "  -geometry                    Check geometry tree consistency                 \n"
    "  -sensitve                    Check consistency between detector and volume   \n"
    "                               settings of sensitive detectors.                \n"
    "  -volmgr                      Check volume manager entries against volIDs of  \n"
    "                               sensitive volume placements.                  \n\n"
    "                               NOTE: Option requires proper PhysVolID setup    \n"
    "                               of the sensitive volume placements !            \n"
    << std::endl;
  std::cout << "Arguments: " << std::endl;
  for(int iarg=0; iarg<argc;++iarg)  {
    std::cout << "Argument[" << iarg << "]  = " << argv[iarg] << std::endl;
  }
  ::exit(EINVAL);
}

/// Action routine to execute the test
long DetectorCheck::run(Detector& description,int argc,char** argv)    {
  string name;
  bool volmgr = false;
  bool geometry = false;
  bool structure = false;
  bool sensitive = false;
  bool placements = false;
  printout(ALWAYS, "DetectorCheck", "++ Processing plugin...");
  for(int iarg=0; iarg<argc;++iarg)  {
    if ( argv[iarg] == 0 ) break;
    if ( ::strncasecmp(argv[iarg], "-name",4) == 0 && (iarg+1) < argc )
      name = argv[++iarg];
    else if ( ::strncasecmp(argv[iarg], "-structure",4) == 0 )
      structure = true;
    else if ( ::strncasecmp(argv[iarg], "-placements",4) == 0 )
      placements = true;
    else if ( ::strncasecmp(argv[iarg], "-volmgr",4) == 0 )
      volmgr = true;
    else if ( ::strncasecmp(argv[iarg], "-geometry",4) == 0 )
      geometry = true;
    else if ( ::strncasecmp(argv[iarg], "-sensitive",4) == 0 )
      sensitive = true;
    else if ( ::strncasecmp(argv[iarg], "-help",4) == 0 )
      help(argc, argv);
    else
      help(argc, argv);
  }
  if ( argc == 0 ) help(argc, argv);
  if ( !name.empty() )   {
    DetectorCheck test(description);
    if ( name == "all" || name == "All" || name == "ALL" )  {
      for (const auto& det : description.detectors() )  {
        printout(INFO, "DetectorCheck", "++ Processing subdetector: %s", det.second.name());
	test.check_structure  = structure;
	test.check_placements = placements;
	test.check_volmgr     = volmgr;
	test.check_geometry   = geometry;
	test.check_sensitive  = sensitive;
	test.execute(det.second, 9999);
      }
      return 1;
    }
    DetElement det = (::strcasecmp(name.c_str(), "world") == 0)
      ? description.world() : description.detector(name);
    printout(INFO, "DetectorCheck", "++ Processing subdetector: %s",name.c_str());
    test.check_structure  = structure;
    test.check_placements = placements;
    test.check_volmgr     = volmgr;
    test.check_geometry   = geometry;
    test.check_sensitive  = sensitive;
    test.execute(det, 9999);
  }
  return 1;
}

DECLARE_APPLY(DD4hep_DetectorCheck,DetectorCheck::run)

/// Action routine to execute the test for backwards compatibility
long run_VolumeMgrTest(Detector& description,int argc, const char*const* argv)    {
  const char* args[] = {"-name", argc > 0 ? argv[0] : "world", "-structure", "-geometry", "-volmgr", 0 };
  return DetectorCheck::run(description, 6, (char**)args); 
}
DECLARE_APPLY(DD4hep_VolumeMgrTest,run_VolumeMgrTest)
