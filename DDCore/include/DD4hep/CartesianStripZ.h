//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank, David Blyth
//  \date     2018-02-20
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_CARTESIANSTRIPZ_H
#define DD4HEP_CARTESIANSTRIPZ_H 1

// Framework include files
#include "DD4hep/Segmentations.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
/// Namespace for base segmentations
namespace DDSegmentation {
class CartesianStripZ;
}

/// We need some abbreviation to make the code more readable.
typedef Handle<SegmentationWrapper<DDSegmentation::CartesianStripZ> > CartesianStripZHandle;

/// Implementation class for the strip Z segmentation.
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
class CartesianStripZ : public CartesianStripZHandle {
   public:
    /// Default constructor
    CartesianStripZ() = default;
    /// Copy constructor
    CartesianStripZ(const CartesianStripZ& e) = default;
    /// Copy Constructor from segmentation base object
    CartesianStripZ(const Segmentation& e) : Handle<Object>(e) {}
    /// Copy constructor from handle
    CartesianStripZ(const Handle<Object>& e) : Handle<Object>(e) {}
    /// Copy constructor from other equivalent handle
    template <typename Q>
    CartesianStripZ(const Handle<Q>& e) : Handle<Object>(e) {}
    /// Assignment operator
    CartesianStripZ& operator=(const CartesianStripZ& seg) = default;
    /// Equality operator
    bool operator==(const CartesianStripZ& seg) const { return m_element == seg.m_element; }

    /// determine the position based on the cell ID
    Position position(const CellID& cellID) const;
    /// determine the cell ID based on the position
    CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
    /// access the strip size in Z
    double stripSizeZ() const;
    /// set the strip size in Z
    void setStripSizeZ(double cellSize) const;
    /// access the coordinate offset in Z
    double offsetZ() const;
    /// set the coordinate offset in Z
    void setOffsetZ(double offset) const;
    /// access the field name used for Z
    const std::string& fieldNameZ() const;
    /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

        Returns a vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 1:
        -# size in x
    */
    std::vector<double> cellDimensions(const CellID& cellID) const;
};
} /* End namespace dd4hep                */
#endif // DD4HEP_CARTESIANSTRIPZ_H
