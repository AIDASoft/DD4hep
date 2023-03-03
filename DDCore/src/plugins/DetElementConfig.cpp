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
//
// Framework includes
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>
#include <XML/DocumentHandler.h>
#include <XML/Utilities.h>

// C/C++ include files
#include <climits>

/// Do not clutter global namespace
namespace  {

  using namespace dd4hep;

  /// Call to add configuration a posteriori to a generic DetElement structure
  /** Plugins to modify DetElement structures after creation.
   *  - Delegate to XML Utilities
   *  
   *  \author M.Frank
   *  \date   03.03.2023
   */
  long configure_detelement(Detector& detector, xml_h e)   {
    xml_comp_t  x_det = e;
    std::string path  = x_det.attr<std::string>(_U(path));
    DetElement  det   = detail::tools::findElement(detector, path.c_str());
    if ( det.isValid() )    {
      std::size_t count = 0;
      bool propagate = det == detector.world() ? false: x_det.attr<bool>(_U(propagate), false);
      if ( xml_dim_t x_vol = x_det.child(_U(volume), false) )   {
	count += xml::configVolume(detector, x_vol, det.volume(), propagate, false);
      }
      PrintLevel lvl = x_det.attr<xml::Attribute>(_U(debug), nullptr) ? ALWAYS : DEBUG;
      printout(lvl, "DetElementConfig", "++ Applied %ld settings to %s", count, path.c_str());
      return 1;
    }
    except("DetElementConfig","FAILED: No valid DetElement. Configuration could not be applied!");
    return 0;
  }
  
  /// Call to add a posteriori configuration to a generic DetElement structure
  /** Plugins to modify Sensitive detector structures after creation.
   *  - Delegate to XML Utilities
   *  
   *  \author M.Frank
   *  \date   03.03.2023
   */
  long configure_sensitive(Detector& detector, xml_h e)   {
    xml_comp_t c = e;
    std::string name  = c.attr<std::string>(_U(detector));
    std::size_t count = xml::configSensitiveDetector(detector, detector.sensitiveDetector(name), e);
    PrintLevel  lvl   = c.attr<xml::Attribute>(_U(debug), nullptr) ? ALWAYS : DEBUG;
    printout(lvl, "SensDetConfig", "++ Applied %ld settings to %s", count, name.c_str());
    return 1;
  }
  
}  // End namespace dd4hep
/// Instantiate factory
DECLARE_XML_PLUGIN(DD4hep_DetElementConfig, configure_detelement)
/// Instantiate factory
DECLARE_XML_PLUGIN(DD4hep_SensitiveDetectorConfig, configure_sensitive)
