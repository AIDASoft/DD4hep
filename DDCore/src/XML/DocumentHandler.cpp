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
#include "XML/Printout.h"
#include "XML/UriReader.h"
#include "XML/DocumentHandler.h"

// C/C++ include files
#include <memory>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <libgen.h>
#endif
#include "TSystem.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::xml;

namespace {
  string undressed_file_name(const string& fn)   {
    if ( !fn.empty() )   {
      TString tfn(fn);
      gSystem->ExpandPathName(tfn);
      return string(tfn.Data());
    }
    return fn;
  }
}

#ifndef __TIXML__
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/MemBufFormatTarget.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/sax/EntityResolver.hpp"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/XMLEntityResolver.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/util/XMLUni.hpp"
#include "xercesc/util/XMLURL.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/ErrorHandler.hpp"

using namespace xercesc;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace xml {

    /// XML-DOM ERror handler class for the XercesC document parser.
    class DocumentErrorHandler: public ErrorHandler, public DOMErrorHandler {
    public:
      /// Constructor
      DocumentErrorHandler() {
      }
      /// Destructor
      virtual ~DocumentErrorHandler()  {
        printout(DEBUG,"DocumentErrorHandler","+++ Destructing the XercesC DOM-XML document error handler....");
      }
      /// Reset errors (Noop)
      void resetErrors() {
      }
      /// Warnings callback. Ignore them
      void warning(const SAXParseException& /* e */) {
      }
      /// Error handler
      void error(const SAXParseException& e);
      /// Fatal error handler
      void fatalError(const SAXParseException& e);
      /// Dom Error handler callback
      virtual bool handleError(const DOMError& domError);
    };

    /// Dom Error handler callback
    bool DocumentErrorHandler::handleError(const DOMError& domError) {
      string err = "DOM UNKNOWN: ";
      switch (domError.getSeverity()) {
      case DOMError::DOM_SEVERITY_WARNING:
        err = "DOM WARNING: ";
        break;
      case DOMError::DOM_SEVERITY_ERROR:
        err = "DOM ERROR:   ";
        break;
      case DOMError::DOM_SEVERITY_FATAL_ERROR:
        err = "DOM FATAL:   ";
        break;
      default:
        return false;
      }
      printout(FATAL,"DocumentErrorHandler", "+++ %s %s: %s", err.c_str(),
               _toString(domError.getType()).c_str(),_toString(domError.getMessage()).c_str());
      DOMLocator* loc = domError.getLocation();
      if (loc) {
        printout(FATAL,"DocumentErrorHandler","+++ Location: Line:%d Column: %d",
                 int(loc->getLineNumber()),int(loc->getColumnNumber()));
      }
      return false;
    }
    /// Error handler
    void DocumentErrorHandler::error(const SAXParseException& e) {
      string m(_toString(e.getMessage()));
      if (m.find("The values for attribute 'name' must be names or name tokens") != string::npos
          || m.find("The values for attribute 'ID' must be names or name tokens") != string::npos
          || m.find("for attribute 'name' must be Name or Nmtoken") != string::npos
          || m.find("for attribute 'ID' must be Name or Nmtoken") != string::npos
          || m.find("for attribute 'name' is invalid Name or NMTOKEN value") != string::npos
          || m.find("for attribute 'ID' is invalid Name or NMTOKEN value") != string::npos)
        return;
      string sys(_toString(e.getSystemId()));
      printout(ERROR,"XercesC","+++ Error at file \"%s\", Line %d Column: %d Message:%s",
               sys.c_str(), int(e.getLineNumber()), int(e.getColumnNumber()), m.c_str());
    }
    /// Fatal error handler
    void DocumentErrorHandler::fatalError(const SAXParseException& e) {
      string m(_toString(e.getMessage()));
      string sys(_toString(e.getSystemId()));
      printout(FATAL,"XercesC","+++ FATAL Error at file \"%s\", Line %d Column: %d Message:%s",
               sys.c_str(), int(e.getLineNumber()), int(e.getColumnNumber()), m.c_str());
    }

    namespace {

      /// Specialized DOM parser to handle special system IDs
      class dd4hepDOMParser : public XercesDOMParser      {
        /// Pointer to URI reader
        UriReader*           m_reader;
        /// Xerces Error handler
        DocumentErrorHandler m_errHandle_tr;
        class Resolver : public XMLEntityResolver {
          dd4hepDOMParser* parser;
        public:
          Resolver(dd4hepDOMParser* p) : parser(p) {}
          virtual ~Resolver() {}
          virtual InputSource *resolveEntity(XMLResourceIdentifier *id)
          {   return parser->read_uri(id);                            }
        };
        Resolver m_resolver;
      public:
        /// Initializing constructor
        dd4hepDOMParser(UriReader* rdr) : XercesDOMParser(), m_reader(rdr), m_resolver(this)  {
          //printout(FATAL,"XercesC","+++ Creating new parser");
          setErrorHandler(&m_errHandle_tr);
          setXMLEntityResolver(&m_resolver);
        }
        /// Default destructor
        virtual ~dd4hepDOMParser() {
          //printout(FATAL,"XercesC","+++ Deleting new parser");
        }
        /// Entity resolver overload to use uri reader
        InputSource *read_uri(XMLResourceIdentifier *id)   {
          if ( m_reader )   {
            string buf, systemID(_toString(id->getSystemId()));
            if ( m_reader->load(systemID, buf) )  {
              const XMLByte* input = (const XMLByte*)XMLString::replicate(buf.c_str());
#if 0
              string baseURI(_toString(id->getBaseURI()));
              string schema(_toString(id->getSchemaLocation()));
              string ns(_toString(id->getNameSpace()));
              printout(INFO,"XercesC","+++ Resolved URI: sysID:%s uri:%s ns:%s schema:%s",
                       systemID.c_str(), baseURI.c_str(), ns.c_str(), schema.c_str());
#endif
              return new MemBufInputSource(input,buf.length(),systemID.c_str(),true);
            }
          }
          return 0;
        }
      };

      /// Helper function to create a XercesC pareser
      XercesDOMParser* make_parser(UriReader* reader=0) {
        XercesDOMParser* parser = new dd4hepDOMParser(reader);
        parser->setValidationScheme(XercesDOMParser::Val_Auto);
        parser->setValidationSchemaFullChecking(true);
        parser->setCreateEntityReferenceNodes(false);
        parser->setDoNamespaces(false);
        parser->setDoSchema(true);
        return parser;
      }
    }

    /// Dump DOM tree using XercesC handles
    void dumpTree(DOMNode* doc, ostream& os) {
      if ( doc )  {
        DOMImplementation  *imp = DOMImplementationRegistry::getDOMImplementation(Strng_t("LS"));
        MemBufFormatTarget *tar = new MemBufFormatTarget();
        DOMLSOutput        *out = imp->createLSOutput();
        DOMLSSerializer    *wrt = imp->createLSSerializer();
        out->setByteStream(tar);
        wrt->getDomConfig()->setParameter(Strng_t("format-pretty-print"), true);
        wrt->write(doc, out);
        os << tar->getRawBuffer() << endl << flush;
        out->release();
        wrt->release();
        return;
      }
      printout(ERROR,"dumpTree","+++ Cannot dump invalid document.");
    }

    /// Dump DOM tree using XercesC handles
    void dump_doc(DOMDocument* doc, ostream& os) {
      dumpTree(doc,os);
    }
    /// Dump DOM tree using XercesC handles
    void dump_tree(Handle_t elt, ostream& os) {
      dumpTree((DOMNode*)elt.ptr(),os);
    }
    /// Dump DOM tree using XercesC handles
    void dump_tree(Document doc, ostream& os) {
      dump_doc((DOMDocument*)doc.ptr(),os);
    }
  }
}

