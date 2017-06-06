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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/detail/DetectorInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Anonymous namespace for plugins
namespace  {
  class VolumeScan {
    std::set<TGeoVolume*> scanned_vols;
  public:
    VolumeScan() {}
    int scan_daughters(TGeoVolume* vol, string path)   {
      int count = 0;
      auto ivol = scanned_vols.find(vol);
      if ( ivol == scanned_vols.end() )  {
        int num_dau = vol->GetNdaughters();
        scanned_vols.insert(vol);
        path += "/";
        path += vol->GetName();
        printout(INFO,"DDDB_vol_dump","%s",path.c_str());
        ++count;
        for(int i=0; i<num_dau; ++i)  {
          TGeoNode*   n = vol->GetNode(i);
          TGeoVolume* v = n->GetVolume();
          count += scan_daughters(v,path);
        }
      }
      return count;
    }
  };

  /// Plugin function
  long dddb_dump_logical_volumes(LCDD& lcdd, int , char** ) {
    VolumeScan scan;
    Volume world_vol = lcdd.worldVolume();
    int count = scan.scan_daughters(world_vol,string());
    printout(INFO,"DDDB_vol_dump","Found %d unique logical volumes.",count);
    return 1;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_LogVolumeDump,dddb_dump_logical_volumes)
//==========================================================================
