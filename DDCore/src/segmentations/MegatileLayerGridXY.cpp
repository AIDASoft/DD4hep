/*
 * MegatileLayerGridXY.cpp
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 *              D Jeans UTokyo
 */

#include <DDSegmentation/MegatileLayerGridXY.h>
#include <DD4hep/Printout.h>

#undef NDEBUG
#include <cmath>
#include <cassert>
#include <algorithm>

namespace dd4hep {
  namespace DDSegmentation {

    /// default constructor using an encoding string
    MegatileLayerGridXY::MegatileLayerGridXY(const std::string& cellEncoding) :
      CartesianGrid(cellEncoding)
    {
      setup();
    }

    MegatileLayerGridXY::MegatileLayerGridXY(const BitFieldCoder* decode) :
      CartesianGrid(decode) {
      setup();
    }

    /// destructor
    MegatileLayerGridXY::~MegatileLayerGridXY() {

    }

    void MegatileLayerGridXY::setup() {
      // define type and description
      _type = "MegatileLayerGridXY";
      _description = "Cartesian segmentation in the local XY-plane: megatiles, containing integer number of tiles/strips/cells";

      registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "cellX");
      registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "cellY");

      registerParameter("identifier_wafer", "Cell encoding identifier for wafer", _identifierWafer, std::string("wafer"),
                        SegmentationParameter::NoUnit, true);

      registerParameter("identifier_layer", "Cell encoding identifier for layer", _identifierLayer, std::string("layer"),
                        SegmentationParameter::NoUnit, true);

      registerParameter("identifier_module", "Cell encoding identifier for module", _identifierModule, std::string("module"),
                        SegmentationParameter::NoUnit, true);

      registerParameter("common_nCellsX", "ncells in x (uniform)",  _unif_nCellsX, int(0), SegmentationParameter::NoUnit, true);
      registerParameter("common_nCellsY", "ncells in y (uniform)",  _unif_nCellsY, int(0), SegmentationParameter::NoUnit, true);

