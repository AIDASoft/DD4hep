//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-10-18
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_NOSEGMENTATION_H
#define DD4HEP_NOSEGMENTATION_H 1

// Framework include files
#include "DD4hep/Segmentations.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class NoSegmentation;  }
  
  /// We need some abbreviation to make the code more readable.
  typedef Handle<SegmentationWrapper<DDSegmentation::NoSegmentation> > NoSegmentationHandle;

  /// Implementation class for the grid XY segmentation.
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class NoSegmentation : public NoSegmentationHandle  {
  public:
    NoSegmentation() = default;
    NoSegmentation(const NoSegmentation& e) = default;
    NoSegmentation(const Segmentation& e) : Handle<Object>(e) { }
    NoSegmentation(const Handle<Object>& e) : Handle<Object>(e) { }
    template <typename Q>
    NoSegmentation(const Handle<Q>& e) : Handle<Object>(e) { }
    NoSegmentation& operator=(const NoSegmentation& seg) = default;
    bool operator==(const NoSegmentation& seg) const {
      return m_element == seg.m_element;
    }

    Position position(const CellID& cellID) const;
    CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
  };
}       /* End namespace dd4hep                */
#endif // DD4HEP_NOSEGMENTATION_H
