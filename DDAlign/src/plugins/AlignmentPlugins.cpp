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
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DetectorTools.h"

using namespace std;
using namespace DD4hep;

#include "DDAlign/AlignmentWriter.h"

namespace DetectorTools = DD4hep::Geometry::DetectorTools;
static long create_alignment_file(Geometry::LCDD& lcdd, int argc, char** argv)   {
  Geometry::DetElement top;
  string output, path = "/world";
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
        throw runtime_error("AlignmentWriter: Invalid argument:"+string(argv[i]));
    }
  }
  printout(ALWAYS,"AlignmentWriter",
           "++++ Writing DD4hep alignment constants of the \"%s\" DetElement tree to file \"%s\"",
           path.c_str(), output.c_str());
  top = DetectorTools::findDaughterElement(lcdd.world(),path);
  if ( top.isValid() )   {
    Alignments::AlignmentWriter wr(lcdd);
    return wr.write(wr.dump(top,enable_transactions), output);
  }
  throw runtime_error("AlignmentWriter: Invalid top level element name:"+path);
}
DECLARE_APPLY(DDAlignmentWriter, create_alignment_file)
