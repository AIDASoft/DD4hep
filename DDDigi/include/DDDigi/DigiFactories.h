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

class TGeoShape;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  class SegmentationObject;
  class DetElement;
  class Detector;

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
    class DigiKernel;
    class DigiEventAction;
    class DigiCellScanner;
    class DigiSignalProcessor;
  }
}

namespace {
  namespace DS = dd4hep::digi;

  /// Factory to create Digi action objects
  DD4HEP_PLUGIN_FACTORY_ARGS_2(DS::DigiEventAction*,const DS::DigiKernel*, std::string)
  {    return new P(*a0,a1);  }
  /// Factory to create Digi signal processor objects
  DD4HEP_PLUGIN_FACTORY_ARGS_2(DS::DigiSignalProcessor*,const DS::DigiKernel*, std::string)
  {    return new P(*a0,a1);  }
  /// Factory to create Digi cell scanner objects
  DD4HEP_PLUGIN_FACTORY_ARGS_1(DS::DigiCellScanner*, dd4hep::SegmentationObject*)
  {    return new P(dd4hep::Segmentation(a0));  }
}

/// Plugin defintion to create DigiAction objects
#define DECLARE_DIGIEVENTACTION_NS(name_space,name)  namespace {\
  using name_space::name;                                               \
  DD4HEP_PLUGINSVC_FACTORY(name,name,dd4hep::digi::DigiEventAction*(const DS::DigiKernel*,std::string),__LINE__) }
/// Plugin defintion to create DigiAction objects
#define DECLARE_DIGIEVENTACTION(name)         DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,name)

#define DECLARE_DIGISIGNALPROCESSOR_NS(name_space,name)  namespace {     \
  using name_space::name;                                               \
  DD4HEP_PLUGINSVC_FACTORY(name,name,dd4hep::digi::DigiSignalProcessor*(const DS::DigiKernel*,std::string),__LINE__) }

/// Plugin defintion to create DigiSignalProcessor objects
#define DECLARE_DIGISIGNALPROCESSOR(name)         DECLARE_DIGISIGNALPROCESSOR_NS(dd4hep::digi,name)

#define DECLARE_DIGICELLSCANNER_NS(name_space,typ,seg,sol)  namespace { \
typedef name_space :: CellScanner<dd4hep:: seg , dd4hep:: sol > Scanner_##typ##_##seg##_##sol##_t; \
 DD4HEP_PLUGINSVC_FACTORY( Scanner_##typ##_##seg##_##sol##_t, typ##_##seg##_##sol ,                   \
                           dd4hep::digi::DigiCellScanner*(dd4hep::SegmentationObject*),__LINE__) }

/// Plugin defintion to create DigiCellscanner objects
#define DECLARE_DIGICELLSCANNER(typ,seg,sol) DECLARE_DIGICELLSCANNER_NS(dd4hep::digi,typ,seg,sol)

#endif // DDG4_DIGI_DIGIFACTORIES_H
