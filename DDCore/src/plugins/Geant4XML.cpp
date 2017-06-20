//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#include "XML/Conversions.h"
#include "DD4hep/DetFactoryHelper.h"

namespace dd4hep {
  struct Geant4;
  class GdmlFile;
  class Property;
  class SensitiveDetector;
}
using namespace dd4hep;

namespace dd4hep {
  template <> void Converter<Geant4>::operator()(xml_h e) const;
  template <> void Converter<GdmlFile>::operator()(xml_h e) const;
  template <> void Converter<Property>::operator()(xml_h e) const;
  template <> void Converter<SensitiveDetector>::operator()(xml_h e) const;
}

template <> void Converter<Geant4>::operator()(xml_h element) const {
  xml_elt_t compact(element);
  //xml_coll_t(compact,_U(includes) ).for_each(_U(gdmlFile), Converter<GdmlFile>(description,param));
  xml_coll_t(compact, _U(properties)).for_each(_U(attributes), Converter < Property > (description, param));
  xml_coll_t(compact, _U(sensitive_detectors)).for_each(_U(sd), Converter < SensitiveDetector > (description, param));
}

static long create_Geant4(Detector& description, const xml_h& element) {
  (Converter < Geant4 > (description))(element);
  return 1;
}
DECLARE_XML_DOC_READER(geant4,create_Geant4)

static Ref_t handle_Geant4(Detector& description, const xml_h& element) {
  (Converter < Geant4 > (description))(element);
  return Ref_t(0);
}
DECLARE_XMLELEMENT(geant4_xml_setup,handle_Geant4)
