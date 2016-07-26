/*
 * MegatileLayerGridXY.h
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 */

#ifndef DDSegmentation_WAFERGRIDXY_H_
#define DDSegmentation_WAFERGRIDXY_H_

#include "DDSegmentation/CartesianGrid.h"

#define MAX_LAYERS 100
#define MAX_WAFERS 100

namespace DD4hep {
namespace DDSegmentation {

class MegatileLayerGridXY: public CartesianGrid {
public:
	/// Default constructor passing the encoding string
	MegatileLayerGridXY(const std::string& cellEncoding = "");
	/// destructor
	virtual ~MegatileLayerGridXY();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;

	/// access the field name used for X
	const std::string& fieldNameX() const {
		return _xId;
	}
	/// access the field name used for Y
	const std::string& fieldNameY() const {
		return _yId;
	}

	// total size of surface in X,Y
//	double totalSizeX(int inLayer)  {
//		return _totalSizeX[ inLayer ];
//	}
//	double totalSizeY() const  {
//		return _totalSizeY;
//	}

	// number of megatiles along X
//	int nMegaX(int inLayer=0) const {
//		return _nMegaY[inLayer];
//	}

	// the dead region at edge of megatiles (e.g. guard ring width)
//	double deadWidth() {
//		return _deadWidth;
//	}
	
		int NStripsY() {
			return _nStripsY;
		}

		int NStripsX() {
			return _nStripsX;
		}

		int NMegaY() {
			return _nMegaY;
		}
	/// set the coordinate offset in X
//	void setSlabOffsetX( int layer, double offset) {
//	  _slabOffsetX[layer] = offset;
//	}
	/// set the coordinate offset in Y
//	void setSlabOffsetY( int layer, double offset) {
//	  _slabOffsetY[layer] = offset;
//	}

	void setIsRegulatingEBU( int layer, int wafer, bool isRegulating ) {
		_isRegulatingEBU[layer][wafer] = isRegulating;
	}

	/// set the coordinate offset in X
	void setWaferOffsetX( int layer, int wafer, double offset) {
	  _waferOffsetX[layer][wafer] = offset;
	}
	/// set the coordinate offset in Y
	void setWaferOffsetY( int layer, int wafer, double offset) {
	  _waferOffsetY[layer][wafer] = offset;
	}

	// total size of surface in X,Y
  void setTotalSizeX(int inLayer, double x) {
    _totalSizeX[inLayer] = x;
	  _calculated=false;
	}
        
	void setTotalSizeY( double y) {
    _totalSizeY = y;
	  _calculated=false;
	}

	// number of megatiles along X
//	void setNMegaY( int n) {
//		_nMegaY = n;
//		_calculated=false;
//	}

	// the dead region at edge of megatiles (e.g. guard ring width)
//	void setDeadWidth(double x) {
//		_deadWidth = x;
//	  _calculated=false;
//	}


	/// set the field name used for X
	void setFieldNameX(const std::string& fieldName) {
		_xId = fieldName;
	}
	/// set the field name used for Y
	void setFieldNameY(const std::string& fieldName) {
		_yId = fieldName;
	}

//	void setWaferIndexX(double waferIndexX) {
//			_waferIndexX = waferIndexX;
//			_calculated=false;
//	}

//	void setWaferIndexY(double waferIndexY) {
//			_waferIndexY = waferIndexY;
//			_calculated=false;
//	}

	/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
	    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

	    Returns a vector of the cellDimensions of the given cell ID
	    \param cellID is ignored as all cells have the same dimension
	    \return std::vector<double> size 2:
	    -# size in x
	    -# size in y
	*/
	virtual std::vector<double> cellDimensions(const CellID& cellID) const;
	virtual std::vector<double> layercellDimensions(const int ilayer) const;

protected:

	// total size of surface in X,Y
	double  _totalSizeX[MAX_LAYERS];
	double  _totalSizeY;
  // number of megatiles along X : together with _totalSizeX, this defines the size of the square megatile
	int  _nMegaY;
  // number of cells per megatile in X, Y
	int  _nStripsX;
	int  _nStripsY;
	int  _nCells;

	// the dead region at edge of megatiles (e.g. guard ring width): assumed constant in each layer
	double _deadWidth;

  // size of megatile (including edge region)
	mutable double  _megaSize;
	// the grid size in X
	mutable double  _gridSizeT;
	// the grid size in Y
	mutable double  _gridSizeL;
	mutable double  _gridSizeS;

	// calculate the derived quantities
	void calculateDerivedQuantities() const;
	mutable bool   _calculated;

	/// the coordinate offset in X
	double _offsetX;
	/// the coordinate offset in Y
	double _offsetY;
	double _slabOffsetX;
	double _slabOffsetY;
	double _waferOffsetX[MAX_LAYERS][MAX_WAFERS];
	double _waferOffsetY[MAX_LAYERS][MAX_WAFERS];

	unsigned int _waferIndexX;
	unsigned int _waferIndexY;

	bool _isRegulatingEBU[MAX_LAYERS][MAX_WAFERS];

	/// the field name used for X
	std::string _xId;
	/// the field name used for Y
	std::string _yId;
	/// encoding field used for the layer
	std::string _identifierLayer; 
	/// encoding field used for the wafer
	std::string _identifierWafer; 
//  std::string _identifierWaferY; 

	//	std::string _identifierGR;

	std::string _layerConfig;

};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_WAFERGRIDXY_H_ */
