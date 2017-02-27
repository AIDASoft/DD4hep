#include "DetSegmentation/GridRPhiEta.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
GridRPhiEta::GridRPhiEta(const std::string& cellEncoding) :
  Segmentation(cellEncoding) {
  // define type and description
  _type = "GridRPhiEta";
  _description = "Projective segmentation in the global coordinates";

  // register all necessary parameters
  registerParameter("grid_size_eta", "Cell size in eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
  registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
  registerParameter("grid_size_r", "Cell size in radial distance", m_gridSizeR, 1., SegmentationParameter::LengthUnit);
  registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerParameter("offset_r", "Angular offset in radial distance", m_offsetR, 0., SegmentationParameter::LengthUnit, true);
  registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
  registerIdentifier("identifier_r", "Cell ID identifier for R", m_rID, "r");
}

/// destructor
GridRPhiEta::~GridRPhiEta() {}

/// determine the local based on the cell ID
Vector3D GridRPhiEta::position(const CellID& cID) const {
  _decoder->setValue(cID);
  return positionFromREtaPhi(r(), eta(), phi());
}

/// determine the cell ID based on the position
CellID GridRPhiEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition, const VolumeID& vID) const {
  _decoder->setValue(vID);
  double lRadius = radiusFromXYZ(globalPosition);
  double lEta = etaFromXYZ(globalPosition);
  double lPhi = phiFromXYZ(globalPosition);
  (*_decoder)[m_etaID] = positionToBin(lEta, m_gridSizeEta, m_offsetEta);
  (*_decoder)[m_phiID] = positionToBin(lPhi, 2 * M_PI / (double) m_phiBins, m_offsetPhi);
  (*_decoder)[m_rID] = positionToBin(lRadius, m_gridSizeR, m_offsetR);
  if ( (*_decoder)[m_rID] != (*_decoder)["active_layer"] ) {
    std::cout <<" Position = ( " << globalPosition.x() << " , " << globalPosition.y() << " , " << globalPosition.z() << " )\t ";
    std::cout << "Radius: " << lRadius << " \tcellsize: " << m_gridSizeR << " \toffset: " << m_offsetR << " -> " << (*_decoder)[m_rID] << " !=  " << (*_decoder)["active_layer"] << std::endl;
  }
  return _decoder->getValue();
}

/// determine the pseudorapidity based on the current cell ID
double GridRPhiEta::eta() const {
  CellID etaValue = (*_decoder)[m_etaID].value();
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}
/// determine the azimuthal angle phi based on the current cell ID
double GridRPhiEta::phi() const {
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, 2.*M_PI/(double)m_phiBins, m_offsetPhi);
}
/// determine the radial distance R based on the current cell ID
double GridRPhiEta::r() const {
  CellID rValue = (*_decoder)[m_rID].value();
  return binToPosition(rValue, m_gridSizeR, m_offsetR);
}

/// determine the polar angle theta based on the cell ID
double GridRPhiEta::eta(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID etaValue = (*_decoder)[m_etaID].value();
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}
/// determine the azimuthal angle phi based on the cell ID
double GridRPhiEta::phi(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, 2.*M_PI/(double)m_phiBins, m_offsetPhi);
}
/// determine the radial distance R based on the cell ID
double GridRPhiEta::r(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID rValue = (*_decoder)[m_rID].value();
  return binToPosition(rValue, m_gridSizeR, m_offsetR);
}
REGISTER_SEGMENTATION(GridRPhiEta)
}
}

