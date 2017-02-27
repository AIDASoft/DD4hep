#ifndef DETSEGMENTATION_GRIDPHIETA_H
#define DETSEGMENTATION_GRIDPHIETA_H

#include "DDSegmentation/Segmentation.h"

/* #include "DDSegmentation/SegmentationUtil.h" */
#include "TVector3.h"
#include <cmath>

/** GridPhiEta Detector/DetSegmentation/DetSegmentation/GridPhiEta.h GridPhiEta.h
 *
 *  Segmentation in eta and phi.
 *  It requires 'export LD_PRELOAD_PATH=$LD_PRELOAD_PATH:build.$BINARY_TAG/lib/libDetSegmentation.so'
 *
 *  @author    Anna Zaborowska
 */

namespace DD4hep {
namespace DDSegmentation {
class GridPhiEta: public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridPhiEta(BitField64* decoder);
  /// default constructor using an arbitrary type
  GridPhiEta(const std::string& aCellEncoding);
  /// destructor
  virtual ~GridPhiEta();

  /**  Determine the global position based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Position.
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
  inline std::string fieldNameEta() const {
    return m_etaID;
  }
  /**  Get the field name for azimuthal angle.
   *   return The field name for phi.
   */
  inline std::string fieldNamePhi() const {
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
  /// calculates the Cartesian position from spherical coordinates (r, phi, eta)
  inline Vector3D positionFromREtaPhi(double ar, double aeta, double aphi) const {
    return Vector3D(ar * std::cos(aphi), ar * std::sin(aphi), ar * std::sinh(aeta));
  }
  /// calculates the pseudorapidity from Cartesian coordinates
  inline double etaFromXYZ(const Vector3D& aposition) const {
    TVector3 vec(aposition.X, aposition.Y, aposition.Z);
    return vec.Eta();
  }
  /// from SegmentationUtil
  /// to be removed once SegmentationUtil can be included w/o linker error
  /// calculates the azimuthal angle phi from Cartesian coordinates
  inline double phiFromXYZ(const Vector3D& aposition) const {
    return std::atan2(aposition.Y, aposition.X);
  }
  /// from SegmentationUtil
  /// to be removed once SegmentationUtil can be included w/o linker error
  /// calculates the radius in the xy-plane from Cartesian coordinates
  inline double radiusFromXYZ(const Vector3D& aposition) const {
    return std::sqrt(aposition.X * aposition.X + aposition.Y * aposition.Y);
  }

private:
  /// determine the pseudorapidity based on the current cell ID
  double eta() const;
  /// determine the azimuthal angle phi based on the current cell ID
  double phi() const;

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
