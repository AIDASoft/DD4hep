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
 * CartesianStripX.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#ifndef DDSEGMENTATION_CARTESIANSTRIPX_H
#define DDSEGMENTATION_CARTESIANSTRIPX_H

#include "DDSegmentation/CartesianStrip.h"

namespace dd4hep {
namespace DDSegmentation {

      /// Segmentation base class describing cartesian strip segmentation in X
class CartesianStripX : public DDSegmentation::CartesianStrip {
   public:
    /// Default constructor passing the encoding string
    CartesianStripX(const std::string& cellEncoding = "");
    /// Default constructor used by derived classes passing an existing decoder
    CartesianStripX(const BitFieldCoder* decoder);
    /// destructor
    virtual ~CartesianStripX();

    /// determine the position based on the cell ID
    virtual Vector3D position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition,
                          const VolumeID& volumeID) const;
    /// access the strip size in X
    double stripSizeX() const { return _stripSizeX; }
    /// access the coordinate offset in X
    double offsetX() const { return _offsetX; }
    /// access the field name used for X
    const std::string& fieldNameX() const { return _xId; }
    /// set the strip size in X
    void setStripSizeX(double cellSize) { _stripSizeX = cellSize; }
    /// set the coordinate offset in X
    void setOffsetX(double offset) { _offsetX = offset; }
    /// set the field name used for X
    void setFieldNameX(const std::string& fieldName) { _xId = fieldName; }
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

        Returns a vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 1:
        -# size in x
    */
    virtual std::vector<double> cellDimensions(const CellID& cellID) const;

   protected:
    /// the strip size in X
    double _stripSizeX;
    /// the coordinate offset in X
    double _offsetX;
    /// the field name used for X
    std::string _xId;
};
}  // namespace DDSegmentation
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CARTESIANSTRIPX_H
