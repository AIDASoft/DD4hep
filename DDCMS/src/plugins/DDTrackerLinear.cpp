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
#include "DD4hep/Printout.h"
#include "DDCMS/DDCMSPlugins.h"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::cms;

static long create_element(Detector& /* description */,
                           ParsingContext& ctxt,
                           xml_h e,
                           SensitiveDetector& /* sens */)
{
  stringstream   str;
  Namespace      ns(ctxt, e, true);
  AlgoArguments  args(ctxt, e);
  int            startcn = args.find("StartCopyNo") ? args.value<int>("StartCopyNo") : 1;
  int            incrcn  = args.find("IncrCopyNo") ? args.value<int>("IncrCopyNo") : 1;
  int            number  = args.value<int>("Number");
  double         theta   = args.value<double>("Theta");
  double         phi     = args.value<double>("Phi");
  double         offset  = args.value<double>("Offset");
  double         delta   = args.value<double>("Delta");
  vector<double> centre  = args.value<vector<double> >("Center");
  string         rotMat  = args.value<string>("Rotation");
  Volume         mother  = ns.volume(args.parentName());
  Volume         child   = ns.volume(args.value<string>("ChildName"));

  printout(INFO,"DDTrackerLinear","+++ Executing Algorithm. rParent:%s",mother.name());
  str << "debug: Parent " << mother.name() 
      << "\tChild " << child.name() << " NameSpace " 
      << ns.name << "\tNumber " << number 
      << "\tAxis (theta/phi) " << theta/dd4hep::deg << ", "
      << phi/dd4hep::deg << "\t(Offset/Delta) " << offset 
      << ", "  << delta << "\tCentre " << centre[0] << ", "
      << centre[1] << ", " << centre[2] << "\tRotation "
      << rotMat;
  printout(ctxt.debug_algorithms ? ALWAYS : DEBUG,"DDTrackerLinear",str);

  Position direction(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  Position base(centre[0],centre[1],centre[2]);
  RotationZYX   rot;
  if ( !rotMat.empty() ) {
    rot = ns.rotation(rotMat);
  }
  for (int i=0, ci=startcn; i<number; i++, ci += incrcn) {
    Position tran = base + (offset + double(i)*delta)*direction;
    // Copy number ???
    /* PlacedVolume pv = */ rotMat.empty()
      ? mother.placeVolume(child,Transform3D(rot,tran))
      : mother.placeVolume(child,tran);
    str << child.name() << " number "
        << ci << " positioned in " << mother.name() << " at "
        << tran << " with " << rot;
    printout(ctxt.debug_algorithms ? ALWAYS : DEBUG,"DDTrackerLinear",str);
  }
  return 1;
}

// first argument is the type from the xml file
DECLARE_DDCMS_DETELEMENT(track_DDTrackerLinear,create_element)

