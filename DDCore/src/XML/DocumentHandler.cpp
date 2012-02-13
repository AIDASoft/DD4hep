#include "XML/DocumentHandler.h"

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

#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

using namespace xercesc;
using namespace std;
using namespace DD4hep;
using namespace DD4hep::XML;

namespace {

  typedef const string& CSTR;

  struct ErrHandler : public ErrorHandler, public DOMErrorHandler    {
    /// Constructor
    ErrHandler()  {}
    /// Reset errors (Noop)
    void resetErrors()                          {      }
    /// Warnings callback. Ignore them
    void warning(const SAXParseException& /* e */)    {     }
    /// Error handler
    void error(const SAXParseException& e);
    /// Fatal error handler
    void fatalError(const SAXParseException& e);
    virtual bool handleError(const DOMError& domError);
  };
  bool ErrHandler::handleError(const DOMError& domError)  {
    switch(domError.getSeverity()) {
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
    if ( loc )  {
      cout << "Location: Line:" << loc->getLineNumber()
        << " Column:" << loc->getColumnNumber() << endl;
    }
    return false;
  }
  void ErrHandler::error(const SAXParseException& e)  {
    string m(_toString(e.getMessage()));
    if (m.find("The values for attribute 'name' must be names or name tokens")!=string::npos ||
      m.find("The values for attribute 'ID' must be names or name tokens")!=string::npos   ||
      m.find("for attribute 'name' must be Name or Nmtoken")!=string::npos                 ||
      m.find("for attribute 'ID' must be Name or Nmtoken")!=string::npos                   ||
      m.find("for attribute 'name' is invalid Name or NMTOKEN value")!=string::npos        ||
      m.find("for attribute 'ID' is invalid Name or NMTOKEN value")!=string::npos      )
      return;
    string sys(_toString(e.getSystemId()));
    cout << "Error at file \""  << sys
      << "\", line " << e.getLineNumber() << ", column " << e.getColumnNumber() << endl
      << "Message: " << m << endl;
  }
  void ErrHandler::fatalError(const SAXParseException& e)  {
    string m(_toString(e.getMessage()));
    string sys(_toString(e.getSystemId()));
    cout << "Fatal Error at file \"" << sys
      << "\", line " << e.getLineNumber() << ", column " << e.getColumnNumber() << endl
      << "Message: " << m << endl;
    //throw runtime_error( "Standard pool exception : Fatal Error on the DOM Parser" );
  }
}

void DD4hep::XML::dumpTree(xercesc::DOMDocument* doc)  {
  DOMImplementation *imp = DOMImplementationRegistry::getDOMImplementation(Strng_t("LS"));
  XMLFormatTarget   *tar = new StdOutFormatTarget();
  DOMLSOutput       *out = imp->createLSOutput();
  DOMLSSerializer   *wrt = imp->createLSSerializer();
  out->setByteStream(tar);
  wrt->getDomConfig()->setParameter(Strng_t("format-pretty-print"), true);
  wrt->write(doc, out);
  out->release();
  wrt->release();
}

DocumentHandler::DocumentHandler() :  m_errHdlr(new ErrHandler())  {
}

DocumentHandler::~DocumentHandler()  {
}

XercesDOMParser* DocumentHandler::makeParser(xercesc::ErrorHandler* err_handler) const  {
  XercesDOMParser* parser = new XercesDOMParser;
  parser->setValidationScheme(XercesDOMParser::Val_Auto);
  parser->setValidationSchemaFullChecking(true);
  parser->setErrorHandler(err_handler ? err_handler : m_errHdlr.get());
  parser->setDoNamespaces(true);
  parser->setDoSchema(true);
  return parser;
}

Document DocumentHandler::load(const string& fname)  const  {
  XMLURL xerurl = (const XMLCh*)Strng_t(fname);
  string path   = _toString(xerurl.getPath());
  string proto  = _toString(xerurl.getProtocolName());
  auto_ptr<XercesDOMParser> parser(makeParser());
  cout << "Loading document :" << path  << endl
       << "         protocol:" << proto << endl
       << "              URI:" << fname << endl;
  try {
    parser->parse(path.c_str());
  }
  catch(std::exception& e) {
    cout << "parse(path):" << e.what() << endl;
    try {
      parser->parse(fname.c_str());
    }
    catch(std::exception& ex) {
      cout << "parse(URI):" << ex.what() << endl;      
    }
  }
  cout << "Document succesfully parsed....." << endl;
  return parser->adoptDocument();
}

Document DocumentHandler::parse(const void* bytes, size_t length)  const {
  auto_ptr<XercesDOMParser> parser(makeParser());
  MemBufInputSource src((const XMLByte*)bytes,length,"memory");
  parser->setValidationSchemaFullChecking(true);
  parser->parse(src);
  DOMDocument* doc  = parser->adoptDocument();
  doc->setXmlStandalone(true);
  doc->setStrictErrorChecking(true);
  return doc;
}
