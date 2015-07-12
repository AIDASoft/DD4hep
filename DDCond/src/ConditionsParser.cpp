// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "DDCond/ConditionsTags.h"
#include "DDCond/ConditionsStack.h"
#include "DDCond/ConditionsHandler.h"
#include "DDCond/ConditionsTransaction.h"

// C/C++ include files
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep  {

  namespace   {
    /// Some utility class to specialize the converters:
    class include;
    class arbitrary;
    class conditions;
  }
  /// Forward declarations for all specialized converters
  template <> void Converter<include>::operator()(xml_h seq)  const;
  template <> void Converter<arbitrary>::operator()(xml_h seq)  const;
  template <> void Converter<conditions>::operator()(xml_h seq)  const;
}

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
typedef ConditionsStack::Entry Entry;

/// Helper: Extract the validity from the xml element
string _getValidity(xml_h elt)  {
  if ( !elt.ptr() )
    return "Infinite";
  else if ( !elt.hasAttr(_U(validity)) )
    return _getValidity(elt.parent());
  return elt.attr<string>(_U(validity));
}

/// Helper: Extract the required detector element from the parsing information
DetElement _getDetector(void* param, xml_h e)  {
  DetElement detector = param ? *(DetElement*)param : DetElement();
  string     subpath  = e.hasAttr(_U(path)) ? e.attr<string>(_U(path)) : string();
  return subpath.empty() ? detector : DetectorTools::findDaughterElement(detector,subpath);
}

/// Helper: Extract the string value from the xml element 
dd4hep_ptr<Entry> _getStackEntry(void* param, xml_h element)  {
  xml_comp_t e(element);
  DetElement elt = _getDetector(param, element);
  string name = e.hasAttr(_U(name)) ? e.nameStr() : e.tag();
  dd4hep_ptr<Entry> result(new Entry(elt,name,e.tag(),_getValidity(element)));
  return result;
}

/** Convert arbitrary conditon objects containing standard tags
 *
 *    Function entry expects as a parameter a valid DetElement handle
 *    pointing to the subdetector, which detector elements should be 
 *    realigned.
 *
 *      <temperature path="/world/TPC" name="AbientTemperatur" value="20.9*Celcius"/>
 *      <temperature path="TPC" name="AbientTemperatur" value="20.9*Celcius"/>
 *      <temperature name="AbientTemperatur" value="20.9*Celcius"/>
 *
 *      <temperature name="AbientTemperatur" value="20.9*Kelvin"/>
 *      <pressure name="external_pressure" value="980*hPa"/>
 *      <include ref="..."/>
 *
 *    The object tag name is passed as the conditons type to the system.
 *    The data payload may either be specified as an attribute to the
 *    element or as text (data payload as the inner XML of the element).
 *
 *  These items have:
 *  - a name defining the condition within the detector element
 *  - a value interpreted as a double. In XML the value may be dressed with a unit
 *    which will be correctly treated by the expression evaluator
 *  - a path (optionally). attribute_values are ALWAYS treated within the context
 *    of the containing detector element. If pathes are relative, they are 
 *    relative to the embedding element. If pathes are absolute, the embedding
 *    element is ignored.
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<arbitrary>::operator()(xml_h e) const {
  xml_comp_t elt(e);
  string tag = elt.tag();
  if ( tag == "open_transaction" )
    return;
  else if ( tag == "close_transaction" ) 
    return;
  else if ( tag == "include" )
    Converter<include>(lcdd,param)(e);
  else if ( tag == "conditions" )  
    Converter<conditions>(lcdd,param)(e);
  else if ( tag == "detelement" )
    Converter<conditions>(lcdd,param)(e);
  else if ( tag == "subdetectors" )
    xml_coll_t(e,_U(star)).for_each(Converter<conditions>(lcdd,param));
  else if ( tag == "detelements" )
    xml_coll_t(e,_U(star)).for_each(Converter<conditions>(lcdd,param));
  else  {
    dd4hep_ptr<Entry> val(_getStackEntry(param,e));
    val->value = elt.hasAttr(_U(value)) ? elt.valueStr() : e.text();
    ConditionsStack::get().insert(val);
  }
}

/** Convert include objects
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<include>::operator()(xml_h element) const {
  XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_coll_t(doc.root(),_U(star)).for_each(Converter<arbitrary>(lcdd,param));
}

/** Convert objects containing standard conditions tags
 *
 *    Function entry expects as a parameter a valid DetElement handle
 *    pointing to the subdetector, which detector elements should be 
 *    realigned. A absolute or relative DetElement path may be supplied by
 *    the element as an attribute:
 *
 *    <conditions path="/world/TPC/TPC_SideA/TPC_SideA_sector02">
 *        ...
 *    </conditions>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<conditions>::operator()(xml_h e) const {
  DetElement elt = _getDetector(param,e);
  xml_coll_t(e,_U(star)).for_each(Converter<arbitrary>(lcdd,&elt));
}

/** Basic entry point to read conditions files
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long setup_Conditions(lcdd_t& lcdd, const xml_h& e) {
  ConditionsHandler::install(lcdd);
  ConditionsTransaction tr(lcdd, e);
  DetElement top = lcdd.world();
  (DD4hep::Converter<DD4hep::conditions>(lcdd,&top))(e);
  return 1;
}
DECLARE_XML_DOC_READER(conditions,setup_Conditions)

/** Basic entry point to install the conditions handler in a LCDD instance
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long install_Conditions(lcdd_t& lcdd, int, char**) {
  ConditionsHandler::install(lcdd);
  return 1;
}
DECLARE_APPLY(DD4hepConditionsInstall,install_Conditions)
