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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorProcessor.h"

using namespace dd4hep;

/// Default destructor
DetectorProcessor::~DetectorProcessor()   {
}

/// Callback to output detector information of an entire DetElement
int DetectorProcessor::process(DetElement de, int level, bool recursive)  const  {
  if ( de.isValid() )  {
    int ret = (*this)(de, level);
    if ( recursive )  {
      for (const auto& c : de.children() )
        ret += process(c.second,level+1,recursive);
    }
    return ret;
  }
  except("Detector","Cannot process an invalid detector element");
  return 0;
}
