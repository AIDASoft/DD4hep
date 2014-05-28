// $Id: XMLTags.h 889 2013-11-14 15:55:39Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_ALIGNMENT_ALIGNMENT_TAGS_H
#define DD4HEP_ALIGNMENT_ALIGNMENT_TAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE 
#define UNICODE(x)  extern const Tag_t Unicode_##x
#endif

// Unicode tags known to the alignment section
namespace DD4hep {
  namespace XML {
    UNICODE(DetectorAlignment);
    UNICODE(subdetectors);
    UNICODE(subdetector);
    UNICODE(detelements);
    UNICODE(detelement);
    UNICODE(subpath);
    UNICODE(path);
    UNICODE(pivot);
    UNICODE(reset);
    UNICODE(placement);
    UNICODE(reset_children);
    UNICODE(open_transaction);
    UNICODE(close_transaction);
    UNICODE(check_overlaps);
  }
}

#undef UNICODE // Do not miss this one!

#include "XML/XMLTags.h"

#endif /* DD4HEP_ALIGNMENT_ALIGNMENT_TAGS_H  */
