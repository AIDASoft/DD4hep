//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================
#ifndef DETECTOR_DETECTORELEMENT_INL_H 
#define DETECTOR_DETECTORELEMENT_INL_H 1

// Framework include files
#include "Detector/DetectorElement.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/Printout.h"

template<typename T> std::map<dd4hep::DetElement, T*>
gaudi::DeHelpers::getChildConditions(ConditionsMap& m, DetElement de, itemkey_type key, int flags)    {
  std::map<dd4hep::DetElement, T*> cache;
  auto children = de.children();
  for(const auto& c : children)   {
    T* p = dynamic_cast<T*>(m.get(c.second, Keys::deKey).ptr());
    if ( p )  {
      cache.insert(std::make_pair(c.second,p));
      continue;
    }
    if ( 0 != (flags&ALL) )  {
      except("DeStatic","fillCache> No such condition:%d for detector element:%s",
             key, de.path().c_str());
    }
  }
  return cache;
}

#endif // DETECTOR_DETECTORELEMENT_INL_H
