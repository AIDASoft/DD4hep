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
#ifndef DD4HEP_GEOMETRY_SEGMENTATIONS_H
#define DD4HEP_GEOMETRY_SEGMENTATIONS_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"
#include "DD4hep/BitField64.h"
#include "DDSegmentation/Segmentation.h"

// C/C++ include files


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Forward declarations
    class DetElementObject;
    class SegmentationObject;
    class SensitiveDetectorObject;
    
    /// Handle class supporting generic Segmentation of sensitive detectors
    /**
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_GEOMETRY
     */
    class Segmentation: public Handle<SegmentationObject> {
    public:
      typedef SegmentationObject           Object;
      typedef DDSegmentation::Segmentation BaseSegmentation;
      typedef DDSegmentation::Parameter    Parameter;
      typedef DDSegmentation::Parameters   Parameters;

    public:
      /// Initializing constructor creating a new object of the given DDSegmentation type
      Segmentation(const std::string& type, const std::string& name, BitField64* decoder);
      /// Default constructor
      Segmentation() : Handle<Object>() { }
      /// Copy Constructor from object
      Segmentation(const Segmentation& e) : Handle<Object>(e) { }
#ifndef __CINT__
      /// Copy Constructor from handle
      Segmentation(const Handle<Object>& e) : Handle<Object>(e) { }
#endif
      /// Constructor to be used when reading the already parsed object
      template <typename Q>
      Segmentation(const Handle<Q>& e) : Handle<Object>(e) { }
      /// Assignment operator
      Segmentation& operator=(const Segmentation& seg) = default;
      /// Access flag for hit positioning
      bool useForHitPosition() const;
      /// Accessor: Segmentation type
      std::string type() const;
      /// Access to the parameters
      Parameters parameters() const;
      /// Access the main detector element using this segmetnation object
      Handle<DetElementObject> detector() const;
      /// Access the sensitive detector using this segmetnation object
      Handle<SensitiveDetectorObject> sensitive() const;
      /// Access the underlying decoder
      BitField64* decoder() const;
      /// Set the underlying decoder
      void setDecoder(BitField64* decoder) const;
      /// determine the local position based on the cell ID
      Position position(const long64& cellID) const;
      /// determine the cell ID based on the local position
      long64 cellID(const Position& localPosition, const Position& globalPosition, const long64& volumeID) const;
      /// Determine the volume ID from the full cell ID by removing all local fields
      VolumeID volumeID(const CellID& cellID) const;
      /// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
      void neighbours(const CellID& cellID, std::set<CellID>& neighbours) const;
    };

  } /* End namespace Geometry              */
} /* End namespace DD4hep                */
#endif    /* DD4HEP_GEOMETRY_SEGMENTATIONS_H     */
