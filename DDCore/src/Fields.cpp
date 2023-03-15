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

#include "DD4hep/Fields.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/Handle.inl"

using namespace std;
using namespace dd4hep;

typedef CartesianField::Object CartesianFieldObject;
DD4HEP_INSTANTIATE_HANDLE(CartesianFieldObject);

typedef OverlayedField::Object OverlayedFieldObject;
DD4HEP_INSTANTIATE_HANDLE(OverlayedFieldObject);

namespace {
  void calculate_combined_field(vector<CartesianField>& v, const Position& pos, double* field) {
    for (const auto& i : v ) i.value(pos, field);
  }
}

/// Default constructor
CartesianField::Object::Object() : TypedObject()  {
  field_type = UNKNOWN;
  InstanceCount::increment(this);
}

/// Default destructor
CartesianField::Object::~Object() {
  InstanceCount::decrement(this);
}

/// Access the field type (string)
const char* CartesianField::type() const {
  return m_element->GetTitle();
}

/// Does the field change the energy of charged particles?
bool CartesianField::changesEnergy() const {
  return ELECTRIC == (fieldType() & ELECTRIC);
}

/// Access to properties container
CartesianField::Properties& CartesianField::properties() const {
  return data<Object>()->properties;
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const Position& pos, Direction& field) const  {
  double fld[3] = {0e0, 0e0, 0e0};
  double position[3] = {pos.X(), pos.Y(), pos.Z()};
  data<Object>()->fieldComponents(position, fld);
  field = Direction(fld[0], fld[1], fld[2]);
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const Position& pos, double* field) const   {
  double position[3] = {pos.X(), pos.Y(), pos.Z()};
  data<Object>()->fieldComponents(position, field);
}

/// Returns the 3 field components (x, y, z).
void CartesianField::value(const double* pos, double* field) const {
  data<Object>()->fieldComponents(pos, field);
}

/// Default constructor
OverlayedField::Object::Object() : TypedObject(), electric(), magnetic()
{
  field_type = CartesianField::OVERLAY;
  InstanceCount::increment(this);
}

/// Default destructor
OverlayedField::Object::~Object() {
  InstanceCount::decrement(this);
}

/// Object constructor
OverlayedField::OverlayedField(const string& nam) : Ref_t() {
  auto* obj = new Object();
  assign(obj, nam, "overlay_field");
  obj->field_type = CartesianField::OVERLAY;
}

/// Access to properties container
OverlayedField::Properties& OverlayedField::properties() const {
  return data<Object>()->properties;
}

/// Does the field change the energy of charged particles?
bool OverlayedField::changesEnergy() const {
  int field = data<Object>()->field_type;
  return CartesianField::ELECTRIC == (field & CartesianField::ELECTRIC);
}

/// Add a new field component
void OverlayedField::add(CartesianField field) {
  if (field.isValid()) {
    Object* o = data<Object>();
    if ( o ) {
      int  typ   = field.fieldType();
      bool isEle = field.ELECTRIC == (typ & field.ELECTRIC);
      bool isMag = field.MAGNETIC == (typ & field.MAGNETIC);
      if (isEle) {
        vector < CartesianField > &v = o->electric_components;
        v.emplace_back(field);
        o->field_type |= field.ELECTRIC;
        o->electric = (v.size() == 1) ? field : CartesianField();
      }
      if (isMag) {
        vector < CartesianField > &v = o->magnetic_components;
        v.emplace_back(field);
        o->field_type |= field.MAGNETIC;
        o->magnetic = (v.size() == 1) ? field : CartesianField();
      }
      if (isMag || isEle)
        return;
      except("OverlayedField","add: Attempt to add an unknown field type.");
    }
    except("OverlayedField","add: Attempt to add an invalid object.");
  }
  except("OverlayedField","add: Attempt to add an invalid field.");
}

/// Returns the 3  magnetic field components (x, y, z).
void OverlayedField::magneticField(const Position& pos, double* field) const   {
  if ( isValid() )   {
    field[0] = field[1] = field[2] = 0.0;
    auto* obj = data<Object>();
    CartesianField f = obj->magnetic;
    if ( f.isValid() )
      f.value(pos, field);
    else
      calculate_combined_field(obj->magnetic_components, pos, field);
    return;
  }
  except("OverlayedField","add: Attempt to add an invalid field.");
}

/// Returns the 3 electric field components (x, y, z).
void OverlayedField::combinedElectric(const Position& pos, double* field) const {
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(data<Object>()->electric_components, pos, field);
}

/// Returns the 3  magnetic field components (x, y, z).
void OverlayedField::combinedMagnetic(const Position& pos, double* field) const {
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(data<Object>()->magnetic_components, pos, field);
}

/// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
void OverlayedField::electromagneticField(const Position& pos, double* field) const {
  Object* o = data<Object>();
  field[0] = field[1] = field[2] = 0.;
  calculate_combined_field(o->electric_components, pos, field);
  calculate_combined_field(o->magnetic_components, pos, field + 3);
}
