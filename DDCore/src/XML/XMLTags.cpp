// $Id$
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
#include "XML/XMLElements.h"
#ifndef __TIXML__
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#endif

// C/C++ include files
#include <stdexcept>
#include <iostream>
#include <map>


namespace {
  struct __Init {
    typedef std::map<std::string,DD4hep::XML::Tag_t*> Inventory_t;
    Inventory_t m_inventory;
    __Init() {
#ifndef __TIXML__
      try {
        xercesc::XMLPlatformUtils::Initialize();
      }
      catch (const xercesc::XMLException& e) {
        std::string xml_err = xercesc::XMLString::transcode(e.getMessage());
        std::string err = "xercesc::XMLPlatformUtils: Xerces-c error in initialization:"+xml_err;
        std::cout << err << std::endl;
        throw std::runtime_error(err);
      }
#endif
    }
    ~__Init() {
#ifndef __TIXML__
      xercesc::XMLPlatformUtils::Terminate();
#endif
    }
    static void register_tag(const std::string& name, DD4hep::XML::Tag_t* tag);
  };
  __Init  __In__;
  void __Init::register_tag(const std::string& name, DD4hep::XML::Tag_t* tag)  {
    Inventory_t::const_iterator i = __In__.m_inventory.find(name);
    if ( i !=  __In__.m_inventory.end() )  {
      std::string err = "XMLTags: Failed to register XML tag: "+name+". [Tag already exists]";
      std::cout << err << std::endl;
      throw std::runtime_error(err);
    }
    __In__.m_inventory[name] = tag;
  }
}

#define UNICODE(x)        extern const Tag_t Unicode_##x (#x, #x, __Init::register_tag)
#include "XML/UnicodeValues.h"

namespace DD4hep {
  namespace XML {

    extern const Tag_t Unicode_NULL("NULL", "0", __Init::register_tag);
    extern const Tag_t Unicode_empty("", "", __Init::register_tag);
    extern const Tag_t Unicode_star("star", "*", __Init::register_tag);
    extern const Tag_t Unicode_PI("PI", "3.14159265358979323846", __Init::register_tag);
    extern const Tag_t Unicode_TWOPI("TWOPI", "6.28318530717958647692", __Init::register_tag);

    void tags_init() {
      static __Init i;
    }

  }
}
