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
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/objects/AlignmentsInterna.h"

#include "DDAlign/AlignmentsManager.h"
#include "DDAlign/AlignmentUpdateCall.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

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
  class AlignmentForward : public DetElement::Processor {
  public:
    LCDD&                  lcdd;
    AlignmentsManager      alignmentMgr;
    AlignmentUpdateCall*   updateCall;
    Conditions::UserPool*  user_pool;

    /// Initializing constructor
    AlignmentForward(LCDD& l, AlignmentUpdateCall* c, UserPool* p)
      : lcdd(l), updateCall(c), user_pool(p)
    {
      alignmentMgr = AlignmentsManager::from(lcdd);
    }
    /// Default destructor
    virtual ~AlignmentForward()   {
      releasePtr(updateCall);
    }
    /// Callback to output conditions information
    virtual int processElement(DetElement de)  {
      if ( de.isValid() )  {
        DetElement parent = de.parent();
        if ( parent.isValid() && parent.hasAlignments() && !de.hasAlignments() )  {
          DetAlign align(de);
          Conditions::ConditionKey k(de.path()+"#alignment/Tranformations");
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
          bool result = alignmentMgr.adoptDependency(b.release());
          if ( result )   {
            printout(INFO,"AlignForward",
                     "++ Added Alignment child dependency Cond:%s Key:%08X",
                     k.name.c_str(), k.hash);
            return 1;
          }
          printout(ERROR,"AlignForward",
                   "++ FAILED to add Alignment dependency Cond:%s Key:%08X",
                   k.name.c_str(), k.hash);
        }
      }
      return 1;
    }
  };
}

#include "DD4hep/PluginTester.h"
#include "DDCond/ConditionsPool.h"
/// Convert alignments conditions to alignment objects
static void* ddalign_AlignmentForward(Geometry::LCDD& lcdd, int argc, char** argv)  {
  std::vector<char*> args_prepare, args_call;

  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( ::strcmp(argv[i],"-call") == 0 )  {
      while( (++i)<argc && argv[i] && 0 != ::strcmp(argv[i],"-call-end") )
        args_call.push_back(argv[i]);
    }
  }

  PluginTester* test = lcdd.extension<PluginTester>();
  Conditions::UserPool* pool = test->extension<Conditions::UserPool>("ConditionsTestUserPool");
  AlignmentUpdateCall*  call = (AlignmentUpdateCall*)
    PluginService::Create<void*>((const char*)args_call[0],&lcdd,
                                 int(args_call.size())-1,
                                 (char**)&args_call[1]);
  if ( 0 == call )  {
    except("AlignForward","++ Failed to create update call!");
  }
  AlignmentForward* obj = new AlignmentForward(lcdd, call, pool);
  return obj;
}
DECLARE_LCDD_CONSTRUCTOR(DDAlign_AlignmentForward,ddalign_AlignmentForward)

