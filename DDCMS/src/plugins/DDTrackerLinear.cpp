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

static Ref_t create_element(Detector& /* description */,
                            ParsingContext& ctxt,
                            xml_h e,
                            SensitiveDetector& /* sens */)
{
  stringstream str;
  Namespace     ns(ctxt);
  AlgoArguments args(ctxt, e);
  xml_det_t      x_det   = e;
  string         name    = x_det.nameStr();
  DetElement     det    (name,x_det.id());
  Assembly       assembly(name);
  string         parentName = args.parentName();
  int            startcn    = 1, incrcn = 1;
  int            number     = args.value<int>("Number");
  double         theta      = args.value<double>("Theta");
  double         phi        = args.value<double>("Phi");
  double         offset     = args.value<double>("Offset");
  double         delta      = args.value<double>("Delta");
  vector<double> centre     = args.value<vector<double> >("Center");
  string         rotMat     = args.value<string>("Rotation");
  
  if ( args.find("StartCopyNo") ) {
    startcn = size_t(args.value<double>("StartCopyNo"));
  }
  if ( args.find("IncrCopyNo") ) {
    incrcn = int(args.value<double>("IncrCopyNo"));
  }
  printout(INFO,"DDTrackerLinear","+++ Executing Algorithm:%-24s rParent:%s",
           name.c_str(), parentName.c_str());

  string childName  = args.value<string>("ChildName");
  str << "DDTrackerLinear debug: Parent " << parentName 
      << "\tChild " << childName << " NameSpace " 
      << ns.name << "\tNumber " << number 
      << "\tAxis (theta/phi) " << theta/dd4hep::deg << ", "
      << phi/dd4hep::deg << "\t(Offset/Delta) " << offset 
      << ", "  << delta << "\tCentre " << centre[0] << ", "
      << centre[1] << ", " << centre[2] << "\tRotation "
      << rotMat;
  printout(INFO,"DDTrackerLinear",str.str().c_str());

  Volume   mother = ctxt.volume(parentName);
  Volume   child  = ctxt.volume(childName);
  Position direction(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  Position base(centre[0],centre[1],centre[2]);
  RotationZYX   rot;
  if ( !rotMat.empty() ) {
    rot = ctxt.rotation(rotMat);
  }
  for (int i=0, ci=startcn; i<number; i++, ci += incrcn) {
    Position tran = base + (offset + double(i)*delta)*direction;
    // Copy number ???
    /* PlacedVolume pv = */
    rotMat.empty()
      ? assembly.placeVolume(child,Transform3D(rot,tran))
      : assembly.placeVolume(child,tran);
    str.str("");
    str << child.name() << " number "
        << ci << " positioned in " << mother.name() << " at "
        << tran << " with " << rot;
    printout(INFO,"DDTrackerLinear",str.str().c_str());
  }
  return det;  
}

// first argument is the type from the xml file
DECLARE_DDCMS_DETELEMENT(track_DDTrackerLinear,create_element)

