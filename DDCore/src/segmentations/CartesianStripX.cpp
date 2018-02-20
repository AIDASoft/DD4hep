/*
 * CartesianStripX.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#include "DDSegmentation/CartesianStripX.h"

namespace dd4hep {
namespace DDSegmentation {
/// default constructor using an encoding string
CartesianStripX::CartesianStripX(const std::string& cellEncoding) : CartesianStrip(cellEncoding) {
    // define type and description
    _type = "CartesianStripX";
    _description = "Cartesian segmentation on the local X axis";

    // register all necessary parameters
    registerParameter("strip_size_x", "Cell size in X", _stripSizeX, 1., SegmentationParameter::LengthUnit);
    registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
    registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "strip");
}

/// Default constructor used by derived classes passing an existing decoder
CartesianStripX::CartesianStripX(const BitFieldCoder* decode) : CartesianStrip(decode) {
    // define type and description
    _type = "CartesianStripX";
    _description = "Cartesian segmentation on the local X axis";

    // register all necessary parameters
    registerParameter("strip_size_x", "Cell size in X", _stripSizeX, 1., SegmentationParameter::LengthUnit);
    registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
    registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "strip");
}

/// destructor
CartesianStripX::~CartesianStripX() {}

/// determine the position based on the cell ID
Vector3D CartesianStripX::position(const CellID& cID) const {
    Vector3D cellPosition;
    cellPosition.X = binToPosition(_decoder->get(cID, _xId), _stripSizeX, _offsetX);
    return cellPosition;
}

/// determine the cell ID based on the position
CellID CartesianStripX::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */,
                               const VolumeID& vID) const {
    CellID cID = vID;
    _decoder->set(cID, _xId, positionToBin(localPosition.X, _stripSizeX, _offsetX));
    return cID;
}

std::vector<double> CartesianStripX::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
    return {_stripSizeX};
#else
    std::vector<double> cellDims(1, 0.0);
    cellDims[0] = _stripSizeX;
    return cellDims;
#endif
}

REGISTER_SEGMENTATION(CartesianStripX)
}  // namespace DDSegmentation
} /* namespace dd4hep */
