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
#include "DD4hep/ConditionsProcessor.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Callback to output conditions information
int ConditionsProcessor::process(Condition /* cond */)    {
  return 1;
}

/// Callback to output conditions information of an entire DetElement
int ConditionsProcessor::processElement(DetElement de)    {
  if ( de.isValid() )  {
    if ( mapping )   {
      int count = 0;
      DetElementConditionsCollector select(de);
      mapping->scan(select);
      for(const auto& c : select.conditions )
        count += process(c);
      return count;
    }
    except("Conditions","Failed to process alignments for DetElement:%s [No slice availible]",
           de.path().c_str());
  }
  except("Conditions","Cannot process conditions of an invalid detector element");
  return 0;
}

/// Overloadable entry: Selection callback: return true if the condition should be selected
int DetElementConditionsCollector::process(Condition cond)   {
  if ( key.detector_key() == cond.detector_key() )  {
    conditions.push_back(cond);
    return 1;
  }
  return 0;
}
