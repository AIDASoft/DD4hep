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
#include "DD4hep/LCDD.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DDCond/ConditionsTags.h"
#include "DDCond/ConditionsEntry.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsDataLoader.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep  {

  namespace {
    /// Some utility class to specialize the converters:
    class include;
    class arbitrary;
    class conditions;

    class iov;
    class iov_type;
    class manager;
    class repository;
    class detelement;
    class align_conditions;
    class align_arbitrary;
    /// Conditions types
    class value;
    class mapping;
    class sequence;
    class alignment;
    class position;
    class rotation;
    class pivot;
  }
  template <> void Converter<include>::operator()(xml_h seq)  const;
  template <> void Converter<arbitrary>::operator()(xml_h seq)  const;
  template <> void Converter<conditions>::operator()(xml_h seq)  const;
}
  
using std::string;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Geometry::DetElement;


namespace DD4hep {
  struct ConversionArg {
    DetElement         detector;
    ConditionsStack*   stack;
    ConversionArg(DetElement det, ConditionsStack* s)
      : detector(det), stack(s)
    {
    }
  };

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
    ConversionArg* arg  = static_cast<ConversionArg*>(param);
    DetElement detector = arg ? arg->detector : DetElement();
    string     subpath  = e.hasAttr(_U(path)) ? e.attr<string>(_U(path)) : string();
    return subpath.empty() ? detector : Geometry::DetectorTools::findDaughterElement(detector,subpath);
  }

  /// Helper: Extract the string value from the xml element 
  Entry* _createStackEntry(void* param, xml_h element)  {
    xml_comp_t e(element);
    DetElement elt = _getDetector(param, element);
    string name = e.hasAttr(_U(name)) ? e.nameStr() : e.tag();
    return new Entry(elt,name,e.tag(),_getValidity(element),hash32(name));
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
    ConversionArg* arg  = _param<ConversionArg>();
    if ( tag == "conditions" )  
      Converter<conditions>(lcdd,param,optional)(e);
    else if ( arg->stack && tag == "detelement" )
      Converter<conditions>(lcdd,param,optional)(e);
    else if ( tag == "open_transaction" )
      return;
    else if ( tag == "close_transaction" ) 
      return;
    else if ( tag == "include" )
      Converter<include>(lcdd,param,optional)(e);
    else if ( tag == "detelements" )
      xml_coll_t(e,_U(star)).for_each(Converter<conditions>(lcdd,param,optional));
    else if ( tag == "subdetectors" )
      xml_coll_t(e,_U(star)).for_each(Converter<conditions>(lcdd,param,optional));
    else if ( tag == "alignment" )   {
      dd4hep_ptr<Entry> val(_createStackEntry(param,e));
      val->value = elt.attr<string>(_U(ref));
      arg->stack->push_back(val.release());
    }
    else  {
      dd4hep_ptr<Entry> val(_createStackEntry(param,e));
      val->value = elt.hasAttr(_U(value)) ? elt.valueStr() : e.text();
      arg->stack->push_back(val.release());
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
    xml_coll_t(doc.root(),_U(star)).for_each(Converter<arbitrary>(lcdd,param,optional));
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
    ConversionArg* arg  = _param<ConversionArg>();
    DetElement elt = arg->detector;
    arg->detector = _getDetector(param,e);
    xml_coll_t(e,_U(star)).for_each(Converter<arbitrary>(lcdd,param,optional));
    arg->detector = elt;
  }
}

/** Basic entry point to read global conditions files
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static void* setup_global_Conditions(lcdd_t& lcdd, int argc, char** argv)  {
  if ( argc == 2 )  {
    xml_h e = xml_h::Elt_t(argv[0]);
    ConditionsStack* stack = (ConditionsStack*)argv[1];
    ConversionArg args(lcdd.world(), stack);
    (DD4hep::Converter<conditions>(lcdd,&args))(e);
    return &lcdd;
  }
  except("XML_DOC_READER","Invalid number of arguments to interprete conditions: %d != %d.",argc,2);
  return 0;
}
DECLARE_LCDD_CONSTRUCTOR(XMLConditionsParser,setup_global_Conditions)
