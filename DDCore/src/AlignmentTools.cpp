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
#include "DD4hep/IOV.h"
#include "DD4hep/Printout.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/AlignmentTools.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"

// ROOT include files
#include "TGeoMatrix.h"

using     DD4hep::Geometry::DetElement;
using     DD4hep::Geometry::PlacedVolume;
using     DD4hep::Alignments::Alignment;
using     DD4hep::Alignments::AlignmentData;
namespace DetectorTools = DD4hep::Geometry::DetectorTools;
typedef   AlignmentData::MaskManipulator MaskManipulator;

namespace {
  void reset_matrix(TGeoHMatrix* m)  {
    double tr[3]  = {0e0,0e0,0e0};
    double rot[9] = {1e0,0e0,0e0,
                     0e0,1e0,0e0,
                     0e0,0e0,1e0};
    m->SetTranslation(tr);
    m->SetRotation(rot);
  }

}

/// Copy alignment object from source object
void DD4hep::Alignments::AlignmentTools::copy(Alignment from, Alignment to)   {
  const AlignmentData& f = from.ptr()->values();
  AlignmentData& t = to.ptr()->values();
  if ( &t != &f )   {
    t.flag          = f.flag;
    t.detectorTrafo = f.detectorTrafo;
    t.worldTrafo    = f.worldTrafo;
    t.trToWorld     = f.trToWorld;
    t.detector      = f.detector;
    t.placement     = f.placement;
    t.nodes         = f.nodes;
    t.delta         = f.delta;
    t.magic         = f.magic;
  }
}

/// Compute the ideal/nominal to-world transformation from the detector element placement
void DD4hep::Alignments::AlignmentTools::computeIdeal(Alignment alignment)   {
  AlignmentData& a = alignment->values();
  MaskManipulator mask(a.flag);
  DetElement parent = a.detector.parent();
  reset_matrix(&a.detectorTrafo);
  if ( parent.isValid() )  {
    DetectorTools::PlacementPath path;
    DetectorTools::placementPath(parent, a.detector, path);

    for (size_t i = 0, n=path.size(); n>0 && i < n-1; ++i)  {
      const PlacedVolume& p = path[i];
      a.detectorTrafo.MultiplyLeft(p->GetMatrix());
      a.nodes.push_back(p);
    }
    //a.worldTrafo = parent.nominal()->worldTrafo;
    //a.worldTrafo.MultiplyLeft(&a.detectorTrafo);
    a.worldTrafo = a.detectorTrafo;
    a.worldTrafo.MultiplyLeft(&parent.nominal().worldTransformation());
    a.trToWorld  = Geometry::_transform(&a.worldTrafo);
    a.placement  = a.detector.placement();
    mask.clear();
    mask.set(AlignmentData::HAVE_PARENT_TRAFO);
    mask.set(AlignmentData::HAVE_WORLD_TRAFO);
    mask.set(AlignmentData::IDEAL);
  }
  else  {
    reset_matrix(&a.worldTrafo);
  }
}
#if 0
/// Compute the ideal/nominal to-world transformation from the detector element placement
void DD4hep::Alignments::AlignmentTools::computeIdeal(Alignment alignment, 
                                                      const Alignment::NodeList& node_list)
{
  AlignmentData& a = alignment->values();
  MaskManipulator mask(a.flag);
  a.nodes = node_list;
  for (size_t i = a.nodes.size(); i > 1; --i) {   // Omit the placement of the parent DetElement
    TGeoMatrix* m = a.nodes[i - 1]->GetMatrix();
    a.worldTrafo.MultiplyLeft(m);
  }
  DetElement::Object* det = a.detector.ptr();
  if ( !a.nodes.empty() ) {
    a.detectorTrafo = a.worldTrafo;
    a.detectorTrafo.MultiplyLeft(a.nodes[0]->GetMatrix());
    a.placement = node_list.back();
  }
  else  {
    a.placement = det->placement;
  }
  a.worldTrafo.MultiplyLeft(&(a.detector.nominal().worldTransformation()));
  a.trToWorld = Geometry::_transform(&a.worldTrafo);
  mask.set(AlignmentData::IDEAL);
}
#endif

/// Compute the survey to-world transformation from the detector element placement with respect to the survey constants
void DD4hep::Alignments::AlignmentTools::computeSurvey(Alignment alignment)
{
  AlignmentData& a = alignment->values();
  DetElement parent = a.detector.parent();
  MaskManipulator mask(a.flag);

  if ( parent.isValid() )  {
    DetectorTools::PlacementPath path;
    DetectorTools::placementPath(parent, a.detector, path);

    // TODO: need to take survey corrections into account!

    for (size_t i = 0, n=path.size(); n>0 && i < n-1; ++i)  {
      const PlacedVolume& p = path[i];
      a.detectorTrafo.MultiplyLeft(p->GetMatrix());
    }
    a.worldTrafo = parent.survey().worldTransformation();
    a.worldTrafo.MultiplyLeft(&a.detectorTrafo);
    a.trToWorld  = Geometry::_transform(&a.worldTrafo);
    a.placement = a.detector.placement();
  }
  mask.set(AlignmentData::SURVEY);
  //mask.clear(AlignmentData::INVALID|AlignmentData::DIRTY);
  //mask.set(AlignmentData::VALID|AlignmentData::IDEAL);
}
