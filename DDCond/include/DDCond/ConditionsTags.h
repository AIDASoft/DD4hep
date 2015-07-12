// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_CONDITIONS_CONDITIONSTAGS_H
#define DD4HEP_CONDITIONS_CONDITIONSTAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE 
#define UNICODE(x)  extern const Tag_t Unicode_##x 
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the AIDA detector description toolkit supporting XML utilities 
  namespace XML {
    /// Namespace of conditions unicode tags
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
