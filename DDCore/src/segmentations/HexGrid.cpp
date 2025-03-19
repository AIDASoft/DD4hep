/*
 * AIDA Detector description implementation 
 * 
 * Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
 * All rights reserved.
 *
 * For the licensing terms see $DD4hepINSTALL/LICENSE.
 * For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
 *
 * HexGrid.cpp  
 *
 * Created on: August 9, 2023
 *      Author: Sebouh J. Paul, UC Riverside
 */
#include <DD4hep/Factories.h>
#include <DDSegmentation/HexGrid.h>

namespace dd4hep {
  namespace DDSegmentation {

    /// Default constructor used by derived classes passing the encoding string
    HexGrid::HexGrid(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
        _type = "HexGridXY";
	_description = "Hexagonal segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("stagger", "stagger mode", _stagger, 1);
	registerParameter("side_length", "Cell size", _sideLength, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerParameter("stagger_keyword", "Volume ID identifier used for determining which volumes to stagger", _staggerKeyword, (std::string)"layer", SegmentationParameter::NoUnit, true);
    }

    /// Default constructor used by derived classes passing an existing decoder
    HexGrid::HexGrid(const BitFieldCoder* decode) : Segmentation(decode) {
        // define type and description
	_type = "HexGridXY";
	_description = "Hexagonal segmentation in the local XY-plane";

	// register all necessary parameters                                                                        
        registerParameter("stagger", "stagger mode", _stagger, 1);
	registerParameter("side_length", "Cell size", _sideLength, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerParameter("stagger_keyword", "Volume ID identifier used for determining which volumes to stagger", _staggerKeyword, (std::string)"layer", SegmentationParameter::NoUnit, true);
    
    }

    /// Destructor
    HexGrid::~HexGrid() {
    }

    /// determine the position based on the cell ID
    Vector3D HexGrid::position(const CellID& cID) const {
        int layer=0;
	if (_stagger) layer= _decoder->get(cID,_staggerKeyword);
		
	Vector3D cellPosition;
	cellPosition.X = _decoder->get(cID,_xId )*1.5*_sideLength+_offsetX+_sideLength/2.;
	cellPosition.Y = _decoder->get(cID,_yId )*std::sqrt(3)/2.*_sideLength+ _offsetY+_sideLength*std::sqrt(3)/2.;
	if (_stagger==0)
	  cellPosition.X+=_sideLength;
	else if (_stagger==1)
	  cellPosition.X+=(layer%3)*_sideLength;
	else if (_stagger==2){
	  switch (layer%4){
	  case 0:
	    cellPosition.X-=0.75*_sideLength;
	    break;
	  case 1:
	    cellPosition.Y+=std::sqrt(3)/4*_sideLength;
	    break;
	  case 2:
	    cellPosition.Y-=std::sqrt(3)/4*_sideLength;
	    break;
	  case 3:
	    cellPosition.X+=0.75*_sideLength;
	    break;
	  }
	}
	return cellPosition;
    }

    inline double positive_modulo(double i, double n) {
      return std::fmod(std::fmod(i,n) + n,n);
    }

    
    /// determine the cell ID based on the position
    CellID HexGrid::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
        CellID cID = vID ;
	int layer=0;
	if (_stagger) layer= _decoder->get(cID,_staggerKeyword);

	double x=localPosition.X-_offsetX;
	double y=localPosition.Y-_offsetY;
	if (_stagger==0)
	  x-=_sideLength;
	else if (_stagger==1)
          x-=(layer%3)*_sideLength;
	else if (_stagger==2){
          switch (layer%4){
	  case 0:
	    x+=0.75*_sideLength;
	    break;
          case 1:
            y-=std::sqrt(3)/4*_sideLength;
            break;
          case 2:
            y+=std::sqrt(3)/4*_sideLength;
            break;
          case 3:
            x-=0.75*_sideLength;
	    break;
          }
        }
	
	double a=positive_modulo(y/(std::sqrt(3)*_sideLength),1);
	double b=positive_modulo(x/(3*_sideLength),1);
	int ix = std::floor(x/(3*_sideLength/2.))+		
	  (b<0.5)*(-std::abs(a-.5)<(b-.5)*3)+(b>0.5)*(std::abs(a-.5)-.5<(b-1)*3);
	int iy=std::floor(y/(std::sqrt(3)*_sideLength/2.));
	iy-=(ix+iy)&1;
	
	_decoder->set( cID,_xId, ix );
	_decoder->set( cID,_yId, iy );
	return cID ;
    }

    std::vector<double> HexGrid::cellDimensions(const CellID&) const {
    return {2*_sideLength, std::sqrt(3)*_sideLength};
}

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
