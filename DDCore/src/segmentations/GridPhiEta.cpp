#include "DDSegmentation/GridPhiEta.h"
#include "DDSegmentation/SegmentationUtil.h"

namespace dd4hep {
namespace DDSegmentation {

GridPhiEta::GridPhiEta(const std::string& cellEncoding) :
  Segmentation(cellEncoding) {
  // define type and description
  _type = "GridPhiEta";
  _description = "Phi-eta segmentation in the global coordinates";

  // register all necessary parameters
  registerParameter("grid_size_eta", "Cell size in Eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
  registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
  registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

GridPhiEta::GridPhiEta(const BitFieldCoder* aDecoder) :
  Segmentation(aDecoder) {
  // define type and description
  _type = "GridPhiEta";
  _description = "Phi-eta segmentation in the global coordinates";

  // register all necessary parameters
  registerParameter("grid_size_eta", "Cell size in Eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
  registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
  registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

Vector3D GridPhiEta::position(const CellID& cID) const {
  return Util::positionFromREtaPhi(1.0, eta(cID), phi(cID));
}

CellID GridPhiEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition, const VolumeID& vID) const {
  double lEta = Util::etaFromXYZ(globalPosition);
  double lPhi = Util::phiFromXYZ(globalPosition);
  CellID cID = vID ;
  _decoder->set( cID, m_etaID, positionToBin(lEta, m_gridSizeEta, m_offsetEta) );
  _decoder->set( cID, m_phiID, positionToBin(lPhi, 2 * M_PI / (double) m_phiBins, m_offsetPhi) );
  return cID;
}

double GridPhiEta::eta(const CellID& cID) const {
  CellID etaValue = _decoder->get(cID, m_etaID);
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}
double GridPhiEta::phi(const CellID& cID) const {
  CellID phiValue = _decoder->get(cID, m_phiID);
  return binToPosition(phiValue, 2.*M_PI/(double)m_phiBins, m_offsetPhi);
}
REGISTER_SEGMENTATION(GridPhiEta)
}
}

