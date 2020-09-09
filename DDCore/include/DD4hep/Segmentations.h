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
#ifndef DD4HEP_SEGMENTATIONS_H
#define DD4HEP_SEGMENTATIONS_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"
#include "DD4hep/BitFieldCoder.h"
#include "DDSegmentation/Segmentation.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declarations
  class DetElementObject;
  class SegmentationObject;
  class SensitiveDetectorObject;
  template <typename T> class SegmentationWrapper;

  /// Handle class supporting generic Segmentations of sensitive detectors
  /**
   *   This basic segmentation implementation supports "generic" access
   *   to specific segmentations exporting the common abstract interface.
   *   Mostly used in simulation.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Segmentation : public Handle<SegmentationObject> {
  public:
    /// Initializing constructor creating a new object of the given DDSegmentation type
    Segmentation(const std::string& type, const std::string& name, const BitFieldCoder* decoder);
    /// Default constructor
    Segmentation() = default;
    /// Move Constructor
    Segmentation(Segmentation&& e) = default;
    /// Copy Constructor
    Segmentation(const Segmentation& e) = default;
#ifndef __CINT__
    /// Move Constructor from handle
    Segmentation(Handle<Object>&& e) : Handle<Object>(e) { }
    /// Copy Constructor from handle
    Segmentation(const Handle<Object>& e) : Handle<Object>(e) { }
#endif
    /// Constructor to be used when reading the already parsed object
    template <typename Q> Segmentation(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Move Assignment operator
    Segmentation& operator=(Segmentation&& seg) = default;
    /// Copy Assignment operator
    Segmentation& operator=(const Segmentation& seg) = default;
    /// Access flag for hit positioning
    bool useForHitPosition() const;
    /// Accessor: Segmentation type
    const char* name() const;
    /// Accessor: Segmentation type
    std::string type() const;
    /// Access to the parameters
    DDSegmentation::Parameters parameters() const;
    /// Access to parameter by name
    DDSegmentation::Parameter  parameter(const std::string& parameterName) const;
    /// Access the main detector element using this segmetnation object
    Handle<DetElementObject> detector() const;
    /// Access the sensitive detector using this segmetnation object
    Handle<SensitiveDetectorObject> sensitive() const;
    /// Access the underlying decoder
    const BitFieldCoder* decoder() const;
    /// Set the underlying decoder
    void setDecoder(const BitFieldCoder* decoder) const;
    /// determine the local position based on the cell ID
    Position position(const long64& cellID) const;
    /// determine the cell ID based on the local position
    long64 cellID(const Position& localPosition, const Position& globalPosition, const long64& volumeID) const;
    /// Determine the volume ID from the full cell ID by removing all local fields
    VolumeID volumeID(const CellID& cellID) const;
    /// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
    void neighbours(const CellID& cellID, std::set<CellID>& neighbours) const;
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
     *  in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi
     *
     *   \param cellID cellID of the cell for which parameters are returned
     *   \return vector<double> in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi
     */
    std::vector<double> cellDimensions(const CellID& cellID) const;
      
    /// Access to the base DDSegmentation object. WARNING: Deprecated call!
    DDSegmentation::Segmentation* segmentation() const;
  };
} /* End namespace dd4hep                */
#endif // DD4HEP_SEGMENTATIONS_H
