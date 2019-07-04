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
#ifndef DDG4_DIGI_DIGIFACTORIES_H
#define DDG4_DIGI_DIGIFACTORIES_H

// Framework include files
#include "DD4hep/Plugins.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <string>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  class DetElement;
  class Detector;

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
    class DigiKernel;
    class DigiAction;
  }
}

namespace {

  namespace DS = dd4hep::digi;
  struct _ns {
    typedef DS::DigiAction  DA;
    typedef DS::DigiKernel  K;
    typedef std::map<std::string,std::string> STRM;
  };

  /// Factory to create Digi action objects
  DD4HEP_PLUGIN_FACTORY_ARGS_2(DS::DigiAction*,const _ns::K*, std::string)
  {    return new P(*a0,a1);  }

}

/// Plugin defintion to create DigiAction objects
#define DECLARE_DIGIACTION_NS(name_space,name)  namespace {\
  using name_space::name;                                               \
  DD4HEP_PLUGINSVC_FACTORY(name,name,dd4hep::digi::DigiAction*(const _ns::K*,std::string),__LINE__) \
    }

/// Plugin defintion to create DigiAction objects
#define DECLARE_DIGIACTION(name)         DECLARE_DIGIACTION_NS(dd4hep::digi,name)

#endif // DDG4_DIGI_DIGIFACTORIES_H
