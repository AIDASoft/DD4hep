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
 * CartesianGrid.h
 *
 *  Created on: August 9, 2023
 *      Author: Sebouh J. Paul, UC Riverside
 */

#ifndef DDSEGMENTATION_HEXGRID_H
#define DDSEGMENTATION_HEXGRID_H

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian grid segmentation
    class HexGrid: public Segmentation {
    public:
      /// Destructor
      virtual ~HexGrid();
      //protected:
      /// Default constructor used by derived classes passing the encoding string
      HexGrid(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      HexGrid(const BitFieldCoder* decoder);

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
      // access the stagger mode: 0=no stagger; 1=stagger cycling through 3 offsets
      int stagger() const {
	return _stagger;
      }
      
      /// access the grid size 
      double sideLength() const {
        return _sideLength;
      }
      /// access the coordinate offset in X
      double offsetX() const {
        return _offsetX;
      }
      /// access the coordinate offset in Y
      double offsetY() const {
        return _offsetY;
      }
      /// access the field name used for X
      const std::string& fieldNameX() const {
        return _xId;
      }
      /// access the field name used for Y
      const std::string& fieldNameY() const {
        return _yId;
      }
      
      /// set the stagger mode: 0=no stagger; 1=stagger cycling through 3 offsets
      void setStagger(int stagger) {
	_stagger= stagger;
      }
      /// set the grid size in X
      void setSideLength(double cellSize) {
        _sideLength = cellSize;
      }
      /// set the coordinate offset in X
      void setOffsetX(double offset) {
        _offsetX = offset;
      }
      /// set the coordinate offset in Y
      void setOffsetY(double offset) {
        _offsetY = offset;
      }
      /// set the field name used for X
      void setFieldNameX(const std::string& fieldName) {
        _xId = fieldName;
      }
      /// set the field name used for Y
      void setFieldNameY(const std::string& fieldName) {
        _yId = fieldName;
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
      /// the stagger mode:  0=off ; 1=cycle through 3 different offsets (H3)
      //  2=cycle through 4 differnt offsets (H4)
      int _stagger;
      /// the length of one side of a hexagon
      double _sideLength;
      /// the coordinate offset in X
      double _offsetX;
      /// the coordinate offset in Y
      double _offsetY;
      /// the field name used for X
      std::string _xId;
      /// the field name used for Y
      std::string _yId;
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_HEXGRID_H
