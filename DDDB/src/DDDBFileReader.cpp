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
#include "DDDB/DDDBFileReader.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"
#include "XML/DocumentHandler.h"
#include "XML/XMLElements.h"

// C/C++ include files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cerrno>
#include "boost/filesystem/path.hpp"
#include "TGeoManager.h"
#include "TRint.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

int DDDBFileReader::getObject(const string& system_id,
			      UserContext* /* ctxt */,
			      string& buffer)
{
  string path = m_directory+system_id;
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

/// Resolve a given URI to a string containing the data
bool DDDBFileReader::load(const string& system_id,
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
    printout(ERROR,"DDDBFileReader","++ Failed to resolve system id: %s [%s]",
             id.c_str(), ::strerror(errno));
  }
  return false;
}

static void check_result(long result)   {
  if ( 0 == result )
    except("DDDBFileLoader","+++ Odd things going on.....");
  //result = *(long*)result;
  if ( result != 1 )
    except("DDDBFileLoader","+++ Odd things going on.....Processing result=%ld",result);
}

static void usage_load_xml_dddb()   {
  cout <<
    "DDDBFileLoader opt [opt]\n"
    "   Note: No '-' signs infront of identifiers!           \n"
    "                                                        \n"
    "   param <file-name>   Preprocessing xml file           \n"
    "   input <file-name>   Directory containing DDDB        \n"
    "   visualize           Call display after processing    \n"
    "   match=<string>      Match string for entity resolver \n"
    "" << endl;

  ::exit(EINVAL);
}

static long load_xml_dddb(Geometry::LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    string sys_id, params, match="conddb:", attr="";
    //DD4hep::LCDDBuildType type = BUILD_DEFAULT;
    long result = 0, visualize = 0, dump = 0;
    DDDBFileReader resolver;

    for(int i=0; i<argc;++i) {
      char c = ::toupper(argv[i][0]);
      switch(c)  {
      case 'A':
        attr = argv[++i];
        break;
      case 'D':
        dump = 1;
        break;
      case 'P':
        params = argv[++i];
        break;
      case 'I':
        sys_id = argv[++i];
        break;
      case 'M':
        match = argv[++i];
        break;
      case 'V':
        visualize = 1;
        break;
      case 'H':
      case '?':
      default:
        usage_load_xml_dddb();
      }
    }
    boost::filesystem::path path = sys_id;
    sys_id = path.normalize().c_str();
    resolver.setMatch(match);
    resolver.setDirectory(path.parent_path().c_str());
    { /// Install helper
      char* arguments[] = {(char*)&resolver, 0};
      lcdd.apply("DDDBInstallHelper", 1, arguments);
    }
    if ( !params.empty() )    { /// Pre-Process Parameters
      XML::DocumentHolder doc(XML::DocumentHandler().load(params, &resolver));
      XML::Handle_t handle = doc.root();
      char* arguments[] = {(char*)handle.ptr(), 0};
      printout(INFO,"DDDBFileLoader","+++ Pre-processing parameters....");
      result = lcdd.apply("DD4hepXMLProcessor", 1, arguments);
      check_result(result);
      printout(INFO,"DDDBFileLoader","                         .... done");
    }
    if ( !sys_id.empty() )   { /// Process XML
      XML::DocumentHolder doc(XML::DocumentHandler().load(sys_id, &resolver));
      XML::Handle_t handle = doc.root();
      char* arguments[] = {(char*)handle.ptr(), 0};
      printout(INFO,"DDDBFileLoader","+++ Processing DDDB: %s", sys_id.c_str());
      result = lcdd.apply("DD4hepXMLProcessor", 1, arguments);
      check_result(result);
      printout(INFO,"DDDBFileLoader","                         .... done");
    }
    if ( !attr.empty() )  {
      XML::DocumentHolder doc(XML::DocumentHandler().load(attr, &resolver));
      XML::Handle_t handle = doc.root();
      char* arguments[] = {(char*)handle.ptr(), 0};
      printout(INFO,"DDDBFileLoader","+++ Processing attrs: %s", attr.c_str());
      result = lcdd.apply("DD4hepXMLProcessor", 1, arguments);
      check_result(result);
    }
    { /// Convert local database to TGeo
      result = lcdd.apply("DDDB2DD4hep", 0, 0);
      check_result(result);
    }
    if ( dump )    {
      printout(INFO,"DDDBFileLoader","------------------> Volume tree dump:");
      lcdd.apply("DD4hepVolumeDump", 0, 0);
      printout(INFO,"DDDBFileLoader","------------------> DetElement tree dump:");
      lcdd.apply("DD4hepDetectorDump", 0, 0);
    }
    if ( visualize )   { /// Fire off display
      pair<int, char**> a(0,0);
      TRint app("DDDB", &a.first, a.second);
      TGeoManager& mgr = lcdd.manager();
      mgr.SetVisLevel(999);
      mgr.SetVisOption(1);
      lcdd.worldVolume()->Draw("ogl");
      app.Run();
    }
  }
  return 1;
}
DECLARE_APPLY(DDDBFileLoader,load_xml_dddb)
