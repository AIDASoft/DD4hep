#include "DDSegmentation/GridRPhiEta.h"
#include "DDSegmentation/SegmentationUtil.h"

namespace dd4hep {
namespace DDSegmentation {

GridRPhiEta::GridRPhiEta(const std::string& cellEncoding) :
  GridPhiEta(cellEncoding) {
  // define type and description
  _type = "GridRPhiEta";
  _description = "R-phi-eta segmentation in the global coordinates";

  // register all necessary parameters (additional to those registered in GridPhiEta)
  registerParameter("grid_size_r", "Cell size in radial distance", m_gridSizeR, 1., SegmentationParameter::LengthUnit);
  registerParameter("offset_r", "Angular offset in radial distance", m_offsetR, 0., SegmentationParameter::LengthUnit, true);
  registerIdentifier("identifier_r", "Cell ID identifier for R", m_rID, "r");
}

GridRPhiEta::GridRPhiEta(BitField64* aDecoder) :
  GridPhiEta(aDecoder) {
  // define type and description
  _type = "GridRPhiEta";
  _description = "R-phi-eta segmentation in the global coordinates";

  // register all necessary parameters (additional to those registered in GridPhiEta)
  registerParameter("grid_size_r", "Cell size in radial distance", m_gridSizeR, 1., SegmentationParameter::LengthUnit);
  registerParameter("offset_r", "Angular offset in radial distance", m_offsetR, 0., SegmentationParameter::LengthUnit, true);
  registerIdentifier("identifier_r", "Cell ID identifier for R", m_rID, "r");
}

Vector3D GridRPhiEta::position(const CellID& cID) const {
  _decoder->setValue(cID);
  return Util::positionFromREtaPhi(r(), eta(), phi());
}

CellID GridRPhiEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition, const VolumeID& vID) const {
  _decoder->setValue(vID);
  double lRadius = Util::radiusFromXYZ(globalPosition);
  double lEta = Util::etaFromXYZ(globalPosition);
  double lPhi = Util::phiFromXYZ(globalPosition);
  (*_decoder)[m_etaID] = positionToBin(lEta, m_gridSizeEta, m_offsetEta);
  (*_decoder)[m_phiID] = positionToBin(lPhi, 2 * M_PI / (double) m_phiBins, m_offsetPhi);
  (*_decoder)[m_rID] = positionToBin(lRadius, m_gridSizeR, m_offsetR);
  return _decoder->getValue();
}

double GridRPhiEta::r() const {
  CellID rValue = (*_decoder)[m_rID].value();
  return binToPosition(rValue, m_gridSizeR, m_offsetR);
}

double GridRPhiEta::r(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID rValue = (*_decoder)[m_rID].value();
  return binToPosition(rValue, m_gridSizeR, m_offsetR);
}
REGISTER_SEGMENTATION(GridRPhiEta)
}
}

