// $Id:$
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
  template <> void Converter<Geant4>::operator()(const xml_h& e)  const;
  template <> void Converter<Geometry::GdmlFile>::operator()(const xml_h& e)  const;
  template <> void Converter<Geometry::Property>::operator()(const xml_h& e)  const;
  template <> void Converter<Geometry::SensitiveDetector>::operator()(const xml_h& e)  const;
}

template <> void Converter<Geant4>::operator()(const xml_h& element)  const  {
  xml_elt_t compact(element);
  //xml_coll_t(compact,_X(includes) ).for_each(_X(gdmlFile), Converter<Geometry::GdmlFile>(lcdd,param));
  xml_coll_t(compact,_X(properties) ).for_each(_X(attributes),Converter<Geometry::Property>(lcdd,param));
  xml_coll_t(compact,_X(sensitive_detectors)).for_each(_X(sd),Converter<Geometry::SensitiveDetector>(lcdd,param));
}

static long create_Geant4(lcdd_t& lcdd, const xml_h& element) {
  (Converter<Geant4>(lcdd))(element);
  return 1;
}
DECLARE_XML_DOC_READER(geant4,create_Geant4);
