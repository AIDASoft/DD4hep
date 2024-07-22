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
#include <DD4hep/DetectorLoad.h>
#include <DD4hep/Detector.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>
#include <XML/XMLElements.h>
#include <XML/DocumentHandler.h>

// C/C++ include files
#include <stdexcept>
#include <csignal>
#include <cstdlib>
#include <iostream>

#ifndef __TIXML__
#include <xercesc/dom/DOMException.hpp>
namespace dd4hep {
  namespace xml {
    typedef xercesc::DOMException XmlException;
  }
}
#endif

using namespace dd4hep;

/// Default constructor (protected, for sub-classes)
DetectorLoad::DetectorLoad(Detector* description) : m_detDesc(description)  {
}

/// Default constructor (public, if used as a handle)
DetectorLoad::DetectorLoad(Detector& description) : m_detDesc(&description)  {
}

/// Standard destructor
DetectorLoad::~DetectorLoad() {
}

/// Process XML unit and adopt all data from source structure.
void DetectorLoad::processXML(const std::string& xmlfile, xml::UriReader* entity_resolver) {

  try {
    xml::DocumentHolder doc(xml::DocumentHandler().load(xmlfile,entity_resolver));
    if ( doc )   {
      xml::Handle_t handle = doc.root();
      if ( handle )   {
        processXMLElement(xmlfile,handle);
        return;
      }
    }
    throw std::runtime_error("dd4hep: Failed to parse the XML file " + xmlfile + " [Invalid XML ROOT handle]");
  }
  catch (const xml::XmlException& e) {
    throw std::runtime_error(xml::_toString(e.msg) + "\ndd4hep: XML-DOM Exception while parsing " + xmlfile);
  }
  catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\ndd4hep: while parsing " + xmlfile);
  }
  catch (...) {
    throw std::runtime_error("dd4hep: UNKNOWN exception while parsing " + xmlfile);
  }
}

/// Process XML unit and adopt all data from source structure.
void DetectorLoad::processXML(const xml::Handle_t& base, const std::string& xmlfile, xml::UriReader* entity_resolver) {
  try {
    xml::Strng_t xml(xmlfile);
    xml::DocumentHolder doc(xml::DocumentHandler().load(base,xml,entity_resolver));
    if ( doc )   {
      xml::Handle_t handle = doc.root();
      if ( handle )   {
        processXMLElement(xmlfile,handle);
        return;
      }
    }
    throw std::runtime_error("dd4hep: Failed to parse the XML file " + xmlfile + " [Invalid XML ROOT handle]");
  }
  catch (const xml::XmlException& e) {
    throw std::runtime_error(xml::_toString(e.msg) + "\ndd4hep: XML-DOM Exception while parsing " + xmlfile);
  }
  catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\ndd4hep: while parsing " + xmlfile);
  }
  catch (...) {
    throw std::runtime_error("dd4hep: UNKNOWN exception while parsing " + xmlfile);
  }
}

/// Process XML unit and adopt all data from source string in momory.
void DetectorLoad::processXMLString(const char* xmldata)   {
  processXMLString(xmldata, 0);
}

/// Process XML unit and adopt all data from source string in momory. Subsequent parsers may use the entity resolver.
void DetectorLoad::processXMLString(const char* xmldata, xml::UriReader* entity_resolver) {
  try {
    if ( xmldata)   {
      xml::DocumentHolder doc(xml::DocumentHandler().parse(xmldata,::strlen(xmldata),"In-Memory",entity_resolver));
      if ( doc )   {
        xml::Handle_t handle = doc.root();
        if ( handle )   {
          processXMLElement("In-Memory-XML",handle);
          return;
        }
      }
    }
    throw std::runtime_error("DetectorLoad::processXMLString: Invalid XML In-memory source [NULL]");
  }
  catch (const xml::XmlException& e) {
    throw std::runtime_error(xml::_toString(e.msg) + "\ndd4hep: XML-DOM Exception while parsing XML in-memory string.");
  }
  catch (const std::exception& e) {
    throw std::runtime_error(std::string(e.what()) + "\ndd4hep: while parsing XML in-memory string.");
  }
  catch (...) {
    throw std::runtime_error("dd4hep: UNKNOWN exception while parsing XML in-memory string.");
  }
}

/// Process a given DOM (sub-) tree
void DetectorLoad::processXMLElement(const std::string& xmlfile, const xml::Handle_t& xml_root) {

  // Install signal handler for SIGINT (Ctrl-C)
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = [](int) {
    std::cerr << "Caught signal SIGINT, exiting..." << std::endl;
    exit(1);
  };
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  if ( xml_root.ptr() )   {
    std::string tag = xml_root.tag();
    std::string type = tag + "_XML_reader";
    xml::Handle_t handle = xml_root;
    long result = PluginService::Create<long>(type, m_detDesc, &handle);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(type, m_detDesc, &handle);
      if ( 0 == result )  {
        throw std::runtime_error("dd4hep: Failed to locate plugin to interprete files of type"
                                 " \"" + tag + "\" - no factory:" + type + ". " + dbg.missingFactory(type));
      }
    }
    result = *(long*) result;
    if (result != 1) {
      throw std::runtime_error("dd4hep: Failed to parse the XML file " + xmlfile + " with the plugin " + type);
    }
    return;
  }
  throw std::runtime_error("dd4hep: Failed to parse the XML file " + xmlfile + " [Invalid XML ROOT handle]");
}

/// Process a given DOM (sub-) tree
void DetectorLoad::processXMLElement(const xml::Handle_t& xml_root, DetectorBuildType /* type */) {

  // Install signal handler for SIGINT (Ctrl-C)
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = [](int) {
    std::cerr << "Caught signal SIGINT, exiting..." << std::endl;
    exit(1);
  };
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  if ( xml_root.ptr() )   {
    std::string tag = xml_root.tag();
    std::string type = tag + "_XML_reader";
    xml::Handle_t handle = xml_root;
    long result = PluginService::Create<long>(type, m_detDesc, &handle);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(type, m_detDesc, &handle);
      if ( 0 == result )  {
        throw std::runtime_error("dd4hep: Failed to locate plugin to interprete files of type"
                                 " \"" + tag + "\" - no factory:" 
                                 + type + ". " + dbg.missingFactory(type));
      }
    }
    result = *(long*) result;
    if (result != 1)   {
      throw std::runtime_error("dd4hep: Failed to parse the XML element with tag " 
                               + tag + " with the plugin " + type);
    }
    return;
  }
  throw std::runtime_error("dd4hep: Failed to parse the XML file [Invalid XML ROOT handle]");
}
