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

// Framework include files
#include "DD4hep/DetectorLoad.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"

// C/C++ include files
#include <stdexcept>

#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace dd4hep {
  namespace xml {
    typedef xercesc::DOMException XmlException;
  }
}
#endif

using namespace dd4hep;
using namespace std;

/// Default constructor
DetectorLoad::DetectorLoad(Detector* description) : m_detDesc(description)  {
}

/// Standard destructor
DetectorLoad::~DetectorLoad() {
}

/// Process XML unit and adopt all data from source structure.
void DetectorLoad::processXML(const string& xmlfile, xml::UriReader* entity_resolver) {
  try {
    xml::DocumentHolder doc(xml::DocumentHandler().load(xmlfile,entity_resolver));
    xml::Handle_t handle = doc.root();
    processXMLElement(xmlfile,handle);
  }
  catch (const xml::XmlException& e) {
    throw runtime_error(xml::_toString(e.msg) + "\ndd4hep: XML-DOM Exception while parsing " + xmlfile);
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\ndd4hep: while parsing " + xmlfile);
  }
  catch (...) {
    throw runtime_error("dd4hep: UNKNOWN exception while parsing " + xmlfile);
  }
}


/// Process XML unit and adopt all data from source structure.
void DetectorLoad::processXML(const xml::Handle_t& base, const string& xmlfile, xml::UriReader* entity_resolver) {
  try {
    xml::Strng_t xml(xmlfile);
    xml::DocumentHolder doc(xml::DocumentHandler().load(base,xml,entity_resolver));
    xml::Handle_t handle = doc.root();
    processXMLElement(xmlfile,handle);
  }
  catch (const xml::XmlException& e) {
    throw runtime_error(xml::_toString(e.msg) + "\ndd4hep: XML-DOM Exception while parsing " + xmlfile);
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\ndd4hep: while parsing " + xmlfile);
  }
  catch (...) {
    throw runtime_error("dd4hep: UNKNOWN exception while parsing " + xmlfile);
  }
}

/// Process a given DOM (sub-) tree
void DetectorLoad::processXMLElement(const std::string& xmlfile, const xml::Handle_t& xml_root) {
  string tag = xml_root.tag();
  string type = tag + "_XML_reader";
  xml::Handle_t handle = xml_root;
  long result = PluginService::Create<long>(type, m_detDesc, &handle);
  if (0 == result) {
    PluginDebug dbg;
    result = PluginService::Create<long>(type, m_detDesc, &handle);
    if ( 0 == result )  {
      throw runtime_error("dd4hep: Failed to locate plugin to interprete files of type"
                          " \"" + tag + "\" - no factory:" + type + ". " + dbg.missingFactory(type));
    }
  }
  result = *(long*) result;
  if (result != 1) {
    throw runtime_error("dd4hep: Failed to parse the XML file " + xmlfile + " with the plugin " + type);
  }
}

/// Process a given DOM (sub-) tree
void DetectorLoad::processXMLElement(const xml::Handle_t& xml_root, DetectorBuildType /* type */) {
  string tag = xml_root.tag();
  string type = tag + "_XML_reader";
  xml::Handle_t handle = xml_root;
  long result = PluginService::Create<long>(type, m_detDesc, &handle);
  if (0 == result) {
    PluginDebug dbg;
    result = PluginService::Create<long>(type, m_detDesc, &handle);
    if ( 0 == result )  {
      throw runtime_error("dd4hep: Failed to locate plugin to interprete files of type"
                          " \"" + tag + "\" - no factory:" 
			  + type + ". " + dbg.missingFactory(type));
    }
  }
  result = *(long*) result;
  if (result != 1)   {
    throw runtime_error("dd4hep: Failed to parse the XML element with tag " 
			+ tag + " with the plugin " + type);
  }
}
