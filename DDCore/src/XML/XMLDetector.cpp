#include "XML/XMLDetector.h"
#include "xercesc/dom/DOM.hpp"

using namespace std;
using namespace DD4hep::XML;

double Dimension::x() const  {
  return m_element.attr<double>(Attr_x);
}

double Dimension::y() const  {
  return m_element.attr<double>(Attr_y);
}

double Dimension::z() const  {
  return m_element.attr<double>(Attr_z);
}

double Dimension::x(double default_val) const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_x);
  return val ? _toDouble(val) : default_val;
}

double Dimension::y(double default_val) const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_y);
  return val ? _toDouble(val) : default_val;
}

double Dimension::z(double default_val) const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_z);
  return val ? _toDouble(val) : default_val;
}

double Dimension::x1() const  {
  return m_element.attr<double>(Attr_x1);
}

double Dimension::x2() const  {
  return m_element.attr<double>(Attr_x2);
}

double Dimension::y1() const  {
  return m_element.attr<double>(Attr_y1);
}

double Dimension::y2() const  {
  return m_element.attr<double>(Attr_y2);
}

double Dimension::rmin() const  {
  return m_element.attr<double>(Attr_rmin);
}

double Dimension::zmin() const  {
  return m_element.attr<double>(Attr_zmin);
}

double Dimension::rmax() const  {
  return m_element.attr<double>(Attr_rmax);
}

double Dimension::deltaphi() const  {
  return m_element.attr<double>(Attr_deltaphi);
}

double Dimension::outer_z() const  {
  return m_element.attr<double>(Attr_outer_z);
}

double Dimension::outer_r() const  {
  return m_element.attr<double>(Attr_outer_r);
}

double Dimension::inner_z() const  {
  return m_element.attr<double>(Attr_inner_z);
}

double Dimension::inner_r() const  {
  return m_element.attr<double>(Attr_inner_r);
}

bool   Dimension::reflect() const  {
  return m_element.attr<bool>(Attr_reflect);
}

bool Dimension::reflect(bool default_value) const {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_reflect);
  return val ? _toBool(val) : default_value;
}

double Dimension::gap() const  {
  return m_element.attr<double>(Attr_gap);
}

double Dimension::z_length() const  {
  return m_element.attr<double>(Attr_z_length);
}

double Dimension::zhalf() const  {
  return m_element.attr<double>(Attr_zhalf);
}

double Dimension::length() const  {
  return m_element.attr<double>(Attr_length);
}

double Dimension::width() const  {
  return m_element.attr<double>(Attr_width);
}

int Dimension::numsides() const  {
  return m_element.attr<int>(Attr_numsides);
}

double Dimension::r_size() const  {
  return m_element.attr<int>(Attr_r_size);
}

double Dimension::phi_size_max() const  {
  return m_element.attr<int>(Attr_phi_size_max);
}

double Dimension::outgoing_r() const  {
  return m_element.attr<double>(Attr_outgoing_r);
}

double Dimension::incoming_r() const  {
  return m_element.attr<double>(Attr_incoming_r);
}

double Dimension::crossing_angle() const  {
  return m_element.attr<double>(Attr_crossing_angle);
}

double Dimension::radius() const {
  return m_element.attr<double>(Attr_radius);
}

double Dimension::offset(double default_value) const {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_offset);
  return val ? _toDouble(val) : default_value;
}

double Dimension::offset() const {
  return m_element.attr<double>(Attr_offset);
}

int Dimension::number() const {
  return m_element.attr<int>(Attr_number);
}

double Dimension::phi0() const {
  return m_element.attr<double>(Attr_phi0);
}

double Dimension::phi0(double default_value) const {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_phi0);
  return val ? _toDouble(val) : default_value;
}

double Dimension::phi_tilt() const {
  return m_element.attr<double>(Attr_phi_tilt);
}

int Dimension::nphi() const {
  return m_element.attr<int>(Attr_nphi);
}

double Dimension::rc()  const {
  return m_element.attr<double>(Attr_rc);
}

