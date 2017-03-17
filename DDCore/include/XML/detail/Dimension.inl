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

/** 
 *  Note: Do NEVER include this file directly!
 *
 *  Also NO header guards!
 *  Include XML/XMLDimension.h or JSON/Dimension.h !
 */


#define XML_ATTR_NS_ACCESSOR(type,ns,name)  type ns::name() const { return m_element.attr<type>(_DD4hep_Unicode_Item(name)); }

#define XML_ATTR_NS_ACCESSOR_DEFAULT(type,ns,name,dressing)	\
  type ns::name(type default_val) const {			\
    const XmlChar* val = m_element.attr_value_nothrow(_DD4hep_Unicode_Item(name));   \
    return val ? dressing(val) : default_val; }

#define XML_ATTR_NS_ACCESSOR_DOUBLE(ns,name)			\
  XML_ATTR_NS_ACCESSOR(double,ns,name)				\
  XML_ATTR_NS_ACCESSOR_DEFAULT(double,ns,name,DD4hep:: DD4HEP_DIMENSION_NS ::_toDouble)

#define XML_ATTR_NS_ACCESSOR_INT(ns,name)			\
  XML_ATTR_NS_ACCESSOR(int,ns,name)				\
  XML_ATTR_NS_ACCESSOR_DEFAULT(int,ns,name,DD4hep:: DD4HEP_DIMENSION_NS ::_toInt)

#define XML_ATTR_NS_ACCESSOR_STRING(ns,name)			\
  XML_ATTR_NS_ACCESSOR(std::string,ns,name)			\
  XML_ATTR_NS_ACCESSOR_DEFAULT(std::string,ns,name,DD4hep:: DD4HEP_DIMENSION_NS ::_toString)


#define XML_ATTR_ACCESSOR(type,name)                  XML_ATTR_NS_ACCESSOR(type,DD4hep:: DD4HEP_DIMENSION_NS ::Dimension,name)
#define XML_ATTR_ACCESSOR_DEFAULT(name,type,dressing) XML_ATTR_NS_ACCESSOR_DEFAULT(type,DD4hep:: DD4HEP_DIMENSION_NS ::Dimension,name,dressing)
#define XML_ATTR_ACCESSOR_DOUBLE(name)		      XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep:: DD4HEP_DIMENSION_NS ::Dimension,name)

#define XML_ATTR_ACCESSOR_INT(name)                   XML_ATTR_NS_ACCESSOR_DEFAULT(int,DD4hep:: DD4HEP_DIMENSION_NS ::Dimension,name,_toInt)
#define XML_ATTR_ACCESSOR_BOOL(name)                  XML_ATTR_NS_ACCESSOR_DEFAULT(bool,DD4hep:: DD4HEP_DIMENSION_NS ::Dimension,name,_toBool)

#define XML_CHILD_ACCESSOR_XML_DIM(name)                        		\
    DD4hep:: DD4HEP_DIMENSION_NS ::Dimension DD4hep:: DD4HEP_DIMENSION_NS ::Dimension::name(bool throw_if_not_present) const {		\
    return m_element.child(_DD4hep_Unicode_Item(name),throw_if_not_present); }
