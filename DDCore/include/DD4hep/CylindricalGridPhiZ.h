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
#ifndef DD4HEP_CYLINDRICALGRIDPHIZ_H
#define DD4HEP_CYLINDRICALGRIDPHIZ_H 1

// Framework include files
#include <DD4hep/Segmentations.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class CylindricalGridPhiZ;  }
    
  /// We need some abbreviation to make the code more readable.
  typedef Handle<SegmentationWrapper<DDSegmentation::CylindricalGridPhiZ> > CylindricalGridPhiZHandle;
 
  /// Implementation class for the grid PhiZ segmentation.
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
   *  \ingroup DD4HEP_CORE
   */
  class CylindricalGridPhiZ : public  CylindricalGridPhiZHandle {
  public:
    /// Default constructor
    CylindricalGridPhiZ() = default;
    /// Copy constructor
    CylindricalGridPhiZ(const CylindricalGridPhiZ& e) = default;
    /// Copy Constructor from segmentation base object
    CylindricalGridPhiZ(const Segmentation& e) : Handle<Object>(e) {}
    /// Copy constructor from handle
    CylindricalGridPhiZ(const Handle<Object>& e) : Handle<Object>(e) {}
    /// Copy constructor from other polymorph/equivalent handle
    template <typename Q>
    CylindricalGridPhiZ(const Handle<Q>& e) : Handle<Object>(e) {}
    /// Assignment operator
    CylindricalGridPhiZ& operator=(const CylindricalGridPhiZ& seg) = default;
    /// Equality operator
    bool operator==(const CylindricalGridPhiZ& seg) const
    {  return m_element == seg.m_element;           }

    /// determine the position based on the cell ID
    Position position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
    /// access the grid size in phi
    double gridSizePhi() const;
    /// access the grid size in Z
    double gridSizeZ() const;
    /// access the radius
    double radius() const;
    /// access the coordinate offset in phi
    double offsetPhi() const;
    /// access the coordinate offset in Z
    double offsetZ() const;
    
    /// set the grid size in phi
    void setGridSizePhi(double cellSize) const;
    /// set the grid size in Z
    void setGridSizeZ(double cellSize) const;
    /// set the coordinate offset in phi
    void setOffsetPhi(double offset) const;
    /// set the coordinate offset in Z
    void setOffsetZ(double offset) const;
    /// set the radius
    void setRadius(double radius);
    /// access the field name used for phi
    const std::string& fieldNamePhi() const;
    /// access the field name used for Z
    const std::string& fieldNameZ() const;
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
}       /* End namespace dd4hep                */
#endif // DD4HEP_CYLINDRICALGRIDPHIZ_H
