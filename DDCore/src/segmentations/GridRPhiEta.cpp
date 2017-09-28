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

GridRPhiEta::GridRPhiEta(const BitFieldCoder* aDecoder) :
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
  return Util::positionFromREtaPhi(r(cID), eta(cID), phi(cID));
}

CellID GridRPhiEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition, const VolumeID& vID) const {
  double lRadius = Util::radiusFromXYZ(globalPosition);
  double lEta = Util::etaFromXYZ(globalPosition);
  double lPhi = Util::phiFromXYZ(globalPosition);
  CellID cID = vID ;
  _decoder->set( cID, m_etaID, positionToBin(lEta, m_gridSizeEta, m_offsetEta) );
  _decoder->set( cID, m_phiID, positionToBin(lPhi, 2 * M_PI / (double) m_phiBins, m_offsetPhi) );
  _decoder->set( cID, m_rID  , positionToBin(lRadius, m_gridSizeR, m_offsetR) );
  return cID;
}

double GridRPhiEta::r(const CellID& cID) const {
  CellID rValue = _decoder->get(cID, m_rID);
  return binToPosition(rValue, m_gridSizeR, m_offsetR);
}
REGISTER_SEGMENTATION(GridRPhiEta)
}
}

