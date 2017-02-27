#include "DetSegmentation/GridPhiEta.h"

namespace DD4hep {
namespace DDSegmentation {

  /// Default constructor used by derived classes passing an existing decoder
  GridPhiEta::GridPhiEta(BitField64* decode)	: Segmentation(decode) {
    // define type and description
    _type = "GridPhiEta";
    _description = "Projective segmentation in the global coordinates";

    // register all necessary parameters
    registerParameter("grid_size_eta", "Cell size in Eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
    registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
    registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
    registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
    registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
    registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
  }

/// default constructor using an encoding string
GridPhiEta::GridPhiEta(const std::string& cellEncoding) :
  Segmentation(cellEncoding) {
  // define type and description
  _type = "GridPhiEta";
  _description = "Projective segmentation in the global coordinates";

  // register all necessary parameters
  registerParameter("grid_size_eta", "Cell size in Eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
  registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
  registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

/// destructor
GridPhiEta::~GridPhiEta() {

}

/// determine the local based on the cell ID
Vector3D GridPhiEta::position(const CellID& cID) const {
  _decoder->setValue(cID);
  return positionFromREtaPhi(1.0, eta(), phi());
}

/// determine the cell ID based on the position
CellID GridPhiEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition, const VolumeID& vID) const {
  _decoder->setValue(vID);
  double lEta = etaFromXYZ(globalPosition);
  double lPhi = phiFromXYZ(globalPosition);
  (*_decoder)[m_etaID] = positionToBin(lEta, m_gridSizeEta, m_offsetEta);
  (*_decoder)[m_phiID] = positionToBin(lPhi, 2 * M_PI / (double) m_phiBins, m_offsetPhi);
  return _decoder->getValue();
}

/// determine the pseudorapidity based on the current cell ID
double GridPhiEta::eta() const {
  CellID etaValue = (*_decoder)[m_etaID].value();
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}
/// determine the azimuthal angle phi based on the current cell ID
double GridPhiEta::phi() const {
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, 2.*M_PI/(double)m_phiBins, m_offsetPhi);
}

/// determine the polar angle theta based on the cell ID
double GridPhiEta::eta(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID etaValue = (*_decoder)[m_etaID].value();
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}
/// determine the azimuthal angle phi based on the cell ID
double GridPhiEta::phi(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, 2.*M_PI/(double)m_phiBins, m_offsetPhi);
}
REGISTER_SEGMENTATION(GridPhiEta)
}
}

