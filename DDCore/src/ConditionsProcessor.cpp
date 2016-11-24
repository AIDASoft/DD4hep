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
#include "DD4hep/DetConditions.h"
#include "DD4hep/ConditionsProcessor.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Callback to output conditions information
int ConditionsProcessor::operator()(Condition /* cond */)    {
  return 1;
}

/// Container callback for object processing
int ConditionsProcessor::operator()(Container container)   {
  if ( m_pool )  {
    for(const auto& k : container.keys() )  {
      Condition c = container.get(k.first,*m_pool);
      (*this)(c);
    }
    return 1;
  }
  except("Conditions","Cannot process conditions container without user-pool.");
  return 0;
}

/// Callback to output conditions information of an entire DetElement
int ConditionsProcessor::processElement(DetElement de)    {
  if ( de.isValid() )  {
    if ( m_pool )  {
      DetConditions conds(de);
      (*this)(conds.conditions());
      return 1;
    }
    except("Conditions","Cannot process DetElement conditions from '%s' without user-pool",de.name());
  }
  except("Conditions","Cannot process conditions of an invalid detector element");
  return 0;
}
