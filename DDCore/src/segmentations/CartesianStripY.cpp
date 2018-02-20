/*
 * CartesianStripY.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#include "DDSegmentation/CartesianStripY.h"

namespace dd4hep {
namespace DDSegmentation {
/// default constructor using an encoding string
CartesianStripY::CartesianStripY(const std::string& cellEncoding) : CartesianStrip(cellEncoding) {
    // define type and description
    _type = "CartesianStripY";
    _description = "Cartesian segmentation on the local Y axis";

    // register all necessary parameters
    registerParameter("strip_size_x", "Cell size in Y", _stripSizeY, 1., SegmentationParameter::LengthUnit);
    registerParameter("offset_x", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
    registerIdentifier("identifier_x", "Cell ID identifier for Y", _xId, "strip");
}

/// Default constructor used by derived classes passing an existing decoder
CartesianStripY::CartesianStripY(const BitFieldCoder* decode) : CartesianStrip(decode) {
    // define type and description
    _type = "CartesianStripY";
    _description = "Cartesian segmentation on the local Y axis";

    // register all necessary parameters
    registerParameter("strip_size_x", "Cell size in Y", _stripSizeY, 1., SegmentationParameter::LengthUnit);
    registerParameter("offset_x", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
    registerIdentifier("identifier_x", "Cell ID identifier for Y", _xId, "strip");
}

/// destructor
CartesianStripY::~CartesianStripY() {}

/// determine the position based on the cell ID
Vector3D CartesianStripY::position(const CellID& cID) const {
    Vector3D cellPosition;
    cellPosition.Y = binToPosition(_decoder->get(cID, _xId), _stripSizeY, _offsetY);
    return cellPosition;
}

/// determine the cell ID based on the position
CellID CartesianStripY::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */,
                               const VolumeID& vID) const {
    CellID cID = vID;
    _decoder->set(cID, _xId, positionToBin(localPosition.Y, _stripSizeY, _offsetY));
    return cID;
}

std::vector<double> CartesianStripY::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
    return {_stripSizeY};
#else
    std::vector<double> cellDims(1, 0.0);
    cellDims[0] = _stripSizeY;
    return cellDims;
#endif
}

REGISTER_SEGMENTATION(CartesianStripY)
}  // namespace DDSegmentation
} /* namespace dd4hep */
