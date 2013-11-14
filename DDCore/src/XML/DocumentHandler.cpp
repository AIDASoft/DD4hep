#include "XML/DocumentHandler.h"
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::XML;

#ifndef __TIXML__
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/sax/EntityResolver.hpp"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/util/XMLUni.hpp"
#include "xercesc/util/XMLURL.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
using namespace xercesc;

namespace DD4hep {
  namespace XML {
    namespace {
      XercesDOMParser* make_parser(xercesc::ErrorHandler* err_handler = 0) {
        XercesDOMParser* parser = new XercesDOMParser;
        parser->setValidationScheme(XercesDOMParser::Val_Auto);
        parser->setValidationSchemaFullChecking(true);
        if (err_handler) {
          parser->setErrorHandler(err_handler);
        }
        parser->setDoNamespaces(true);
        parser->setDoSchema(true);
        return parser;
      }

      Document parse_document(const void* bytes, size_t length, xercesc::ErrorHandler* err_handler) {
        auto_ptr<XercesDOMParser> parser(make_parser(err_handler));
        MemBufInputSource src((const XMLByte*) bytes, length, "memory");
        parser->setValidationSchemaFullChecking(true);
        parser->parse(src);
        DOMDocument* doc = parser->adoptDocument();
        doc->setXmlStandalone(true);
        doc->setStrictErrorChecking(true);
        return (XmlDocument*) doc;
      }
    }
    struct DocumentErrorHandler: public ErrorHandler, public DOMErrorHandler {
      /// Constructor
      DocumentErrorHandler() {
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
      virtual bool handleError(const DOMError& domError);
    };
    bool DocumentErrorHandler::handleError(const DOMError& domError) {
      switch (domError.getSeverity()) {
      case DOMError::DOM_SEVERITY_WARNING:
        cout << "DOM WARNING: ";
        break;
      case DOMError::DOM_SEVERITY_ERROR:
        cout << "DOM ERROR:   ";
        break;
      case DOMError::DOM_SEVERITY_FATAL_ERROR:
        cout << "DOM FATAL:   ";
        break;
      default:
        cout << "DOM UNKNOWN: ";
        return false;
      }
      cout << _toString(domError.getType()) << ": " << _toString(domError.getMessage()) << endl;
      DOMLocator* loc = domError.getLocation();
      if (loc) {
        cout << "Location: Line:" << loc->getLineNumber() << " Column:" << loc->getColumnNumber() << endl;
      }
      return false;
    }
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
      cout << "Error at file \"" << sys << "\", line " << e.getLineNumber() << ", column " << e.getColumnNumber() << endl
          << "Message: " << m << endl;
    }
    void DocumentErrorHandler::fatalError(const SAXParseException& e) {
      string m(_toString(e.getMessage()));
      string sys(_toString(e.getSystemId()));
      cout << "Fatal Error at file \"" << sys << "\", line " << e.getLineNumber() << ", column " << e.getColumnNumber() << endl
          << "Message: " << m << endl;
      //throw runtime_error( "Standard pool exception : Fatal Error on the DOM Parser" );
    }

    void dumpTree(xercesc::DOMDocument* doc) {
      DOMImplementation *imp = DOMImplementationRegistry::getDOMImplementation(Strng_t("LS"));
      XMLFormatTarget *tar = new StdOutFormatTarget();
      DOMLSOutput *out = imp->createLSOutput();
      DOMLSSerializer *wrt = imp->createLSSerializer();
      out->setByteStream(tar);
      wrt->getDomConfig()->setParameter(Strng_t("format-pretty-print"), true);
      wrt->write(doc, out);
      out->release();
      wrt->release();
    }
  }
}

/// Default constructor of a document handler using XercesC
DocumentHandler::DocumentHandler()
    : m_errHdlr(new DocumentErrorHandler()) {
}

/// Default destructor of a document handler using XercesC
DocumentHandler::~DocumentHandler() {
}

Document DocumentHandler::load(Handle_t base, const XMLCh* fname) const {
  xercesc::DOMElement* e = (xercesc::DOMElement*) base.ptr();
  xercesc::XMLURL base_url(e->getBaseURI());
  xercesc::XMLURL ref_url(base_url, fname);
  return load(_toString(ref_url.getURLText()));
}

