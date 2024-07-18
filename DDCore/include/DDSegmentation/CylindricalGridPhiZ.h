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
 * CylindricalGridPhiZ.h
 *
 *  Created on: Jun 28, 2024
 *      Author: Yann Bedfer, ePIC/Saclay
 */

#ifndef DDSEGMENTATION_CYLINDRICALGRIDPHIZ_H
#define DDSEGMENTATION_CYLINDRICALGRIDPHIZ_H

#include <DDSegmentation/CylindricalSegmentation.h>

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cylindrical grid segmentation in the Phi-Z cylinder
    class CylindricalGridPhiZ: public CylindricalSegmentation {
    public:
      /// default constructor using an arbitrary type
      CylindricalGridPhiZ(const std::string& cellEncoding);
      /// Default constructor used by derived classes passing an existing decoder
      CylindricalGridPhiZ(const BitFieldCoder* decoder);
      /// destructor
      virtual ~CylindricalGridPhiZ();

      /// determine the local based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
      /// access the grid size in phi
      double gridSizePhi() const {
        return _gridSizePhi;
      }
      /// access the grid size in Z
      double gridSizeZ() const {
        return _gridSizeZ;
      }
      /// access the coordinate offset in phi
      double offsetPhi() const {
        return _offsetPhi;
      }
      /// access the coordinate offset in Z
      double offsetZ() const {
        return _offsetZ;
      }
      /// access the radius
      double radius() const {
        return _radius;
      }
      /// access the field name used for phi
      const std::string& fieldNamePhi() const {
        return _phiId;
      }
      /// access the field name used for Z
      const std::string& fieldNameZ() const {
        return _zId;
      }
      /// set the grid size in phi
      void setGridSizePhi(double cellSize) {
        _gridSizePhi = cellSize;
      }
      /// set the grid size in Z
      void setGridSizeZ(double cellSize) {
        _gridSizeZ = cellSize;
      }
      /// set the coordinate offset in phi
      void setOffsetPhi(double offset) {
        _offsetPhi = offset;
      }
      /// set the coordinate offset in Z
      void setOffsetZ(double offset) {
        _offsetZ = offset;
      }
      /// set the radius
      void setRadius(double radius) {
        _radius = radius;
      }
      /// set the field name used for phi
      void setFieldNamePhi(const std::string& fieldName) {
        _phiId = fieldName;
      }
      /// set the field name used for Z
      void setFieldNameZ(const std::string& fieldName) {
        _zId = fieldName;
      }
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in the following order: R*dPhi,dZ

          Returns a vector of the cellDimensions of the given cell ID
          \param cellID is ignored as all cells have the same dimension
          \return std::vector<double> size 2:
          -# size in x = R * size in phi
          -# size in z
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

    protected:
      /// the grid size in phi
      double _gridSizePhi;
      /// the coordinate offset in phi
      double _offsetPhi;
      /// the grid size in Z
      double _gridSizeZ;
      /// the coordinate offset in Z
      double _offsetZ;
      /// the radius
      double _radius;
      /// the field name used for phi
      std::string _phiId;
      /// the field name used for Z
      std::string _zId;
      /// encoding field used for the module
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CYLINDRICALGRIDPHIZ_H
