// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "XML/XMLDetector.h"

using namespace std;
using namespace DD4hep::XML;

string Component::materialStr() const   {
  return m_element.attr<string>(_U(material));
}

bool Component::isSensitive() const  {
  char val = m_element.hasAttr(_U(sensitive)) ? m_element.attr<string>(_U(sensitive))[0] : 'f';
  val = ::toupper(val);
  return val == 'T' || val == 'Y';
}

int DetElement::id() const   {
  return m_element.hasAttr(_U(id)) ? m_element.attr<int>(_U(id)) : -1;
}

string DetElement::materialStr() const  {
  Handle_t h = m_element.child(_U(material));
  if ( h && h.hasAttr(_U(name)) )  {
    return h.attr<string>(_U(name));
  }
  return "";
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

bool DetElement::isTracker() const   {
  if ( m_element )  {
    string type = attr<string>(_U(type));
    if ( type.find("Tracker") != string::npos && hasAttr(_U(readout)) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const   {
  if ( m_element )  {
    string type = attr<string>(_U(type));
    if ( type.find("Calorimeter") != string::npos && hasAttr(_U(readout)) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isInsideTrackingVolume() const  {
  if ( m_element && hasAttr(_U(insideTrackingVolume)) )
    return attr<bool>(_U(insideTrackingVolume));
  else if ( isTracker() )
    return true;
  return false;
}
