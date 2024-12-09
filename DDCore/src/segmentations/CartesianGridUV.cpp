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
 * CartesianGridUV.cpp
 *
 *  Created on: December 5, 2024
 *      Author: Yann Bedfer, ePIC/Saclay
 *
 *  Derived from "./CartesianGridXY.cpp".
 */
#include <DDSegmentation/CartesianGridUV.h>

namespace dd4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
CartesianGridUV::CartesianGridUV(const std::string& cellEncoding) :
		CartesianGrid(cellEncoding) {
	// define type and description
	_type = "CartesianGridUV";
	_description = "Cartesian segmentation along U,V rotated some angle from local X,Y axes";

	// register all necessary parameters
	registerParameter("grid_size_u", "Cell size in U",   _gridSizeU, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_v", "Cell size in V",   _gridSizeV, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_u",    "Cell offset in U", _offsetU,   0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_v",    "Cell offset in V", _offsetV,   0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_u", "Cell ID identifier for U", _uId, "u");
	registerIdentifier("identifier_v", "Cell ID identifier for V", _vId, "v");
	registerParameter("grid_angle", "Angle of U measurement axis in X,Y frame", _gridAngle, 0., SegmentationParameter::AngleUnit);
};

/// Default constructor used by derived classes passing an existing decoder
CartesianGridUV::CartesianGridUV(const BitFieldCoder* decode) :
		CartesianGrid(decode)
{
	// define type and description
	_type = "CartesianGridUV";
	_description = "Cartesian segmentation along U,V rotated some angle from local X,Y axes";

	// register all necessary parameters
	registerParameter("grid_size_u", "Cell size in U",   _gridSizeU, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_v", "Cell size in V",   _gridSizeV, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_u",    "Cell offset in U", _offsetU,   0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_v",    "Cell offset in V", _offsetV,   0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_u", "Cell ID identifier for U", _uId, "u");
	registerIdentifier("identifier_v", "Cell ID identifier for V", _vId, "v");
	registerParameter("grid_angle", "Angle of U measurement axis in X,Y frame", _gridAngle, 0., SegmentationParameter::AngleUnit);
};

/// destructor
CartesianGridUV::~CartesianGridUV() {

}

/// determine the position based on the cell ID
Vector3D CartesianGridUV::position(const CellID& cID) const {
	Vector3D cellPosition;
	cellPosition.X = binToPosition( _decoder->get(cID,_uId ), _gridSizeU, _offsetU);
	cellPosition.Y = binToPosition( _decoder->get(cID,_vId ), _gridSizeV, _offsetV);
	cellPosition = RotationZ(-_gridAngle)*cellPosition;
	return cellPosition;
}

/// determine the cell ID based on the position
CellID CartesianGridUV::cellID(const Vector3D& localPosition,
                               const Vector3D& /* globalPosition */,
                               const VolumeID& vID) const {
	CellID cID = vID;
	const Vector3D& localUV = RotationZ(_gridAngle)*localPosition;
	_decoder->set( cID,_uId, positionToBin(localUV.X, _gridSizeU, _offsetU) );
	_decoder->set( cID,_vId, positionToBin(localUV.Y, _gridSizeV, _offsetV) );
	return cID;
}

  std::vector<double> CartesianGridUV::cellDimensions(const CellID& /* cellID */) const {
#if __cplusplus >= 201103L
  return {_gridSizeU, _gridSizeV};
#else
  std::vector<double> cellDims(2,0.0);
  cellDims[0] = _gridSizeU;
  cellDims[1] = _gridSizeV;
  return cellDims;
#endif
}


} /* namespace DDSegmentation */
} /* namespace dd4hep */

// This is done DDCore/src/plugins/ReadoutSegmentations.cpp so the plugin is not part of libDDCore
// needs also #include <DD4hep/Factories.h>
// DECLARE_SEGMENTATION(CartesianGridUV,dd4hep::create_segmentation<dd4hep::DDSegmentation::CartesianGridUV>)
