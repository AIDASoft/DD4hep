/*
 * CartesianStripZ.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#include "DDSegmentation/CartesianStripZ.h"

namespace dd4hep {
namespace DDSegmentation {
/// default constructor using an encoding string
CartesianStripZ::CartesianStripZ(const std::string& cellEncoding) : CartesianStrip(cellEncoding) {
    // define type and description
    _type = "CartesianStripZ";
    _description = "Cartesian segmentation on the local Z axis";

    // register all necessary parameters
    registerParameter("strip_size_x", "Cell size in Z", _stripSizeZ, 1., SegmentationParameter::LengthUnit);
    registerParameter("offset_x", "Cell offset in Z", _offsetZ, 0., SegmentationParameter::LengthUnit, true);
    registerIdentifier("identifier_x", "Cell ID identifier for Z", _xId, "strip");
}

/// Default constructor used by derived classes passing an existing decoder
CartesianStripZ::CartesianStripZ(const BitFieldCoder* decode) : CartesianStrip(decode) {
    // define type and description
    _type = "CartesianStripZ";
    _description = "Cartesian segmentation on the local Z axis";

    // register all necessary parameters
    registerParameter("strip_size_x", "Cell size in Z", _stripSizeZ, 1., SegmentationParameter::LengthUnit);
    registerParameter("offset_x", "Cell offset in Z", _offsetZ, 0., SegmentationParameter::LengthUnit, true);
    registerIdentifier("identifier_x", "Cell ID identifier for Z", _xId, "strip");
}

/// destructor
CartesianStripZ::~CartesianStripZ() {}

/// determine the position based on the cell ID
Vector3D CartesianStripZ::position(const CellID& cID) const {
    Vector3D cellPosition;
    cellPosition.Z = binToPosition(_decoder->get(cID, _xId), _stripSizeZ, _offsetZ);
    return cellPosition;
}

/// determine the cell ID based on the position
CellID CartesianStripZ::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */,
                               const VolumeID& vID) const {
    CellID cID = vID;
    _decoder->set(cID, _xId, positionToBin(localPosition.Z, _stripSizeZ, _offsetZ));
    return cID;
}

std::vector<double> CartesianStripZ::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
    return {_stripSizeZ};
#else
    std::vector<double> cellDims(1, 0.0);
    cellDims[0] = _stripSizeZ;
    return cellDims;
#endif
}

REGISTER_SEGMENTATION(CartesianStripZ)
}  // namespace DDSegmentation
} /* namespace dd4hep */
