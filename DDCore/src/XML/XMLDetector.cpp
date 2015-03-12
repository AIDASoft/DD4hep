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
#include "DD4hep/Plugins.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep::XML;

namespace Geometry { class LCDD; }

string Component::materialStr() const {
  return m_element.attr < string > (_U(material));
}

bool Component::isSensitive() const {
  char val = m_element.hasAttr(_U(sensitive)) ? m_element.attr < string > (_U(sensitive))[0] : 'f';
  val = ::toupper(val);
  return val == 'T' || val == 'Y';
}

DD4hep::NamedObject* Component::createShape() const {
  using namespace DD4hep::Geometry;
  Dimension child_dim(m_element);
  string typ = child_dim.typeStr();
  string fac = typ + "__shape_constructor";
  Handle_t solid_elt = m_element;
  LCDD* lcdd = 0;
  NamedObject* solid = PluginService::Create<NamedObject*>(fac, lcdd, &solid_elt);
  if ( !solid )  {
    PluginDebug dbg;
    PluginService::Create<NamedObject*>(typ, lcdd, &solid_elt);
    throw runtime_error("Failed to create solid of type " + typ + ". " + dbg.missingFactory(typ));
  }
  return solid;
}

int DetElement::id() const {
  return m_element.hasAttr(_U(id)) ? m_element.attr<int>(_U(id)) : -1;
}

bool DetElement::isSensitive() const {
  char val = m_element.hasAttr(_U(sensitive)) ? m_element.attr < string > (_U(sensitive))[0] : 'f';
  val = ::toupper(val);
  return val == 'T' || val == 'Y';
}

string DetElement::materialStr() const {
  Handle_t h = m_element.child(_U(material));
  if (h && h.hasAttr(_U(name))) {
    return h.attr < string > (_U(name));
  }
  return "";
}

void DetElement::check(bool condition, const string& msg) const {
  if (condition) {
    throw runtime_error(msg);
  }
}

bool DetElement::isTracker() const {
  if (m_element) {
    string typ = attr < string > (_U(type));
    if (typ.find("Tracker") != string::npos && hasAttr(_U(readout))) {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const {
  if (m_element) {
    string typ = attr < string > (_U(type));
    if (typ.find("Calorimeter") != string::npos && hasAttr(_U(readout))) {
      return true;
    }
  }
  return false;
}

bool DetElement::isInsideTrackingVolume() const {
  if (m_element && hasAttr(_U(insideTrackingVolume)) )
    return attr<bool>(_U(insideTrackingVolume));
  else if ( isTracker() )
    return true;
  return false;
}