#ifdef DD4HEP_NONE
/// System ID of a given XML entity
string DocumentHandler::system_path(Handle_t base, const string& fn)   {
  string path = system_path(base);
  string dir  = ::dirname((char*)path.c_str());
  return dir+fn;
}
#else

#include "TUri.h"
#include "TUrl.h"
#endif

/// System ID of a given XML entity
string DocumentHandler::system_path(Handle_t base, const string& fn)   {
  string path, dir = system_path(base);
  TUri uri_base(dir.c_str()), uri_rel(fn.c_str());
  TUrl url_base(dir.c_str());
  path = TUri::MergePaths(uri_rel,uri_base);
  TUri final(path.c_str());
  final.Normalise();
  path = url_base.GetProtocol()+string("://")+final.GetUri().Data();
  if ( path[path.length()-1]=='/' ) path = path.substr(0,path.length()-1);
  return path;
}

/// System ID of a given XML entity
string DocumentHandler::system_path(Handle_t base)   {
  DOMElement* elt = (DOMElement*)base.ptr();
  string path = _toString(elt->getBaseURI());
  if ( path[0] == '/' )  {
    string tmp = "file:"+path;
    return tmp;
  }
  return path;
}

/// Load secondary XML file with relative addressing with respect to handle
Document DocumentHandler::load(Handle_t base, const XMLCh* fname, UriReader* reader) const {
  string path;
  DOMElement* elt = (DOMElement*)base.ptr();
  try  {
    Document doc;
    Strng_t p = _toString(fname);    
    path      = _toString(fname);
    /// This is a bit complicated, but if the primary source is in-memory
    try  {
      XMLURL  ref_url(elt->getBaseURI(), p);
      path = _toString(ref_url.getURLText());
    }
    catch(...)   {
    }
    return load(path, reader);
  }
  catch(const exception& exc)   {
    string b = _toString(elt->getBaseURI());
    string e = _toString(fname);
    printout(DEBUG,"DocumentHandler","+++ URI exception: %s -> %s [%s]",b.c_str(),e.c_str(),exc.what());
  }
  catch(...)   {
    string b = _toString(elt->getBaseURI());
    string e = _toString(fname);
    printout(DEBUG,"DocumentHandler","+++ URI exception: %s -> %s",b.c_str(),e.c_str());
  }
  if ( reader )   {
    string buf, sys = system_path(base,fname);
#if 0
    string buf, sys, dir = _toString(elt->getBaseURI());
    string fn = _toString(fname);
    dir = ::dirname((char*)dir.c_str());
    while( fn.substr(0,3) == "../" )  {
      dir = ::dirname((char*)dir.c_str());
      fn = fn.substr(3);
    }
    sys = dir + "/" + fn;
#endif
    if ( reader->load(sys, buf) )  {
#if 0
      Document doc = parse(buf.c_str(), buf.length(), sys.c_str(), reader);
      dumpTree(doc);
      return doc;
#endif
      return parse(buf.c_str(), buf.length(), sys.c_str(), reader);
    }
  }
  return Document(0);
}

