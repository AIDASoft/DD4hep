/*
 * CartesianStripY.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#ifndef DDSegmentation_CARTESIANSTRIPY_H_
#define DDSegmentation_CARTESIANSTRIPY_H_

#include "DDSegmentation/CartesianStrip.h"

namespace dd4hep {
namespace DDSegmentation {
class CartesianStripY : public DDSegmentation::CartesianStrip {
   public:
    /// Default constructor passing the encoding string
    CartesianStripY(const std::string& cellEncoding = "");
    /// Default constructor used by derived classes passing an existing decoder
    CartesianStripY(const BitFieldCoder* decoder);
    /// destructor
    virtual ~CartesianStripY();

    /// determine the position based on the cell ID
    virtual Vector3D position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition,
                          const VolumeID& volumeID) const;
    /// access the strip size in Y
    double stripSizeY() const { return _stripSizeY; }
    /// access the coordinate offset in Y
    double offsetY() const { return _offsetY; }
    /// access the field name used for Y
    const std::string& fieldNameY() const { return _xId; }
    /// set the strip size in Y
    void setStripSizeY(double cellSize) { _stripSizeY = cellSize; }
    /// set the coordinate offset in Y
    void setOffsetY(double offset) { _offsetY = offset; }
    /// set the field name used for Y
    void setFieldNameY(const std::string& fieldName) { _xId = fieldName; }
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

        Returns a vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 1:
        -# size in x
    */
    virtual std::vector<double> cellDimensions(const CellID& cellID) const;

   protected:
    /// the strip size in Y
    double _stripSizeY;
    /// the coordinate offset in Y
    double _offsetY;
    /// the field name used for Y
    std::string _xId;
};
}  // namespace DDSegmentation
} /* namespace dd4hep */
#endif  // DDSegmentation_CARTESIANSTRIPY_H_
