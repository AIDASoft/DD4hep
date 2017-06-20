//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-10-18
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_DDCORE_POLARGRIDRPHI2_H 
#define DD4HEP_DDCORE_POLARGRIDRPHI2_H 1

// Framework include files
#include "DD4hep/Segmentations.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class PolarGridRPhi2;  }
  
  /// We need some abbreviation to make the code more readable.
  typedef Handle<SegmentationWrapper<DDSegmentation::PolarGridRPhi2> > PolarGridRPhi2Handle;

  /// Implementation class for the grid XZ segmentation.
  /**
   *  Concrete user handle to serve specific needs of client code
   *  which requires access to the base functionality not served
   *  by the super-class Segmentation.
   *
   *  Note:
   *  We only check the validity of the underlying handle.
   *  If for whatever reason the implementation object is not valid
   *  This is not checked.
   *  In principle this CANNOT happen unless some brain-dead has
   *  fiddled with the handled object directly.....
   *
   *  Note:
   *  The handle base corrsponding to this object in for
   *  conveniance reasons instantiated in dd4hep/src/Segmentations.cpp.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_GEOMETRY
   */
  class PolarGridRPhi2 : public PolarGridRPhi2Handle  {
  public:
    /// Default constructor
    PolarGridRPhi2() = default;
    /// Copy constructor
    PolarGridRPhi2(const PolarGridRPhi2& e) = default;
    /// Copy Constructor from segmentation base object
    PolarGridRPhi2(const Segmentation& e) : Handle<Object>(e) {}
    /// Copy constructor from handle
    PolarGridRPhi2(const Handle<Object>& e) : Handle<Object>(e) {}
    /// Copy constructor from other polymorph/equivalent handle
    template <typename Q> PolarGridRPhi2(const Handle<Q>& e) : Handle<Object>(e) {}
    /// Assignment operator
    PolarGridRPhi2& operator=(const PolarGridRPhi2& seg) = default;
    /// Equality operator
    bool operator==(const PolarGridRPhi2& seg) const
    {  return m_element == seg.m_element;      }
    /// determine the position based on the cell ID
    Position position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
    /// access the grid size in R
    double gridSizeR() const;
    /// access the grid size in Phi
    double gridSizePhi() const;
    /// access the coordinate offset in R
    double offsetR() const;
    /// access the coordinate offset in Phi
    double offsetPhi() const;
    /// set the coordinate offset in R
    void setOffsetR(double offset) const;
    /// set the coordinate offset in Phi
    void setOffsetPhi(double offset) const;
    /// access the grid size in R
    std::vector<double> gridRValues() const;
    /// access the grid size in Phi
    std::vector<double>  gridPhiValues() const;
    /// set the grid Boundaries in R
    void setGridSizeR(double cellSize, int rID) const;
    /// set the grid size in Phi
    void setGridSizePhi(double cellSize, int phiID) const;
    /// set the grid boundaries for R
    /** @param rValues The boundaries of the segments in radius,
     *                 the first value is the lower boundary of the first bin, 
     *                 the highest value is the upper boundary of the last bin
     */
    void setGridRValues(std::vector<double> const& rValues) const;
    /// set the grid size in Phi for each bin in R
    /** @param phiValues The size in phi for each bin in R, 
     *                   this vector is one smaller than the vector 
     *                   for PolarGridRPhi2::setGridRValues
     */
    void setGridPhiValues(std::vector<double> const& phiValues) const;

    /// access the field name used for R
    const std::string& fieldNameR() const;
    /// access the field name used for Phi
    const std::string& fieldNamePhi() const;
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

        Returns a vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 2:
        -# size in x
        -# size in z
    */
    std::vector<double> cellDimensions(const CellID& cellID) const;
  };
}      /* End namespace dd4hep                */
#endif // DD4HEP_DDCORE_POLARGRIDRPHI2_H
