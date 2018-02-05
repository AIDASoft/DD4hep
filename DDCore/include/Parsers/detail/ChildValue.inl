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


/** 
 *  Note: Do NEVER include this file directly!
 *
 *  Use the specific include files in the XML or JSON directory!
 *  Also NO header guards!
 */


#define XML_CHILDVALUE_childValue(name,type)                            \
  m_element.child(_dd4hep_Unicode_Item(name)).attr<type>(_dd4hep_Unicode_Item(value))

#define XML_CHILDVALUE_childValueDefault(name,type,def)                 \
  Handle_t __h = m_element.child(_dd4hep_Unicode_Item(name),false);     \
  if ( __h.ptr() && __h.hasAttr(_dd4hep_Unicode_Item(value)) ) return __h.attr < type > (_dd4hep_Unicode_Item(value)); \
  return def;

#define XML_CHILDVALUE_ACCESSOR(type,name)                              \
  type ChildValue::name() const { return XML_CHILDVALUE_childValue(name,type); }

#define XML_CHILDVALUE_ACCESSOR_DEFAULT(name,type,dressing)             \
  type ChildValue::name(type default_val) const {                       \
    Handle_t __h = m_element.child(_dd4hep_Unicode_Item(name),false);   \
    if ( __h.ptr() )  {                                                 \
      const XmlChar* val = __h.attr_value_nothrow(_dd4hep_Unicode_Item(value)); \
      return val ? dressing(val) : default_val; }                       \
    return default_val; }

#define XML_CHILDVALUE_ACCESSOR_DOUBLE(name) XML_CHILDVALUE_ACCESSOR_DEFAULT(name,double,_toDouble)
#define XML_CHILDVALUE_ACCESSOR_INT(name)    XML_CHILDVALUE_ACCESSOR_DEFAULT(name,int,_toInt)
#define XML_CHILDVALUE_ACCESSOR_BOOL(name)   XML_CHILDVALUE_ACCESSOR_DEFAULT(name,bool,_toBool)
