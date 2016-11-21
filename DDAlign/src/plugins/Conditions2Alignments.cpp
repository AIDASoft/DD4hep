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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/objects/AlignmentsInterna.h"

#include "DDAlign/AlignmentsManager.h"
#include "DDAlign/AlignmentUpdateCall.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;
using Conditions::Condition;
using Conditions::ConditionKey;
using Conditions::ConditionsManager;
using Conditions::DetConditions;
using Conditions::DependencyBuilder;

// ======================================================================================

/// Anonymous namespace for plugins
namespace  {

  /// Create lignment dependencies from conditions
  /**
   *   Conditions analyser to select alignments.and create the
   *   corresponding alignment condition dependencies.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    31/03/2016
   *   \ingroup DD4HEP_DDALIGN
   */
  class Conditions2Alignments : public DetElement::Processor {
  public:
    LCDD&                            lcdd;
    Alignments::AlignmentsManager    alignmentMgr;
    AlignmentUpdateCall*             updateCall;
    Conditions::UserPool*            user_pool;

    /// Initializing constructor
    Conditions2Alignments(LCDD& l, AlignmentUpdateCall* c, UserPool* p)
      : lcdd(l), updateCall(c), user_pool(p)
    {
      alignmentMgr = AlignmentsManager::from(lcdd);
    }
    /// Default destructor
    virtual ~Conditions2Alignments()   {
      releasePtr(updateCall);
    }
    /// Callback to output conditions information
    virtual int operator()(DetElement de)  {
      if ( de.isValid() )  {
        if ( de.hasConditions() )  {
          DetAlign align(de);
          DetConditions conditions(de);
          Conditions::Container cont = conditions.conditions();
          printout(INFO,"Conditions2Alignments",
                   "++ Processing DE %s hasConditions:%s [%d entries]",
                   de.path().c_str(), yes_no(de.hasConditions()), int(cont.numKeys()));
          for ( const auto& c : cont.keys() )  {
            Condition cond = cont.get(c.first, *user_pool);
            printout(INFO,"Conditions2Alignments",
                     "++ Processing DE %s Cond:%s Key:%08X flags:%d",
                     de.path().c_str(), cond.name().c_str(), cond.key(), cond->flags);
            if ( (cond->flags&Condition::ALIGNMENT) )  {
              ConditionKey k(cond.name()+"/Tranformations");
              //
              // The alignment access through the DetElement object is optional!
              // It is slow and deprecated. The access using the UserPool directly
              // is highly favored.
              //
              align.alignments()->addKey(k.name);
              align.alignments()->addKey("Alignment",k.name);
              //
              // Now add the dependency to the alignmant manager
              DependencyBuilder b(k, updateCall->addRef());
              b->detector = de;
              b.add(ConditionKey(cond->name));
              Conditions::ConditionDependency* dep = b.release();
              bool result = alignmentMgr.adoptDependency(dep);
              if ( result )   {
                printout(INFO,"Conditions2Alignments",
                         "++ Added Alignment dependency Cond:%s Key:%08X",
                         k.name.c_str(), k.hash);
                continue;
              }
              printout(ERROR,"Conditions2Alignments",
                       "++ FAILED to add Alignment dependency Cond:%s Key:%08X",
                       k.name.c_str(), k.hash);
            }
          }
          return 1;
        }
        printout(INFO,"Conditions2Alignments","++ Processing DE %s hasConditions:%s",
                 de.path().c_str(), yes_no(de.hasConditions()));
      }
      return 1;
    }
  };
}


#include "DDAlign/DDAlignTest.h"

/// Convert alignments conditions to alignment objects
static void* ddalign_Conditions2Alignments(Geometry::LCDD& lcdd, int argc, char** argv)  {
  std::vector<char*> args_prepare, args_call;

  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( ::strcmp(argv[i],"-prepare") == 0 )  {
      while( 0 != ::strcmp(argv[++i],"-prepare-end") && i<argc )
        args_prepare.push_back(argv[i]);
    }
    if ( ::strcmp(argv[i],"-call") == 0 )  {
      while( 0 != ::strcmp(argv[++i],"-call-end") && i<argc )
        args_call.push_back(argv[i]);
    }
  }

  DDAlignTest* test = lcdd.extension<DDAlignTest>();
  Conditions::UserPool* pool = (Conditions::UserPool*)
    PluginService::Create<void*>((const char*)args_prepare[0],&lcdd,
                                 int(args_prepare.size())-1,
                                 (char**)&args_prepare[1]);
  if ( 0 == pool )  {
    except("Conditions2Alignments","++ Failed to prepare conditions user-pool!");
  }
  test->alignmentsPool.adopt(pool);
  AlignmentUpdateCall* call = (AlignmentUpdateCall*)
    PluginService::Create<void*>((const char*)args_call[0],&lcdd,
                                 int(args_call.size())-1,
                                 (char**)&args_call[1]);
  if ( 0 == call )  {
    except("Conditions2Alignments","++ Failed to create update call!");
  }
  Conditions2Alignments* obj = new Conditions2Alignments(lcdd, call, pool);
  return obj;
}
DECLARE_LCDD_CONSTRUCTOR(DDAlign_Conditions2Alignments,ddalign_Conditions2Alignments)
