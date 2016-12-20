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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorProcessor.h"

using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Callback to output detector information of an single DetElement
int DetectorProcessor::operator()(DetElement de, int /* level */)    {
  if ( de.isValid() )  {
    return 1;
  }
  except("Detector","Cannot process an invalid detector element");
  return 0;
}

/// Callback to output detector information of an entire DetElement
int DetectorProcessor::process(DetElement de, int level, bool recursive)    {
  if ( de.isValid() )  {
    int ret = 1;
    (*this)(de, level);
    for (const auto& c : de.children() )
      ret += process(c.second,level+1,recursive);
    return ret;
  }
  except("Detector","Cannot process an invalid detector element");
  return 0;
}

/// Callback to output detector information of an single DetElement
int DetectorCollector::operator()(DetElement de, int level)    {
  if ( de.isValid() )  {
    detectors.push_back(std::make_pair(level,de));
    return 1;
  }
  except("Detector","Cannot process an invalid detector element");
  return 0;
}
