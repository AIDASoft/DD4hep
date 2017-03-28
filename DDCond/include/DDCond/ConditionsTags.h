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
#ifndef DD4HEP_CONDITIONS_CONDITIONSTAGS_H
#define DD4HEP_CONDITIONS_CONDITIONSTAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE 
#define UNICODE(x)  extern const ::DD4hep::XML::Tag_t Unicode_##x 
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the AIDA detector description toolkit supporting XML utilities 
  namespace XML {
    /// Namespace of conditions unicode tags
    namespace Conditions  {
      UNICODE(address);
      UNICODE(condition);
      UNICODE(conditions);
      UNICODE(detelements);
      UNICODE(detelement);
      UNICODE(temperature);
      UNICODE(pressure);
      UNICODE(path);
      UNICODE(validity);
      UNICODE(open_transaction);
      UNICODE(close_transaction);

      UNICODE(iov);
      UNICODE(iov_type);
      UNICODE(manager);
      UNICODE(property);

      UNICODE(hash);
      UNICODE(mapping);
      UNICODE(sequence);
      UNICODE(alignment);
      UNICODE(repository);
    }
    // User must ensure there are no clashes. If yes, then the clashing entry is unnecessary.
    using namespace ::DD4hep::XML::Conditions;
  }
}

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"

#define _UC(x) ::DD4hep::XML::Conditions::Unicode_##x

#endif /* DD4HEP_CONDITIONS_CONDITIONSTAGS_H  */
