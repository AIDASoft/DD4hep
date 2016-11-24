// $Id$
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
#if 0

// Framework includes
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDAlign/AlignmentUpdateCall.h"
#include "DDAlign/AlignmentsManager.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"

using std::string;
using namespace DD4hep;
using namespace DD4hep::Alignments;
using Conditions::Condition;
using Conditions::ConditionKey;
using Conditions::ConditionsManager;
using Conditions::DetConditions;
using Conditions::DependencyBuilder;

// ======================================================================================
/// Compute dependent alignment conditions
int computeDependencies(dd4hep_ptr<UserPool>& user_pool,
                        ConditionsManager conds,
                        AlignmentsManager align,
                        long time)
{
  const IOVType* iovType = conds.iovType("epoch");
  IOV  iov(iovType, IOV::Key(time,time));
  long num_expired = conds.prepare(iov, user_pool);
  printout(INFO,"Conditions",
           "+++ ConditionsUpdate: Updated %ld conditions... IOV:%s",
           num_expired, iov.str().c_str());
  align.compute(user_pool);
  return 1;
}
/// Access dependent alignment conditions from DetElement object using global and local keys
int access(ConditionsManager conds,AlignmentsManager align,long time)  {
  typedef ConditionsManager::Dependencies Deps;
  dd4hep_ptr<UserPool> pool;
  int   ret  = computeDependencies(pool, conds, align, time);
  if ( ret == 1 )  {
    const Deps& deps = align.knownDependencies();
    int count = 0;
    for(Deps::const_iterator i=deps.begin(); i!=deps.end(); ++i)   {
      const ConditionDependency* d = (*i).second.get();
      if ( d->detector.hasAlignments() )   {
        UserPool& p = *(pool.get());
        Alignments::DetAlign     det(d->detector);
        const ConditionKey&      k = d->target;
        Alignments::Container    c = det.alignments();
        {
          Alignments::Alignment    a = c.get(k.hash,p);
          const Alignments::Delta& D = a.data().delta;
          printout(INFO,"Alignment","++ [%08X] (%11s-%8s-%5s) Cond:%p '%s'", k.hash,
                   D.hasTranslation() ? "Translation" : "",
                   D.hasRotation() ? "Rotation" : "",
                   D.hasPivot() ? "Pivot" : "",
                   a.data().hasCondition() ? a.data().condition.ptr() : 0,
                   k.name.c_str());
          ++count;
        }
        {
          Alignments::Alignment    a = c.get("Alignment",p);
          const Alignments::Delta& D = a.data().delta;
          printout(INFO,"Alignment","++ [%08X] (%11s-%8s-%5s) Cond:%p 'Alignment'", k.hash,
                   D.hasTranslation() ? "Translation" : "",
                   D.hasRotation() ? "Rotation" : "",
                   D.hasPivot() ? "Pivot" : "",
                   a.data().hasCondition() ? a.data().condition.ptr() : 0);
          ++count;
        }
      }
    }
    printout(INFO,m_name,"++ Accessed %d conditions from the DetElement objects.",count);
  }
  return ret;
}
#endif
