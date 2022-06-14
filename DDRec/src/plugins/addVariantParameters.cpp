//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : P.Gessinger
//
//==========================================================================
#include "DD4hep/Detector.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"

#include "DDRec/DetectorData.h"

#include <regex>


namespace dd4hep::rec {

/**
 *  @author P.Gessinger, CERN
 *  @date May, 25 2022
 *
 *  \brief Plugin which attaches parameters with string keys and int, double, 
 *  string or bool values to detector elements
 *
 *  This plugin can be configured to attach a varying number of variant paremeters
 *  to a single detector elements. The first argument is the detector element name.
 *  Any following arguments should conform to the following format:
 *    
 *    key: type = value
 *  
 *  where type can be one of "int", "bool", "double", or "str", with "str" being the default.
 *  This will ensure an instance of the @c VariantParameters extension is attached to a detector
 *  element, and insert @c key with a @c value of type @c type.
 */

static long addVariantParameters(Detector& description, int argc, char** argv) {

  if(argc < 2) {
    printout(ERROR,"ParametersPlugin","+++ Invalid arguments");
    return 0;
  }

  printout(INFO,"ParametersPlugin","+++ Applying %d parameters", argc);


  static std::regex expr{"^(\\w+)(?:: ?(.*?)?)? ?= ?(.*)"};

  std::string detector = argv[0];

  for(int i=1;i<argc;i++) {
    std::string kv = argv[i];
    std::smatch m;
    if(!std::regex_match(kv, m, expr)) {
      throw std::runtime_error("Unable to parse key-value pair to assign");
    }
    if (m.size() != 4) {
      throw std::runtime_error("Unable to parse key-value pair to assign");
    }

    std::string key = m.str(1);
    std::string type = m.str(2);
    if (type == "") {
      type = "str";
    }
    std::string value = m.str(3);

    printout(INFO,"ParametersPlugin","+++ %s -> %s: %s = %s", detector.c_str(), key.c_str(), type.c_str(), value.c_str());

    DetElement elt = description.detector(detector);
    if(!elt.isValid()){
      printout(ERROR,"ParametersPlugin","+++ Did not find element with name '%s'", detector.c_str());
      return 0;
    }

    VariantParameters* extension = elt.extension<VariantParameters>(false);
    if(extension == nullptr) {
      extension = new VariantParameters();
      elt.addExtension<VariantParameters>(extension);
    }

    if (type == "str" || type == "string") {
      extension->variantParameters[key] = value;
    }
    else if (type == "double") {
      extension->variantParameters[key] = dd4hep:_toDouble(value);
    }
    else if (type == "int") {
      extension->variantParameters[key] = dd4hep::_toInt(value);
    }
    else if (type == "bool") {
      if (value == "true") {
        extension->variantParameters[key] = true;
      }
      else if (value == "false") {
        extension->variantParameters[key] = false;
      }
      else {
        throw std::runtime_error{"Invalid boolean value: " + value};
      }
    }
    else {
      throw std::runtime_error{"Unknown type '"+type+"'"};
    }
  }


  return 1;
}

}

DECLARE_APPLY(DD4hep_ParametersPlugin, dd4hep::rec::addVariantParameters)

