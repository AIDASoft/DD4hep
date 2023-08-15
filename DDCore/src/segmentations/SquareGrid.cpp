/*
 *  SquareGrid.cpp
 *  A square grid in X,Y which has options for staggering in the S2 pattern
 *  Created on: August 9, 2023
 *      Author: Sebouh J. Paul, UC Riverside
 */
#include "DD4hep/Factories.h"
#include "SquareGrid.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Default constructor used by derived classes passing the encoding string
    SquareGrid::SquareGrid(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
      _type = "SquareGridXY";
	_description = "Square segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("stagger", "stagger mode", _stagger, 1);
	registerParameter("side_length", "Cell size", _sideLength, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
    }

    /// Default constructor used by derived classes passing an existing decoder
    SquareGrid::SquareGrid(const BitFieldCoder* decode) : Segmentation(decode) {
      // define type and description
	_type = "SquareGridXY";
	_description = "Square segmentation in the local XY-plane";

	// register all necessary parameters                                                                        
        registerParameter("stagger", "stagger mode", _stagger, 1);
	registerParameter("side_length", "Cell size", _sideLength, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
    
    }

    /// Destructor
    SquareGrid::~SquareGrid() {
    }

    /// determine the position based on the cell ID
    Vector3D SquareGrid::position(const CellID& cID) const {
        int layer= _decoder->get(cID,"layer");
	Vector3D cellPosition;
	cellPosition.X = _decoder->get(cID,_xId )*_sideLength+_offsetX+_sideLength/2.;
	cellPosition.Y = _decoder->get(cID,_yId )*_sideLength+_offsetY+_sideLength/2.;


	if (_stagger==1){
	  cellPosition.X+=(layer%2)*_sideLength/2;
	  cellPosition.Y+=(layer%2)*_sideLength/2;
	}
	
	return cellPosition;
    }

    inline double positive_modulo(double i, double n) {
      return std::fmod(std::fmod(i,n) + n,n);
    }

    //inline double positive_floor(
    
    /// determine the cell ID based on the position
    CellID SquareGrid::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
        CellID cID = vID ;
	int layer= _decoder->get(cID,"layer");

	double x=localPosition.X-_offsetX;
	double y=localPosition.Y-_offsetY;
	
	if (_stagger==1){
          x-=(layer%2)*_sideLength/2;
	  y-=(layer%2)*_sideLength/2;
	}

	int ix = std::floor(x/_sideLength);
	int iy = std::floor(y/_sideLength);
			
	_decoder->set( cID,_xId, ix );
	_decoder->set( cID,_yId, iy );
	return cID ;
    }

    std::vector<double> SquareGrid::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
      return {_sideLength, _sideLength};
#else
      std::vector<double> cellDims(2,0.0);
      cellDims[0] = _sideLength;
      cellDims[1] = _sideLength;
      return cellDims;
#endif
}

  } /* namespace DDSegmentation */
} /* namespace dd4hep */

DECLARE_SEGMENTATION(SquareGrid, create_segmentation<dd4hep::DDSegmentation::SquareGrid>)
