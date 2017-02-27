#ifndef DETSEGMENTATION_GRIDRPHIETA_H
#define DETSEGMENTATION_GRIDRPHIETA_H

#include "DDSegmentation/Segmentation.h"

/* #include "DDSegmentation/SegmentationUtil.h" */
#include "TVector3.h"
#include <cmath>

/** GridRPhiEta Detector/DetSegmentation/DetSegmentation/GridRPhiEta.h GridRPhiEta.h
 *
 *  Segmentation in eta and phi.
 *  It requires 'export LD_PRELOAD_PATH=$LD_PRELOAD_PATH:build.$BINARY_TAG/lib/libDetSegmentation.so'
 *
 *  @author    Anna Zaborowska
 */

namespace DD4hep {
namespace DDSegmentation {
class GridRPhiEta: public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridRPhiEta(const std::string& aCellEncoding);
  /// destructor
  virtual ~GridRPhiEta();

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
  /**  Determine the radius based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Radius.
   */
  double r(const CellID& aCellID) const;
  /**  Get the grid size in pseudorapidity.
   *   return Grid size in eta.
   */
  inline double gridSizeEta() const {
    return m_gridSizeEta;
  }
  /**  Get the grid size in radial distance from the detector centre.
   *   return Grid size in radial distance.
   */
  inline double gridSizeR() const {
    return m_gridSizeR;
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
  /**  Get the coordinate offset in radial distance.
   *   return The offset in R.
   */
  inline double offsetR() const {
    return m_offsetR;
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
  /**  Get the field name for radial distance.
   *   return The field name for radial distance.
   */
  inline std::string fieldNameR() const {
    return m_rID;
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
  /**  Set the grid size in radial distance.
   *   @param[in] aCellSize Cell size in radial distance.
   */
  void setGridSizeR(double aCellSize) {
    m_gridSizeR = aCellSize;
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
  /**  Set the coordinate offset in radial distance.
   *   @param[in] aOffset Offset in radial distance.
   */
  inline void setOffsetR(double offset) {
    m_offsetR = offset;
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
  /**  Set the field name used for radial distance.
   *   @param[in] aFieldName Field name for R.
   */
  inline void setFieldNameR(const std::string& fieldName) {
    m_rID = fieldName;
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
  /// determine the radial distance R based on the current cell ID
  double r() const;

  /// the grid size in eta
  double m_gridSizeEta;
  /// the number of bins in phi
  int m_phiBins;
  /// the grid size in r
  double m_gridSizeR;
  /// the coordinate offset in eta
  double m_offsetEta;
  /// the coordinate offset in phi
  double m_offsetPhi;
  /// the coordinate offset in r
  double m_offsetR;
  /// the field name used for eta
  std::string m_etaID;
  /// the field name used for phi
  std::string m_phiID;
  /// the field name used for r
  std::string m_rID;
};
}
}
#endif /* DETSEGMENTATION_GRIDRPHIETA_H */
