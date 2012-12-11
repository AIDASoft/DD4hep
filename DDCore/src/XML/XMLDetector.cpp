#include "XML/XMLDetector.h"

using namespace std;
using namespace DD4hep::XML;

#define XML_ATTR_ACCESSOR(type,name)  type Dimension::name() const { return m_element.attr<type>(Attr_##name); }
#define XML_ATTR_ACCESSOR_DOUBLE(name)	                                \
  double Dimension::name(double default_val) const {		       	\
    const XmlChar* val = m_element.attr_value_nothrow(Attr_##name);	\
    return val ? _toDouble(val) : default_val; }
#define XML_ATTR_ACCESSOR_INT(name)	                                \
  int Dimension::name(int default_val) const {				\
    const XmlChar* val = m_element.attr_value_nothrow(Attr_##name);	\
    return val ? _toInt(val) : default_val; }
#define XML_ATTR_ACCESSOR_BOOL(name)	                                \
  bool Dimension::name(bool default_val) const {		       	\
    const XmlChar* val = m_element.attr_value_nothrow(Attr_##name);	\
    return val ? _toBool(val) : default_val; }

XML_ATTR_ACCESSOR(int,id)
  XML_ATTR_ACCESSOR_INT(id)

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
  XML_ATTR_ACCESSOR(double,psi)
  XML_ATTR_ACCESSOR(double,phi)
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
#if 0
  XML_ATTR_ACCESSOR(double,)
  XML_ATTR_ACCESSOR(double,)
  XML_ATTR_ACCESSOR(double,)
  XML_ATTR_ACCESSOR(double,)
  XML_ATTR_ACCESSOR(double,)
#endif

string Dimension::padType()  const  {
  return m_element.attr<string>(Attr_pads);
}

string  DetElement::Component::nameStr()  const  {
  return m_element.attr<string>(Attr_name);
}

string DetElement::Component::materialStr() const   {
  return m_element.attr<string>(Attr_material);
}

string  DetElement::Component::moduleStr()  const  {
  return m_element.hasAttr(Attr_module) ? m_element.attr<string>(Attr_module) : string();
}

string  DetElement::Component::typeStr()  const  {
  return m_element.attr<string>(Attr_type);
}

bool DetElement::Component::isSensitive() const  {
  char val = m_element.hasAttr(Attr_sensitive) ? m_element.attr<string>(Attr_sensitive)[0] : 'f';
  val = ::toupper(val);
  return val == 'T' || val == 'Y';
}

string DetElement::Component::visStr()  const  {
  return m_element.hasAttr(Attr_vis) ? m_element.attr<string>(Attr_vis) : string();
}

string DetElement::Component::readoutStr()  const  {
  return m_element.hasAttr(Attr_readout) ? m_element.attr<string>(Attr_readout) : string();
}

string DetElement::Component::regionStr() const {
  return m_element.hasAttr(Attr_region) ? m_element.attr<string>(Attr_region) : string();
}

string DetElement::Component::limitsStr() const {
  return m_element.hasAttr(Attr_limits) ? m_element.attr<string>(Attr_limits) : string();
}

Dimension DetElement::Component::dimensions()  const  {
  return Dimension(m_element.child(Tag_dimensions));
}

int DetElement::id() const   {
  return m_element.hasAttr(Attr_id) ? m_element.attr<int>(Attr_id) : -1;
}

string DetElement::nameStr() const   {
  return m_element.attr<string>(Attr_name);
}

string  DetElement::typeStr()  const  {
  return m_element.attr<string>(Attr_type);
}

string  DetElement::visStr()  const  {
  return m_element.attr<string>(Attr_vis);
}

Dimension DetElement::dimensions()  const  {
  return Dimension(m_element.child(Tag_dimensions));
}

string DetElement::materialStr() const  {
  Handle_t h = m_element.child(Tag_material);
  if ( h && h.hasAttr(Attr_name) )  {
    return h.attr<string>(Attr_name);
  }
  return "";
}

string DetElement::regionStr() const {
  return m_element.hasAttr(Attr_region) ? m_element.attr<string>(Attr_region) : string();
}

string DetElement::limitsStr() const {
  return m_element.hasAttr(Attr_limits) ? m_element.attr<string>(Attr_limits) : string();
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

bool DetElement::isTracker() const   {
  if ( m_element )  {
    string type = attr<string>(Attr_type);
    if ( type.find("Tracker") != string::npos && hasAttr(Attr_readout) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const   {
  if ( m_element )  {
    string type = attr<string>(Attr_type);
    if ( type.find("Calorimeter") != string::npos && hasAttr(Attr_readout) ) {
        return true;
    }
  }
  return false;
}

bool DetElement::isInsideTrackingVolume() const  {
  if ( m_element && hasAttr(Attr_insideTrackingVolume) )
    return attr<bool>(Attr_insideTrackingVolume);
  else if ( isTracker() )
    return true;
  return false;
}
