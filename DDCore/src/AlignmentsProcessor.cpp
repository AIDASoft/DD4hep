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
#include "DD4hep/detail/ContainerHelpers.h"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Callback to output alignments information
template <typename T> int DeltaCollector<T>::operator()(DetElement de, int level)  const {
  if ( de.isValid() )  {
    int count = 0;
    vector<Conditions::Condition> conditions;
    conditionsCollector(mapping,conditions)(de,level);
    for( auto cond : conditions )   {
      if ( cond->testFlag(Conditions::Condition::ALIGNMENT_DELTA) )  {
        insert_item(deltas, de, cond.get<Delta>());
        ++count;
      }
    }
    return count;
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;  
}
template class DeltaCollector<list<Delta> >;
template class DeltaCollector<vector<Delta> >;
template class DeltaCollector<map<DetElement,Delta> >;
template class DeltaCollector<vector<pair<DetElement,Delta> > >;
template class DeltaCollector<vector<pair<string,Delta> > >;

template class DeltaCollector<multimap<DetElement,Delta> >;
template class DeltaCollector<map<string,Delta> >;
template class DeltaCollector<multimap<string,Delta> >;

/// Callback to output alignments information
template <typename T>
int AlignmentsCollector<T>::operator()(DetElement de, int level)  const  {
  if ( de.isValid() )  {
    int count = 0;
    vector<Conditions::Condition> conditions;
    conditionsCollector(mapping,conditions)(de,level);
    for( auto cond : conditions )   {
      if ( cond->testFlag(Conditions::Condition::ALIGNMENT_DERIVED) )  {
        Alignment align = cond;
        insert_item(alignments, de, align);
        ++count;
      }
    }
    return count;
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;  
}
template class AlignmentsCollector<list<Alignment> >;
template class AlignmentsCollector<vector<Alignment> >;
template class AlignmentsCollector<map<DetElement,Alignment> >;
template class AlignmentsCollector<vector<pair<DetElement,Alignment> > >;
template class AlignmentsCollector<vector<pair<string,Alignment> > >;

template class AlignmentsCollector<multimap<DetElement,Alignment> >;
template class AlignmentsCollector<map<string,Alignment> >;
template class AlignmentsCollector<multimap<string,Alignment> >;
