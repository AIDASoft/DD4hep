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
#include "DDDigi/DigiSubdetectorSequence.h"
#include "DDDigi/DigiSegmentation.h"
#include "DDDigi/DigiKernel.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Detector.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/detail/VolumeManagerInterna.h"
#include "TClass.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiSubdetectorSequence::DigiSubdetectorSequence(const DigiKernel& kernel, const string& nam)
  : DigiActionSequence(kernel, nam)
{
  declareProperty("detector",m_detectorName);
  declareProperty("parallize_by",m_segmentName);
  m_cellHandler = [this](DigiContext& context, const DigiCellScanner& scanner, const DigiCellData& data)  {
    this->process_cell(context, scanner, data);
  };
  InstanceCount::increment(this);
}

/// Default destructor
DigiSubdetectorSequence::~DigiSubdetectorSequence() {
  InstanceCount::decrement(this);
}

/// Initialize subdetector sequencer
void DigiSubdetectorSequence::initialize()   {
  info("Initializing detector sequencer for detector: %s",m_detectorName.c_str());
  m_detector     = subdetector(m_detectorName);
  m_sensDet      = sensitiveDetector(m_detectorName);
  m_parallelVid.clear();
  m_parallelDet.clear();
  if ( m_detector.isValid() && m_sensDet.isValid() )   {
    m_idDesc       = m_sensDet.readout().idSpec();
    m_segmentation = m_sensDet.readout().segmentation();
    const VolIDs& ids = m_detector.placement().volIDs();
    VolumeID      vid = m_idDesc.encode(ids);
    VolumeID      msk = m_idDesc.get_mask(ids);
    scan_detector(m_detector, vid, msk);
  }
}

void DigiSubdetectorSequence::scan_sensitive(PlacedVolume pv, VolumeID vid, VolumeID mask)   {
  Volume vol = pv.volume();
  if ( vol.isSensitive() )    {
    Solid sol = vol.solid();
    auto  key = make_pair(sol->IsA(), m_segmentation);
    auto  is  = m_scanners.find(key);
    if ( is == m_scanners.end() )  {
      is = m_scanners.insert(make_pair(key, create_cell_scanner(sol, m_segmentation))).first;
    }
  }
  for (int idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau) {
    PlacedVolume  p(pv->GetDaughter(idau));
    const VolIDs& new_ids = p.volIDs();
    if ( !new_ids.empty() )   {
      VolumeID new_vid = vid  | m_idDesc.encode(new_ids);
      VolumeID new_msk = mask | m_idDesc.get_mask(new_ids);
      scan_sensitive(p, new_vid, new_msk);
      continue;
    }
    scan_sensitive(p, vid, mask);
  }
}

void DigiSubdetectorSequence::scan_detector(DetElement de, VolumeID vid, VolumeID mask)   {
  const VolIDs& new_ids = de.placement().volIDs();
  VolumeID      new_vid = vid;
  VolumeID      new_msk = mask;
  if ( !new_ids.empty() )   {
    new_vid |= m_idDesc.encode(new_ids);
    new_msk |= m_idDesc.get_mask(new_ids);
    for (const auto& id : new_ids)   {
      if ( id.first == m_segmentName )   {
        VolumeID rid = detail::reverseBits<VolumeID>(new_vid);
        m_parallelVid.emplace(make_pair(rid, Context(de, new_vid, rid, new_msk)));
        m_parallelDet.emplace(make_pair(de, new_vid));
        scan_sensitive(de.placement(), new_vid, new_msk);
        return;
      }
    }
  }
  for ( const auto& c : de.children() )
    scan_detector(c.second, new_vid, new_msk);
}


void DigiSubdetectorSequence::process_cell(DigiContext&, const DigiCellScanner& , const DigiCellData& data)  const   {
#if 0
  Segmentation seg  = m_sensDet.readout().segmentation();
    string       desc = m_idDesc.str(data.cell_id);
    info("Sensitive: [%s/%s]   vid:%s %s",
         data.solid->IsA()->GetName(),
         seg.type().c_str(),
         volumeID(data.cell_id).c_str(),
         desc.c_str());
#endif
  if ( data.cell_id )  {
  }
}

void DigiSubdetectorSequence::process_context(DigiContext& context,
                                              const Context& c,
                                              PlacedVolume pv,
                                              VolumeID vid,
                                              VolumeID mask)   const
{
  Volume vol = pv.volume();
  if ( vol.isSensitive() )    {
    auto key = make_pair(vol->GetShape()->IsA(), m_segmentation);
    auto is  = m_scanners.find(key);
    if ( is == m_scanners.end() )   {
      except("Fatal error in process_context: Invalid cell scanner. vid: %016X",vid);
    }
    (*(is->second))(context, pv, vid, m_cellHandler);
    return;
  }
  for (int idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau) {
    PlacedVolume p(pv->GetDaughter(idau));
    const VolIDs& new_ids = p.volIDs();
    if ( !new_ids.empty() )
      process_context(context, c, p, vid | m_idDesc.encode(new_ids), mask | m_idDesc.get_mask(new_ids));
    else
      process_context(context, c, p, vid, mask);
  }
}

/// Pre-track action callback
void DigiSubdetectorSequence::execute(DigiContext& context)  const   {
  for( const auto& d : m_parallelVid )   {
    const Context& c = d.second;
    auto vid = c.detector_id;
    auto det = c.detector;
    string id_desc   = m_idDesc.str(vid);
    info("  Order:%-64s    vid:%s %s %s",
         det.path().c_str(), volumeID(d.first).c_str(), volumeID(vid).c_str(), id_desc.c_str());
    process_context(context, c, c.detector.placement(), c.detector_id, c.detector_mask);
  }
  this->DigiSynchronize::execute(context);
  debug("+++ Event: %8d (DigiSubdetectorSequence) Parallel: %s Done.",
        context.event().eventNumber, yes_no(m_parallel));
}

/// Access subdetector from the detector description
dd4hep::DetElement DigiSubdetectorSequence::subdetector(const string& nam)   const   {
  return m_kernel.detectorDescription().detector(nam);
}

/// Access sensitive detector from the detector description
dd4hep::SensitiveDetector DigiSubdetectorSequence::sensitiveDetector(const string& nam)   const   {
  return m_kernel.detectorDescription().sensitiveDetector(nam);
}
