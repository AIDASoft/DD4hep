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

// Framework includes
#include "DD4hep/FieldTypes.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace dd4hep;

static Ref_t convert_constant_field(Detector&, xml_h field, Ref_t object) {
  xml_doc_t doc = xml_elt_t(field).document();
  ConstantField* fld = object.data<ConstantField>();
  field.setAttr(_U(lunit), "mm");
  //field.setAttr(_U(funit),"tesla");
  if (fld->type == CartesianField::ELECTRIC)
    field.setAttr(_U(field), "electric");
  else if (fld->type == CartesianField::MAGNETIC)
    field.setAttr(_U(field), "magnetic");

  xml_elt_t strength = xml_elt_t(doc, _U(strength));
  strength.setAttr(_U(x), fld->direction.X());
  strength.setAttr(_U(y), fld->direction.Y());
  strength.setAttr(_U(z), fld->direction.Z());
  field.append(strength);
  return object;
}
DECLARE_XML_PROCESSOR(ConstantField_Convert2Detector,convert_constant_field)

static Ref_t convert_solenoid(Detector&, xml_h field, Ref_t object) {
  char text[128];
  SolenoidField* fld = object.data<SolenoidField>();
  field.setAttr(_U(lunit), "mm");
  field.setAttr(_U(funit), "tesla");
  ::snprintf(text, sizeof(text), "%g/mm", fld->outerRadius);
  field.setAttr(_U(outer_radius), dd4hep::_toDouble(text));
  ::snprintf(text, sizeof(text), "%g/mm", fld->innerRadius);
  field.setAttr(_U(inner_radius), dd4hep::_toDouble(text));
  ::snprintf(text, sizeof(text), "%g/tesla", fld->innerField);
  field.setAttr(_U(inner_field), dd4hep::_toDouble(text));
  ::snprintf(text, sizeof(text), "%g/tesla", fld->outerField);
  field.setAttr(_U(outer_field), dd4hep::_toDouble(text));
  field.setAttr(_U(zmin), fld->minZ);
  field.setAttr(_U(zmax), fld->maxZ);
  return object;
}
DECLARE_XML_PROCESSOR(solenoid_Convert2Detector,convert_solenoid)

static Ref_t convert_dipole(Detector&, xml_h field, Ref_t object) {
  char text[128];
  xml_doc_t doc = xml_elt_t(field).document();
  DipoleField* fld = object.data<DipoleField>();
  field.setAttr(_U(lunit), "mm");
  field.setAttr(_U(funit), "tesla");
  ::snprintf(text, sizeof(text), "%g/mm", fld->rmax);
  field.setAttr(_U(rmax), dd4hep::_toDouble(text));
  ::snprintf(text, sizeof(text), "%g/mm", fld->zmax);
  field.setAttr(_U(zmax), dd4hep::_toDouble(text));
  ::snprintf(text, sizeof(text), "%g/mm", fld->zmin);
  field.setAttr(_U(zmin), dd4hep::_toDouble(text));
  for (auto c : fld->coefficents )  {
    xml_elt_t coeff = xml_elt_t(doc, _U(dipole_coeff));
    coeff.setValue(dd4hep::_toString(c));
    field.append(coeff);
  }
  return object;
}
DECLARE_XML_PROCESSOR(DipoleField_Convert2Detector,convert_dipole)

