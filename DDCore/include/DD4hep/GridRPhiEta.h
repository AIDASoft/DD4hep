#ifndef DD4HEP_DDCORE_GRIDRPHIETA_H
#define DD4HEP_DDCORE_GRIDRPHIETA_H 1

// Framework includes
#include "DDSegmentation/GridRPhiEta.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/objects/SegmentationsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

/// Namespace for base segmentations

/// Namespace for the geometry part of the AIDA detector description toolkit
namespace Geometry {

// Forward declarations
class Segmentation;
template <typename T>
class SegmentationWrapper;

/// We need some abbreviation to make the code more readable.
typedef Handle<SegmentationWrapper<DDSegmentation::GridRPhiEta>> GridRPhiEtaHandle;

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
 *  conveniance reasons instantiated in DD4hep/src/Segmentations.cpp.
 *
 *  \author  A. Zaborowska
 *  \version 1.0
 */
class GridRPhiEta : public GridRPhiEtaHandle {
public:
  /// Defintiion of the basic handled object
  typedef GridRPhiEtaHandle::Implementation Object;

public:
  /// Default constructor
  GridRPhiEta() = default;
  /// Copy constructor
  GridRPhiEta(const GridRPhiEta& e) = default;
  /// Copy Constructor from segmentation base object
  GridRPhiEta(const Segmentation& e) : Handle<Object>(e) {}
  /// Copy constructor from handle
  GridRPhiEta(const Handle<Object>& e) : Handle<Object>(e) {}
  /// Copy constructor from other polymorph/equivalent handle
  template <typename Q>
  GridRPhiEta(const Handle<Q>& e) : Handle<Object>(e) {}
  /// Assignment operator
  GridRPhiEta& operator=(const GridRPhiEta& seg) = default;
  /// Equality operator
  bool operator==(const GridRPhiEta& seg) const { return m_element == seg.m_element; }
  /// determine the position based on the cell ID
  inline Position position(const CellID& id) const { return Position(access()->implementation->position(id)); }

  /// determine the cell ID based on the position
  inline DD4hep::CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const {
    return access()->implementation->cellID(local, global, volID);
  }

  /// access the grid size in Eta
  inline double gridSizeEta() const { return access()->implementation->gridSizeEta(); }

  /// access the grid size in Phi
  inline int phiBins() const { return access()->implementation->phiBins(); }

  /// access the grid size in Phi
  inline double gridSizeR() const { return access()->implementation->gridSizeR(); }

  /// access the coordinate offset in R
  inline double offsetR() const { return access()->implementation->offsetR(); }

  /// access the coordinate offset in Eta
  inline double offsetEta() const { return access()->implementation->offsetEta(); }

  /// access the coordinate offset in Phi
  inline double offsetPhi() const { return access()->implementation->offsetPhi(); }

  /// set the coordinate offset in R
  inline void setOffsetEta(double offset) const { access()->implementation->setOffsetEta(offset); }

  /// set the coordinate offset in Phi
  inline void setOffsetR(double offset) const { access()->implementation->setOffsetR(offset); }

  /// set the coordinate offset in Phi
  inline void setOffsetPhi(double offset) const { access()->implementation->setOffsetPhi(offset); }

  /// set the grid size in Eta
  inline void setGridSizeEta(double cellSize) const { access()->implementation->setGridSizeEta(cellSize); }

  /// set the grid size in R
  inline void setGridSizeR(double cellSize) const { access()->implementation->setGridSizeR(cellSize); }

  /// set the grid size in Phi
  inline void setPhiBins(int cellSize) const { access()->implementation->setPhiBins(cellSize); }

  /// access the field name used for R
  inline const std::string& fieldNameR() const { return access()->implementation->fieldNameR(); }

  /// access the field name used for Eta
  inline const std::string& fieldNameEta() const { return access()->implementation->fieldNameEta(); }

  /// access the field name used for Phi
  inline const std::string& fieldNamePhi() const { return access()->implementation->fieldNamePhi(); }

  /** \brief Returns a std::vector<double> of the cellDimensions of the given cell ID
      in natural order of dimensions (dR, dPhi, dEta)

      Returns a std::vector of the cellDimensions of the given cell ID
      \param cellID is ignored as all cells have the same dimension
      \return std::vector<double> size 3:
      -# size in r
      -# size in phi
      -# size in eta
  */
  inline std::vector<double> cellDimensions(const CellID& /*id*/) const {
    return {access()->implementation->gridSizeR(),
            access()->implementation->gridSizePhi(),
            access()->implementation->gridSizeEta()};
  }
};

} /* End namespace Geometry              */
} /* End namespace DD4hep                */
#endif  // DD4HEP_DDCORE_GRIDRPHIETA_H
