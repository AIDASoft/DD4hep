#ifndef DETSEGMENTATION_GRIDPHIETA_H
#define DETSEGMENTATION_GRIDPHIETA_H

#include "DDSegmentation/Segmentation.h"

/* #include "DDSegmentation/SegmentationUtil.h" */
#include <cmath>

/** GridPhiEta DDSegmentation/include/DDSegmentation/GridPhiEta.h GridPhiEta.h
 *
 *  Segmentation in eta and phi.
 *
 *  @author    Anna Zaborowska
 */

namespace dd4hep {
namespace DDSegmentation {
class GridPhiEta : public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridPhiEta(const std::string& aCellEncoding);
  /// Default constructor used by derived classes passing an existing decoder
  GridPhiEta(const BitFieldCoder* decoder);

  /// destructor
  virtual ~GridPhiEta() = default;

  /**  Determine the global position based on the cell ID.
   *   @warning This segmentation has no knowledge of radius, so radius = 1 is taken into calculations.
   *   @param[in] aCellId ID of a cell.
   *   return Position (radius = 1).
   */
  virtual Vector3D position(const CellID& aCellID) const;
  /**  Determine the cell ID based on the position.
   *   @param[in] aLocalPosition (not used).
   *   @param[in] aGlobalPosition position in the global coordinates.
   *   @param[in] aVolumeId ID of a volume.
   *   return Cell ID.
   */
  virtual CellID cellID(const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition, const VolumeID& aVolumeID) const;
  /**  Determine the pseudorapidity based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Pseudorapidity.
   */
  double eta(const CellID& aCellID) const;
  /**  Determine the azimuthal angle based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Phi.
   */
  double phi(const CellID& aCellID) const;
  /**  Get the grid size in pseudorapidity.
   *   return Grid size in eta.
   */
  inline double gridSizeEta() const {
    return m_gridSizeEta;
  }
  /**  Get the grid size in phi.
   *   return Grid size in phi.
   */
  inline double gridSizePhi() const {
    return 2 * M_PI / static_cast<double>(m_phiBins);
  }
  /**  Get the number of bins in azimuthal angle.
   *   return Number of bins in phi.
   */
  inline int phiBins() const {
    return m_phiBins;
  }
  /**  Get the coordinate offset in pseudorapidity.
   *   return The offset in eta.
   */
  inline double offsetEta() const {
    return m_offsetEta;
  }
  /**  Get the coordinate offset in azimuthal angle.
   *   return The offset in phi.
   */
  inline double offsetPhi() const {
    return m_offsetPhi;
  }
  /**  Get the field name used for pseudorapidity
   *   return The field name for eta.
   */
  inline const std::string& fieldNameEta() const {
    return m_etaID;
  }
  /**  Get the field name for azimuthal angle.
   *   return The field name for phi.
   */
  inline const std::string& fieldNamePhi() const {
    return m_phiID;
  }
  /**  Set the grid size in pseudorapidity.
   *   @param[in] aCellSize Cell size in eta.
   */
  void setGridSizeEta(double aCellSize) {
    m_gridSizeEta = aCellSize;
  }
  /**  Set the number of bins in azimuthal angle.
   *   @param[in] aNumberBins Number of bins in phi.
   */
  inline void setPhiBins(int bins) {
    m_phiBins = bins;
  }
  /**  Set the coordinate offset in pseudorapidity.
   *   @param[in] aOffset Offset in eta.
   */
  inline void setOffsetEta(double offset) {
    m_offsetEta = offset;
  }
  /**  Set the coordinate offset in azimuthal angle.
   *   @param[in] aOffset Offset in phi.
   */
  inline void setOffsetPhi(double offset) {
    m_offsetPhi = offset;
  }
  /**  Set the field name used for pseudorapidity.
   *   @param[in] aFieldName Field name for eta.
   */
  inline void setFieldNameEta(const std::string& fieldName) {
    m_etaID = fieldName;
  }
  /**  Set the field name used for azimuthal angle.
   *   @param[in] aFieldName Field name for phi.
   */
  inline void setFieldNamePhi(const std::string& fieldName) {
    m_phiID = fieldName;
  }

protected:

  /// the grid size in eta
  double m_gridSizeEta;
  /// the number of bins in phi
  int m_phiBins;
  /// the coordinate offset in eta
  double m_offsetEta;
  /// the coordinate offset in phi
  double m_offsetPhi;
  /// the field name used for eta
  std::string m_etaID;
  /// the field name used for phi
  std::string m_phiID;
};
}
}
#endif /* DETSEGMENTATION_GRIDPHIETA_H */
