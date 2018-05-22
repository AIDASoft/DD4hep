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
 * PolarGridRPhi.h
 *
 *  Created on: Sept 13, 2014
 *      Author: Marko Petric
 */

#ifndef DDSegmentation_POLARGRIDRPHI_H_
#define DDSegmentation_POLARGRIDRPHI_H_

#include "DDSegmentation/PolarGrid.h"
#include <math.h>

namespace dd4hep {
  namespace DDSegmentation {

    /// A segmentation for arbitrary sizes in R and R-dependent sizes in Phi
    class PolarGridRPhi: public PolarGrid {
    public:
      /// Default constructor passing the encoding string
      PolarGridRPhi(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      PolarGridRPhi(const BitFieldCoder* decoder);
      /// destructor
      virtual ~PolarGridRPhi();

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
      /// access the grid size in R
      double gridSizeR() const {
        return _gridSizeR;
      }
      /// access the grid size in Phi
      double gridSizePhi() const {
        return _gridSizePhi;
      }
      /// access the coordinate offset in R
      double offsetR() const {
        return _offsetR;
      }
      /// access the coordinate offset in Phi
      double offsetPhi() const {
        return _offsetPhi;
      }
      /// access the field name used for R
      const std::string& fieldNameR() const {
        return _rId;
      }
      /// access the field name used for Phi
      const std::string& fieldNamePhi() const {
        return _phiId;
      }
      /// set the grid size in R
      void setGridSizeR(double cellSize) {
        _gridSizeR = cellSize;
      }
      /// set the grid size in Phi
      void setGridSizePhi(double cellSize) {
        _gridSizePhi = cellSize;
      }
      /// set the coordinate offset in R
      void setOffsetR(double offset) {
        _offsetR = offset;
      }
      /// set the coordinate offset in Phi
      void setOffsetPhi(double offset) {
        _offsetPhi = offset;
      }
      /// set the field name used for X
      void setFieldNameR(const std::string& fieldName) {
        _rId = fieldName;
      }
      /// set the field name used for Y
      void setFieldNamePhi(const std::string& fieldName) {
        _phiId = fieldName;
      }
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions: dr, r*dPhi

          Returns a vector of the cellDimensions of the given cell ID
          \param cID cellID
          \return std::vector<double> size 2:
          -# size in r
          -# size of r*dPhi at the radial centre of the pad
      */
      virtual std::vector<double> cellDimensions(const CellID& cID) const;

    protected:
      /// the grid size in R
      double _gridSizeR;
      /// the coordinate offset in R
      double _offsetR;
      /// the grid size in Phi
      double _gridSizePhi;
      /// the coordinate offset in Phi
      double _offsetPhi;
      /// the field name used for R
      std::string _rId;
      /// the field name used for Phi
      std::string _phiId;
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_POLARGRIDRPHI_H_ */
