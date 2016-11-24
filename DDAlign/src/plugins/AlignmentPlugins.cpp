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
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;

// ======================================================================================
#include "DDAlign/AlignmentsManager.h"

namespace {
  /// Plugin function:
  /// Install the alignment manager as an extension to the central LCDD object
  int ddalign_install_align_mgr (Geometry::LCDD& lcdd, int argc, char** argv)  {
    Handle<AlignmentsManagerObject> mgr(lcdd.extension<AlignmentsManagerObject>(false));
    if ( !mgr.isValid() )  {
      AlignmentsManager mgr_handle("LCDD_AlignmentManager");
      lcdd.addExtension<AlignmentsManagerObject>(mgr_handle.ptr());
      printout(INFO,"AlignmentsManager",
               "+++ Successfully installed alignments manager instance to LCDD.");
      mgr = mgr_handle;
    }
    if ( argc == 2 )  {
      if ( ::strncmp(argv[0],"-handle",7)==0 )  {
        Handle<NamedObject>* h = (Handle<NamedObject>*)argv[1];
        *h = mgr;
      }
    }
    return 1;
  }
}  /* End anonymous namespace  */
DECLARE_APPLY(DD4hep_AlignmentsManagerInstaller,ddalign_install_align_mgr)

// ======================================================================================
#include "DDAlign/GlobalAlignmentWriter.h"
namespace {
  namespace DetectorTools = DD4hep::Geometry::DetectorTools;
  long create_global_alignment_file(Geometry::LCDD& lcdd, int argc, char** argv)   {
    Geometry::DetElement top;
    string output, path = "/world";
    bool enable_transactions = false, arg_error = false;
    for(int i=1; i<argc;++i) {
      if ( argv[i] && (argv[i][0]=='-' || argv[i][0]=='/') ) {
        const char* p = ::strchr(argv[i],'=');
        if ( p && strncmp(argv[i]+1,"-output",7)==0 )
          output = p+1;
        else if ( p && strncmp(argv[i]+1,"-path",5)==0 )
          path = p+1;
        else if ( strncmp(argv[i]+1,"-transactions",5)==0 )
          enable_transactions = true;
        else
          arg_error = true;
      }
    }

    if ( arg_error || output.empty() || path.empty() )  {
      /// Help printout describing the basic command line interface
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                      \n"
        "     name:   factory nameDD4hep_GlobalAlignmentWriter                \n\n"
        "     -output <string>         Path to the output file generated.       \n"
        "     -path   <string>         Path to the detector element for which   \n"
        "                              the alignment file should be written.    \n"
        "     -transactions            Enable output transactions.              \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }

    printout(ALWAYS,"AlignmentWriter",
             "++ Writing DD4hep alignment constants of the \"%s\" DetElement tree to file \"%s\"",
             path.c_str(), output.c_str());
    top = DetectorTools::findDaughterElement(lcdd.world(),path);
    if ( top.isValid() )   {
      GlobalAlignmentWriter wr(lcdd);
      return wr.write(wr.dump(top,enable_transactions), output);
    }
    except("AlignmentWriter","++ Invalid top level detector element name: %s",path.c_str());
    return 1;
  }
}  /* End anonymous namespace  */
DECLARE_APPLY(DD4hep_GlobalAlignmentWriter, create_global_alignment_file)

// ======================================================================================
#include "DDAlign/DDAlignUpdateCall.h"
static void* create_DDAlignUpdateCall(Geometry::LCDD& /* lcdd */, int /* argc */, char** /* argv */)   {
  return (AlignmentUpdateCall*)(new DDAlignUpdateCall());
}
DECLARE_LCDD_CONSTRUCTOR(DDAlign_UpdateCall, create_DDAlignUpdateCall)

// ======================================================================================
#include "DDAlign/DDAlignForwardCall.h"
static void* create_DDAlignForwardCall(Geometry::LCDD& /* lcdd */, int /* argc */, char** /* argv */)   {
  return (AlignmentUpdateCall*)(new DDAlignForwardCall());
}
DECLARE_LCDD_CONSTRUCTOR(DDAlign_ForwardCall, create_DDAlignForwardCall)

#include "DD4hep/PluginTester.h"
#include "DDCond/ConditionsPool.h"
// ======================================================================================
static long compute_alignments(Geometry::LCDD& lcdd, int /* argc */, char** /* argv */)   {
  AlignmentsManager mgr = AlignmentsManager::from(lcdd);
  PluginTester*     tst = lcdd.extension<PluginTester>();
  dd4hep_ptr<UserPool> pool(tst->extension<UserPool>("ConditionsTestUserPool"));
  mgr.compute(pool);
  pool.release();
  return 1;
}
DECLARE_APPLY(DDAlign_ComputeAlignments, compute_alignments)
