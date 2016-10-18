// $Id$
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
#ifndef DDSegmentation_MULITSEGMENTATION_H_
#define DDSegmentation_MULITSEGMENTATION_H_

#include "DDSegmentation/Segmentation.h"

/// Main handle class to hold a TGeo alignment object of type TGeoPhysicalNode
namespace DD4hep {

  namespace DDSegmentation {

    /// Wrapper to support multiple segmentations
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DDSEGMENTATION
     */
    class MultiSegmentation : public Segmentation {
    public:
      /// Default constructor passing the encoding string
      MultiSegmentation(const std::string& cellEncoding = "");

      /// Default constructor used by derived classes passing an existing decoder
      MultiSegmentation(BitField64* decoder);

      /// Default destructor
      virtual ~MultiSegmentation();

      /// Add subsegmentation.
      virtual void addSubsegmentation(long key_min, long key_max, Segmentation* entry);

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;

      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;

      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

          \param cellID cellID of the cell for which parameters are returned
          \return vector<double> in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

      /// access the field name used to discriminate sub-segmentations
      const std::string& discriminatorName() const {  return m_discriminatorId;  }

      /// Discriminating bitfield entry
      BitFieldValue* discriminator() const         {  return m_discriminator;    }

      /// Set the underlying decoder
      virtual void setDecoder(BitField64* decoder);

    protected:
      struct Entry {
        long key_min, key_max;
        Segmentation* segmentation;
      };
      typedef std::vector<Entry> Segmentations;

      /// Access subsegmentation by cell identifier
      const Segmentation& subsegmentation(const CellID& cellID) const;

      /// Sub-segmentaion container
      Segmentations  m_segmentations;

      /// the field name used to discriminate sub-segmentations
      std::string    m_discriminatorId;

      /// Bitfield corresponding to dicriminator identifier
      BitFieldValue* m_discriminator;

      /// Debug flags
      int m_debug;
    };

  } /* namespace DDSegmentation */
} /* namespace DD4hep */

#endif /* DDSegmentation_MULITSEGMENTATION_H_ */
