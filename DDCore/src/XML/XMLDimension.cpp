// $Id: XMLElements.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "XML/XMLDimension.h"

using namespace std;
using namespace DD4hep::XML;

#define XML_ATTR_ACCESSOR(type,name)  type Dimension::name() const { return m_element.attr<type>(Unicode_##name); }
#define XML_ATTR_ACCESSOR_DEFAULT(name,type,dressing)                   \
  type Dimension::name(type default_val) const {                        \
    const XmlChar* val = m_element.attr_value_nothrow(Unicode_##name);  \
    return val ? dressing(val) : default_val; }

#define XML_ATTR_ACCESSOR_DOUBLE(name)                  \
  XML_ATTR_ACCESSOR(double,name)                        \
  XML_ATTR_ACCESSOR_DEFAULT(name,double,_toDouble)

#define XML_ATTR_ACCESSOR_INT(name)      XML_ATTR_ACCESSOR_DEFAULT(name,int,_toInt)
#define XML_ATTR_ACCESSOR_BOOL(name)     XML_ATTR_ACCESSOR_DEFAULT(name,bool,_toBool)

#define XML_CHILD_ACCESSOR_XML_DIM(name)                                \
  Dimension Dimension::name(bool throw_if_not_present) const {          \
    return m_element.child(Unicode_##name,throw_if_not_present); }

XML_ATTR_ACCESSOR(int, id)
XML_ATTR_ACCESSOR_INT(id)
XML_ATTR_ACCESSOR(bool, combineHits)

XML_ATTR_ACCESSOR_DOUBLE(x)
XML_ATTR_ACCESSOR_DOUBLE(X)
XML_ATTR_ACCESSOR_DOUBLE(dx)

XML_ATTR_ACCESSOR_DOUBLE(x0)
XML_ATTR_ACCESSOR_DOUBLE(x1)
XML_ATTR_ACCESSOR_DOUBLE(x2)
XML_ATTR_ACCESSOR_DOUBLE(x3)
XML_ATTR_ACCESSOR_DOUBLE(x4)
XML_ATTR_ACCESSOR_DOUBLE(xmin)
XML_ATTR_ACCESSOR_DOUBLE(xmax)
XML_ATTR_ACCESSOR_DOUBLE(x_offset)
XML_ATTR_ACCESSOR_DOUBLE(dim_x)

XML_ATTR_ACCESSOR_DOUBLE(y)
XML_ATTR_ACCESSOR_DOUBLE(Y)
XML_ATTR_ACCESSOR_DOUBLE(dy)
XML_ATTR_ACCESSOR_DOUBLE(y0)
XML_ATTR_ACCESSOR_DOUBLE(y1)
XML_ATTR_ACCESSOR_DOUBLE(y2)
XML_ATTR_ACCESSOR_DOUBLE(ymin)
XML_ATTR_ACCESSOR_DOUBLE(ymax)
XML_ATTR_ACCESSOR_DOUBLE(y_offset)
XML_ATTR_ACCESSOR_DOUBLE(dim_y)

XML_ATTR_ACCESSOR_DOUBLE(z)
XML_ATTR_ACCESSOR_DOUBLE(Z)
XML_ATTR_ACCESSOR_DOUBLE(dz)
XML_ATTR_ACCESSOR_DOUBLE(z0)
XML_ATTR_ACCESSOR_DOUBLE(z1)
XML_ATTR_ACCESSOR_DOUBLE(z2)
XML_ATTR_ACCESSOR_DOUBLE(zmin)
XML_ATTR_ACCESSOR_DOUBLE(zmax)
XML_ATTR_ACCESSOR_DOUBLE(z_offset)
XML_ATTR_ACCESSOR_DOUBLE(dim_z)
XML_ATTR_ACCESSOR(double, outer_z)
XML_ATTR_ACCESSOR(double, inner_z)

XML_ATTR_ACCESSOR_DOUBLE(a)
XML_ATTR_ACCESSOR_DOUBLE(b)
XML_ATTR_ACCESSOR_DOUBLE(c)
XML_ATTR_ACCESSOR(double, distance)
XML_ATTR_ACCESSOR(double, g)
XML_ATTR_ACCESSOR(double, B)
XML_ATTR_ACCESSOR(double, G)

XML_ATTR_ACCESSOR_DOUBLE(r)
XML_ATTR_ACCESSOR_DOUBLE(R)
XML_ATTR_ACCESSOR_DOUBLE(dr)

XML_ATTR_ACCESSOR_DOUBLE(rmin)
XML_ATTR_ACCESSOR_DOUBLE(rmax)
XML_ATTR_ACCESSOR_DOUBLE(rmin1)
XML_ATTR_ACCESSOR_DOUBLE(rmax1)
XML_ATTR_ACCESSOR_DOUBLE(rmin2)
XML_ATTR_ACCESSOR_DOUBLE(rmax2)
XML_ATTR_ACCESSOR_DOUBLE(radius)
XML_ATTR_ACCESSOR(double, outer_r)
XML_ATTR_ACCESSOR(double, outer_radius)
XML_ATTR_ACCESSOR(double, inner_r)
XML_ATTR_ACCESSOR(double, inner_radius)

XML_ATTR_ACCESSOR(double, angle)
XML_ATTR_ACCESSOR_DOUBLE(alpha)
XML_ATTR_ACCESSOR_DOUBLE(alpha1)
XML_ATTR_ACCESSOR_DOUBLE(alpha2)
XML_ATTR_ACCESSOR_DOUBLE(beta)
XML_ATTR_ACCESSOR_DOUBLE(gamma)
XML_ATTR_ACCESSOR_DOUBLE(delta)
XML_ATTR_ACCESSOR_DOUBLE(epsilon)
XML_ATTR_ACCESSOR_DOUBLE(theta)
XML_ATTR_ACCESSOR_DOUBLE(deltatheta)
XML_ATTR_ACCESSOR(int, thetaBins)
XML_ATTR_ACCESSOR_DOUBLE(psi)
XML_ATTR_ACCESSOR_DOUBLE(phi)
XML_ATTR_ACCESSOR(int, phiBins)
XML_ATTR_ACCESSOR_DOUBLE(phi0)
XML_ATTR_ACCESSOR_DOUBLE(phi1)
XML_ATTR_ACCESSOR_DOUBLE(deltaphi)
XML_ATTR_ACCESSOR_DOUBLE(startphi)

XML_ATTR_ACCESSOR(double, length)
XML_ATTR_ACCESSOR(double, width)
XML_ATTR_ACCESSOR(double, height)
XML_ATTR_ACCESSOR(double, depth)
XML_ATTR_ACCESSOR_DOUBLE(offset)
XML_ATTR_ACCESSOR(double, crossing_angle)
XML_ATTR_ACCESSOR(double, incoming_r)
XML_ATTR_ACCESSOR(double, outgoing_r)
XML_ATTR_ACCESSOR(double, phi_size_max)
XML_ATTR_ACCESSOR(double, r_size)

XML_ATTR_ACCESSOR_DOUBLE(skew)
XML_ATTR_ACCESSOR_DOUBLE(coefficient)

XML_ATTR_ACCESSOR(double, gap)
XML_ATTR_ACCESSOR(double, z_length)
XML_ATTR_ACCESSOR(double, zhalf)
XML_ATTR_ACCESSOR(double, phi_tilt)
XML_ATTR_ACCESSOR(int, nphi)
XML_ATTR_ACCESSOR(double, rc)
XML_ATTR_ACCESSOR(int, nz)
XML_ATTR_ACCESSOR(double, zstart)
XML_ATTR_ACCESSOR_DOUBLE(start)
XML_ATTR_ACCESSOR_DOUBLE(end)
XML_ATTR_ACCESSOR(double, thickness)
XML_ATTR_ACCESSOR(double, inner_stereo)
XML_ATTR_ACCESSOR(double, outer_stereo)
XML_ATTR_ACCESSOR(int, numsides)
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

XML_ATTR_ACCESSOR_DOUBLE(cut)
XML_ATTR_ACCESSOR_DOUBLE(threshold)
XML_ATTR_ACCESSOR_DOUBLE(lunit)
XML_ATTR_ACCESSOR_DOUBLE(eunit)

#if 0
XML_ATTR_ACCESSOR(double,)
XML_ATTR_ACCESSOR(double,)
XML_ATTR_ACCESSOR(double,)
XML_ATTR_ACCESSOR(double,)
#endif

XML_CHILD_ACCESSOR_XML_DIM(dimensions)
XML_CHILD_ACCESSOR_XML_DIM(position)
XML_CHILD_ACCESSOR_XML_DIM(rotation)
XML_CHILD_ACCESSOR_XML_DIM(trd)
XML_CHILD_ACCESSOR_XML_DIM(tubs)
XML_CHILD_ACCESSOR_XML_DIM(staves)
XML_CHILD_ACCESSOR_XML_DIM(beampipe)

string Dimension::padType() const {
  return m_element.attr < string > (_U(pads));
}

string Dimension::nameStr() const {
  return m_element.attr < string > (_U(name));
}

string Dimension::refStr() const {
  return m_element.attr < string > (_U(ref));
}

string Dimension::typeStr() const {
  return m_element.attr < string > (_U(type));
}

/// Access "value" attribute as STL string
std::string Dimension::valueStr() const   {
  return m_element.attr < string > (_U(value));
}

string Dimension::regionStr() const {
  return m_element.hasAttr(_U(region)) ? m_element.attr < string > (_U(region)) : string();
}

string Dimension::limitsStr() const {
  return m_element.hasAttr(_U(limits)) ? m_element.attr < string > (_U(limits)) : string();
}

string Dimension::visStr() const {
  return m_element.hasAttr(_U(vis)) ? m_element.attr < string > (_U(vis)) : string();
}

string Dimension::readoutStr() const {
  return m_element.hasAttr(_U(readout)) ? m_element.attr < string > (_U(readout)) : string();
}

string Dimension::moduleStr() const {
  return m_element.hasAttr(_U(module)) ? m_element.attr < string > (_U(module)) : string();
}
