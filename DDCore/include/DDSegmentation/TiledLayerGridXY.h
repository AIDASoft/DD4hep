/*
 * TiledLayerGridXY.h
 *
 *  Created on: November 12, 2015
 *      Author: Shaojun Lu, DESY
 */

#ifndef DDSegmentation_TILEDLAYERGRIDXY_H_
#define DDSegmentation_TILEDLAYERGRIDXY_H_

#include "DDSegmentation/CartesianGrid.h"

// C/C++ includes
#include <string>
#include <vector>

namespace dd4hep {
namespace DDSegmentation {

class TiledLayerGridXY: public CartesianGrid {
public:
	/// Default constructor passing the encoding string
	TiledLayerGridXY(const std::string& cellEncoding = "");
	/// Default constructor used by derived classes passing an existing decoder
	TiledLayerGridXY(BitField64* decoder);
	/// destructor
	virtual ~TiledLayerGridXY();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
	/// access the grid size in X
	double gridSizeX() const {
		return _gridSizeX;
	}
	/// access the grid size in Y
	double gridSizeY() const {
		return _gridSizeY;
	}
	/// access the coordinate offset in X
	double offsetX() const {
		return _offsetX;
	}
	/// access the coordinate offset in Y
	double offsetY() const {
		return _offsetY;
	}
	/// access the coordinate layerOffset in X
	std::vector<double> layerOffsetX() const {
		return  _layerOffsetX;
	}
	/// access the coordinate layerOffset in Y
	std::vector<double> layerOffsetY() const {
		return  _layerOffsetY;
	}
	/// access the boundary dimension in X
	std::vector<double> boundaryLayerX() const {
	  return _layerDimX;
	}
	/// access the fraction cell size in X
	std::vector<double> FractCellSizeXPerLayer() const {
	  return _fractCellSizeXPerLayer;
	}
	/// access the field name used for X
	const std::string& fieldNameX() const {
		return _xId;
	}
	/// access the field name used for Y
	const std::string& fieldNameY() const {
		return _yId;
	}
	/// access the field name used for Layer
	const std::string& fieldNameLayer() const {
		return _identifierLayer;
	}
	/// set the grid size in X
	void setGridSizeX(double cellSize) {
		_gridSizeX = cellSize;
	}
	/// set the grid size in Y
	void setGridSizeY(double cellSize) {
		_gridSizeY = cellSize;
	}
	/// set the coordinate offset in X
	void setOffsetX(double offset) {
		_offsetX = offset;
	}
	/// set the coordinate offset in Y
	void setOffsetY(double offset) {
		_offsetY = offset;
	}
	/// set the coordinate layerOffset in X
	void setLayerOffsetX(double offset) {
	  _layerOffsetX.push_back(offset);
	}
	/// set the coordinate layerOffset in Y
	void setLayerOffsetY(double offset) {
	  _layerOffsetY.push_back(offset);
	}
	/// set the field name used for X
	void setFieldNameX(const std::string& fieldName) {
		_xId = fieldName;
	}
	/// set the field name used for Y
	void setFieldNameY(const std::string& fieldName) {
		_yId = fieldName;
	}
	/// set the field name used for Y
	void setFieldNameLayer(const std::string& fieldName) {
	        _identifierLayer= fieldName;
	}
	/// set the layer boundary dimension for X
	void setBoundaryLayerX(double halfX)
	{
	  _layerDimX.push_back(halfX);
	};
	/// set the layer fraction cell size for X
	void setFractCellSizeXPerLayer(double newFractCellSizeX)
	{
	  _fractCellSizeXPerLayer.push_back(newFractCellSizeX);
	}
	/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
	    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

	    Returns a vector of the cellDimensions of the given cell ID
	    \param cellID is ignored as all cells have the same dimension
	    \return std::vector<double> size 2:
	    -# size in x
	    -# size in y
	*/
	virtual std::vector<double> cellDimensions(const CellID& cellID) const;

protected:
	/// the grid size in X
	double _gridSizeX;
	/// the coordinate offset in X
	double _offsetX;
	/// the grid size in Y
	double _gridSizeY;
	/// the coordinate offset in Y
	double _offsetY;
	/// the field name used for X
	std::string _xId;
	/// the field name used for Y
	std::string _yId;
	/// encoding field used for the layer
	std::string _identifierLayer; 
	/// list of layer x offset
	std::vector<double> _layerOffsetX;
	/// list of layer y offset
	std::vector<double> _layerOffsetY;
	/// list of layer boundary dimension for X
	std::vector<double> _layerDimX;
	/// list of the layer fraction cell size for X
	std::vector<double> _fractCellSizeXPerLayer;
};

} /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_TILEDLAYERGRIDXY_H_ */
