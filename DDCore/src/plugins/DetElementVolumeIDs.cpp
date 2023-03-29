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
#ifndef DD4HEP_DETELEMENTVOLUMEIDS_H
#define DD4HEP_DETELEMENTVOLUMEIDS_H

// Framework include files
#include <DD4hep/Detector.h>
#include <DD4hep/Volumes.h>

// C/C++ includes

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Helper class to assign volume identifiers to DetElements in a subdetector tree
  /**
   *  \author  M.Frank
   *  \version 1.0
   */
  class DetElementVolumeIDs   {
  private:
    using Chain = std::vector<PlacedVolume>;
    
    /// Reference to the Detector instance
    const Detector& m_detDesc;
    /// Node counter
    std::size_t     m_numNodes = 0;

  public:
    /// Encoding/mask for sensitive volumes
    struct Encoding  {
      VolumeID identifier;
      VolumeID mask;
    };
    /// Set of already added entries
    std::map<DetElement, std::vector<Encoding> > entries;

  private:
    /// Scan a single physical volume and look for sensitive elements below
    std::size_t scanPhysicalVolume(DetElement&        parent,
				   DetElement         e,
				   PlacedVolume       pv, 
				   Encoding           parent_encoding,
				   SensitiveDetector& sd,
				   Chain&             chain);
  public:
    /// Default constructor
    DetElementVolumeIDs(const Detector& description);
    /// Access node count
    std::size_t numNodes()  const  {   return m_numNodes;  }
    /// Populate the Volume manager
    std::size_t populate(DetElement e);
  };
}         /* End namespace dd4hep                */
#endif // DD4HEP_DETELEMENTVOLUMEIDS_H
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
#include <DD4hep/Printout.h>
#include <DD4hep/Factories.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/detail/DetectorInterna.h>

/// Namespace for the AIDA detector description toolkit
using namespace dd4hep;

using Encoding = DetElementVolumeIDs::Encoding;
using Chain    = std::vector<PlacedVolume>;
using VolIDs   = PlacedVolume::VolIDs;

namespace  {

  /// Compute the encoding for a set of VolIDs within a readout descriptor
  Encoding update_encoding(const IDDescriptor iddesc, const VolIDs& ids, const Encoding& initial)  {
    VolumeID volume_id = initial.identifier, mask = initial.mask;
    for (VolIDs::const_iterator i = ids.begin(); i != ids.end(); ++i) {
      const auto& id = (*i);
      const BitFieldElement* f = iddesc.field(id.first);
      VolumeID msk = f->mask();
      int      off = f->offset();
      VolumeID val = id.second;    // Necessary to extend volume IDs > 32 bit
      volume_id   |= ((f->value(val << off) << off)&msk);
      mask        |= msk;
    }
    return { volume_id, mask };
  }
  