double Dimension::dr()  const {
  return m_element.attr<double>(Attr_dr);
}

double Dimension::dz()  const {
  return m_element.attr<double>(Attr_dz);
}

double Dimension::dz(double default_value) const {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_dz);
  return val ? _toDouble(val) : default_value;
}

double Dimension::r()  const {
  return m_element.attr<double>(Attr_r);
}

double Dimension::r(double default_value) const {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_r);
  return val ? _toDouble(val) : default_value;
}

double Dimension::z0() const {
  return m_element.attr<double>(Attr_z0);
}

int    Dimension::nz() const {
  return m_element.attr<int>(Attr_nz);
}

double Dimension::zstart() const {
  return m_element.attr<double>(Attr_zstart);
}

int DetElement::Component::id()  const  {
  return m_element.attr<int>(Attr_id);
}

int DetElement::Component::id(int default_value)  const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_id);
  return val ? _toDouble(val) : default_value;
}

string  DetElement::Component::nameStr()  const  {
  return m_element.attr<string>(Attr_name);
}

string DetElement::Component::materialStr() const   {
  return m_element.attr<string>(Attr_material);
}

string  DetElement::Component::moduleStr()  const  {
  return m_element.hasAttr(Attr_module) ? m_element.attr<string>(Attr_module) : string();
}

string  DetElement::Component::typeStr()  const  {
  return m_element.attr<string>(Attr_type);
}

int DetElement::Component::repeat()  const  {
  return m_element.attr<int>(Attr_repeat);
}

double DetElement::Component::thickness() const  {
  return m_element.attr<double>(Attr_thickness);
}

bool DetElement::Component::isSensitive() const  {
  return m_element.hasAttr(Attr_sensitive) && m_element.attr<bool>(Attr_sensitive);
}

string DetElement::Component::visStr()  const  {
  return m_element.hasAttr(Attr_vis) ? m_element.attr<string>(Attr_vis) : string();
}

string DetElement::Component::regionStr() const {
  return m_element.hasAttr(Attr_region) ? m_element.attr<string>(Attr_region) : string();
}

string DetElement::Component::limitsStr() const {
  return m_element.hasAttr(Attr_limits) ? m_element.attr<string>(Attr_limits) : string();
}

Dimension DetElement::Component::dimensions()  const  {
  return Dimension(m_element.child(Tag_dimensions));
}

int DetElement::id() const   {
  Attribute a = m_element.attr_nothrow(Attr_id);
  return a ? m_element.attr<int>(Attr_id) : -1;
}

string DetElement::nameStr() const   {
  return m_element.attr<string>(Attr_name);
}

string  DetElement::typeStr()  const  {
  return m_element.attr<string>(Attr_type);
}

string  DetElement::visStr()  const  {
  return m_element.attr<string>(Attr_vis);
}

Dimension DetElement::dimensions()  const  {
  return Dimension(m_element.child(Tag_dimensions));
}

string DetElement::materialStr() const  {
  Handle_t   h = m_element.child(Tag_material);
  if ( h && h.hasAttr(Attr_name) )  {
    return h.attr<string>(Attr_name);
  }
  return "";
}

string DetElement::regionStr() const {
  return m_element.hasAttr(Attr_region) ? m_element.attr<string>(Attr_region) : string();
}

string DetElement::limitsStr() const {
  return m_element.hasAttr(Attr_limits) ? m_element.attr<string>(Attr_limits) : string();
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

bool DetElement::isTracker() const   {
  if ( m_element )  {
    string type = attr<string>(Attr_type);
    if ( type.find("Tracker") != string::npos && hasAttr(Attr_readout) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const   {
  if ( m_element )  {
    string type = attr<string>(Attr_type);
    if ( type.find("Calorimeter") != string::npos && hasAttr(Attr_readout) ) {
        return true;
    }
  }
  return false;
}

bool DetElement::isInsideTrackingVolume() const  {
  if ( m_element && hasAttr(Attr_insideTrackingVolume) )
    return attr<bool>(Attr_insideTrackingVolume);
  else if ( isTracker() )
    return true;
  return false;
}