/// Load XML file and parse it using URI resolver to read data.
Document DocumentHandler::load(const string& fname, UriReader* reader) const   {
  string path;
  printout(DEBUG,"DocumentHandler","+++ Loading document URI: %s",fname.c_str());
  try  {
    size_t idx = fname.find(':');
    size_t idq = fname.find('/');
    if ( idq == string::npos ) idq = 0;
    XMLURL xerurl = (const XMLCh*) Strng_t(idx==string::npos || idx>idq ? "file:"+fname : fname);
    string proto  = _toString(xerurl.getProtocolName());
    path = _toString(xerurl.getPath());
    printout(DEBUG,"DocumentHandler","+++             protocol:%s path:%s",proto.c_str(), path.c_str());
  }
  catch(...)   {
  }
  unique_ptr < XercesDOMParser > parser(make_parser(reader));
  try {
    if ( !path.empty() )  {
      parser->parse(path.c_str());
      if ( reader ) reader->parserLoaded(path);
    }
    else   {
      if ( reader && reader->load(fname, path) )  {
        MemBufInputSource src((const XMLByte*)path.c_str(), path.length(), fname.c_str(), false);
        parser->parse(src);
        return (XmlDocument*)parser->adoptDocument();
      }
      return (XmlDocument*)0;
    }
  }
  catch (const exception& e) {
    printout(ERROR,"DocumentHandler","+++ Exception(XercesC): parse(path):%s",e.what());
    try {
      parser->parse(fname.c_str());
      if ( reader ) reader->parserLoaded(path);
    }
    catch (const exception& ex) {
      printout(FATAL,"DocumentHandler","+++ Exception(XercesC): parse(URI):%s",ex.what());
      throw;
    }
  }
  printout(DEBUG,"DocumentHandler","+++ Document %s succesfully parsed with XercesC .....",path.c_str());
  return (XmlDocument*)parser->adoptDocument();
}

