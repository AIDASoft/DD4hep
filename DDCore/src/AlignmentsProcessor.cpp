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
#include <DD4hep/Printout.h>
#include <DD4hep/AlignmentsProcessor.h>
#include <DD4hep/ConditionsProcessor.h>
#include <DD4hep/detail/ContainerHelpers.h>
#include <DD4hep/detail/ConditionsInterna.h>

using namespace dd4hep;
using namespace dd4hep::align;

/// Callback to output alignments information
template <typename T>
int DeltaCollector<T>::operator()(DetElement de, int level)  const  {
  if ( de.isValid() )  {
    int count = 0;
    std::vector<Condition> conditions;
    cond::conditionsCollector(mapping,conditions)(de,level);
    for( auto cond : conditions )   {
      if ( cond->testFlag(Condition::ALIGNMENT_DELTA) )  {
        insert_item(deltas, de, cond.get<Delta>());
        ++count;
      }
    }
    return count;
  }
  except("Alignments","Cannot process alignments of an invalid detector element");
  return 0;  
}

/// Callback to output alignments information
template <typename T>
int AlignmentsCollector<T>::operator()(DetElement de, int level)  const  {
  if ( de.isValid() )  {
    int count = 0;
    std::vector<Condition> conditions;
    cond::conditionsCollector(mapping,conditions)(de,level);
    for( auto cond : conditions )   {
      if ( cond->testFlag(Condition::ALIGNMENT_DERIVED) )  {
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


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace align {

    template class DeltaCollector<std::list<Delta> >;
    template class DeltaCollector<std::vector<Delta> >;
    template class DeltaCollector<std::map<DetElement,Delta> >;
    template class DeltaCollector<std::vector<std::pair<DetElement,Delta> > >;
    template class DeltaCollector<std::vector<std::pair<std::string,Delta> > >;

    template class DeltaCollector<std::multimap<DetElement,Delta> >;
    template class DeltaCollector<std::map<std::string,Delta> >;
    template class DeltaCollector<std::multimap<std::string,Delta> >;


    template class AlignmentsCollector<std::list<Alignment> >;
    template class AlignmentsCollector<std::vector<Alignment> >;
    template class AlignmentsCollector<std::map<DetElement,Alignment> >;
    template class AlignmentsCollector<std::vector<std::pair<DetElement,Alignment> > >;
    template class AlignmentsCollector<std::vector<std::pair<std::string,Alignment> > >;

    template class AlignmentsCollector<std::multimap<DetElement,Alignment> >;
    template class AlignmentsCollector<std::map<std::string,Alignment> >;
    template class AlignmentsCollector<std::multimap<std::string,Alignment> >;

  }    /* End namespace align  */
}      /* End namespace dd4hep      */
