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
 * PolarGridRPhi2.h
 *
 *  Created on: Sept 13, 2014
 *      Author: Marko Petric
 */

#ifndef DDSEGMENTATION_POLARGRIDRPHI2_H
#define DDSEGMENTATION_POLARGRIDRPHI2_H

#include "DDSegmentation/PolarGrid.h"
#include <cmath>
#include <vector>

namespace dd4hep {
  namespace DDSegmentation {


    /// A segmentation for arbitrary sizes in R and R-dependent sizes in Phi
    /**
     *  Note: Counting of phi-ids starts at the offset. There are no negative phi-IDs<br>
     * The grid_r_values give the boundaries of the bin, the grid_phi_values give the size of the bin for each radial bin
     * see also PolarGridRPhi2::setGridRValues and PolarGridRPhi2::setGridPhiValues<br>
     * Example:<br>
     @verbatim
     <segmentation type="PolarGridRPhi2"
     grid_r_values="3.2*cm 3.7*cm 4.9*cm 5.5*cm<br/>
     6.*cm 7.*cm 7.5*cm 8.7*cm<br/>
     9.5*cm 10.*cm 11.*cm 11.8*cm<br/>
     12.*cm 13.*cm 14.*cm 15.0*cm"
     grid_phi_values="360/(4*8)*degree 360/(5*8)*degree
     360/(6*8)*degree 360/(6*8)*degree
     360/(7*8)*degree 360/(8*8)*degree
     360/(9*8)*degree 360/(9*8)*degree
     360/(10*8)*degree 360/(11*8)*degree
     360/(12*8)*degree 360/(12*8)*degree
     360/(13*8)*degree 360/(14*8)*degree
     360/(15*8)*degree"
     offset_phi="-180.0*degree"
     />
     <id>system:8,barrel:3,layer:8,slice:5,r:32:16,phi:16</id>
     @endverbatim
     *    @version $Id$
     *    @date  2015-03
     */
    class PolarGridRPhi2: public PolarGrid {
    public:
      /// Default constructor passing the encoding string
      PolarGridRPhi2(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      PolarGridRPhi2(const BitFieldCoder* decoder);
      /// destructor
      virtual ~PolarGridRPhi2();

      /// determine the position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
      /// access the grid size in R
      std::vector<double> gridRValues() const {
        return _gridRValues;
      }
      /// access the grid size in Phi
      std::vector<double>  gridPhiValues() const {
        return _gridPhiValues;
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
      /// set the grid Boundaries in R
      void setGridRValues(double cellSize, int rID) {
        _gridRValues[rID] = cellSize;
      }
      /// set the grid size in Phi
      void setGridSizePhi(double cellSize, int phiID) {
        _gridPhiValues[phiID] = cellSize;
      }

      /// set the grid boundaries for R
      /// @param rValues The boundaries of the segments in radius, the first value is the lower boundary of the first bin, the highest value is the upper boundary of the last bin
      void setGridRValues(std::vector<double> const& rValues) {
        _gridRValues = std::vector<double>(rValues);
      }

      /// set the grid size in Phi for each bin in R
      /// @param phiValues The size in phi for each bin in R, this vector is one smaller than the vector for PolarGridRPhi2::setGridRValues
      void setGridPhiValues(std::vector<double> const& phiValues) {
        _gridPhiValues = std::vector<double>(phiValues);
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
          \param cellID is ignored as all cells have the same dimension
          \return std::vector<double> size 2:
          -# size in r
          -# size of r*dPhi at the radial centre of the pad
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

    protected:
      /// the grid boundaries in R
      std::vector<double> _gridRValues;
      /// the coordinate offset in R
      double _offsetR;
      /// the grid sizes in Phi
      std::vector<double> _gridPhiValues;
      /// the coordinate offset in Phi
      double _offsetPhi;
      /// the field name used for R
      std::string _rId;
      /// the field name used for Phi
      std::string _phiId;
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_POLARGRIDRPHI2_H
