#include "ILDExVXD.h"
#include "VXDData.h"
#include <iostream>

using namespace std;

using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Constructor from another handle. Checks validity of assignment internally
ILDExVXD::ILDExVXD(const Ref_t& e) : Geometry::DetElement(e), m_data(0)
{
  if ( e.isValid() && (!isValid() || !(m_data=dynamic_cast<VXDData*>(e.ptr()))) )  {
    Ref_t::bad_assignment(typeid(*e.ptr()),typeid(VXDData));
  }
}

/// Assignment from another handle. Checks validity of assignment internally
ILDExVXD& ILDExVXD::operator=(const Ref_t& e) {
  this->DetElement::operator=(e);
  if ( e.isValid() && (!isValid() || !(m_data=dynamic_cast<VXDData*>(e.ptr()))) )  {
    Ref_t::bad_assignment(typeid(*e.ptr()),typeid(VXDData));
  }
  return *this;
}

/// Assignment from same type of object
ILDExVXD& ILDExVXD::operator=(const ILDExVXD& e) {
  if ( &e != this ) {
    this->DetElement::operator=(e);
    m_data = e.m_data;
  }
  return *this;
}

/// The total number of layers.
int ILDExVXD::getNLayers() const {
  VXDData* obj = data<VXDData>();
  return obj->_lVec.size();
}

/// The number of ladders in the layer layerIndex
int ILDExVXD::getNLadders(int layerIndex) const { 
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).NLadders; 
}

/// Azimuthal angle of the (outward pointing) normal of the first ladder.
double ILDExVXD::getPhi0(int layerIndex) const { 
  const VXDData* obj = data<VXDData>();
  return M_PI/2. - obj->_lVec.at(layerIndex).internalPhi0;
}
  
/// The radiation length in the support structure ladders of layer layerIndex
double ILDExVXD::getLadderRadLength(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).RadLength;
}
  
/// The distance of ladders in layer layerIndex from the IP
double ILDExVXD::getLadderDistance(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).Distance;
}

/// The thickness in mm of the ladders in layerIndex
double ILDExVXD::getLadderThickness(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).Thickness; 
}

/// The offset of the ladder
double ILDExVXD::getLadderOffset(int layerIndex) const { 
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).Offset; 
}

/// The width of the ladder in layer in mm for ladders in layer layerIndex -
double ILDExVXD::getLadderWidth(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).Width;
}

/// The length of the ladder in z direction in mm for ladders in layer layerIndex
double ILDExVXD::getLadderLength(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_lVec.at(layerIndex).Length;
}
  
/// The radiation length in sensitive volumes in layer layerIndex
double ILDExVXD::getSensitiveRadLength(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_sVec.at(layerIndex).RadLength; 
}

/// The distance of sensitive area in ladders in layer layerIndex from the IP.
double ILDExVXD::getSensitiveDistance(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_sVec.at(layerIndex).Distance; 
}

/// The thickness in mm of the sensitive area in ladders in layer layerIndex.
double ILDExVXD::getSensitiveThickness(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_sVec.at(layerIndex).Thickness; 
}

/// Same as getLadderOffset() except for the sensitive part of the ladder.
double ILDExVXD::getSensitiveOffset(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_sVec.at(layerIndex).Offset; 
}

/// The width of the sensitive area in ladders in layer layerIndex in mm.
double ILDExVXD::getSensitiveWidth(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_sVec.at(layerIndex).Width; 
}

/// The length of the sensitive area in ladders in z direction in mm for ladders in layer layerIndex.
double ILDExVXD::getSensitiveLength(int layerIndex) const {
  const VXDData* obj = data<VXDData>();
  return obj->_sVec.at(layerIndex).Length; 
}
      

double ILDExVXD::getMaxRadius( int layerIndex, bool sensitive ) const   {
  const VXDData* obj = data<VXDData>();
  const VXDData::Layer& l = !sensitive ? obj->_lVec.at( layerIndex ) : obj->_sVec.at( layerIndex );
  float d = l.Distance + l.Thickness ;
  float w = l.Width / 2 + fabs( l.Offset ) ;
  double max = sqrt( d*d + w*w ) ;
  return max ;
}

// returns starting phi for first ladder in layer (on side to IP)
double ILDExVXD::getStartInnerPhi( int layerIndex , bool sensitive ) const    {
  const VXDData* obj = data<VXDData>();
  const VXDData::Layer& l = !sensitive ? obj->_lVec.at( layerIndex ) : obj->_sVec.at( layerIndex );
  return ( l.internalPhi0 + atan( (-l.Width /2 - l.Offset) / l.Distance) ) ;
}

// returns ending phi for first ladder in layer (on side to IP)
double ILDExVXD::getEndInnerPhi( int layerIndex , bool sensitive ) const   {
  const VXDData* obj = data<VXDData>();
  const VXDData::Layer& l = !sensitive ? obj->_lVec.at( layerIndex ) : obj->_sVec.at( layerIndex );
  return ( l.internalPhi0 + atan( (l.Width/2 - l.Offset)  / l.Distance ) ) ;
}

// returns starting phi for first ladder in layer (on side away from IP)
double ILDExVXD::getStartOuterPhi( int layerIndex , bool sensitive ) const   {
  const VXDData* obj = data<VXDData>();
  const VXDData::Layer& l = sensitive ? obj->_lVec.at( layerIndex ) : obj->_sVec.at( layerIndex );
  return ( l.internalPhi0 - atan( (l.Width /2 + l.Offset) / (l.Distance + l.Thickness) ) )  ;
}

// returns ending phi for first ladder in layer (on side away from IP)
double ILDExVXD::getEndOuterPhi( int layerIndex , bool sensitive ) const    {
  const VXDData* obj = data<VXDData>();
  const VXDData::Layer& l = sensitive ? obj->_lVec.at( layerIndex ) : obj->_sVec.at( layerIndex );
  return ( l.internalPhi0 + atan( (l.Width/2 - l.Offset)  / (l.Distance + l.Thickness) ) ) ;
}

// returns thickness under a certain angle
double ILDExVXD::getThicknessForPhi( int layerIndex , double phi, bool sensitive ) const  {
  const VXDData* obj = data<VXDData>();
  const VXDData::Layer& l = (!sensitive) ? obj->_lVec.at( layerIndex ) : obj->_sVec.at( layerIndex );
  double angularThickness ;

  // first check if layer is completely out of ladder
  if( phi < getStartInnerPhi( layerIndex , sensitive ) || phi > getEndInnerPhi( layerIndex, sensitive ) ) {
    return -1 ;
  }
  // check if angle is withhin outer boundaries - easy calculation then
  if( phi >= getStartOuterPhi( layerIndex , sensitive ) && phi <= getEndOuterPhi( layerIndex , sensitive ) ) {
    return ( l.Thickness / cos( phi ) ) ;
  }

  // the angel is in corner area:
  // calculate distance from space point to intersection point with straight line from IP under phi
  double distanceSpacePoint = l.Distance * tan( phi ) ;
  // calculate length of edge that is cut away on side facing IP
  double cutAwayLength = l.Width/2 - l.Offset - distanceSpacePoint ;
  // now one can calculate the angular thickness
  angularThickness = cutAwayLength / sin( phi ) ;
  return angularThickness ;
}    
      

