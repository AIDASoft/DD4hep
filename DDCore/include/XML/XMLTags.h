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

#ifndef XML_XMLTAGS_H
#define XML_XMLTAGS_H

#define DECLARE_UNICODE_TAG(x)  namespace dd4hep { namespace xml { extern const Tag_t Unicode_##x (#x); }}

// Framework include files
#include "XML/XMLElements.h"
#define UNICODE(x)  extern const Tag_t Unicode_##x

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {
#include "XML/UnicodeValues.h"
  }
}

#undef  UNICODE

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _dd4hep_Unicode_Item(a) Unicode_##a
#define _U(a)              ::dd4hep::xml::Unicode_##a
#define _Unicode(a)        ::dd4hep::xml::Strng_t(#a)

#endif // XML_XMLTAGS_H
