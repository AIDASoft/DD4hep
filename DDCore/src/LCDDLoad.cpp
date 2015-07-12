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

// Framework include files
#include "DD4hep/LCDDLoad.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"

// C/C++ include files
#include <stdexcept>

#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace DD4hep {
  namespace XML {
    typedef xercesc::DOMException XmlException;
  }
}
#endif

using namespace DD4hep;
using namespace std;

/// Default constructor
LCDDLoad::LCDDLoad(Geometry::LCDD* lcdd) : m_lcdd(lcdd)  {
}

/// Standard destructor
LCDDLoad::~LCDDLoad() {
}

void LCDDLoad::processXML(const string& xmlfile) {
  try {
    XML::DocumentHolder doc(XML::DocumentHandler().load(xmlfile));
    XML::Handle_t handle = doc.root();
    processXMLElement(xmlfile,handle);
  }
  catch (const XML::XmlException& e) {
    throw runtime_error(XML::_toString(e.msg) + "\nDD4hep: XML-DOM Exception while parsing " + xmlfile);
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\nDD4hep: while parsing " + xmlfile);
  }
  catch (...) {
    throw runtime_error("DD4hep: UNKNOWN exception while parsing " + xmlfile);
  }
}


void LCDDLoad::processXML(const XML::Handle_t& base, const string& xmlfile) {
  try {
    XML::Strng_t xml(xmlfile);
    XML::DocumentHolder doc(XML::DocumentHandler().load(base,xml));
    XML::Handle_t handle = doc.root();
    processXMLElement(xmlfile,handle);
  }
  catch (const XML::XmlException& e) {
    throw runtime_error(XML::_toString(e.msg) + "\nDD4hep: XML-DOM Exception while parsing " + xmlfile);
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\nDD4hep: while parsing " + xmlfile);
  }
  catch (...) {
    throw runtime_error("DD4hep: UNKNOWN exception while parsing " + xmlfile);
  }
}

void LCDDLoad::processXMLElement(const std::string& xmlfile, const XML::Handle_t& xml_root) {
  string tag = xml_root.tag();
  string type = tag + "_XML_reader";
  XML::Handle_t handle = xml_root;
  long result = PluginService::Create<long>(type, m_lcdd, &handle);
  if (0 == result) {
    PluginDebug dbg;
    result = PluginService::Create<long>(type, m_lcdd, &xml_root);
    if ( 0 == result )  {
      throw runtime_error("DD4hep: Failed to locate plugin to interprete files of type"
                          " \"" + tag + "\" - no factory:" + type + ". " + dbg.missingFactory(type));
    }
  }
  result = *(long*) result;
  if (result != 1) {
    throw runtime_error("DD4hep: Failed to parse the XML file " + xmlfile + " with the plugin " + type);
  }
}
