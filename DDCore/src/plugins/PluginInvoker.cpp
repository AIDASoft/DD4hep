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

// Framework includes
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/DetFactoryHelper.h"

// C/C++ include files
#include <stdexcept>

/*
 *   dd4hep namespace declaration
 */
namespace dd4hep  {

  namespace   {
    /// Some utility class to specialize the converters:
    class include_file;
    class plugins;
    class plugin;
    class arg;
  }

  // Converters re-used from compact:
  template <> void Converter<Readout>::operator()(xml_h element) const;
  template <> void Converter<LimitSet>::operator()(xml_h element) const;
  template <> void Converter<Constant>::operator()(xml_h element) const;
  template <> void Converter<VisAttr>::operator()(xml_h element) const;
  // Converters implemented here:
  template <> void Converter<include_file>::operator()(xml_h element) const;
  template <> void Converter<plugins>::operator()(xml_h element)  const;
  template <> void Converter<plugin>::operator()(xml_h element)  const;
  template <> void Converter<arg>::operator()(xml_h element)  const;
}
using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

/** Convert arg objects
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<arg>::operator()(xml_h e)  const  {
  xml_comp_t c(e);
  string val = c.valueStr();
  vector<string>* args = (vector<string>*)param;
  args->emplace_back(val);
}

/** Convert plugin objects
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<plugin>::operator()(xml_h e)  const  {
  xml_comp_t c(e);
  string nam = c.nameStr();
  vector<string> args;
  vector<const char*> cargs;
  //args.emplace_back("plugin:"+nam);

  xml_coll_t(e,"arg").for_each(Converter<arg>(description,&args));
  for(vector<string>::const_iterator i=args.begin(); i!=args.end();++i)
    cargs.emplace_back((*i).c_str());
  printout(INFO,"ConverterPlugin","+++ Now executing plugin:%s [%d args]",nam.c_str(),int(cargs.size()));
  description.apply(nam.c_str(),int(cargs.size()),(char**)&cargs[0]);
}

/** Convert include_file objects
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<include_file>::operator()(xml_h element) const   {
  xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h node = doc.root();
  string tag = node.tag();
  if ( tag == "plugin" )
    Converter<plugin>(description,param)(node);
  else if ( tag == "plugins" )
    Converter<plugins>(description,param)(node);
  else
    throw runtime_error("Undefined tag name in XML structure:"+tag+" XML parsing abandoned.");
}

/** Convert plugins objects
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<plugins>::operator()(xml_h e)  const  {
  xml_coll_t(e, _U(define)).for_each(_U(constant),  Converter<Constant>(description,param));
  xml_coll_t(e, _U(display)).for_each(_U(vis),      Converter<VisAttr>(description,param));
  xml_coll_t(e, _U(include)).for_each(              Converter<include_file>(description,param));
  xml_coll_t(e, _U(includes)).for_each(_U(include), Converter<include_file>(description,param));
  xml_coll_t(e, _U(limits)).for_each(_U(limitset),  Converter<LimitSet>(description,param));
  xml_coll_t(e, _U(readouts)).for_each(_U(readout), Converter<Readout>(description,param));
  xml_coll_t(e, _U(plugin)).for_each(               Converter<plugin>(description,param));
}

static long handle_plugins(Detector& description, const xml_h& element) {
  (dd4hep::Converter < plugins > (description))(element);
  return 1;
}
DECLARE_XML_DOC_READER(plugins,handle_plugins)

