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

#include "DDAlign/AlignmentsRegister.h"
#include "DDAlign/AlignmentsUpdateCall.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;
using Conditions::Condition;

// ======================================================================================

/// Initializing constructor
AlignmentsRegister::AlignmentsRegister(AlignmentsManager m, AlignmentsUpdateCall* c, UserPool* p)
  : alignmentMgr(m), updateCall(c), user_pool(p), extension("/Tranformations"),
    alias("Alignment"), haveAlias(true), printLevel(DEBUG)
{
}

/// Default destructor
AlignmentsRegister::~AlignmentsRegister()   {
  releasePtr(updateCall);
}

/// Overloadable: call to construct the alignment conditions name. Specialize for change
std::string
AlignmentsRegister::construct_name(DetElement /* de */, Condition cond) const
{
  return cond.name()+extension;
}

/// Callback to output conditions information
int AlignmentsRegister::processElement(DetElement de)  {
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
                 "++ Processing DE %s Cond:%s Key:%16llX flags:%d",
                 de.path().c_str(), cond.name(), cond.key(), cond->flags);
        //
        // Due to this very check we have to load the condition.....
        //
        if ( (cond->flags&Condition::ALIGNMENT) )  {
          std::string alignment_name = construct_name(de, cond);
          if ( !alignment_name.empty() )  {
            Conditions::ConditionKey k(alignment_name);
            //
            // The alignment access through the DetElement object is optional!
            // It is slow and deprecated. The access using the UserPool directly
            // is highly favored.
            //
            align.alignments().addKey(k.name);
            if ( haveAlias && !alias.empty() )  {
              align.alignments().addKey(alias,k.name);
            }
            //
            // Now add the dependency to the alignmant manager
            Conditions::DependencyBuilder b(k, updateCall, de);
            b.add(Conditions::ConditionKey(cond->name));
            bool result = alignmentMgr.adoptDependency(b.release());
            if ( result )   {
              printout(printLevel,"AlignRegister",
                       "++ Added Alignment dependency Cond:%s Key:%16llX flags:%d",
                       k.name.c_str(), k.hash, cond->flags);
              continue;
            }
            printout(ERROR,"AlignRegister",
                     "++ FAILED to add Alignment dependency Cond:%s Key:%16llX flags:%d",
                     k.name.c_str(), k.hash, cond->flags);
          }
        }
      }
      return 1;
    }
    printout(DEBUG,"AlignRegister","++ Processing DE %s hasConditions:%s",
             de.path().c_str(), yes_no(de.hasConditions()));
  }
  return 1;
}

