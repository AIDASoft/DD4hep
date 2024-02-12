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
#include <DD4hep/ConditionsProcessor.h>
#include <DD4hep/detail/ContainerHelpers.h>

using namespace dd4hep;
using namespace dd4hep::cond;


/// Callback to output conditions information
template <typename T>
int ConditionsCollector<T>::operator()(DetElement de, int)  const  {
  struct Collector : public Condition::Processor  {
    DetElement det;
    T&         container;
    /// Constructor
    Collector(DetElement d, T& c) : det(d), container(c) {}
    /// Processing callback
    virtual int process(Condition c)  const override { insert_item(container, det, c); return 1; }
  };
  if ( de.isValid() )  {
    mapping.scan(de, ConditionsMap::FIRST_ITEM, ConditionsMap::LAST_ITEM, Collector(de,conditions));
    return (int)conditions.size();
  }
  except("Conditions","Cannot process conditions of an invalid detector element");
  return 0;  
}


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace cond   {

    //template class ConditionsCollector<ConditionsMap>;
    template class ConditionsCollector<std::list<Condition> >;
    template class ConditionsCollector<std::vector<Condition> >;
    template class ConditionsCollector<std::map<DetElement,Condition> >;
    template class ConditionsCollector<std::vector<std::pair<DetElement,Condition> > >;
    template class ConditionsCollector<std::vector<std::pair<std::string,Condition> > >;

    template class ConditionsCollector<std::multimap<DetElement,Condition> >;
    template class ConditionsCollector<std::map<std::string,Condition> >;
    template class ConditionsCollector<std::multimap<std::string,Condition> >;
  }       /* End namespace cond               */
}         /* End namespace dd4hep                   */
