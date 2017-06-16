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
#ifndef DD4HEP_DDCORE_CARTESIANGRIDXY_H 
#define DD4HEP_DDCORE_CARTESIANGRIDXY_H 1

// Framework include files
#include "DD4hep/Segmentations.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class CartesianGridXY;  }
  
  /// We need some abbreviation to make the code more readable.
  typedef Handle<SegmentationWrapper<DDSegmentation::CartesianGridXY> > CartesianGridXYHandle;

  /// Implementation class for the grid XY segmentation.
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
  class CartesianGridXY : public CartesianGridXYHandle  {
  public:
    /// Default constructor
    CartesianGridXY() = default;
    /// Copy constructor
    CartesianGridXY(const CartesianGridXY& e) = default;
    /// Copy Constructor from segmentation base object
    CartesianGridXY(const Segmentation& e) : Handle<Object>(e) { }
    /// Copy constructor from handle
    CartesianGridXY(const Handle<Object>& e) : Handle<Object>(e) { }
    /// Copy constructor from other equivalent handle
    template <typename Q> CartesianGridXY(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Assignment operator
    CartesianGridXY& operator=(const CartesianGridXY& seg) = default;
    /// Equality operator
    bool operator==(const CartesianGridXY& seg) const {
      return m_element == seg.m_element;
    }

    /// determine the position based on the cell ID
    Position position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
    /// access the grid size in X
    double gridSizeX() const;
    /// access the grid size in Y
    double gridSizeY() const;
    /// set the grid size in X
    void setGridSizeX(double cellSize) const;
    /// set the grid size in Y
    void setGridSizeY(double cellSize) const;
    /// access the coordinate offset in X
    double offsetX() const;
    /// access the coordinate offset in Y
    double offsetY() const;
    /// set the coordinate offset in X
    void setOffsetX(double offset) const;
    /// set the coordinate offset in Y
    void setOffsetY(double offset) const;
    /// access the field name used for X
    const std::string& fieldNameX() const;
    /// access the field name used for Y
    const std::string& fieldNameY() const;
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

        Returns a vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 2:
        -# size in x
        -# size in y
    */
    std::vector<double> cellDimensions(const CellID& cellID) const;
  };

} /* End namespace dd4hep                */
#endif // DD4HEP_DDCORE_CARTESIANGRIDXY_H