/// Parse a standalong XML string into a document.
Document DocumentHandler::parse(const char* bytes, size_t length, const char* sys_id, UriReader* rdr) const {
  unique_ptr < XercesDOMParser > parser(make_parser(rdr));
  MemBufInputSource src((const XMLByte*)bytes, length, sys_id, false);
  parser->parse(src);
  DOMDocument* doc = parser->adoptDocument();
  doc->setXmlStandalone(true);
  doc->setStrictErrorChecking(true);
  return (XmlDocument*) doc;
}

/// Write xml document to output file (stdout if file name empty)
int DocumentHandler::output(Document doc, const string& fname) const {
  XMLFormatTarget *tar = 0;
  DOMImplementation *imp = DOMImplementationRegistry::getDOMImplementation(Strng_t("LS"));
  DOMLSOutput *out = imp->createLSOutput();
  DOMLSSerializer *wrt = imp->createLSSerializer();

  if (fname.empty())
    tar = new StdOutFormatTarget();
  else   {
    string fn = undressed_file_name(fname);
    tar = new LocalFileFormatTarget(Strng_t(fn));
  }
  out->setByteStream(tar);
  wrt->getDomConfig()->setParameter(Strng_t("format-pretty-print"), true);
  wrt->write((xercesc::DOMDocument*) doc.ptr(), out);
  out->release();
  wrt->release();
  delete tar;
  return 1;
}

#else

#include "XML/tinyxml.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace xml {

    /// XML-DOM ERror handler class for the TinyXML document parser (Compatibility class)
    class DocumentErrorHandler {};

    union Xml {
      Xml(void* ptr) : p(ptr) {}
      Xml(const void* ptr) : cp(ptr) {}
      void* p;
      const void* cp;
      TiXmlElement* e;
      XmlElement* xe;
      TiXmlAttribute* a;
      Attribute attr;
      TiXmlNode* n;
      TiXmlDocument* d;
      XmlDocument* xd;
    };
  }}

namespace {
  static string _clean_fname(const string& s) {
    std::string const& temp = getEnviron(s);
    std::string temp2 = undressed_file_name(temp.empty() ? s : temp);
    if ( strncmp(temp2.c_str(),"file:",5)==0 ) return temp2.substr(5);
    return temp2;
  }
}

/// System ID of a given XML entity
string DocumentHandler::system_path(Handle_t base, const string& fname)   {
  string fn, clean = _clean_fname(fname);
  struct stat st;
  Element elt(base);
  // Poor man's URI handling. Xerces is much much better here
  if ( elt ) {
    string bn = Xml(elt.document()).d->Value();
#ifdef _WIN32
    char drive[_MAX_DRIVE], dir[_MAX_DIR], file[_MAX_FNAME], ext[_MAX_EXT];
    ::_splitpath(bn.c_str(),drive,dir,file,ext);
    fn = drive;
    fn += ":";
    fn += dir;
    fn += "/";
    fn += clean;
#else
    fn = ::dirname((char*)bn.c_str());
#endif
    fn += "/";
    fn += _clean_fname(fname);
  }
  if ( ::stat(fn.c_str(),&st)==0 )
    return fn;
  else if ( ::stat(clean.c_str(),&st)==0 )
    return clean;
  return fname;
}

