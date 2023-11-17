/*
 * CartesianGridXYStaggered.cpp
 *
 *  Created on: Sept 15, 2023
 *      Author: Sebouh J. Paul, UCR
 */

#include "DDSegmentation/CartesianGridXYStaggered.h"

namespace dd4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
CartesianGridXYStaggered::CartesianGridXYStaggered(const std::string& cellEncoding) :
		CartesianGrid(cellEncoding) {
	// define type and description
	_type = "CartesianGridXYStaggered";
	_description = "Cartesian segmentation in the local XY-plane, with options for staggering";

	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("stagger_x", "Option to stagger the layers in x (ie, add grid_size_x/2 to offset_x for odd layers)", _staggerX, 0,
		SegmentationParameter::NoUnit, true);
	registerParameter("stagger_y", "Option to stagger the layers in y (ie, add grid_size_y/2 to offset_y for odd layers)", _staggerY, 0, 
		SegmentationParameter::NoUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerParameter("stagger_keyword", "Volume ID identifier used for determining which volumes to stagger", _staggerKeyword, (std::string)"layer", 
		SegmentationParameter::NoUnit, true);
}

/// Default constructor used by derived classes passing an existing decoder
CartesianGridXYStaggered::CartesianGridXYStaggered(const BitFieldCoder* decode) :
		CartesianGrid(decode)
{
	// define type and description
	_type = "CartesianGridXYStaggered";
	_description = "Cartesian segmentation in the local XY-plane, with options for staggering";

	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("stagger_x", "Option to stagger the layers in x (ie, add grid_size_x/2 to offset_x for odd layers)", _staggerX, 0,
		SegmentationParameter::NoUnit, true);
	registerParameter("stagger_y", "Option to stagger the layers in y (ie, add grid_size_y/2 to offset_y for odd layers)", _staggerY, 0,
		SegmentationParameter::NoUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerParameter("stagger_keyword", "Volume ID identifier used for determining which volumes to stagger", _staggerKeyword, (std::string)"layer",
		SegmentationParameter::NoUnit, true);
}

/// destructor
CartesianGridXYStaggered::~CartesianGridXYStaggered() {

}

/// determine the position based on the cell ID
Vector3D CartesianGridXYStaggered::position(const CellID& cID) const {
	Vector3D cellPosition;
	if (_staggerX || _staggerY){
		int layer= _decoder->get(cID,_staggerKeyword);
		cellPosition.X = binToPosition( _decoder->get(cID,_xId ), _gridSizeX, _offsetX+_staggerX*_gridSizeX*(layer%2)/2.);
		cellPosition.Y = binToPosition( _decoder->get(cID,_yId ), _gridSizeY, _offsetY+_staggerY*_gridSizeY*(layer%2)/2.);
	} else {
		cellPosition.X = binToPosition( _decoder->get(cID,_xId ), _gridSizeX, _offsetX);
		cellPosition.Y = binToPosition( _decoder->get(cID,_yId ), _gridSizeY, _offsetY);
	}
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID CartesianGridXYStaggered::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
        CellID cID = vID ;
	if (_staggerX || _staggerY){
		int layer= _decoder->get(cID,_staggerKeyword);
		_decoder->set( cID,_xId, positionToBin(localPosition.X, _gridSizeX, _offsetX+_staggerX*_gridSizeX*(layer%2)/2) );
		_decoder->set( cID,_yId, positionToBin(localPosition.Y, _gridSizeY, _offsetY+_staggerY*_gridSizeY*(layer%2)/2) );
	} else {
		_decoder->set( cID,_xId, positionToBin(localPosition.X, _gridSizeX, _offsetX));
		_decoder->set( cID,_yId, positionToBin(localPosition.Y, _gridSizeY, _offsetY));
	}
	return cID ;
}


std::vector<double> CartesianGridXYStaggered::cellDimensions(const CellID& cellID) const {
  return {_gridSizeX, _gridSizeY};
}


} /* namespace DDSegmentation */
} /* namespace dd4hep */
