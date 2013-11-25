// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "XML/Conversions.h"
#include "DD4hep/DetFactoryHelper.h"

namespace DD4hep {
  struct Geant4;
  namespace Geometry {
    struct GdmlFile;
    struct Property;
    struct SensitiveDetector;
  }
}
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep {
  template <> void Converter<Geant4>::operator()(xml_h e) const;
  template <> void Converter<Geometry::GdmlFile>::operator()(xml_h e) const;
  template <> void Converter<Geometry::Property>::operator()(xml_h e) const;
  template <> void Converter<Geometry::SensitiveDetector>::operator()(xml_h e) const;
}

template <> void Converter<Geant4>::operator()(xml_h element) const {
  xml_elt_t compact(element);
  //xml_coll_t(compact,_U(includes) ).for_each(_U(gdmlFile), Converter<Geometry::GdmlFile>(lcdd,param));
  xml_coll_t(compact, _U(properties)).for_each(_U(attributes), Converter < Geometry::Property > (lcdd, param));
  xml_coll_t(compact, _U(sensitive_detectors)).for_each(_U(sd), Converter < Geometry::SensitiveDetector > (lcdd, param));
}

static long create_Geant4(lcdd_t& lcdd, const xml_h& element) {
  (Converter < Geant4 > (lcdd))(element);
  return 1;
}
DECLARE_XML_DOC_READER(geant4,create_Geant4);

static Ref_t handle_Geant4(lcdd_t& lcdd, const xml_h& element) {
  (Converter < Geant4 > (lcdd))(element);
  return Ref_t(0);
}
DECLARE_XMLELEMENT(geant4_xml_setup,handle_Geant4);

#include <iostream>
using namespace std;
struct Blabla;
namespace DD4hep {
template <> void Converter<Blabla>::operator()(xml_h element)  const  {
  xml_elt_t e(element);
  xml_h s = e.child(_Unicode(electronics_structure_thickness));
  double value = _toDouble(e.child(_Unicode(electronics_structure_thickness)).text());
  cout << s.tag() << " = " << s.text() << " --> double value:" << value << endl;
}
}
static long handle_blabla(lcdd_t& lcdd, const xml_h& element) {
  (Converter<Blabla>(lcdd))(element);
  return 1;
}
DECLARE_XML_DOC_READER(blabla,handle_blabla);
