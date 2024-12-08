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
 * CartesianGridUV.h
 *
 *  Created on: December 5, 2024
 *      Author: Yann Bedfer, ePIC/Saclay
 */

#ifndef DDSEGMENTATION_CARTESIANGRIDUV_H
#define DDSEGMENTATION_CARTESIANGRIDUV_H

#include <DDSegmentation/CartesianGrid.h>

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian grid segmentation in along U,V rotated some angle from local X,Y axes
    class CartesianGridUV: public CartesianGrid {
    public:
      /// Default constructor passing the encoding string
      CartesianGridUV(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      CartesianGridUV(const BitFieldCoder* decoder);
      /// destructor
      virtual ~CartesianGridUV();

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
      /// access the grid size in U
      double gridSizeU() const {
        return _gridSizeU;
      }
      /// access the grid size in V
      double gridSizeV() const {
        return _gridSizeV;
      }
      /// access the coordinate offset in U
      double offsetU() const {
        return _offsetU;
      }
      /// access the coordinate offset in V
      double offsetV() const {
        return _offsetV;
      }
      /// access the rotation angle
      double gridAngle() const {
        return _gridAngle;
      }
      /// access the field name used for U
      const std::string& fieldNameU() const {
        return _uId;
      }
      /// access the field name used for V
      const std::string& fieldNameV() const {
        return _vId;
      }
      /// set the grid size in U
      void setGridSizeU(double cellSize) {
        _gridSizeU = cellSize;
      }
      /// set the grid size in V
      void setGridSizeV(double cellSize) {
        _gridSizeV = cellSize;
      }
      /// set the coordinate offset in U
      void setOffsetU(double offset) {
        _offsetU = offset;
      }
      /// set the coordinate offset in V
      void setOffsetV(double offset) {
        _offsetV = offset;
      }
      /// set the rotation angle
      void setGridAngle(double angle) {
        _gridAngle = angle;
      }
      /// set the field name used for U
      void setFieldNameU(const std::string& fieldName) {
        _uId = fieldName;
      }
      /// set the field name used for V
      void setFieldNameV(const std::string& fieldName) {
        _vId = fieldName;
      }
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

          Returns a vector of the cellDimensions of the given cell ID
          \param cellID is ignored as all cells have the same dimension
          \return std::vector<double> size 2:
          -# size in U
          -# size in V
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

    protected:
      /// the grid size in U
      double _gridSizeU;
      /// the coordinate offset in U
      double _offsetU;
      /// the grid size in V
      double _gridSizeV;
      /// the coordinate offset in V
      double _offsetV;
      /// the field name used for U
      std::string _uId;
      /// the field name used for V
      std::string _vId;
      /// the U grid angle
      double _gridAngle;
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CARTESIANGRIDUV_H