Document DocumentHandler::load(const string& fname) const {
  cout << "Loading document URI:" << fname << endl;
  XMLURL xerurl = (const XMLCh*) Strng_t(fname);
  string path = _toString(xerurl.getPath());
  string proto = _toString(xerurl.getProtocolName());
  auto_ptr < XercesDOMParser > parser(make_parser(m_errHdlr.get()));
  cout << "            protocol:" << proto << endl << "                path:" << path << endl;
  try {
    parser->parse(path.c_str());
  }
  catch (exception& e) {
    cout << "parse(path):" << e.what() << endl;
    try {
      parser->parse(fname.c_str());
    }
    catch (exception& ex) {
      cout << "parse(URI):" << ex.what() << endl;
    }
  }
  cout << "Document succesfully parsed....." << endl;
  return (XmlDocument*) parser->adoptDocument();
}

/// Parse a standalong XML string into a document.
Document DocumentHandler::parse(const char* bytes, size_t length) const {
  auto_ptr < XercesDOMParser > parser(make_parser(m_errHdlr.get()));
  MemBufInputSource src((const XMLByte*) bytes, length, "memory");
  parser->setValidationSchemaFullChecking(true);
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
  else
    tar = new LocalFileFormatTarget(Strng_t(fname));

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
#ifndef _WIN32
#include <libgen.h>
#endif
#include <sys/stat.h>

namespace DD4hep {namespace XML {
    struct DocumentErrorHandler {};
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
    if ( strncmp(s.c_str(),"file:",5)==0 ) return s.substr(5);
    return s;
  }
}

/// Default constructor of a document handler using TiXml
DocumentHandler::DocumentHandler() {}

/// Default destructor of a document handler using TiXml
DocumentHandler::~DocumentHandler() {}

/// Load a document based on the relative URI of the parent document
Document DocumentHandler::load(Handle_t base, const XmlChar* fname) const {
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
  return load(fn);
  else if ( ::stat(clean.c_str(),&st)==0 )
  return load(clean);
  return load(fname);
}

Document DocumentHandler::load(const string& fname) const {
  cout << "Loading document URI:" << fname << endl;
  TiXmlDocument* doc = new TiXmlDocument(_clean_fname(fname).c_str());
  bool result = false;
  try {
    result = doc->LoadFile();
    if ( !result ) {
      if ( doc->Error() ) {
        cout << "Unknown error whaile parsing XML document with TiXml:" << endl;
        cout << "Document:" << doc->Value() << endl;
        cout << "Location: Line:" << doc->ErrorRow()
        << " Column:" << doc->ErrorCol() << endl;
        throw runtime_error(string("DD4hep: ")+doc->ErrorDesc());
      }
      else
      throw runtime_error("DD4hep: Unknown error whaile parsing XML document with TiXml.");
    }
  }
  catch(exception& e) {
    cout << "parse(path):" << e.what() << endl;
  }
  if ( result ) {
    cout << "Document " << fname << " succesfully parsed....." << endl;
    return (XmlDocument*)doc;
  }
  delete doc;
  return 0;
}

/// Parse a standalong XML string into a document.
Document DocumentHandler::parse(const char* doc_string, size_t /* length */) const {
  TiXmlDocument* doc = new TiXmlDocument();
  try {
    if ( 0 == doc->Parse(doc_string) ) {
      return (XmlDocument*)doc;
    }
    if ( doc->Error() ) {
      cout << "Unknown error whaile parsing XML document with TiXml:" << endl;
      cout << "Document:" << doc->Value() << endl;
      cout << "Location: Line:" << doc->ErrorRow()
      << " Column:" << doc->ErrorCol() << endl;
      throw runtime_error(string("DD4hep: ")+doc->ErrorDesc());
    }
    throw runtime_error("DD4hep: Unknown error whaile parsing XML document with TiXml.");
  }
  catch(exception& e) {
    cout << "parse(xml-string):" << e.what() << endl;
  }
  delete doc;
  return 0;
}

/// Write xml document to output file (stdout if file name empty)
int DocumentHandler::output(Document doc, const string& fname) const {
  FILE* file = fname.empty() ? stdout : ::fopen(fname.c_str(),"w");
  if ( !file ) {
    cout << "Failed to open output file:" << fname << endl;
    return 0;
  }
  TiXmlDocument* d = (TiXmlDocument*)doc.ptr();
  d->Print(file);
  if ( !fname.empty() ) ::fclose(file);
  return 1;
}

#endif

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
