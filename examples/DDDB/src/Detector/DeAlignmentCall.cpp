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

// Framework include files
#include "Detector/DeAlignmentCall.h"
#include "Detector/DetectorElement.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/AlignmentsCalculator.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"

using namespace dd4hep;
using namespace dd4hep::cond;
using namespace dd4hep::align;

/// Interface to client Callback in order to update the condition
Condition gaudi::DeAlignmentCall::operator()(const ConditionKey& /* key */,
                                             ConditionUpdateContext& ctxt)  {

  namespace tools = dd4hep::detail::tools;
  Condition       cond;
  UserPool* conditions = dynamic_cast<UserPool*>(&ctxt.resolver->conditionsMap());
  if ( conditions )    {
    typedef AlignmentsCalculator::OrderedDeltas Deltas;
    ConditionsHashMap    slice;
    AlignmentsCalculator calc;
    const IOV&        iov = conditions->validity();
    ConditionsManager mgr = ctxt.resolver->manager();
    cond = Condition(gaudi::Keys::alignmentsComputedKeyName,"Calculator");
    Deltas& deltas = cond.bind<Deltas>();
    //conditions->print("");
    // Test. Need to be replaced by a special scanner
    DetectorScanner().scan(AlignmentsCalculator::Scanner(ctxt,deltas),top);
    AlignmentsCalculator::Result ares = calc.compute(deltas, slice);
    ConditionsPool* iov_pool = mgr.registerIOV(*iov.iovType,iov.key());
    for(auto i=std::begin(slice.data); i != std::end(slice.data); ++i)   {
      Condition c = (*i).second;
      mgr.registerUnlocked(*iov_pool,c);
      conditions->insert(c);
    }
    printout(INFO,"Align","Alignments:(C:%ld,M:%ld,*:%ld) IOV:%s",
             ares.computed, ares.missing, ares.multiply, ctxt.iov->str().c_str());
    return cond;
  }
  except("DeAlignmentCall","No conditions slice present!");
  return cond;
}
