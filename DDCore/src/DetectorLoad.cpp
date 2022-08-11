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
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"

// C/C++ include files
#include <stdexcept>
#include <clocale>

#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace dd4hep {
  namespace xml {
    typedef xercesc::DOMException XmlException;
  }
}
#endif

using namespace dd4hep;

namespace  {
  bool verify_locale()   {
    /// We want reasonable parsing with XercesC.
    /// If the geometry is opened, set the locale to standard "C" if not yet already done.
    if ( std::strcmp(std::setlocale(LC_NUMERIC, ""), "C") != 0 ||
	 std::strcmp(std::setlocale(LC_TIME,    ""), "C") != 0 ||
	 std::strcmp(std::setlocale(LC_CTYPE,   ""), "C") != 0 )   {
      std::setlocale(LC_ALL,"C");
      printout(ALWAYS,"DetectorLoad","++ Setting locale to \"C\".");
    }
    return true;
  }
}

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
  verify_locale();
  try {
    xml::DocumentHolder doc(xml::DocumentHandler().load(xmlfile,entity_resolver));
    if ( doc )   {
      xml::Handle_t handle = doc.root();
      if ( handle )   {
        processXMLElement(xmlfile,handle);
        return;
      }
    }
    except("DetectorLoad","Failed to parse the XML file %s [Invalid XML ROOT handle]", xmlfile.c_str());
  }
  catch (const xml::XmlException& e) {
    except("DetectorLoad","%s\ndd4hep: XML-DOM Exception while parsing %s", xml::_toString(e.msg).c_str(), xmlfile.c_str());
  }
  catch (const std::exception& e) {
    except("DetectorLoad","%s\ndd4hep: Exception while parsing %s", e.what(), xmlfile.c_str());
  }
  catch (...) {
    except("DetectorLoad","dd4hep: UNKNOWN exception while parsing %s", xmlfile.c_str());
  }
}

/// Process XML unit and adopt all data from source structure.
void DetectorLoad::processXML(const xml::Handle_t& base, const std::string& xmlfile, xml::UriReader* entity_resolver) {
  verify_locale();
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
    except("DetectorLoad","Failed to parse the XML file %s [Invalid XML ROOT handle]", xmlfile.c_str());
  }
  catch (const xml::XmlException& e) {
    except("DetectorLoad","%s\ndd4hep: XML-DOM Exception while parsing %s.", xml::_toString(e.msg).c_str(), xmlfile.c_str());
  }
  catch (const std::exception& e) {
    except("DetectorLoad","%s\ndd4hep: Exception while parsing %s.", e.what(), xmlfile.c_str());
  }
  catch (...) {
    except("DetectorLoad","dd4hep: UNKNOWN exception while parsing %s.", xmlfile.c_str());
  }
}

/// Process XML unit and adopt all data from source string in momory.
void DetectorLoad::processXMLString(const char* xmldata)   {
  processXMLString(xmldata, 0);
}

/// Process XML unit and adopt all data from source string in momory. Subsequent parsers may use the entity resolver.
void DetectorLoad::processXMLString(const char* xmldata, xml::UriReader* entity_resolver) {
  try {
    if ( xmldata && verify_locale() )   {
      xml::DocumentHolder doc(xml::DocumentHandler().parse(xmldata,::strlen(xmldata),"In-Memory",entity_resolver));
      if ( doc )   {
        xml::Handle_t handle = doc.root();
        if ( handle )   {
          processXMLElement("In-Memory-XML",handle);
          return;
        }
      }
    }
    except("DetectorLoad","processXMLString: Invalid XML In-memory source [NULL]");
  }
  catch (const xml::XmlException& e) {
    except("DetectorLoad","%s\ndd4hep: XML-DOM Exception while parsing XML in-memory string.", xml::_toString(e.msg).c_str());
  }
  catch (const std::exception& e) {
    except("DetectorLoad","%s\ndd4hep: Exception while parsing XML in-memory string.", e.what());
  }
  catch (...) {
    except("DetectorLoad","dd4hep: UNKNOWN exception while parsing XML in-memory string.");
  }
}

/// Process a given DOM (sub-) tree
void DetectorLoad::processXMLElement(const std::string& xmlfile, const xml::Handle_t& xml_root) {
  if ( xml_root.ptr() && verify_locale() )   {
    std::string tag = xml_root.tag();
    std::string type = tag + "_XML_reader";
    xml::Handle_t handle = xml_root;
    long result = PluginService::Create<long>(type, m_detDesc, &handle);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(type, m_detDesc, &handle);
      if ( 0 == result )  {
	except("DetectorLoad",
	       "Failed to locate plugin to interprete files of type \"%s\"  - no factory: %s. %s",
	       tag.c_str(), type.c_str(), dbg.missingFactory(type).c_str());
      }
    }
    result = *(long*) result;
    if (result != 1) {
      except("DetectorLoad",
	     "Failed to parse the XML file %s with the plugin %s.", xmlfile.c_str(), type.c_str());
    }
    return;
  }
  except("DetectorLoad",
	 "Failed to parse the XML file %s [Invalid XML ROOT handle].", xmlfile.c_str());
}

/// Process a given DOM (sub-) tree
void DetectorLoad::processXMLElement(const xml::Handle_t& xml_root, DetectorBuildType /* type */) {
  if ( xml_root.ptr() && verify_locale() )   {
    std::string tag = xml_root.tag();
    std::string type = tag + "_XML_reader";
    xml::Handle_t handle = xml_root;
    long result = PluginService::Create<long>(type, m_detDesc, &handle);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(type, m_detDesc, &handle);
      if ( 0 == result )  {
	except("DetectorLoad",
	       "Failed to locate plugin to interprete files of type \"%s\"  - no factory: %s. %s",
	       tag.c_str(), type.c_str(), dbg.missingFactory(type).c_str());
      }
    }
    result = *(long*) result;
    if (result != 1)   {
      except("DetectorLoad",
	     "Failed to parse the XML element with tag %s with the plugin %s.", tag.c_str(), type.c_str());
    }
    return;
  }
  except("DetectorLoad", "Failed to parse the XML file [Invalid XML ROOT handle].");
}
