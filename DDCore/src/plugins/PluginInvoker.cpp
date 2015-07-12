// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
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

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/DetFactoryHelper.h"

// C/C++ include files
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep  {

  namespace   {
    /// Some utility class to specialize the converters:
    class include_file;
    class plugins;
    class plugin;
    class arg;
  }

  template <> void Converter<include_file>::operator()(xml_h element) const;
  template <> void Converter<plugins>::operator()(xml_h e)  const;
  template <> void Converter<plugin>::operator()(xml_h e)  const;
  template <> void Converter<arg>::operator()(xml_h e)  const;
}
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

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
  args->push_back(val);
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
  //args.push_back("plugin:"+nam);

  xml_coll_t(e,"arg").for_each(Converter<arg>(lcdd,&args));
  for(vector<string>::const_iterator i=args.begin(); i!=args.end();++i)
    cargs.push_back((*i).c_str());
  printout(INFO,"Converter<plugin>","+++ Now executing plugin:%s [%d args]",nam.c_str(),int(cargs.size()));
  lcdd.apply(nam.c_str(),int(cargs.size()),(char**)&cargs[0]);
}

/** Convert include_file objects
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<include_file>::operator()(xml_h element) const {
  XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h node = doc.root();
  string tag = node.tag();
  if ( tag == "plugin" )
    Converter<plugin>(lcdd,param)(node);
  else if ( tag == "plugins" )
    Converter<plugins>(lcdd,param)(node);
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
  xml_coll_t(e,"include").for_each(Converter<include_file>(lcdd,param));
  xml_coll_t(e,"plugin").for_each(Converter<plugin>(lcdd,param));
}

static long handle_plugins(lcdd_t& lcdd, const xml_h& element) {
  (DD4hep::Converter < plugins > (lcdd))(element);
  return 1;
}
DECLARE_XML_DOC_READER(plugins,handle_plugins)