      _nCellsX.clear();
      _nCellsY.clear();
    }


    /// determine the position based on the cell ID
    Vector3D MegatileLayerGridXY::position(const CellID& cID) const {
      // this is local position within the megatile

      unsigned int layerIndex = _decoder->get(cID,_identifierLayer);
      unsigned int waferIndex = _decoder->get(cID,_identifierWafer);
      int cellIndexX = _decoder->get(cID,_xId);
      int cellIndexY = _decoder->get(cID,_yId);

      // segmentation info for this megatile ("wafer")
      getSegInfo(layerIndex, waferIndex);

      Vector3D cellPosition(0,0,0);
      cellPosition.X = ( cellIndexX + 0.5 ) * (_currentSegInfo.megaTileSizeX / _currentSegInfo.nCellsX ) + _currentSegInfo.megaTileOffsetX;
      cellPosition.Y = ( cellIndexY + 0.5 ) * (_currentSegInfo.megaTileSizeY / _currentSegInfo.nCellsY ) + _currentSegInfo.megaTileOffsetY;

      if ( fabs( cellPosition.X )>10000e0 || fabs( cellPosition.Y )>10000e0 ) {
        printout(WARNING,"MegatileLayerGridXY", "crazy cell position: x: %f y: %f ", cellPosition.X, cellPosition.Y);
        printout(WARNING,"MegatileLayerGridXY", "layer, wafer, cellx,y indices: %d %d %d %d",
		 layerIndex, waferIndex, cellIndexX, cellIndexY);
        assert(0 && "crazy cell position?");
      }

      return cellPosition;
    }


    /// determine the cell ID based on the position
    CellID MegatileLayerGridXY::cellID(const Vector3D& localPosition,
                                       const Vector3D& /* globalPosition */,
                                       const VolumeID& vID) const
    {
      // this is the local position within a megatile, local coordinates

      // get the layer, wafer, module indices from the volumeID
      unsigned int layerIndex = _decoder->get(vID,_identifierLayer);
      unsigned int waferIndex = _decoder->get(vID,_identifierWafer);

      // segmentation info for this megatile ("wafer")
      getSegInfo(layerIndex, waferIndex);

      double localX = localPosition.X;
      double localY = localPosition.Y;

      // correct for offset : move origin to corner of megatile
      localX -= _currentSegInfo.megaTileOffsetX;
      localY -= _currentSegInfo.megaTileOffsetY;

      // the cell index (counting from the corner)
      int _cellIndexX = int ( localX / ( _currentSegInfo.megaTileSizeX / _currentSegInfo.nCellsX ) );
      int _cellIndexY = int ( localY / ( _currentSegInfo.megaTileSizeY / _currentSegInfo.nCellsY ) );

      CellID cID = vID ;
      _decoder->set(cID,_xId,_cellIndexX);
      _decoder->set(cID,_yId,_cellIndexY);

      return cID;
    }


    std::vector<double> MegatileLayerGridXY::cellDimensions(const CellID& cID) const {
      unsigned int layerIndex = _decoder->get(cID,_identifierLayer);
      unsigned int waferIndex = _decoder->get(cID,_identifierWafer);
      return cellDimensions(layerIndex, waferIndex);
    }

    void MegatileLayerGridXY::setSpecialMegaTile( unsigned int layer, unsigned int tile,
                                                  double sizex, double sizey,
                                                  double offsetx, double offsety,
                                                  unsigned int ncellsx, unsigned int ncellsy ) {

      std::pair <int, int> tileid(layer, tile);
      segInfo sinf;
      sinf.megaTileSizeX = sizex;
      sinf.megaTileSizeY = sizey;
      sinf.megaTileOffsetX = offsetx;
      sinf.megaTileOffsetY = offsety;
      sinf.nCellsX = ncellsx;
      sinf.nCellsY = ncellsy;
      specialMegaTiles_layerWafer[tileid] = sinf;
    }


    void MegatileLayerGridXY::getSegInfo( unsigned int layerIndex, unsigned int waferIndex) const {

      std::pair < unsigned int, unsigned int > tileid(layerIndex, waferIndex);
      if ( specialMegaTiles_layerWafer.find( tileid ) == specialMegaTiles_layerWafer.end() ) { // standard megatile
        _currentSegInfo.megaTileSizeX   = _megaTileSizeX;
        _currentSegInfo.megaTileSizeY   = _megaTileSizeY;
        _currentSegInfo.megaTileOffsetX = _megaTileOffsetX;
        _currentSegInfo.megaTileOffsetY = _megaTileOffsetY;

	if ( _unif_nCellsX>0 && _unif_nCellsY>0 ) {
	  _currentSegInfo.nCellsX         = _unif_nCellsX;
	  _currentSegInfo.nCellsY         = _unif_nCellsY;
	} else {
	  assert ( layerIndex<_nCellsX.size() && "MegatileLayerGridXY ERROR: too high layer index?" );
	  _currentSegInfo.nCellsX         = _nCellsX[layerIndex];
	  _currentSegInfo.nCellsY         = _nCellsY[layerIndex];
	}

      } else { // special megatile
        _currentSegInfo = specialMegaTiles_layerWafer.find( tileid )->second;
      }
    }

    std::vector<double> MegatileLayerGridXY::cellDimensions(const unsigned int layerIndex, const unsigned int waferIndex) const {
      // calculate the cell size for a given wafer in a given layer

      getSegInfo(layerIndex, waferIndex);

      double xsize = _currentSegInfo.megaTileSizeX/_currentSegInfo.nCellsX;
      double ysize = _currentSegInfo.megaTileSizeY/_currentSegInfo.nCellsY;

#if __cplusplus >= 201103L
      return {xsize, ysize};
#else
      std::vector<double> cellDims(2,0.0);
      cellDims[0] = xsize;
      cellDims[1] = ysize;
      return cellDims;
#endif
    }


  } /* namespace DDSegmentation */
} /* namespace dd4hep */

// This is done DDCore/src/plugins/ReadoutSegmentations.cpp so the plugin is not part of libDDCore
// needs also #include "DD4hep/Factories.h"
// DECLARE_SEGMENTATION(MegatileLayerGridXY,create_segmentation<dd4hep::DDSegmentation::MegatileLayerGridXY>)
