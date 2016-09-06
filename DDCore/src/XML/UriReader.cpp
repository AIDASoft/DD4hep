// $Id$
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
#include "XML/UriReader.h"

/// Default destructor
DD4hep::XML::UriReader::~UriReader()   {
}

/// Resolve a given URI to a string containing the data
bool DD4hep::XML::UriReader::load(const std::string& system_id, std::string& data)   {
  return this->load(system_id, context(), data);
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void DD4hep::XML::UriReader::parserLoaded(const std::string& system_id)  {
  this->parserLoaded(system_id, context());
}

/// Default constructor
DD4hep::XML::UriContextReader::UriContextReader(UriReader* reader, UriReader::UserContext* ctxt)
  : m_reader(reader), m_context(ctxt)
{
}

/// Copy constructor
DD4hep::XML::UriContextReader::UriContextReader(const UriContextReader& copy)
  : m_reader(copy.m_reader), m_context(copy.m_context)
{
}

/// Default destructor
DD4hep::XML::UriContextReader::~UriContextReader()   {
}

/// Resolve a given URI to a string containing the data
bool DD4hep::XML::UriContextReader::load(const std::string& system_id, std::string& data)   {
  return m_reader->load(system_id, context(), data);
}

/// Resolve a given URI to a string containing the data
bool DD4hep::XML::UriContextReader::load(const std::string& system_id, UserContext* ctxt, std::string& data)   {
  return m_reader->load(system_id, ctxt, data);
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void DD4hep::XML::UriContextReader::parserLoaded(const std::string& system_id)  {
  m_reader->parserLoaded(system_id, context());
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void DD4hep::XML::UriContextReader::parserLoaded(const std::string& system_id, UserContext* ctxt)  {
  m_reader->parserLoaded(system_id, ctxt);
}
