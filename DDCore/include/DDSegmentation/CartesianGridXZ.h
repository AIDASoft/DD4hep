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

#ifndef DDSEGMENTATION_CARTESIANGRIDXZ_H
#define DDSEGMENTATION_CARTESIANGRIDXZ_H

#include "DDSegmentation/CartesianGrid.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian grid segmentation in the X-Z plane
    class CartesianGridXZ: public CartesianGrid {
    public:
      /// default constructor using an arbitrary type
      CartesianGridXZ(const std::string& cellEncoding);
      /// Default constructor used by derived classes passing an existing decoder
      CartesianGridXZ(const BitFieldCoder* decoder);
      /// destructor
      virtual ~CartesianGridXZ();

      /// determine the local based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
      /// access the grid size in X
      double gridSizeX() const {
        return _gridSizeX;
      }
      /// access the grid size in Z
      double gridSizeZ() const {
        return _gridSizeZ;
      }
      /// access the coordinate offset in X
      double offsetX() const {
        return _offsetX;
      }
      /// access the coordinate offset in Z
      double offsetZ() const {
        return _offsetZ;
      }
      /// access the field name used for X
      const std::string& fieldNameX() const {
        return _xId;
      }
      /// access the field name used for Z
      const std::string& fieldNameZ() const {
        return _zId;
      }
      /// set the grid size in X
      void setGridSizeX(double cellSize) {
        _gridSizeX = cellSize;
      }
      /// set the grid size in Z
      void setGridSizeZ(double cellSize) {
        _gridSizeZ = cellSize;
      }
      /// set the coordinate offset in X
      void setOffsetX(double offset) {
        _offsetX = offset;
      }
      /// set the coordinate offset in Z
      void setOffsetZ(double offset) {
        _offsetZ = offset;
      }
      /// set the field name used for X
      void setFieldNameX(const std::string& fieldName) {
        _xId = fieldName;
      }
      /// set the field name used for Y
      void setFieldNameZ(const std::string& fieldName) {
        _zId = fieldName;
      }
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

          Returns a vector of the cellDimensions of the given cell ID
          \param cellID is ignored as all cells have the same dimension
          \return std::vector<double> size 2:
          -# size in x
          -# size in z
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

    protected:
      /// the grid size in X
      double _gridSizeX;
      /// the coordinate offset in X
      double _offsetX;
      /// the grid size in Z
      double _gridSizeZ;
      /// the coordinate offset in Z
      double _offsetZ;
      /// the field name used for X
      std::string _xId;
      /// the field name used for Z
      std::string _zId;
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CARTESIANGRIDXZ_H
