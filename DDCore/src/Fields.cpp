// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Fields.h"

using namespace std;
using namespace DD4hep::Geometry;

namespace {
  void calculate_combined_field(vector<CartesianField>& v, const double* pos, double* field) {
    for(vector<CartesianField>::iterator i=v.begin(); i!=v.end(); ++i)
      (*i).value(pos,field);
  }
}

/// Access the field type (string)
const char* CartesianField::type() const {
  return m_element->GetTitle();
}

/// Does the field change the energy of charged particles?
bool CartesianField::changesEnergy() const {
  return ELECTRIC==(fieldType()&ELECTRIC);
}

/// Access to properties container
CartesianField::Properties& CartesianField::properties()  const   {
  return data<Object>()->properties; 
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const double* pos, double* val)  const    { 
  data<Object>()->fieldComponents(pos,val); 
}

/// Object constructor
OverlayedField::OverlayedField(const string& name) : Ref_t() {
  Value<TNamed,Object>* ptr = new Value<TNamed,Object>();
  assign(ptr,name,"overlay_field");
}

/// Access to properties container
OverlayedField::Properties& OverlayedField::properties()  const   {
  return data<Object>()->properties; 
}

/// Does the field change the energy of charged particles?
bool OverlayedField::changesEnergy() const {
  int field = data<Object>()->type;
  return CartesianField::ELECTRIC==(field&CartesianField::ELECTRIC);
}

/// Add a new field component
void OverlayedField::add(CartesianField field) {
  if ( field.isValid() ) {
    Object* o = data<Object>();
    if ( o ) {
      int typ = field.fieldType();
      bool isEle = field.ELECTRIC==(typ&field.ELECTRIC);
      bool isMag = field.MAGNETIC==(typ&field.MAGNETIC);
      if ( isEle ) {
	vector<CartesianField>& v = o->electric_components;
	v.push_back(field);
	o->type |= field.ELECTRIC;
	o->electric = v.size()==1 ? field : CartesianField();
      }
      if ( isMag ) {
	vector<CartesianField>& v = o->magnetic_components;
	v.push_back(field);
	o->type |= field.MAGNETIC;
	o->magnetic = v.size()==1 ? field : CartesianField();
      }
      if ( isMag ||isEle ) return;
      throw runtime_error("OverlayedField::add: Attempt to add an unknown field type.");
    }
    throw runtime_error("OverlayedField::add: Attempt to add to an invalid object.");
  }
  throw runtime_error("OverlayedField::add: Attempt to add an invalid field.");
}

/// Returns the 3 electric field components (x, y, z).
void OverlayedField::combinedElectric(const double* pos, double* field)   const {
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(data<Object>()->electric_components,pos,field);
}

/// Returns the 3  magnetic field components (x, y, z).
void OverlayedField::combinedMagnetic(const double* pos, double* field)   const {
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(data<Object>()->magnetic_components,pos,field);
}

/// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
void OverlayedField::electromagneticField(const double* pos, double* field)   const {
  Object* o = data<Object>();
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(o->electric_components,pos,field);
  calculate_combined_field(o->magnetic_components,pos,field+3);
}
