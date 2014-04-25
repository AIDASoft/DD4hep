// $Id: XMLTags.h 889 2013-11-14 15:55:39Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_CONDITIONS_CONDITIONSTAGS_H
#define DD4HEP_CONDITIONS_CONDITIONSTAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE 
#define UNICODE(x)  extern const Tag_t Unicode_##x 
#endif

// Unicode tags known to the conditions section
namespace DD4hep {
  namespace XML {
    namespace Conditions  {
      UNICODE(conditions);
      UNICODE(detelements);
      UNICODE(detelement);
      UNICODE(temperature);
      UNICODE(pressure);
      UNICODE(path);
      UNICODE(validity);
      UNICODE(open_transaction);
      UNICODE(close_transaction);
    }
    // User must ensure there are no clashes. If yes, then the clashing entry is unnecessary.
    using namespace DD4hep::XML::Conditions;
  }
}

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"
#endif /* DD4HEP_CONDITIONS_CONDITIONSTAGS_H  */
