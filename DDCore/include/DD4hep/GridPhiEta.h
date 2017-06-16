//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   A. Zaborowska
//  \date     2016-10-18
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_DDCORE_GRIDPHIETA_H
#define DD4HEP_DDCORE_GRIDPHIETA_H 1

// Framework includes
#include "DDSegmentation/GridPhiEta.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/detail/SegmentationsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// We need some abbreviation to make the code more readable.
  typedef Handle<SegmentationWrapper<DDSegmentation::GridPhiEta>> GridPhiEtaHandle;

  /// Implementation class for the grid phi-eta segmentation.
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
   *  \author  A. Zaborowska
   *  \version 1.0
   */
  class GridPhiEta : public GridPhiEtaHandle {
  public:
    /// Default constructor
    GridPhiEta() = default;
    /// Copy constructor
    GridPhiEta(const GridPhiEta& e) = default;
    /// Copy Constructor from segmentation base object
    GridPhiEta(const Segmentation& e) : Handle<Object>(e) {}
    /// Copy constructor from handle
    GridPhiEta(const Handle<Object>& e) : Handle<Object>(e) {}
    /// Copy constructor from other polymorph/equivalent handle
    template <typename Q>
    GridPhiEta(const Handle<Q>& e) : Handle<Object>(e) {}
    /// Assignment operator
    GridPhiEta& operator=(const GridPhiEta& seg) = default;
    /// Equality operator
    bool operator==(const GridPhiEta& seg) const { return m_element == seg.m_element; }
    /// determine the position based on the cell ID
    inline Position position(const CellID& id) const { return Position(access()->implementation->position(id)); }

    /// determine the cell ID based on the position
    inline dd4hep::CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const {
      return access()->implementation->cellID(local, global, volID);
    }

    /// access the grid size in Eta
    inline double gridSizeEta() const { return access()->implementation->gridSizeEta(); }

    /// access the grid size in Phi
    inline int phiBins() const { return access()->implementation->phiBins(); }

    /// access the coordinate offset in Eta
    inline double offsetEta() const { return access()->implementation->offsetEta(); }

    /// access the coordinate offset in Phi
    inline double offsetPhi() const { return access()->implementation->offsetPhi(); }

    /// set the coordinate offset in Eta
    inline void setOffsetEta(double offset) const { access()->implementation->setOffsetEta(offset); }

    /// set the coordinate offset in Phi
    inline void setOffsetPhi(double offset) const { access()->implementation->setOffsetPhi(offset); }

    /// set the grid size in Eta
    inline void setGridSizeEta(double cellSize) const { access()->implementation->setGridSizeEta(cellSize); }

    /// set the grid size in Phi
    inline void setPhiBins(int cellSize) const { access()->implementation->setPhiBins(cellSize); }

    /// access the field name used for Eta
    inline const std::string& fieldNameEta() const { return access()->implementation->fieldNameEta(); }

    /// access the field name used for Phi
    inline const std::string& fieldNamePhi() const { return access()->implementation->fieldNamePhi(); }

    /** \brief Returns a std::vector<double> of the cellDimensions of the given cell ID
        in natural order of dimensions (dPhi, dEta)

        Returns a std::vector of the cellDimensions of the given cell ID
        \param cellID is ignored as all cells have the same dimension
        \return std::vector<double> size 2:
        -# size in phi
        -# size in eta
    */
    inline std::vector<double> cellDimensions(const CellID& /*id*/) const {
      return {access()->implementation->gridSizePhi(), access()->implementation->gridSizeEta()};
    }
  };
}       /* End namespace dd4hep                */
#endif  // DD4HEP_DDCORE_POLARGRIDRPHI_H
