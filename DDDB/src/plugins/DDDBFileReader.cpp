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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBReader.h"
#include "DD4hep/IOV.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// Class supporting the interface of the LHCb conditions database to dd4hep
    /**
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class DDDBFileReader : public DDDBReader   {
      long m_validityLowerLimit = 0;
      long m_validityUpperLimit = IOV::MAX_KEY;

    public:
      /// Standard constructor
      DDDBFileReader();
      /// Default destructor
      virtual ~DDDBFileReader()  {}
      /// Read raw XML object from the database / file
      virtual int getObject(const std::string& system_id, UserContext* ctxt, std::string& data);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id, UserContext* ctxt);
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, std::string& buffer);
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, UserContext* ctxt, std::string& buffer);

    };
  }    /* End namespace DDDB            */
}      /* End namespace dd4hep          */


//==========================================================================
// Framework includes
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"

// C/C++ include files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/// Standard constructor
dd4hep::DDDB::DDDBFileReader::DDDBFileReader()
  : DDDBReader()
{
  declareProperty("ValidityLower",     m_context.valid_since);
  declareProperty("ValidityUpper",     m_context.valid_until);
  declareProperty("ValidityLowerLimit",m_validityLowerLimit);
  declareProperty("ValidityUpperLimit",m_validityUpperLimit);
}

int dd4hep::DDDB::DDDBFileReader::getObject(const std::string& system_id,
                                            UserContext*       /* ctxt */,
                                            std::string&       buffer)
{
  std::string path = system_id;
  int fid  = ::open(path.c_str(), O_RDONLY);

  if ( fid == -1 )   {
    std::string p = m_directory+system_id;
    if ( p.substr(0,7) == "file://" ) path = p.substr(6);
    else if ( p.substr(0,5) == "file:" ) path = p.substr(5);
    else path = p;
    fid  = ::open(path.c_str(), O_RDONLY);
  }
  if ( fid != -1 )   {
    struct stat buff;
    if ( 0 == ::fstat(fid, &buff) )  {
      int done = 0, len = buff.st_size;
      char* b  = new char[len+1];
      b[0] = 0;
      while ( done<len )  {
        int sc = ::read(fid, b+done, buff.st_size-done);
        if ( sc >= 0 ) { done += sc; continue; }
        break;
      }
      ::close(fid);
      b[done] = 0;
      buffer = b;
      delete [] b;
      if ( done>=len ) {
        return 1;
      }
      return 0;
    }
    ::close(fid);
  }
  return 0;
}

/// Resolve a given URI to a string containing the data
bool dd4hep::DDDB::DDDBFileReader::load(const std::string& system_id, std::string& buffer)   {
  return xml::UriReader::load(system_id, buffer);
}

/// Resolve a given URI to a string containing the data
bool dd4hep::DDDB::DDDBFileReader::load(const std::string& system_id,
                                        UserContext*  ctxt,
                                        std::string& buffer)
{
  return DDDBReader::load(system_id, ctxt, buffer);
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void dd4hep::DDDB::DDDBFileReader::parserLoaded(const std::string& system_id)  {
  DDDBReader::parserLoaded(system_id);
}

/// Inform reader about a locally (e.g. by XercesC) handled source load
void dd4hep::DDDB::DDDBFileReader::parserLoaded(const std::string& /* system_id */, UserContext* ctxt)  {
  DDDBReaderContext *ct = (DDDBReaderContext *)ctxt;
  // Adjust IOV period according to setup (files have no IOV)
  ct->valid_since = m_context.valid_since;
  ct->valid_until = m_context.valid_until;
  // Check lower bound
  if (ct->valid_since < m_validityLowerLimit)
    ct->valid_since = m_validityLowerLimit;
  else if (ct->valid_since > m_validityUpperLimit)
    ct->valid_since = m_validityUpperLimit;
  // Check upper bound
  if (ct->valid_until < m_validityLowerLimit)
    ct->valid_until = m_validityLowerLimit;
  else if (ct->valid_until > m_validityUpperLimit)
    ct->valid_until = m_validityUpperLimit;
  //DDDBReader::parserLoaded(system_id, ctxt);
}

namespace {
  void* create_dddb_xml_file_reader(const char* /* arg */) {
    return new dd4hep::DDDB::DDDBFileReader();
  }
}
DECLARE_CONSTRUCTOR(DDDB_FileReader,create_dddb_xml_file_reader)
