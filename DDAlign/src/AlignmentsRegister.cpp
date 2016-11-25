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
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/objects/AlignmentsInterna.h"

#include "DDAlign/AlignmentsRegister.h"
#include "DDAlign/AlignmentUpdateCall.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

// ======================================================================================

/// Initializing constructor
AlignmentsRegister::AlignmentsRegister(AlignmentsManager m, AlignmentUpdateCall* c, UserPool* p)
  : alignmentMgr(m), updateCall(c), user_pool(p)
{
}

/// Default destructor
AlignmentsRegister::~AlignmentsRegister()   {
  releasePtr(updateCall);
}

/// Callback to output conditions information
int AlignmentsRegister::processElement(DetElement de)  {
  using Conditions::Condition;
  if ( de.isValid() )  {
    if ( de.hasConditions() )  {
      DetAlign align(de);
      Conditions::DetConditions conditions(de);
      Conditions::Container cont = conditions.conditions();
      printout(DEBUG,"AlignRegister",
               "++ Processing DE %s hasConditions:%s [%d entries]",
               de.path().c_str(), yes_no(de.hasConditions()), int(cont.numKeys()));
      for ( const auto& c : cont.keys() )  {
        Condition cond = cont.get(c.first, *user_pool);
        printout(DEBUG,"AlignRegister",
                 "++ Processing DE %s Cond:%s Key:%08X flags:%d",
                 de.path().c_str(), cond.name(), cond.key(), cond->flags);
        if ( (cond->flags&Condition::ALIGNMENT) )  {
          Conditions::ConditionKey k(cond->name+"/Tranformations");
          //
          // The alignment access through the DetElement object is optional!
          // It is slow and deprecated. The access using the UserPool directly
          // is highly favored.
          //
          align.alignments()->addKey(k.name);
          align.alignments()->addKey("Alignment",k.name);
          //
          // Now add the dependency to the alignmant manager
          Conditions::DependencyBuilder b(k, updateCall->addRef(), de);
          b.add(Conditions::ConditionKey(cond->name));
          bool result = alignmentMgr.adoptDependency(b.release());
          if ( result )   {
            printout(INFO,"AlignRegister",
                     "++ Added Alignment dependency Cond:%s Key:%08X flags:%d",
                     k.name.c_str(), k.hash, cond->flags);
            continue;
          }
          printout(ERROR,"AlignRegister",
                   "++ FAILED to add Alignment dependency Cond:%s Key:%08X flags:%d",
                   k.name.c_str(), k.hash, cond->flags);
        }
      }
      return 1;
    }
    printout(DEBUG,"AlignRegister","++ Processing DE %s hasConditions:%s",
             de.path().c_str(), yes_no(de.hasConditions()));
  }
  return 1;
}

