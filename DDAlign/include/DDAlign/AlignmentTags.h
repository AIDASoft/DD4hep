// $Id: $
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
#ifndef DD4HEP_ALIGNMENT_ALIGNMENT_TAGS_H
#define DD4HEP_ALIGNMENT_ALIGNMENT_TAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE
#define UNICODE(x)  extern const ::DD4hep::XML::Tag_t Unicode_##x 
#endif

// Unicode tags known to the alignment section
namespace DD4hep {
  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DDAlign {

    // Alignment tags
    UNICODE(DetectorAlignment);
    UNICODE(subdetectors);
    UNICODE(subdetector);
    UNICODE(detelements);
    UNICODE(detelement);
    UNICODE(subpath);
    UNICODE(path);
    UNICODE(pivot);
    UNICODE(reset);
    UNICODE(overlap);
    UNICODE(placement);
    UNICODE(reset_children);
    UNICODE(open_transaction);
    UNICODE(close_transaction);
    UNICODE(check_overlaps);
    UNICODE(debug);
  }
}

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"

#define _ALU(a) ::DD4hep::DDAlign::Unicode_##a

#endif /* DD4HEP_ALIGNMENT_ALIGNMENT_TAGS_H  */