/// System ID of a given XML entity
string DocumentHandler::system_path(Handle_t base)   {
  string fn;
  Element elt(base);
  // Poor man's URI handling. Xerces is much much better here
  if ( elt ) {
    fn = Xml(elt.document()).d->Value();
  }
  return undressed_file_name(fn);
}

/// Load XML file and parse it using URI resolver to read data.
Document DocumentHandler::load(const std::string& fname, UriReader* reader) const  {
  string clean = _clean_fname(fname);
  if ( reader )   {
    printout(WARNING,"DocumentHandler","+++ Loading document URI: %s %s",
             fname.c_str(),"[URI Resolution is not supported by TiXML]");
  }
  else  {
    printout(INFO,"DocumentHandler","+++ Loading document URI: %s [Resolved:'%s']",
             fname.c_str(),clean.c_str());
  }
  TiXmlDocument* doc = new TiXmlDocument(clean.c_str());
  bool result = false;
  try {
    result = doc->LoadFile();
    if ( !result ) {
      if ( doc->Error() ) {
        printout(FATAL,"DocumentHandler","+++ Error (TinyXML) parsing XML document:%s [%s]",
                 fname.c_str(), clean.c_str());
        printout(FATAL,"DocumentHandler","+++ Error (TinyXML) XML parsing error:%s",
                 doc->ErrorDesc());
        printout(FATAL,"DocumentHandler","+++ Document:%s Location Line:%d Column:%d",
                 doc->Value(), doc->ErrorRow(), doc->ErrorCol());
        except("dd4hep:XML","++ file:%s error:%s",clean.c_str(),doc->ErrorDesc());
      }
      except("dd4hep:XML","++ Unknown error (TinyXML) while parsing:%s",fname.c_str());
    }
  }
  catch(exception& e) {
    printout(ERROR,"DocumentHandler","+++ Exception (TinyXML): parse(path):%s",e.what());
  }
  if ( result ) {
    printout(INFO,"DocumentHandler","+++ Document %s succesfully parsed with TinyXML .....",
             fname.c_str());
    return (XmlDocument*)doc;
  }
  delete doc;
  return 0;
}

/// Load XML file and parse it using URI resolver to read data.
Document DocumentHandler::load(Handle_t base, const XmlChar* fname, UriReader* reader) const  {
  string path = system_path(base, fname);
  return load(path,reader);
}

/// Parse a standalong XML string into a document.
Document DocumentHandler::parse(const char* bytes, size_t /* length */, const char* /* sys_id */, UriReader* reader) const {
  if ( reader )   {
    printout(WARNING,"DocumentHandler","+++ Parsing memory document %s",
             "[URI Resolution is not supported by TiXML]");
  }
  TiXmlDocument* doc = new TiXmlDocument();
  try  {
    if ( bytes )   {
      size_t len = ::strlen(bytes);
      // TiXml does not support white spaces at the end. Check and remove.
      if ( bytes[len-1] != 0 || ::isspace(bytes[len-2]) )   {
        char* buff = new char[len+1];
        try  {
          ::memcpy(buff, bytes, len+1);
          buff[len] = 0;
          for(size_t i=len-1; ::isspace(buff[i]); --i) buff[i] = 0;
          if ( 0 == doc->Parse(buff) ) {
            delete [] buff;
            return (XmlDocument*)doc;
          }
        }
        catch(...)   {
        }
        delete [] buff;
      }
      if ( 0 == doc->Parse(bytes) ) {
        return (XmlDocument*)doc;
      }
      if ( doc->Error() ) {
        printout(FATAL,"DocumentHandler",
                 "+++ Error (TinyXML) while parsing XML string [%s]",
                 doc->ErrorDesc());
        printout(FATAL,"DocumentHandler",
                 "+++ XML Document error: %s Location Line:%d Column:%d",
                 doc->Value(), doc->ErrorRow(), doc->ErrorCol());
        throw runtime_error(string("dd4hep: ")+doc->ErrorDesc());
      }
      throw runtime_error("dd4hep: Unknown error while parsing XML document string with TiXml.");
    }
    throw runtime_error("dd4hep: FAILED to parse invalid document string [NULL] with TiXml.");
  }
  catch(exception& e) {
    printout(ERROR,"DocumentHandler","+++ Exception (TinyXML): parse(string):%s",e.what());
  }
  delete doc;
  return 0;
}

