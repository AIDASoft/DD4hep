// -*- C++ -*-
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// @author F.Gaede, DESY
// @date June, 2017
//==========================================================================
#ifndef dd4hep_NAMESPACE_H
#define dd4hep_NAMESPACE_H 1


#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"



/// The top level namespace in DD4hep 
namespace DD4hep{

  /// holds all classes relevant for the geometry description
  namespace Geometry{}

  /// simulation classes
  namespace Simulation{}
  
  /// dedicated view into the geometry for reconstruction
  namespace DDRec{}
  
  /// alignment module
  namespace DDAlign{}

  /// alignment classes 
  namespace Alignments{}
  
  /// condition handling
  namespace Conditions{}

  /// segmentations
  namespace DDSegmentation{}

  /// utilities for boost::spirit
  namespace Utils{}

  /// general utilities
  namespace Utilities{}
  
  /// xml stuff
  namespace XML{}

}


namespace DDSurfaces{}



/** 
 * Convenient namespace that combines all public namespaces in DD4hep into one global namespace.
 */

namespace dd4hep {
  
  using namespace DD4hep ;
  using namespace DDSurfaces ;
  
  using namespace DD4hep::Geometry ;
  using namespace DD4hep::DDRec ;
  using namespace DD4hep::Alignments ;
  using namespace DD4hep::Conditions ;
  using namespace DD4hep::DDSegmentation ;
  using namespace DD4hep::DDAlign ;
  using namespace DD4hep::Utils ;
  using namespace DD4hep::Utilities ;
  using namespace DD4hep::Simulation ;
  using namespace DD4hep::XML ;


  /// define a better type name for the main detector description class
  typedef LCDD DetDescription ;

}


#endif
