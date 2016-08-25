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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// Class supporting the interface of the LHCb conditions database to DD4hep
    /**
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class DDDBFileReader : public DDDBReader   {
    public:
      /// Standard constructor
      DDDBFileReader() : DDDBReader() {}
      /// Default destructor
      virtual ~DDDBFileReader()  {}
      /// Read raw XML object from the database / file
      virtual int getObject(const std::string& system_id, UserContext* ctxt, std::string& data);

    };
  }    /* End namespace DDDB            */
}      /* End namespace DD4hep          */


//==========================================================================
// Framework includes
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int DD4hep::DDDB::DDDBFileReader::getObject(const std::string& system_id,
                                            UserContext* /* ctxt */,
                                            std::string& buffer)
{
  std::string path = m_directory+system_id;
  struct stat buff;
  if ( 0 == ::stat(path.c_str(), &buff) )  {
    int fid  = ::open(path.c_str(), O_RDONLY);
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
    if ( done>=len ) return 1;
  }
  return 0;
}

namespace {
  void* create_dddb_xml_file_reader(const char* /* arg */) {
    return new DD4hep::DDDB::DDDBFileReader();
  }
}
DECLARE_CONSTRUCTOR(DDDB_FileReader,create_dddb_xml_file_reader)