/// Write xml document to output file (stdout if file name empty)
int DocumentHandler::output(Document doc, const string& fname) const {
  string fn = undressed_file_name(fname);
  FILE* file = fn.empty() ? stdout : ::fopen(fn.c_str(),"w");
  if ( !file ) {
    printout(ERROR,"DocumentHandler","+++ Failed to open output file: %s",fname.c_str());
    return 0;
  }
  TiXmlDocument* d = (TiXmlDocument*)doc.ptr();
  d->Print(file);
  if ( !fn.empty() ) ::fclose(file);
  return 1;
}

/// Dump partial or full XML trees
void dd4hep::xml::dump_tree(Handle_t elt, ostream& os) {
  TiXmlNode* node = (TiXmlNode*)elt.ptr();
  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  node->Accept( &printer );
  os << printer.Str();
}

/// Dump partial or full XML documents
void dd4hep::xml::dump_tree(Document doc, ostream& os) {
  TiXmlDocument* node = (TiXmlDocument*)doc.ptr();
  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  node->Accept( &printer );
  os << printer.Str();
}
#endif


/// Default constructor of a document handler using TiXml
DocumentHandler::DocumentHandler() {}

/// Default destructor of a document handler using TiXml
DocumentHandler::~DocumentHandler() {}

/// Default comment string
std::string DocumentHandler::defaultComment()  {
  const char comment[] = "\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "      ++++   dd4hep generated alignment file using the         ++++\n"
    "      ++++   dd4hep Detector description XML generator.        ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++   Parser:"
    XML_IMPLEMENTATION_TYPE
    "                ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++                              M.Frank CERN/LHCb      ++++\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
  return comment;
}

/// Load XML file and parse it.
Document DocumentHandler::load(const std::string& fname) const {
  return load(fname, 0);
}

/// Load secondary XML file with relative addressing with respect to handle
Document DocumentHandler::load(Handle_t base, const XmlChar* fname) const {
  return load(base, fname, 0);
}

/// Parse a standalong XML string into a document.
Document DocumentHandler::parse(const char* bytes, size_t length) const {
  return parse(bytes, length, "xml-memory-buffer", 0);
}

/// System ID of a given XML entity
string DocumentHandler::system_path(Handle_t base, const XmlChar* fname)   {
  string fn = _toString(fname);
  return system_path(base, fn);
}

/// System directory of a given XML entity
string DocumentHandler::system_directory(Handle_t base, const XmlChar* fname)   {
  string path = system_path(base,fname);
  string dir = ::dirname((char*)path.c_str());
  return dir;
}

/// System directory of a given XML entity
string DocumentHandler::system_directory(Handle_t base)   {
  string path = system_path(base);
  string dir = ::dirname((char*)path.c_str());
  return dir;
}

/// Create new XML document by parsing empty xml buffer
Document DocumentHandler::create(const char* tag, const char* comment) const {
  string top(tag);
  string empty = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
  empty += "<" + top + "/>\0\0";
  Document doc = parse(empty.c_str(), empty.length() + 1);
  if (comment) {
    Element top_elt = doc.root();
    top_elt.addComment(comment);
  }
  return doc;
}

// Create new XML document by parsing empty xml buffer
Document DocumentHandler::create(const std::string& tag, const std::string& comment) const   {
  return create(tag.c_str(), comment.c_str());
}

/// Dump partial or full XML trees to stdout
void dd4hep::xml::dump_tree(Handle_t elt) {
  dump_tree(elt,cout);
}

/// Dump partial or full XML documents to stdout
void dd4hep::xml::dump_tree(Document doc) {
  dump_tree(doc,cout);
}
