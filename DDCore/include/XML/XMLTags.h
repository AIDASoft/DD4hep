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

#ifndef DD4HEP_XML_TAGS_H
#define DD4HEP_XML_TAGS_H

#define DECLARE_UNICODE_TAG(x)  namespace DD4hep { namespace XML { extern const Tag_t Unicode_##x (#x); }}

// Framework include files
#include "XML/XMLElements.h"
#define UNICODE(x)  extern const Tag_t Unicode_##x

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {
#include "XML/UnicodeValues.h"
  }
}

#undef  UNICODE

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _DD4hep_Unicode_Item(a) Unicode_##a
#define _U(a)              ::DD4hep::XML::Unicode_##a
#define _Unicode(a)        ::DD4hep::XML::Strng_t(#a)

#endif // DD4HEP_XML_TAGS_H
