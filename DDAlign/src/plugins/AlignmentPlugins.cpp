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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

// ======================================================================================
#include "DDAlign/AlignmentsManager.h"

namespace {
  /// Plugin function:
  /// Install the alignment manager as an extension to the central LCDD object
  int ddalign_install_align_mgr (Geometry::LCDD& lcdd, int /* argc */, char** /* argv */)  {
    Handle<AlignmentsManagerObject> mgr(lcdd.extension<AlignmentsManagerObject>(false));
    if ( !mgr.isValid() )  {
      AlignmentsManager mgr_handle("LCDD_AlignmentManager");
      lcdd.addExtension<AlignmentsManagerObject>(mgr_handle.ptr());
      printout(INFO,"AlignmentsManager","+++ Successfully installed alignments manager instance to LCDD.");
    }
    return 1;
  }
}  /* End anonymous namespace  */
DECLARE_APPLY(DD4hep_AlignmentsManagerInstaller,ddalign_install_align_mgr)

// ======================================================================================
#include "DDAlign/AlignmentWriter.h"
namespace {
  namespace DetectorTools = DD4hep::Geometry::DetectorTools;
  long create_global_alignment_file(Geometry::LCDD& lcdd, int argc, char** argv)   {
    Geometry::DetElement top;
    std::string output, path = "/world";
    bool enable_transactions = false;
    for(int i=1; i<argc;++i) {
      if ( argv[i][0]=='-' || argv[i][0]=='/' ) {
        const char* p = ::strchr(argv[i],'=');
        if ( p && strncmp(argv[i]+1,"-output",7)==0 )
          output = p+1;
        else if ( p && strncmp(argv[i]+1,"-path",5)==0 )
          path = p+1;
        else if ( strncmp(argv[i]+1,"-transactions",5)==0 )
          enable_transactions = true;
        else
          throw std::runtime_error("AlignmentWriter: Invalid argument:"+std::string(argv[i]));
      }
    }
    printout(ALWAYS,"AlignmentWriter",
             "++++ Writing DD4hep alignment constants of the \"%s\" DetElement tree to file \"%s\"",
             path.c_str(), output.c_str());
    top = DetectorTools::findDaughterElement(lcdd.world(),path);
    if ( top.isValid() )   {
      AlignmentWriter wr(lcdd);
      return wr.write(wr.dump(top,enable_transactions), output);
    }
    throw std::runtime_error("AlignmentWriter: Invalid top level element name:"+path);
  }
}  /* End anonymous namespace  */
DECLARE_APPLY(DD4hep_GlobalAlignmentWriter, create_global_alignment_file)
// ======================================================================================
