// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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

// Framework include files
#include "XML/XMLChildValue.h"

using namespace std;
using namespace DD4hep::XML;

#define childValue(name,type)   m_element.child(Unicode_##name).attr<type>(Unicode_value)

#define childValueDefault(name,type,def)                                \
  Handle_t __h = m_element.child(Unicode_##name,false);                 \
  if ( __h.ptr() && __h.hasAttr(Unicode_value) ) return __h.attr < type > (Unicode_value); \
  return def;

#define XML_ATTR_ACCESSOR(type,name)  type ChildValue::name() const { return childValue(name,type); }
#define XML_ATTR_ACCESSOR_DEFAULT(name,type,dressing)             \
  type ChildValue::name(type default_val) const {                 \
    Handle_t __h = m_element.child(Unicode_##name,false);         \
    if ( __h.ptr() )  {                                           \
      const XmlChar* val = __h.attr_value_nothrow(Unicode_value); \
      return val ? dressing(val) : default_val; }                 \
    return default_val; }

#define XML_ATTR_ACCESSOR_DOUBLE(name) XML_ATTR_ACCESSOR_DEFAULT(name,double,_toDouble)
#define XML_ATTR_ACCESSOR_INT(name)    XML_ATTR_ACCESSOR_DEFAULT(name,int,_toInt)
#define XML_ATTR_ACCESSOR_BOOL(name)   XML_ATTR_ACCESSOR_DEFAULT(name,bool,_toBool)

XML_ATTR_ACCESSOR(int, id)
XML_ATTR_ACCESSOR_INT(id)
XML_ATTR_ACCESSOR(bool, combineHits)

XML_ATTR_ACCESSOR(double, x)
XML_ATTR_ACCESSOR_DOUBLE(x)
XML_ATTR_ACCESSOR(double, X)
XML_ATTR_ACCESSOR(double, dx)
XML_ATTR_ACCESSOR_DOUBLE(dx)
XML_ATTR_ACCESSOR(double, x0)
XML_ATTR_ACCESSOR(double, x1)
XML_ATTR_ACCESSOR(double, x2)
XML_ATTR_ACCESSOR(double, xmin)
XML_ATTR_ACCESSOR(double, xmax)
XML_ATTR_ACCESSOR(double, x_offset)
XML_ATTR_ACCESSOR(double, dim_x)

XML_ATTR_ACCESSOR(double, y)
XML_ATTR_ACCESSOR_DOUBLE(y)
XML_ATTR_ACCESSOR(double, Y)
XML_ATTR_ACCESSOR(double, dy)
XML_ATTR_ACCESSOR_DOUBLE(dy)
XML_ATTR_ACCESSOR(double, y0)
XML_ATTR_ACCESSOR(double, y1)
XML_ATTR_ACCESSOR(double, y2)
XML_ATTR_ACCESSOR(double, ymin)
XML_ATTR_ACCESSOR(double, ymax)
XML_ATTR_ACCESSOR(double, y_offset)
XML_ATTR_ACCESSOR(double, dim_y)

XML_ATTR_ACCESSOR(double, z)
XML_ATTR_ACCESSOR_DOUBLE(z)
XML_ATTR_ACCESSOR(double, Z)
XML_ATTR_ACCESSOR(double, dz)
XML_ATTR_ACCESSOR_DOUBLE(dz)
XML_ATTR_ACCESSOR(double, z0)
XML_ATTR_ACCESSOR(double, z1)
XML_ATTR_ACCESSOR(double, z2)
XML_ATTR_ACCESSOR(double, zmin)
XML_ATTR_ACCESSOR(double, zmax)
XML_ATTR_ACCESSOR(double, z_offset)
XML_ATTR_ACCESSOR(double, dim_z)
XML_ATTR_ACCESSOR(double, outer_z)
XML_ATTR_ACCESSOR(double, inner_z)

XML_ATTR_ACCESSOR(double, b)
XML_ATTR_ACCESSOR(double, g)
XML_ATTR_ACCESSOR(double, B)
XML_ATTR_ACCESSOR(double, G)
XML_ATTR_ACCESSOR(double, r)
XML_ATTR_ACCESSOR_DOUBLE(r)
XML_ATTR_ACCESSOR(double, R)
XML_ATTR_ACCESSOR(double, dr)
XML_ATTR_ACCESSOR(double, rmin)
XML_ATTR_ACCESSOR(double, rmax)
XML_ATTR_ACCESSOR(double, rmin1)
XML_ATTR_ACCESSOR(double, rmax1)
XML_ATTR_ACCESSOR(double, rmin2)
XML_ATTR_ACCESSOR(double, rmax2)
XML_ATTR_ACCESSOR(double, radius)
XML_ATTR_ACCESSOR(double, outer_r)
XML_ATTR_ACCESSOR(double, distance)
XML_ATTR_ACCESSOR(double, outer_radius)
XML_ATTR_ACCESSOR(double, inner_r)
XML_ATTR_ACCESSOR(double, inner_radius)

XML_ATTR_ACCESSOR(double, angle)
XML_ATTR_ACCESSOR(double, alpha)
XML_ATTR_ACCESSOR(double, beta)
XML_ATTR_ACCESSOR(double, gamma)
XML_ATTR_ACCESSOR(double, delta)
XML_ATTR_ACCESSOR(double, epsilon)
XML_ATTR_ACCESSOR(double, theta)
XML_ATTR_ACCESSOR(int, thetaBins)
XML_ATTR_ACCESSOR(double, psi)
XML_ATTR_ACCESSOR(double, phi)
XML_ATTR_ACCESSOR(int, phiBins)
XML_ATTR_ACCESSOR(double, phi0)
XML_ATTR_ACCESSOR(double, phi0_offset)
XML_ATTR_ACCESSOR_DOUBLE(phi0)
XML_ATTR_ACCESSOR(double, phi1)
XML_ATTR_ACCESSOR_DOUBLE(phi1)
XML_ATTR_ACCESSOR(double, deltaphi)

XML_ATTR_ACCESSOR(double, length)
XML_ATTR_ACCESSOR(double, width)
XML_ATTR_ACCESSOR(double, height)
XML_ATTR_ACCESSOR(double, depth)
XML_ATTR_ACCESSOR(double, offset)
XML_ATTR_ACCESSOR_DOUBLE(offset)
XML_ATTR_ACCESSOR(double, crossing_angle)
XML_ATTR_ACCESSOR(double, incoming_r)
XML_ATTR_ACCESSOR(double, outgoing_r)
XML_ATTR_ACCESSOR(double, phi_size_max)
XML_ATTR_ACCESSOR(double, r_size)

XML_ATTR_ACCESSOR(double, gap)
XML_ATTR_ACCESSOR(double, z_length)
XML_ATTR_ACCESSOR(double, zhalf)
XML_ATTR_ACCESSOR(double, phi_tilt)
XML_ATTR_ACCESSOR(int, nphi)
XML_ATTR_ACCESSOR(double, rc)
XML_ATTR_ACCESSOR(int, nz)
XML_ATTR_ACCESSOR(double, zstart)
XML_ATTR_ACCESSOR(double, start)
XML_ATTR_ACCESSOR(double, end)
XML_ATTR_ACCESSOR(double, thickness)
XML_ATTR_ACCESSOR(int, numsides)
XML_ATTR_ACCESSOR(int, nsides)
XML_ATTR_ACCESSOR(int, nsides_inner)
XML_ATTR_ACCESSOR(int, nsides_outer)
XML_ATTR_ACCESSOR(int, number)
XML_ATTR_ACCESSOR(int, repeat)
XML_ATTR_ACCESSOR(bool, reflect)
XML_ATTR_ACCESSOR_BOOL(reflect)

XML_ATTR_ACCESSOR(int, nmodules)
XML_ATTR_ACCESSOR(int, nModules)
XML_ATTR_ACCESSOR(int, RowID)
XML_ATTR_ACCESSOR(int, nPads)
XML_ATTR_ACCESSOR(double, moduleHeight)
XML_ATTR_ACCESSOR(double, moduleWidth)
XML_ATTR_ACCESSOR(double, modulePosX)
XML_ATTR_ACCESSOR(double, modulePosY)
XML_ATTR_ACCESSOR(double, modulePitch)
XML_ATTR_ACCESSOR(double, rowPitch)
XML_ATTR_ACCESSOR(double, padPitch)
XML_ATTR_ACCESSOR(double, rowHeight)
XML_ATTR_ACCESSOR(double, inner_field)
XML_ATTR_ACCESSOR(double, outer_field)
XML_ATTR_ACCESSOR(int, type)

XML_ATTR_ACCESSOR(bool, visible)
XML_ATTR_ACCESSOR(bool, show_daughters)
#if 0
XML_ATTR_ACCESSOR(double,)
XML_ATTR_ACCESSOR(double,)
XML_ATTR_ACCESSOR(double,)
XML_ATTR_ACCESSOR(double,)
#endif


string ChildValue::padType() const {
  return childValue(pads,string);
}

string ChildValue::nameStr() const {
  return childValue(name,string);
}

string ChildValue::refStr() const {
  return childValue(ref,string);
}

string ChildValue::typeStr() const {
  return childValue(type,string);
}

/// Access "value" attribute as STL string
std::string ChildValue::valueStr() const   {
  return childValue(value,string);
}

string ChildValue::regionStr() const {
  childValueDefault(region,string,string());
}

string ChildValue::limitsStr() const {
  childValueDefault(limits,string,string());
}

string ChildValue::visStr() const {
  childValueDefault(vis,string,string());
}

string ChildValue::readoutStr() const {
  childValueDefault(readout,string,string());
}

string ChildValue::moduleStr() const {
  childValueDefault(module,string,string());
}
