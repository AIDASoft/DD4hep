//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

/*
 * CartesianStripZ.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#ifndef DDSEGMENTATION_CARTESIANSTRIPZ_H
#define DDSEGMENTATION_CARTESIANSTRIPZ_H

#include "DDSegmentation/CartesianStrip.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian strip segmentation in Z
    class CartesianStripZ : public DDSegmentation::CartesianStrip {
    public:
      /// Default constructor passing the encoding string
      CartesianStripZ(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      CartesianStripZ(const BitFieldCoder* decoder);
      /// destructor
      virtual ~CartesianStripZ();

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition,
                            const VolumeID& volumeID) const;
      /// access the strip size in Z
      double stripSizeZ() const { return _stripSizeZ; }
      /// access the coordinate offset in Z
      double offsetZ() const { return _offsetZ; }
      /// access the field name used for Z
      const std::string& fieldNameZ() const { return _xId; }
      /// set the strip size in Z
      void setStripSizeZ(double cellSize) { _stripSizeZ = cellSize; }
      /// set the coordinate offset in Z
      void setOffsetZ(double offset) { _offsetZ = offset; }
      /// set the field name used for Z
      void setFieldNameZ(const std::string& fieldName) { _xId = fieldName; }
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

          Returns a vector of the cellDimensions of the given cell ID
          \param cellID is ignored as all cells have the same dimension
          \return std::vector<double> size 1:
          -# size in x
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

    protected:
      /// the strip size in Z
      double _stripSizeZ;
      /// the coordinate offset in Z
      double _offsetZ;
      /// the field name used for Z
      std::string _xId;
    };
  }  // namespace DDSegmentation
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CARTESIANSTRIPZ_H
