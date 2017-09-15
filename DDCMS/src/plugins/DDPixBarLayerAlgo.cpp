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
#include "CLHEP/Units/SystemOfUnits.h"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::cms;

static Ref_t create_element(Detector& description, ParsingContext& ctxt, xml_h e, SensitiveDetector& /* sens */)  {
  stringstream  str;
  PlacedVolume  pv;
  Namespace     ns(ctxt);
  AlgoArguments args(ctxt, e);
  string        parentName = args.parentName();

  printout(INFO,"DDCMS","+++ Parsing arguments for Algorithm:%-24s rParent:%s",
           args.name.c_str(), parentName.c_str());

  string genMat    = args.value<string>("GeneralMaterial");
  int    number    = args.value<int>("Ladders");
  double layerDz   = args.value<double>("LayerDz");
  double sensorEdge= args.value<double>("SensorEdge");
  double coolDz    = args.value<double>("CoolDz");
  double coolWidth = args.value<double>("CoolWidth");
  double coolSide  = args.value<double>("CoolSide");
  double coolThick = args.value<double>("CoolThick");
  double coolDist  = args.value<double>("CoolDist");
  string coolMat   = args.value<string>("CoolMaterial");
  string tubeMat   = args.value<string>("CoolTubeMaterial");

  str.str("");
  str << "DDPixBarLayerAlgo debug: Parent " << parentName 
      << " NameSpace " << ns.name << "\n"
      << "\tLadders " << number << "\tGeneral Material " 
      << genMat << "\tLength " << layerDz << "\tSensorEdge "
      << sensorEdge << "\tSpecification of Cooling Pieces:\n"
      << "\tLength " << coolDz << " Width " << coolWidth 
      << " Side " << coolSide << " Thickness of Shell " 
      << coolThick << " Radial distance " << coolDist 
      << " Materials " << coolMat << ", " << tubeMat;
  printout(DEBUG,"PixelGeom",str.str().c_str());

  vector<string> ladder      = args.value<vector<string> >("LadderName");
  vector<double> ladderWidth = args.value<vector<double> >("LadderWidth");
  vector<double> ladderThick = args.value<vector<double> >("LadderThick");
  
  str.str("");
  str << "DDPixBarLayerAlgo debug: Full Ladder " 
      << ladder[0] << " width/thickness " << ladderWidth[0]
      << ", " << ladderThick[0] << "\tHalf Ladder " 
      << ladder[1] << " width/thickness " << ladderWidth[1]
      << ", " << ladderThick[1];
  printout(DEBUG,"PixelGeom",str.str().c_str());

  string mother = parentName;
  const std::string &idName = mother;

  double dphi = CLHEP::twopi/number;
  double d2   = 0.5*coolWidth;
  double d1   = d2 - coolSide*sin(0.5*dphi);
  double x1   = (d1+d2)/(2.*sin(0.5*dphi));
  double x2   = coolDist*sin(0.5*dphi);
  double rmin = (coolDist-0.5*(d1+d2))*cos(0.5*dphi)-0.5*ladderThick[0];
  double rmax = (coolDist+0.5*(d1+d2))*cos(0.5*dphi)+0.5*ladderThick[0];
  double rmxh = rmax - 0.5*ladderThick[0] + ladderThick[1];
  str.str("");
  str << "DDPixBarLayerAlgo test: Rmin/Rmax " << rmin 
      << ", " << rmax << " d1/d2 " << d1 << ", " << d2 
      << " x1/x2 " << x1 << ", " << x2;
  printout(DEBUG,"PixelGeom",str.str().c_str());

  double rtmi = rmin + 0.5*ladderThick[0] - ladderThick[1];
  double rtmx = sqrt(rmxh*rmxh+ladderWidth[1]*ladderWidth[1]);
  Solid solid = Tube(rtmi, rtmx, 0.5*layerDz, 0, CLHEP::twopi);
  solid.setName(idName);
  str.str("");
  str << "DDPixBarLayerAlgo test: " 
      << idName << " Tubs made of " 
      << genMat << " from 0 to " << CLHEP::twopi/CLHEP::deg 
      << " with Rin " << rtmi << " Rout " << rtmx 
      << " ZHalf " << 0.5*layerDz;
  printout(DEBUG,"PixelGeom",str.str().c_str());

  Material matter;
  Volume   layer(solid.name(), solid, description.material(genMat));

  double rr = 0.5*(rmax+rmin);
  double dr = 0.5*(rmax-rmin);
  double h1 = 0.5*coolSide*cos(0.5*dphi);
  std::string name = idName + "CoolTube";
  solid = Trap(0.5*coolDz, 0, 0, h1, d2, d1, 0, h1, d2, d1, 0);
  solid.setName(idName+"CoolTube");
  str.str("");
  str << "DDPixBarLayerAlgo test: " <<solid.name() 
      << " Trap made of " << tubeMat << " of dimensions " 
      << 0.5*coolDz << ", 0, 0, " << h1 << ", " << d2 
      << ", " << d1 << ", 0, " << h1 << ", " << d2 << ", " 
      << d1 << ", 0";
  printout(DEBUG,"PixelGeom",str.str().c_str());
  Volume coolTube(solid.name(), solid, description.material(tubeMat));
  
  h1  -= coolThick;
  d1  -= coolThick;
  d2  -= coolThick;
  solid = Trap(0.5*coolDz, 0, 0, h1, d2, d1, 0, h1, d2, d1, 0);
  solid.setName(idName + "Coolant");
  
  str.str("");
  str << "DDPixBarLayerAlgo test: " <<solid.name() 
      << " Trap made of " << coolMat << " of dimensions " 
      << 0.5*coolDz << ", 0, 0, " << h1 << ", " << d2
      << ", " << d1 << ", 0, " << h1 << ", " << d2 << ", " 
      << d1 << ", 0";
  printout(DEBUG,"PixelGeom",str.str().c_str());

  Volume cool(solid.name(), solid, description.material(coolMat));

  //cpv.position(cool, coolTube, 1, DDTranslation(0.0, 0.0, 0.0), DDRotation());
  pv = coolTube.placeVolume(cool);
  str.str("");
  str << "DDPixBarLayerAlgo test: " << cool.name() 
      << " number 1 positioned in " << coolTube.name() 
      << " at (0,0,0) with no rotation";
  printout(DEBUG,"PixelGeom",str.str().c_str());

  string ladderFull = ladder[0];
  string ladderHalf = ladder[1];
  int nphi=number/2, copy=1, iup=-1;
  double phi0 = 90*CLHEP::deg;
  Volume ladderHalfVol = ctxt.volume(ladderHalf);
  Volume ladderFullVol = ctxt.volume(ladderFull);

  for (int i=0; i<number; i++) {
    double phi = phi0 + i*dphi;
    double phix, phiy, rrr, xx;
    std::string rots;
    Position    tran;
    Rotation3D rot;
    if (i == 0 || i == nphi) {
      rrr  = rr + dr + 0.5*(ladderThick[1]-ladderThick[0]);
      xx   = (0.5*ladderWidth[1] - sensorEdge) * sin(phi);
      tran = Position(xx, rrr*sin(phi), 0);
      rots = idName + std::to_string(copy);
      phix = phi-90*CLHEP::deg;
      phiy = 90*CLHEP::deg+phix;
      str.str("");
      str << "DDPixBarLayerAlgo test: Creating a new "
                            << "rotation: " << rots << "\t90., " 
                            << phix/CLHEP::deg << ", 90.," << phiy/CLHEP::deg 
                            << ", 0, 0";
      printout(DEBUG,"PixelGeom",str.str().c_str());
      rot = make_rotation3D(90*CLHEP::deg, phix, 90*CLHEP::deg, phiy, 0.,0.);

      //cpv.position(ladderHalf, layer, copy, tran, rot);
      pv = layer.placeVolume(ladderHalfVol, Transform3D(rot,tran));
      if ( !pv.isValid() )  {  }
      str.str("");
      str << "DDPixBarLayerAlgo test: " << ladderHalfVol.name()
          << " number " << copy << " positioned in " 
          << layer.name() << " at " << tran << " with " 
          << rot;
      printout(DEBUG,"PixelGeom",str.str().c_str());

      copy++;
      iup  = -1;
      rrr  = rr - dr - 0.5*(ladderThick[1]-ladderThick[0]);
      tran = Position(-xx, rrr*sin(phi), 0);
      rots = idName + std::to_string(copy);
      phix = phi+90*CLHEP::deg;
      phiy = 90*CLHEP::deg+phix;
      str.str("");
      str << "DDPixBarLayerAlgo test: Creating a new "
          << "rotation: " << rots << "\t90., " 
          << phix/CLHEP::deg << ", 90.," << phiy/CLHEP::deg 
          << ", 0, 0";
      printout(DEBUG,"PixelGeom",str.str().c_str());

      rot = make_rotation3D(90*CLHEP::deg, phix, 90*CLHEP::deg, phiy, 0.,0.);
      //cpv.position(ladderHalf, layer, copy, tran, rot);
      pv = layer.placeVolume(ladderHalfVol, Transform3D(rot,tran));
      if ( !pv.isValid() )  {  }
      str.str("");
      str << "DDPixBarLayerAlgo test: " << ladderHalfVol.name()
          << " number " << copy << " positioned in " 
          << layer.name() << " at " << tran << " with " 
          << rot;
      printout(DEBUG,"PixelGeom",str.str().c_str());
      copy++;
    } else {
      iup  =-iup;
      rrr  = rr + iup*dr;
      tran = Position(rrr*cos(phi), rrr*sin(phi), 0);
      rots = idName + std::to_string(copy);
      if (iup > 0) phix = phi-90*CLHEP::deg;
      else         phix = phi+90*CLHEP::deg;
      phiy = phix+90.*CLHEP::deg;
      str.str("");
      str << "DDPixBarLayerAlgo test: Creating a new "
          << "rotation: " << rots << "\t90., " 
          << phix/CLHEP::deg << ", 90.," << phiy/CLHEP::deg 
          << ", 0, 0";
      printout(DEBUG,"PixelGeom",str.str().c_str());

      rot = make_rotation3D(90*CLHEP::deg, phix, 90*CLHEP::deg, phiy, 0.,0.);

      //cpv.position(ladderFull, layer, copy, tran, rot);
      pv = layer.placeVolume(ladderFullVol, Transform3D(rot,tran));
      if ( !pv.isValid() )  {  }
      str.str("");
      str << "DDPixBarLayerAlgo test: " << ladderFullVol.name()
          << " number " << copy << " positioned in " 
          << layer.name() << " at " << tran << " with " 
          << rot;
      printout(DEBUG,"PixelGeom",str.str().c_str());
      copy++;
    }
    rrr  = coolDist*cos(0.5*dphi);
    tran = Position(rrr*cos(phi)-x2*sin(phi), rrr*sin(phi)+x2*cos(phi), 0);
    rots = idName + std::to_string(i+100);
    phix = phi+0.5*dphi;
    if (iup > 0) phix += 180*CLHEP::deg;
    phiy = phix+90.*CLHEP::deg;
    str.str("");
    str << "DDPixBarLayerAlgo test: Creating a new "
        << "rotation: " << rots << "\t90., " 
        << phix/CLHEP::deg << ", 90.," << phiy/CLHEP::deg 
        << ", 0, 0";
    printout(DEBUG,"PixelGeom",str.str().c_str());
    rot = make_rotation3D(90*CLHEP::deg, phix, 90*CLHEP::deg, phiy, 0.,0.);
    //cpv.position(coolTube, layer, i+1, tran, rot);
    pv = layer.placeVolume(coolTube,Transform3D(rot,tran));
    if ( !pv.isValid() )  {  }
    str.str("");
    str << "DDPixBarLayerAlgo test: " << coolTube.name() 
        << " number " << i+1 << " positioned in " 
        << layer.name() << " at " << tran << " with "<< rot;
    printout(DEBUG,"PixelGeom",str.str().c_str());
  }

  // Now we have built the layer.....create the DetElement object
  string     det_name = "Layer_"+idName;
  DetElement det_elt(det_name,0);
  Assembly   assembly(det_name);
  Volume     motherVol = description.pickMotherVolume(det_elt);
  pv = assembly.placeVolume(layer);
  pv = motherVol.placeVolume(assembly);
  det_elt.setPlacement(pv);
  return det_elt;
}

// first argument is the type from the xml file
DECLARE_DDCMS_DETELEMENT(track_DDPixBarLayerAlgo,create_element)

