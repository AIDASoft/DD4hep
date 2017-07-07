/*
 * MegatileLayerGridXY.h
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 */

#ifndef DDSegmentation_MEGATILELAYERGRIDXY_H_
#define DDSegmentation_MEGATILELAYERGRIDXY_H_

#include "DDSegmentation/CartesianGrid.h"

#include <cassert>

#define MAX_LAYERS 100

/*

  a megatile is a rectangule in x-y, split into a grid along x and y, with an exactly integer number of cells in x and y.

  this class assumes a mostly-common megatile size, with possibility for a number of "special" megatiles of non-standard size / segmentation

  the segmentation of standard megatiles is defined layer-by-layer.

  some changes wrt previous version from Kotera et al.
  - significantly simplified. 
  - complications due to end-of-slab moved to higher level detector drivers.

  D. Jeans - Nov 2016

*/


namespace dd4hep {
  namespace DDSegmentation {

    class MegatileLayerGridXY: public CartesianGrid {
    public:
      /// Default constructor passing the encoding string
      MegatileLayerGridXY(const std::string& cellEncoding = "");

      /// Default constructor used by derived classes passing an existing decoder
      MegatileLayerGridXY(BitField64* decoder);

      /// destructor
      virtual ~MegatileLayerGridXY();

      void setup();

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;

      // set size of megatile in X,Y
      void setMegaTileSizeXY(double x, double y) {
        _megaTileSizeX = x;
        _megaTileSizeY = y;
      }

      /// set the coordinate offset in X, Y
      void setMegaTileOffsetXY( double x, double y) {
        _megaTileOffsetX = x;
        _megaTileOffsetY = y;
      }

      void setMegaTileCellsXY( unsigned int layer, int ix, int iy ) {
        assert ( layer < MAX_LAYERS );
        _nCellsX[layer] = ix;
        _nCellsY[layer] = iy;
      }

      void setSpecialMegaTile( unsigned int layer, unsigned int tile, 
                               double sizex, double sizey,
                               double offsetx, double offsety,
                               unsigned int ncellsx, unsigned int ncellsy );

      /// access the field name used for X
      const std::string& fieldNameX() const {
        return _xId;
      }
      /// access the field name used for Y
      const std::string& fieldNameY() const {
        return _yId;
      }
      
      /// set the field name used for X
      void setFieldNameX(const std::string& fieldName) {
        _xId = fieldName;
      }
      /// set the field name used for Y
      void setFieldNameY(const std::string& fieldName) {
        _yId = fieldName;
      }

      virtual std::vector<double> cellDimensions(const CellID& cellID) const;
      virtual std::vector<double> cellDimensions(const unsigned int ilayer, const unsigned int iwafer) const;

      struct segInfo {
        double megaTileSizeX = 0;
        double megaTileSizeY = 0;
        double megaTileOffsetX = 0;
        double megaTileOffsetY = 0;
        unsigned int nCellsX = 0;
        unsigned int nCellsY = 0;
        segInfo() = default;
      };

      
    protected:

      mutable segInfo _currentSegInfo;

      void getSegInfo( unsigned int layerIndex, unsigned int waferIndex) const;

      // the "usual" megatiles
      //  megatile size and offset is constant in all layers
      //  the segmentation may change layer-to-layer (e.g. orthogonal strips)

      // total size of surface in X,Y
      double  _megaTileSizeX = 0; // [MAX_LAYERS][MAX_WAFERS];
      double  _megaTileSizeY = 0; //[MAX_LAYERS][MAX_WAFERS];

      double  _megaTileOffsetX = 0;
      double  _megaTileOffsetY = 0;

      // number of cells per megatile in X, Y
      unsigned int _nCellsX[MAX_LAYERS];
      unsigned int _nCellsY[MAX_LAYERS];

      std::map < std::pair < unsigned int, unsigned int > , segInfo > specialMegaTiles_layerWafer;

      
      /// the field name used for X
      std::string _xId;
      /// the field name used for Y
      std::string _yId;
      /// encoding field used for the layer
      std::string _identifierLayer;
      /// encoding field used for the wafer
      std::string _identifierWafer;

      std::string _layerConfig;

      std::string _identifierModule;

    };

  }    /* namespace DDSegmentation              */
}      /* namespace dd4hep                      */
#endif /* DDSegmentation_MEGATILELAYERGRIDXY_H_ */
