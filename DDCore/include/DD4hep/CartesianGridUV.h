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
#ifndef DD4HEP_CARTESIANGRIDUV_H
#define DD4HEP_CARTESIANGRIDUV_H 1

// Framework include files
#include <DD4hep/Segmentations.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class CartesianGridUV;  }
  
  /// We need some abbreviation to make the code more readable.
  typedef Handle<SegmentationWrapper<DDSegmentation::CartesianGridUV> > CartesianGridUVHandle;

  /// Implementation class for the grid UV segmentation.
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
   *  The handle base corresponding to this object in for
   *  conveniance reasons instantiated in dd4hep/src/Segmentations.cpp.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class CartesianGridUV : public CartesianGridUVHandle  {
  public:
    /// Default constructor
    CartesianGridUV() = default;
    /// Copy constructor
    CartesianGridUV(const CartesianGridUV& e) = default;
    /// Copy Constructor from segmentation base object
    CartesianGridUV(const Segmentation& e) : Handle<Object>(e) { }
    /// Copy constructor from handle
    CartesianGridUV(const Handle<Object>& e) : Handle<Object>(e) { }
    /// Copy constructor from other equivalent handle
    template <typename Q> CartesianGridUV(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Assignment operator
    CartesianGridUV& operator=(const CartesianGridUV& seg) = default;
    /// Equality operator
    bool operator==(const CartesianGridUV& seg) const {
      return m_element == seg.m_element;
    }

    /// determine the position based on the cell ID
    Position position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
    /// access the grid size in U
    double gridSizeU() const;
    /// access the grid size in V
    double gridSizeV() const;
    /// set the grid size in U
    void setGridSizeU(double cellSize) const;
    /// set the grid size in V
    void setGridSizeV(double cellSize) const;
    /// access the coordinate offset in U
    double offsetU() const;
    /// access the coordinate offset in V
    double offsetV() const;
    /// access the rotation angle
    double gridAngle() const;
    /// set the coordinate offset in U
    void setOffsetU(double offset) const;
    /// set the coordinate offset in V
    void setOffsetV(double offset) const;
    /// set the rotation angle
    void setGridAngle(double angle) const;
    /// access the field name used for U
    const std::string& fieldNameU() const;
    /// access the field name used for V
    const std::string& fieldNameV() const;
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

        Returns a vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 2:
        -# size in U
        -# size in V
    */
    std::vector<double> cellDimensions(const CellID& cellID) const;
  };

} /* End namespace dd4hep                */
#endif // DD4HEP_CARTESIANGRIDUV_H
