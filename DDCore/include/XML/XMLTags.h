// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_TAGS_H
#define DD4hep_XML_TAGS_H

#define DECLARE_UNICODE_TAG(x)  extern const Tag_t Unicode_##x (#x)

#include "XML/XMLElements.h"
#define UNICODE(x)  extern const Tag_t Unicode_##x
#include "XML/UnicodeValues.h"
#undef  UNICODE

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _U(a) DD4hep::XML::Unicode_##a
#define _Unicode(a) DD4hep::XML::Strng_t(#a)

#endif // DD4hep_XML_TAGS_H
