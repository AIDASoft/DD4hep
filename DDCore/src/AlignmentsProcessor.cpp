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
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Callback to output alignments information
int AlignmentsProcessor::operator()(Alignment /* alignment */)    {
  return 1;
}

/// Callback to output alignments information of an entire DetElement
int AlignmentsProcessor::processElement(Geometry::DetElement de)    {
  if ( de.isValid() )  {
    if ( mapping )   {
      Conditions::DetElementConditionsCollector select(de);
      int count = 0;
      mapping->scan(select);
      for( auto cond : select.conditions )   {
        if ( cond->testFlag(Conditions::Condition::ALIGNMENT_DERIVED) )  {
          Alignment a = cond;
          count += (*this)(a);
        }
      }
      return count;
    }
    except("Alignments","Failed to process alignments for DetElement:%s [No slice availible]",
           de.path().c_str());
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;
}


/// Callback to output alignments information
int DetElementDeltaCollector::operator()(DetElement de, int)   {
  if ( de.isValid() )  {
    if ( mapping )   {
      Conditions::DetElementConditionsCollector select(de);
      int count = 0;
      mapping->scan(select);
      for( auto cond : select.conditions )   {
        if ( cond->testFlag(Conditions::Condition::ALIGNMENT_DELTA) )  {
          deltas.insert(std::make_pair(de,cond.get<Delta>()));
          ++count;
        }
      }
      return count;
    }
    except("Alignments","Failed to process alignments for DetElement:%s [No slice availible]",
           de.path().c_str());
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;  
}

/// Overloadable entry: Selection callback: return true if the alignment should be selected
int DetElementAlignmentsCollector::process(Conditions::Condition cond)     {
  if ( cond->testFlag(Conditions::Condition::ALIGNMENT_DERIVED) )  {
    Alignment a = cond;
    alignments.push_back(a);
    return 1;
  }
  return 0;
}
