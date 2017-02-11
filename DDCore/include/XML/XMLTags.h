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

#ifndef DD4hep_XML_TAGS_H
#define DD4hep_XML_TAGS_H

#define DECLARE_UNICODE_TAG(x)  extern const ::DD4hep::XML::Tag_t Unicode_##x (#x)

// Framework include files
#include "XML/XMLElements.h"
#define UNICODE(x)  extern const Tag_t Unicode_##x
#include "XML/UnicodeValues.h"
#undef  UNICODE

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _U(a) ::DD4hep::XML::Unicode_##a
#define _Unicode(a) ::DD4hep::XML::Strng_t(#a)

#endif // DD4hep_XML_TAGS_H
