//==========================================================================
//  AIDA Detector description implementation 
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
// Specialized generic detector constructor
// 
//==========================================================================

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DDCMS/DDCMSPlugins.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cms;

static long algorithm(Detector& /* description */,
                      ParsingContext& ctxt,
                      xml_h e,
                      SensitiveDetector& /* sens */)
{
  Namespace      ns(ctxt, e, true);
  AlgoArguments  args(ctxt, e);
  int            startCopyNo = args.find("StartCopyNo") ? args.value<int>("StartCopyNo") : 1;
  int            incrCopyNo  = args.find("IncrCopyNo") ? args.value<int>("IncrCopyNo") : 1;
  Volume         mother      = ns.volume(args.parentName());
  Volume         child       = ns.volume(args.value<string>("ChildName"));
  vector<double> zvec        = args.value<vector<double> >("ZPositions");    // Z positions
  vector<string> rotMat      = args.value<vector<string> >("Rotations");   // Names of rotation matrices

  LogDebug("TrackerGeom") << "debug: Parent " << mother.name() 
                          << "\tChild " << child.name() << " NameSpace " 
                          << ns.name << "\tCopyNo (Start/Increment) " 
                          << startCopyNo << ", " << incrCopyNo << "\tNumber " << zvec.size();
  for (int i = 0; i < (int)(zvec.size()); i++)
    LogDebug("TrackerGeom") << "\t[" << i << "]\tZ = " << zvec[i]
                            << ", Rot.Matrix = " << rotMat[i];

  for (int i=0, copy = startCopyNo; i<(int)(zvec.size()); i++) {
    Position tran(0, 0, zvec[i]);
    Rotation3D rot;
    /* PlacedVolume pv = */ rotMat[i] != "NULL"
      ? mother.placeVolume(child,Transform3D(ns.rotation(rotMat[i]),tran))
      : mother.placeVolume(child,tran);
    LogDebug("TrackerGeom") << "test: " << child.name() 
                            <<" number " << copy << " positioned in " 
                            << mother.name() << " at " << tran << " with " << rot;
    copy += incrCopyNo;
  }
  return 1;
}

// first argument is the type from the xml file
DECLARE_DDCMS_DETELEMENT(DDCMS_track_DDTrackerZPosAlgo,algorithm)

