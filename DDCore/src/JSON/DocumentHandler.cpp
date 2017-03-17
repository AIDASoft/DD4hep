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
#include "JSON/DocumentHandler.h"

// C/C++ include files
#include "boost/property_tree/json_parser.hpp"
#include <memory>
#include <stdexcept>

using namespace std;
using namespace DD4hep::JSON;

/// Default constructor
DocumentHandler::DocumentHandler()  {
}
                                    
/// Default destructor
DocumentHandler::~DocumentHandler()   {
}

/// Load XML file and parse it.
Document DocumentHandler::load(const string& fname) const   {
  string fn = fname;
  if ( fname.find("://") != string::npos ) fn = fname.substr(fname.find("://")+3);
  //string cmd = "cat "+fn;
  //::printf("\n\n+++++ Dump json file: %s\n\n\n",fn.c_str());
  //::system(cmd.c_str());
  unique_ptr<JsonElement> doc(new JsonElement(fn, ptree()));
  boost::property_tree::read_json(fn,doc->second);
  return doc.release();
}

/// Parse a standalong XML string into a document.
Document DocumentHandler::parse(const char* doc_string, size_t length) const   {
  if ( doc_string && length ) {}
  throw runtime_error("Bla");
}
