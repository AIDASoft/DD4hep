/*
 * MegatileLayerGridXY.cpp
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 *              D Jeans UTokyo
 */

#include "DDSegmentation/MegatileLayerGridXY.h"

#undef NDEBUG
#include <cassert>

#include <algorithm>

namespace DD4hep {
  namespace DDSegmentation {

    /// default constructor using an encoding string
    MegatileLayerGridXY::MegatileLayerGridXY(const std::string& cellEncoding) :
      CartesianGrid(cellEncoding) {
      setup();
    }

    MegatileLayerGridXY::MegatileLayerGridXY(BitField64* decode) :
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

      for (int i=0; i<MAX_LAYERS; i++) {
        _nCellsX[i]=0;
        _nCellsY[i]=0;
      }
    }


    /// determine the position based on the cell ID
    Vector3D MegatileLayerGridXY::position(const CellID& cID) const {
      // this is local position within the megatile

      _decoder->setValue(cID);
      unsigned int layerIndex = (*_decoder)[_identifierLayer];
      unsigned int waferIndex = (*_decoder)[_identifierWafer];
      int cellIndexX = (*_decoder)[_xId];
      int cellIndexY = (*_decoder)[_yId];

      // segmentation info for this megatile ("wafer")
      getSegInfo(layerIndex, waferIndex);

      Vector3D cellPosition(0,0,0);
      cellPosition.X = cellIndexX * (_currentSegInfo.megaTileSizeX / _currentSegInfo.nCellsX ) + _currentSegInfo.megaTileOffsetX;
      cellPosition.Y = cellIndexY * (_currentSegInfo.megaTileSizeY / _currentSegInfo.nCellsY ) + _currentSegInfo.megaTileOffsetY;

      if ( abs( cellPosition.X )>10000 || abs( cellPosition.Y )>10000 ) {
        std::cout << "crazy cell position: " << cellPosition.X << " " << cellPosition.Y << std::endl;
        std::cout << "layer, wafer, cellx,y indices: " << layerIndex << " " << waferIndex << " " << cellIndexX << " " << cellIndexY << std::endl;
        assert(0 && "crazy cell position?");
      }

      return cellPosition;
    }


    /// determine the cell ID based on the position
    CellID MegatileLayerGridXY::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
      // this is the local position within a megatile, local coordinates

      // get the layer, wafer, module indices from the volumeID
      _decoder->setValue(vID);
      unsigned int layerIndex = (*_decoder)[_identifierLayer];
      unsigned int waferIndex = (*_decoder)[_identifierWafer];

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

      (*_decoder)[_xId] = _cellIndexX;
      (*_decoder)[_yId] = _cellIndexY;

      return _decoder->getValue();
    }


    std::vector<double> MegatileLayerGridXY::cellDimensions(const CellID& cID) const {
      _decoder->setValue( cID );
      unsigned int layerIndex = (*_decoder)[_identifierLayer];
      unsigned int waferIndex = (*_decoder)[_identifierWafer];
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

      assert ( layerIndex < MAX_LAYERS && "layer index too high" );

      std::pair < unsigned int, unsigned int > tileid(layerIndex, waferIndex);
      if ( specialMegaTiles_layerWafer.find( tileid ) == specialMegaTiles_layerWafer.end() ) { // standard megatile
        _currentSegInfo.megaTileSizeX   = _megaTileSizeX;
        _currentSegInfo.megaTileSizeY   = _megaTileSizeY;
        _currentSegInfo.megaTileOffsetX = _megaTileOffsetX;
        _currentSegInfo.megaTileOffsetY = _megaTileOffsetY;
        _currentSegInfo.nCellsX         = _nCellsX[layerIndex];
        _currentSegInfo.nCellsY         = _nCellsY[layerIndex];
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

    REGISTER_SEGMENTATION(MegatileLayerGridXY)

  } /* namespace DDSegmentation */
} /* namespace DD4hep */
