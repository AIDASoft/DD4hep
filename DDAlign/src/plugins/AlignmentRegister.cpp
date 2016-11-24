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
  class AlignmentRegister : public DetElement::Processor {
  public:
    LCDD&                            lcdd;
    Alignments::AlignmentsManager    alignmentMgr;
    AlignmentUpdateCall*             updateCall;
    Conditions::UserPool*            user_pool;

    /// Initializing constructor
    AlignmentRegister(LCDD& l, AlignmentUpdateCall* c, UserPool* p)
      : lcdd(l), updateCall(c), user_pool(p)
    {
      alignmentMgr = AlignmentsManager::from(lcdd);
    }
    /// Default destructor
    virtual ~AlignmentRegister()   {
      releasePtr(updateCall);
    }
    /// Callback to output conditions information
    virtual int processElement(DetElement de)  {
      if ( de.isValid() )  {
        if ( de.hasConditions() )  {
          DetAlign align(de);
          DetConditions conditions(de);
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
              ConditionKey k(cond->name+"/Tranformations");
              //
              // The alignment access through the DetElement object is optional!
              // It is slow and deprecated. The access using the UserPool directly
              // is highly favored.
              //
              align.alignments()->addKey(k.name);
              align.alignments()->addKey("Alignment",k.name);
              //
              // Now add the dependency to the alignmant manager
              DependencyBuilder b(k, updateCall->addRef(), de);
              b.add(ConditionKey(cond->name));
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
  };
}

#include "DD4hep/PluginTester.h"
#include "DDCond/ConditionsPool.h"
/// Convert alignments conditions to alignment objects
static void* ddalign_AlignmentRegister(Geometry::LCDD& lcdd, int argc, char** argv)  {
  std::vector<char*> args_prepare, args_call;

  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( ::strcmp(argv[i],"-prepare") == 0 )  {
      while( (++i)<argc && argv[i] && 0 != ::strcmp(argv[i],"-prepare-end") )
        args_prepare.push_back(argv[i]);
    }
    if ( ::strcmp(argv[i],"-call") == 0 )  {
      while( (++i)<argc && argv[i] && 0 != ::strcmp(argv[i],"-call-end") )
        args_call.push_back(argv[i]);
    }
  }

  PluginTester* test = lcdd.extension<PluginTester>();
  Conditions::UserPool* pool = (Conditions::UserPool*)
    PluginService::Create<void*>((const char*)args_prepare[0],&lcdd,
                                 int(args_prepare.size())-1,
                                 (char**)&args_prepare[1]);
  if ( 0 == pool )  {
    except("AlignRegister","++ Failed to prepare conditions user-pool!");
  }
  test->addExtension<Conditions::UserPool>(pool,"ConditionsTestUserPool");
  AlignmentUpdateCall* call = (AlignmentUpdateCall*)
    PluginService::Create<void*>((const char*)args_call[0],&lcdd,
                                 int(args_call.size())-1,
                                 (char**)&args_call[1]);
  if ( 0 == call )  {
    except("AlignRegister","++ Failed to create update call!");
  }
  AlignmentRegister* obj = new AlignmentRegister(lcdd, call, pool);
  return obj;
}
DECLARE_LCDD_CONSTRUCTOR(DDAlign_AlignmentRegister,ddalign_AlignmentRegister)

