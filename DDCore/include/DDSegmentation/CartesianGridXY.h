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
 * CartesianGridXY.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSEGMENTATION_CARTESIANGRIDXY_H
#define DDSEGMENTATION_CARTESIANGRIDXY_H

#include "DDSegmentation/CartesianGrid.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian grid segmentation in the X-Y plane
    class CartesianGridXY: public CartesianGrid {
    public:
      /// Default constructor passing the encoding string
      CartesianGridXY(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      CartesianGridXY(const BitFieldCoder* decoder);
      /// destructor
      virtual ~CartesianGridXY();

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
      /// access the field name used for X
      const std::string& fieldNameX() const {
        return _xId;
      }
      /// access the field name used for Y
      const std::string& fieldNameY() const {
        return _yId;
      }
      /// access the staggering option in X
      int staggerX() const {
        return _staggerX;
      }
      /// access the staggering option in Y
      int staggerY() const {
        return _staggerY;
      }
      /// access the keyword for staggering
      const std::string& staggerKeyword() const {
        return _staggerKeyword;
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
      /// set the field name used for X
      void setFieldNameX(const std::string& fieldName) {
        _xId = fieldName;
      }
      /// set the field name used for Y
      void setFieldNameY(const std::string& fieldName) {
        _yId = fieldName;
      }
      /// set the staggering option in X
      void setStaggerX(int staggerX) {
        _staggerX = staggerX;
      }
      /// set the staggering option in Y
      void setStaggerY(int staggerY) {
        _staggerY = staggerY;
      }
      /// set the staggering option in Y
      void setStaggerKeyword(const std::string& staggerKeyword) {
        _staggerKeyword = staggerKeyword;
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
      /// staggering option in X.  0 = no staggering.  1 = stagger by _gridSizeX/2.0 in odd layers
      int _staggerX;
      /// the grid size in Y
      double _gridSizeY;
      /// the coordinate offset in Y
      double _offsetY;
      /// staggering option in Y.  0 = no staggering.  1 = stagger by _gridSizeY/2.0 in odd layers
      int _staggerY;
      /// the field name used for X
      std::string _xId;
      /// the field name used for Y
      std::string _yId;
      /// the keyword used for staggering
      std::string _staggerKeyword;
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CARTESIANGRIDXY_H
