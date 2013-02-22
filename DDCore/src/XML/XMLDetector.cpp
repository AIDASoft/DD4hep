#include "XML/XMLDetector.h"

using namespace std;
using namespace DD4hep::XML;

#define XML_ATTR_ACCESSOR(type,name)  type Dimension::name() const { return m_element.attr<type>(Unicode_##name); }
#define XML_ATTR_ACCESSOR_DOUBLE(name)	                                \
  double Dimension::name(double default_val) const {		       	\
    const XmlChar* val = m_element.attr_value_nothrow(Unicode_##name);	\
    return val ? _toDouble(val) : default_val; }
#define XML_ATTR_ACCESSOR_INT(name)	                                \
  int Dimension::name(int default_val) const {				\
    const XmlChar* val = m_element.attr_value_nothrow(Unicode_##name);	\
    return val ? _toInt(val) : default_val; }
#define XML_ATTR_ACCESSOR_BOOL(name)	                                \
  bool Dimension::name(bool default_val) const {		       	\
    const XmlChar* val = m_element.attr_value_nothrow(Unicode_##name);	\
    return val ? _toBool(val) : default_val; }

#define XML_CHILD_ACCESSOR_XML_DIM(name)	                        \
  Dimension Dimension::name(bool throw_if_not_present) const {          \
    return m_element.child(Unicode_##name,throw_if_not_present); }

XML_ATTR_ACCESSOR(int,id)
  XML_ATTR_ACCESSOR_INT(id)
  XML_ATTR_ACCESSOR(bool,combineHits)

  XML_ATTR_ACCESSOR(double,x)
  XML_ATTR_ACCESSOR_DOUBLE(x)
  XML_ATTR_ACCESSOR(double,X)
  XML_ATTR_ACCESSOR(double,dx)
  XML_ATTR_ACCESSOR_DOUBLE(dx)
  XML_ATTR_ACCESSOR(double,x0)
  XML_ATTR_ACCESSOR(double,x1)
  XML_ATTR_ACCESSOR(double,x2)
  XML_ATTR_ACCESSOR(double,xmin)
  XML_ATTR_ACCESSOR(double,xmax)
  XML_ATTR_ACCESSOR(double,x_offset)
  XML_ATTR_ACCESSOR(double,dim_x)
  
  XML_ATTR_ACCESSOR(double,y)
  XML_ATTR_ACCESSOR_DOUBLE(y)
  XML_ATTR_ACCESSOR(double,Y)
  XML_ATTR_ACCESSOR(double,dy)
  XML_ATTR_ACCESSOR_DOUBLE(dy)
  XML_ATTR_ACCESSOR(double,y0)
  XML_ATTR_ACCESSOR(double,y1)
  XML_ATTR_ACCESSOR(double,y2)
  XML_ATTR_ACCESSOR(double,ymin)
  XML_ATTR_ACCESSOR(double,ymax)
  XML_ATTR_ACCESSOR(double,y_offset)
  XML_ATTR_ACCESSOR(double,dim_y)

  XML_ATTR_ACCESSOR(double,z)
  XML_ATTR_ACCESSOR_DOUBLE(z)
  XML_ATTR_ACCESSOR(double,Z)
  XML_ATTR_ACCESSOR(double,dz)
  XML_ATTR_ACCESSOR_DOUBLE(dz)
  XML_ATTR_ACCESSOR(double,z0)
  XML_ATTR_ACCESSOR(double,z1)
  XML_ATTR_ACCESSOR(double,z2)
  XML_ATTR_ACCESSOR(double,zmin)
  XML_ATTR_ACCESSOR(double,zmax)
  XML_ATTR_ACCESSOR(double,z_offset)
  XML_ATTR_ACCESSOR(double,dim_z)
  XML_ATTR_ACCESSOR(double,outer_z)
  XML_ATTR_ACCESSOR(double,inner_z)

  XML_ATTR_ACCESSOR(double,r)
  XML_ATTR_ACCESSOR_DOUBLE(r)
  XML_ATTR_ACCESSOR(double,R)
  XML_ATTR_ACCESSOR(double,dr)
  XML_ATTR_ACCESSOR(double,rmin)
  XML_ATTR_ACCESSOR(double,rmax)
  XML_ATTR_ACCESSOR(double,radius)
  XML_ATTR_ACCESSOR(double,outer_r)
  XML_ATTR_ACCESSOR(double,outer_radius)
  XML_ATTR_ACCESSOR(double,inner_r)
  XML_ATTR_ACCESSOR(double,inner_radius)

  XML_ATTR_ACCESSOR(double,angle)
  XML_ATTR_ACCESSOR(double,theta)
  XML_ATTR_ACCESSOR(int,thetaBins)
  XML_ATTR_ACCESSOR(double,psi)
  XML_ATTR_ACCESSOR(double,phi)
  XML_ATTR_ACCESSOR(int,phiBins)
  XML_ATTR_ACCESSOR(double,phi0)
  XML_ATTR_ACCESSOR_DOUBLE(phi0)
  XML_ATTR_ACCESSOR(double,deltaphi)


  XML_ATTR_ACCESSOR(double,length)
  XML_ATTR_ACCESSOR(double,width)
  XML_ATTR_ACCESSOR(double,height)
  XML_ATTR_ACCESSOR(double,depth)
  XML_ATTR_ACCESSOR(double,offset)
  XML_ATTR_ACCESSOR_DOUBLE(offset)
  XML_ATTR_ACCESSOR(double,crossing_angle)
  XML_ATTR_ACCESSOR(double,incoming_r)
  XML_ATTR_ACCESSOR(double,outgoing_r)
  XML_ATTR_ACCESSOR(double,phi_size_max)
  XML_ATTR_ACCESSOR(double,r_size)
  
  XML_ATTR_ACCESSOR(double,gap)
  XML_ATTR_ACCESSOR(double,z_length)
  XML_ATTR_ACCESSOR(double,zhalf)
  XML_ATTR_ACCESSOR(double,phi_tilt)
  XML_ATTR_ACCESSOR(int,nphi)
  XML_ATTR_ACCESSOR(double,rc)
  XML_ATTR_ACCESSOR(int,nz)
  XML_ATTR_ACCESSOR(double,zstart)
  XML_ATTR_ACCESSOR(double,start)
  XML_ATTR_ACCESSOR(double,end)
  XML_ATTR_ACCESSOR(double,thickness)
  XML_ATTR_ACCESSOR(int,numsides)
  XML_ATTR_ACCESSOR(int,number)
  XML_ATTR_ACCESSOR(int,repeat)
  XML_ATTR_ACCESSOR(bool,reflect)
  XML_ATTR_ACCESSOR_BOOL(reflect)

  XML_ATTR_ACCESSOR(int,nmodules)
  XML_ATTR_ACCESSOR(int,nModules)
  XML_ATTR_ACCESSOR(int,RowID)
  XML_ATTR_ACCESSOR(int,nPads)
  XML_ATTR_ACCESSOR(double,moduleHeight)
  XML_ATTR_ACCESSOR(double,moduleWidth)
  XML_ATTR_ACCESSOR(double,modulePosX)
  XML_ATTR_ACCESSOR(double,modulePosY)
  XML_ATTR_ACCESSOR(double,modulePitch)
  XML_ATTR_ACCESSOR(double,rowPitch)
  XML_ATTR_ACCESSOR(double,padPitch)
  XML_ATTR_ACCESSOR(double,rowHeight)
  XML_ATTR_ACCESSOR(double,inner_field)
  XML_ATTR_ACCESSOR(double,outer_field)
  XML_ATTR_ACCESSOR(int,type)
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


  string Dimension::padType()  const  {
  return m_element.attr<string>(_U(pads));
}

string Dimension::nameStr()  const  {
  return m_element.attr<string>(_U(name));
}

string Dimension::typeStr()  const  {
  return m_element.attr<string>(_U(type));
}

string Dimension::regionStr() const {
  return m_element.hasAttr(_U(region)) ? m_element.attr<string>(_U(region)) : string();
}

string Dimension::limitsStr() const {
  return m_element.hasAttr(_U(limits)) ? m_element.attr<string>(_U(limits)) : string();
}

string Dimension::visStr()  const  {
  return m_element.hasAttr(_U(vis)) ? m_element.attr<string>(_U(vis)) : string();
}

string Dimension::readoutStr()  const  {
  return m_element.hasAttr(_U(readout)) ? m_element.attr<string>(_U(readout)) : string();
}

string Dimension::moduleStr()  const  {
  return m_element.hasAttr(_U(module)) ? m_element.attr<string>(_U(module)) : string();
}

string Component::materialStr() const   {
  return m_element.attr<string>(_U(material));
}

bool Component::isSensitive() const  {
  char val = m_element.hasAttr(_U(sensitive)) ? m_element.attr<string>(_U(sensitive))[0] : 'f';
  val = ::toupper(val);
  return val == 'T' || val == 'Y';
}

int DetElement::id() const   {
  return m_element.hasAttr(_U(id)) ? m_element.attr<int>(_U(id)) : -1;
}

string DetElement::materialStr() const  {
  Handle_t h = m_element.child(_U(material));
  if ( h && h.hasAttr(_U(name)) )  {
    return h.attr<string>(_U(name));
  }
  return "";
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

bool DetElement::isTracker() const   {
  if ( m_element )  {
    string type = attr<string>(_U(type));
    if ( type.find("Tracker") != string::npos && hasAttr(_U(readout)) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const   {
  if ( m_element )  {
    string type = attr<string>(_U(type));
    if ( type.find("Calorimeter") != string::npos && hasAttr(_U(readout)) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isInsideTrackingVolume() const  {
  if ( m_element && hasAttr(_U(insideTrackingVolume)) )
    return attr<bool>(_U(insideTrackingVolume));
  else if ( isTracker() )
    return true;
  return false;
}
