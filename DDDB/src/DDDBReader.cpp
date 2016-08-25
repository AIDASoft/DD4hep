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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBReader.h"
#include "DD4hep/Printout.h"
//#include "DD4hep/LCDD.h"
//#include "XML/DocumentHandler.h"
//#include "XML/XMLElements.h"

// C/C++ include files
#include <ctime>
#include <cstring>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

/// Standard constructor
DDDBReader::DDDBReader(const std::string& dir)
  : m_directory(dir), m_match("conddb:")
{
	m_context.valid_since = makeTime(1970,1,1);
	m_context.valid_until = makeTime(2030,1,1);
	m_context.event_time  = makeTime(2015,7,1,12,0,0);
}


long long int DDDBReader::makeTime(int year, int month, int day,
                                   int hour, int minutes, int seconds)
{
  struct tm tm_init;
  ::memset(&tm_init,0,sizeof(tm_init));
  tm_init.tm_year = year;
  tm_init.tm_mon  = month;
  tm_init.tm_mday = day;
  tm_init.tm_hour = hour;
  tm_init.tm_min  = minutes;
  tm_init.tm_sec  = seconds;
  return ::mktime(&tm_init);
}

/// Resolve a given URI to a string containing the data
bool DDDBReader::load(const string& system_id, string& buffer)   {
  return XML::UriReader::load(system_id, buffer);
}

/// Resolve a given URI to a string containing the data
bool DDDBReader::load(const string& system_id,
                          UserContext*  ctxt,
                          string& buffer)
{
  if ( system_id.substr(0,m_match.length()) == m_match )  {
    string mm = m_match + "//";
    const string& sys = system_id;
    string id = sys.c_str() + (sys.substr(0,mm.length()) == mm ? 9 : 7);
    // Extract the COOL field name from the condition path
    // "conddb:/path/to/field@folder"
    string::size_type at_pos = id.find('@');
    if ( at_pos != id.npos ) {
      string::size_type slash_pos = id.rfind('/',at_pos);
      // always remove '@' from the path
      id = id.substr(0,slash_pos+1) +  id.substr(at_pos+1);
    }
    // GET: 1458055061070516000 /lhcb.xml 0 0 SUCCESS
    int ret = getObject(id, ctxt, buffer);
    if ( ret == 1 ) return true;
    printout(ERROR,"DDDBReader","++ Failed to resolve system id: %s [%s]",
             id.c_str(), ::strerror(errno));
  }
  return false;
}

int DDDBReader::getObject(const string& sys_id, UserContext* /* ctxt */, string& /* out */)
{
  except("DDDBReader","DDDBReader::getObject is a virtual method, "
         "which must be overloaded by the concrete instance!! sys_id:%s", sys_id.c_str());
  return 0;
}

