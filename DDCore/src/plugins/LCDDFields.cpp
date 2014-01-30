// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework includes
#include "DD4hep/FieldTypes.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace DD4hep::Geometry;

static Ref_t convert_constant_field(LCDD&, xml_h field, Ref_t object) {
  xml_doc_t doc = xml_elt_t(field).document();
  ConstantField* s = object.data<ConstantField>();
  field.setAttr(_U(lunit), "mm");
  //field.setAttr(_U(funit),"tesla");
  if (s->type == CartesianField::ELECTRIC)
    field.setAttr(_U(field), "electric");
  else if (s->type == CartesianField::MAGNETIC)
    field.setAttr(_U(field), "magnetic");

  xml_elt_t strength = xml_elt_t(doc, _U(strength));
  strength.setAttr(_U(x), s->direction.X());
  strength.setAttr(_U(y), s->direction.Y());
  strength.setAttr(_U(z), s->direction.Z());
  field.append(strength);
  return object;
}
DECLARE_XML_PROCESSOR(ConstantField_Convert2LCDD,convert_constant_field)

static Ref_t convert_solenoid(LCDD&, xml_h field, Ref_t object) {
  char text[128];
  SolenoidField* s = object.data<SolenoidField>();
  field.setAttr(_U(lunit), "mm");
  field.setAttr(_U(funit), "tesla");
  ::snprintf(text, sizeof(text), "%g/mm", s->outerRadius);
  field.setAttr(_U(outer_radius), _toDouble(text));
  ::snprintf(text, sizeof(text), "%g/mm", s->innerRadius);
  field.setAttr(_U(inner_radius), _toDouble(text));
  ::snprintf(text, sizeof(text), "%g/tesla", s->innerField);
  field.setAttr(_U(inner_field), _toDouble(text));
  ::snprintf(text, sizeof(text), "%g/tesla", s->outerField);
  field.setAttr(_U(outer_field), _toDouble(text));
  field.setAttr(_U(zmin), s->minZ);
  field.setAttr(_U(zmax), s->maxZ);
  return object;
}
DECLARE_XML_PROCESSOR(solenoid_Convert2LCDD,convert_solenoid)

static Ref_t convert_dipole(LCDD&, xml_h field, Ref_t object) {
  char text[128];
  xml_doc_t doc = xml_elt_t(field).document();
  DipoleField* s = object.data<DipoleField>();
  field.setAttr(_U(lunit), "mm");
  field.setAttr(_U(funit), "tesla");
  ::snprintf(text, sizeof(text), "%g/mm", s->rmax);
  field.setAttr(_U(rmax), _toDouble(text));
  ::snprintf(text, sizeof(text), "%g/mm", s->zmax);
  field.setAttr(_U(zmax), _toDouble(text));
  ::snprintf(text, sizeof(text), "%g/mm", s->zmin);
  field.setAttr(_U(zmin), _toDouble(text));
  DipoleField::Coefficents::const_iterator i = s->coefficents.begin();
  for (; i != s->coefficents.end(); ++i) {
    xml_elt_t coeff = xml_elt_t(doc, _U(dipole_coeff));
    coeff.setValue(_toString(*i));
    field.append(coeff);
  }
  return object;
}
DECLARE_XML_PROCESSOR(DipoleField_Convert2LCDD,convert_dipole)

