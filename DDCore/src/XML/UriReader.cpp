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
#include "XML/UriReader.h"

/// Default destructor
dd4hep::xml::UriReader::~UriReader()   {
}

/// Resolve a given URI to a string containing the data
bool dd4hep::xml::UriReader::load(const std::string& system_id, std::string& data)   {
  return this->load(system_id, context(), data);
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void dd4hep::xml::UriReader::parserLoaded(const std::string& system_id)  {
  this->parserLoaded(system_id, context());
}

/// Default constructor
dd4hep::xml::UriContextReader::UriContextReader(UriReader* reader, UriReader::UserContext* ctxt)
  : m_reader(reader), m_context(ctxt)
{
}

/// Copy constructor
dd4hep::xml::UriContextReader::UriContextReader(const UriContextReader& copy)
  : m_reader(copy.m_reader), m_context(copy.m_context)
{
}

/// Default destructor
dd4hep::xml::UriContextReader::~UriContextReader()   {
}

/// Resolve a given URI to a string containing the data
bool dd4hep::xml::UriContextReader::load(const std::string& system_id, std::string& data)   {
  return m_reader->load(system_id, context(), data);
}

/// Resolve a given URI to a string containing the data
bool dd4hep::xml::UriContextReader::load(const std::string& system_id, UserContext* ctxt, std::string& data)   {
  return m_reader->load(system_id, ctxt, data);
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void dd4hep::xml::UriContextReader::parserLoaded(const std::string& system_id)  {
  m_reader->parserLoaded(system_id, context());
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void dd4hep::xml::UriContextReader::parserLoaded(const std::string& system_id, UserContext* ctxt)  {
  m_reader->parserLoaded(system_id, ctxt);
}
