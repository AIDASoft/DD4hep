/*
 * WaferGridXY.cpp
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 */

#include "DDSegmentation/WaferGridXY.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
WaferGridXY::WaferGridXY(const std::string& cellEncoding) :
		CartesianGrid(cellEncoding) {
	// define type and description
	_type = "WaferGridXY";
	_description = "Cartesian segmentation in the local XY-plane for both Normal wafer and Magic wafer(depending on the layer dimensions)";

	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
        registerParameter("identifier_groupMGWafer", "Cell encoding identifier for Magic Wafer group", _identifierMGWaferGroup, std::string("layer"),
                        SegmentationParameter::NoUnit, true);
        registerParameter("identifier_wafer", "Cell encoding identifier for wafer", _identifierWafer, std::string("wafer"),
                        SegmentationParameter::NoUnit, true);
}

/// destructor
WaferGridXY::~WaferGridXY() {

}

/// determine the position based on the cell ID
Vector3D WaferGridXY::position(const CellID& cID) const {
	_decoder->setValue(cID);
        unsigned int _groupMGWaferIndex;
        unsigned int _waferIndex;
	Vector3D cellPosition;

        _groupMGWaferIndex = (*_decoder)[_identifierMGWaferGroup];
        _waferIndex = (*_decoder)[_identifierWafer];

	if ( _waferOffsetX[_groupMGWaferIndex][_waferIndex] > 0 || _waferOffsetX[_groupMGWaferIndex][_waferIndex] < 0 )
	  {
	    cellPosition.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX+_waferOffsetX[_groupMGWaferIndex][_waferIndex]);
	  }
	else
	  {
	    cellPosition.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	  }

	if ( _waferOffsetY[_groupMGWaferIndex][_waferIndex] > 0 || _waferOffsetY[_groupMGWaferIndex][_waferIndex] < 0 )
	  {
	    cellPosition.Y = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY+_waferOffsetY[_groupMGWaferIndex][_waferIndex]);
	  }
	else
	  {
	    cellPosition.Y = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	  }

	return cellPosition;
}

/// determine the cell ID based on the position
  CellID WaferGridXY::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	_decoder->setValue(vID);
        unsigned int _groupMGWaferIndex;
        unsigned int _waferIndex;

        _groupMGWaferIndex = (*_decoder)[_identifierMGWaferGroup];
        _waferIndex = (*_decoder)[_identifierWafer];

	if ( _waferOffsetX[_groupMGWaferIndex][_waferIndex] > 0 || _waferOffsetX[_groupMGWaferIndex][_waferIndex] < 0 )
	  {
	    (*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX+_waferOffsetX[_groupMGWaferIndex][_waferIndex]);
	  }
	else
	  {
	    (*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX);
	  }

	if ( _waferOffsetY[_groupMGWaferIndex][_waferIndex] > 0 ||  _waferOffsetY[_groupMGWaferIndex][_waferIndex] < 0)
	  {
	    (*_decoder)[_yId] = positionToBin(localPosition.Y, _gridSizeY, _offsetY+_waferOffsetY[_groupMGWaferIndex][_waferIndex]);
	  }
	else
	  {
	    (*_decoder)[_yId] = positionToBin(localPosition.Y, _gridSizeY, _offsetY);
	  }

	return _decoder->getValue();
}

std::vector<double> WaferGridXY::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
  return {_gridSizeX, _gridSizeY};
#else
  std::vector<double> cellDims(2,0.0);
  cellDims[0] = _gridSizeX;
  cellDims[1] = _gridSizeY;
  return cellDims;
#endif
}

REGISTER_SEGMENTATION(WaferGridXY)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