  /// Basic entry point to assign volume identifiers to detector elements
  /**
   *  Factory: DD4hep_DetElementVolumeIDs
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  long assign_de_volumeIDs(Detector& description, int argc, char** argv) {
    std::string detector = "/world";
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-detector",argv[i],4) )
	detector = argv[++i];
      else  {
	std::cout <<
	  "Usage: -plugin DD4hep_DetElementVolumeIDs  -arg [-arg]               \n\n"
	  "     -detector <string> Top level DetElement path. Default: '/world' \n"
	  "     -help              Print this help output                       \n"
	  "     Arguments given: " << arguments(argc,argv) << std::endl << std::flush;
	::exit(EINVAL);
      }
    }
    DetElement element = description.world();
    if ( detector != "/world" )   {
      element = detail::tools::findElement(description,detector);
      if ( !element.isValid() )  {
	except("DD4hep_DetElementVolumeIDs","+++ Invalid DetElement path: %s",detector.c_str());
      }
    }
    DetElementVolumeIDs mgr(description);
    auto count = mgr.populate(element);
    if ( count == 0 )   {
      except("DD4hep_DetElementVolumeIDs",
	     "+++ NO volume identifiers assigned to DetElement %s. %s",
	     "Something went wrong!",detector.c_str());
    }
    return count > 0 ? 1 : 0;
  }
}
DECLARE_APPLY(DD4hep_DetElementVolumeIDs,assign_de_volumeIDs)

/// Default constructor
DetElementVolumeIDs::DetElementVolumeIDs(const Detector& description)
  : m_detDesc(description)
{
}

/// Populate the Volume manager
std::size_t DetElementVolumeIDs::populate(DetElement det) {
  std::size_t  count = 0UL;
  Encoding     encoding { 0, 0 };
  PlacedVolume pv = det.placement();

  entries.clear();
  if ( !pv.isValid() )   {
    except("DetElementVolumeIDs",
	   "+++ Top level DetElement %s has no valid placement. %s",
	   "[Something awfully wrong]", det.path().c_str());
  }
  if ( det == m_detDesc.world() )   {
    for (const auto& i : det.children() )  {
      DetElement   de = i.second;
      pv = de.placement();
      if (pv.isValid()) {
	Chain    chain;
	Encoding coding {0, 0};
	SensitiveDetector sd(0);
	count += scanPhysicalVolume(de, de, pv, coding, sd, chain);
	continue;
      }
      printout(WARNING, "DetElementVolumeIDs", "++ Detector element %s of type %s has no placement.", 
	       de.name(), de.type().c_str());
    }
    printout(INFO, "DetElementVolumeIDs", "++ Assigned %ld volume identifiers to DetElements.", count); 
    return count;
  }
  SensitiveDetector sd = m_detDesc.sensitiveDetector(det.name());
  if ( !pv.volIDs().empty() && !sd.isValid() )   {
    except("DetElementVolumeIDs",
	   "+++ No sensitive detector available for top level DetElement %s.",
	   det.path().c_str());
  }
  Chain chain;
  count += scanPhysicalVolume(det, det, pv, encoding, sd, chain);
  printout(INFO, "DetElementVolumeIDs", "++ Assigned %ld volume identifiers to DetElements.", count); 
  return count;
}

/// Scan a single physical volume and look for sensitive elements below
std::size_t DetElementVolumeIDs::scanPhysicalVolume(DetElement&        parent,
						    DetElement         e,
						    PlacedVolume       pv, 
						    Encoding           parent_encoding,
						    SensitiveDetector& sd,
						    Chain&             chain)
{
  TGeoNode* node = pv.ptr();
  std::size_t count = 0;
  if (node) {
    Volume vol = pv.volume();
    const VolIDs& pv_ids   = pv.volIDs();
    Encoding vol_encoding  = parent_encoding;
    bool     is_sensitive  = vol.isSensitive();
    bool     have_encoding = pv_ids.empty();
    bool     compound      = e.type() == "compound";

    if ( compound )  {
      sd = SensitiveDetector(0);
      vol_encoding = Encoding();
    }
    else if ( !sd.isValid() )  {
      if ( is_sensitive )
	sd = vol.sensitiveDetector();
      else if ( (parent->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR) )
	sd = m_detDesc.sensitiveDetector(parent.name());
      else if ( (e->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR) )
	sd = m_detDesc.sensitiveDetector(e.name());
    }
    chain.emplace_back(node);
    if ( sd.isValid() && !pv_ids.empty() )   {
      Readout ro = sd.readout();
      if ( ro.isValid() )   {
	vol_encoding  = update_encoding(ro.idSpec(), pv_ids, parent_encoding);
	have_encoding = true;
      }
      else {
	printout(WARNING, "DetElementVolumeIDs",
		 "%s: Strange constellation volume %s is sensitive, but has no readout! sd:%p",
		 parent.name(), pv.volume().name(), sd.ptr());
      }
    }
    for (int idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
      TGeoNode*    daughter = node->GetDaughter(idau);
      PlacedVolume place_dau(daughter);
      if ( place_dau.data() ) {
	DetElement   de_dau;
	/// Check if this particular volume is the placement of one of the
	/// children of this detector element. If the daughter placement is also
	/// a detector child, then we must reset the node chain.
	for( const auto& de : e.children() )  {
	  if ( de.second.placement().ptr() == daughter )  {
	    de_dau = de.second;
	    break;
	  }
	}
	if ( de_dau.isValid() ) {
	  Chain dau_chain;
	  count += scanPhysicalVolume(parent, de_dau, place_dau, vol_encoding, sd, dau_chain);
	}
	else { // there may be several layers of volumes between mother-child of DE
	  count += scanPhysicalVolume(parent, e, place_dau, vol_encoding, sd, chain);
	}
      }
      else  {
	except("DetElementVolumeIDs",
	       "Invalid not instrumented placement: %s %s", daughter->GetName(),
	       " [Internal error -- bad detector constructor]");
      }
      /// For compounds the upper level sensitive detector does not exist,
      /// because there are multiple at lower layers
      if ( compound )  {
	sd = SensitiveDetector(0);
      }
    }
    if ( sd.isValid() )   {
      if ( !have_encoding && !compound )   {
	printout(ERROR, "DetElementVolumeIDs",
		 "Element %s: Missing SD encoding. Volume manager won't work!",
		 e.path().c_str());
      }
      if ( is_sensitive || count > 0 )  {
	// Either this layer is sensitive of a layer below.
	if ( node == e.placement().ptr() )  {
	  // These here are placement nodes, which at the same time are DetElement placements
	  // 1) We recuperate volumes from lower levels by reusing the subdetector
	  //    This only works if there is exactly one sensitive detector per subdetector!
	  // 2) DetElements in the upper hierarchy of the sensitive also get a volume id,
	  //    and the volume is registered. (to be discussed)
	  //
	  e.object<DetElement::Object>().volumeID = vol_encoding.identifier;
	}
	// Collect all sensitive volumes, which belong to the next DetElement
	if ( entries.find(e) == entries.end()) {
	  entries[e].emplace_back(vol_encoding);
	  ++m_numNodes;
	}
	++count;
      }
    }
    chain.pop_back();
  }
  return count;
}
