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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetFactoryHelper.h"
#include "XML/VolumeBuilder.h"
#include <cmath>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;


// makes sure that the RotationMatrix built is
// LEFT-handed coordinate system (i.e. reflected)
namespace   {
  struct NestedBoxReflection : public xml::tools::VolumeBuilder  {
    using VolumeBuilder::VolumeBuilder;
    
    Rotation3D makeRotReflect(double thetaX, double phiX, double thetaY, double phiY, double thetaZ, double phiZ) {
      // define 3 unit std::vectors forming the new left-handed axes
      Position x(cos(phiX) * sin(thetaX), sin(phiX) * sin(thetaX), cos(thetaX));
      Position y(cos(phiY) * sin(thetaY), sin(phiY) * sin(thetaY), cos(thetaY));
      Position z(cos(phiZ) * sin(thetaZ), sin(phiZ) * sin(thetaZ), cos(thetaZ));

      constexpr double tol = 1.0e-3;       // Geant4 compatible
      double check = (x.Cross(y)).Dot(z);  // in case of a LEFT-handed orthogonal system this must be -1
      if (abs(1. + check) > tol) {
	except("NestedBoxReflection", "+++ FAILED to construct Rotation is not LEFT-handed!");
      }
      printout(INFO, "NestedBoxReflection", "+++ Constructed LEFT-handed reflection rotation.");
      Rotation3D rotation(x.x(), y.x(), z.x(), x.y(), y.y(), z.y(), x.z(), y.z(), z.z());
      return rotation;
    }

    TGeoCombiTrans* createPlacement(const Rotation3D& iRot, const Position& iTrans) {
      TGeoRotation r;
      double elements[9];
      iRot.GetComponents(elements);
      r.SetMatrix(elements);
      return new TGeoCombiTrans(TGeoTranslation(iTrans.x(), iTrans.y(), iTrans.z()), r);
    }

    //static Transform3D transform_reflect(xml_h element)   {
    TGeoCombiTrans* transform_reflect(xml_h element)   {
      xml_dim_t xrot(element.child(_U(rotation)));
      xml_dim_t xpos(element.child(_U(position)));
      double thetaX = xrot.attr<double>(Unicode("thetaX"));
      double phiX   = xrot.attr<double>(Unicode("phiX"));
      double thetaY = xrot.attr<double>(Unicode("thetaY"));
      double phiY   = xrot.attr<double>(Unicode("phiY"));
      double thetaZ = xrot.attr<double>(Unicode("thetaZ"));
      double phiZ   = xrot.attr<double>(Unicode("phiZ"));
      printout(INFO, "NestedBoxReflection",
	       "+++ Adding reflection rotation \"%s\": "
	       "(theta/phi)[rad] X: %6.3f %6.3f Y: %6.3f %6.3f Z: %6.3f %6.3f",
	       element.attr<string>(_U(name)).c_str(), thetaX, phiX, thetaY, phiY, thetaZ, phiZ);
      Rotation3D rot = makeRotReflect(thetaX, phiX, thetaY, phiY, thetaZ, phiZ);
      Position   pos = Position(xpos.x(),xpos.y(),xpos.z());
      //  return Transform3D(rot, pos);
      return createPlacement(rot, pos);
    }

    void place_boxes(int level, Volume vol)    {
      if ( level >= 0 )   {
	Box          box = vol.solid();
	double       bx  = box.x();
	double       by  = box.y();
	double       bz  = box.z();
	Material     mat = vol.material();
	Box          small_box(bx*0.2, by*0.2, bz*0.2);
	const char*  cols[4] = {"VisibleRed","VisibleBlue","VisibleGreen","VisibleYellow"};
	const char*  c;
	PlacedVolume pv;
	Volume       v;

	c = cols[(0+level)%4];
	v = Volume(_toString(1,"box%d"), small_box, mat);
	v.setRegion(vol.region());
	v.setSensitiveDetector(sensitive);
	v.setLimitSet(vol.limitSet());
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0,0,0));
	pv.addPhysVolID(_toString(level,"lvl%d"), 1);
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
	place_boxes(level-1, v);

	c = cols[(1+level)%4];
	v = Volume(_toString(2,"box%d"), small_box, mat);
	v.setRegion(vol.region());
	v.setSensitiveDetector(sensitive);
	v.setLimitSet(vol.limitSet());
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0.95*bx,0,0));
	pv.addPhysVolID(_toString(level,"lvl%d"), 2);
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
	place_boxes(level-1, v);
    
	v = Volume(_toString(1,"axis_x"), Box(bx*0.25, by*0.03, bz*0.03), mat);
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0.5*bx,0,0));
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);

	c = cols[(2+level)%4];
	v = Volume(_toString(3,"box%d"), small_box, mat);
	v.setRegion(vol.region());
	v.setSensitiveDetector(sensitive);
	v.setLimitSet(vol.limitSet());
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0,0.95*by,0));
	pv.addPhysVolID(_toString(level,"lvl%d"), 3);
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
	place_boxes(level-1, v);

	v = Volume(_toString(1,"axis_y"), Box(bx*0.03, by*0.25, bz*0.03), mat);
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0,0.5*by,0));
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);

	c = cols[(3+level)%4];
	v = Volume(_toString(4,"box%d"), small_box, mat);
	v.setRegion(vol.region());
	v.setSensitiveDetector(sensitive);
	v.setLimitSet(vol.limitSet());
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0,0,0.95*bz));
	pv.addPhysVolID(_toString(level,"lvl%d"), 4);
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
	place_boxes(level-1, v);

	v = Volume(_toString(1,"axis_z"), Box(bx*0.03, by*0.03, bz*0.25), mat);
	v.setVisAttributes(description, c);
	pv = vol.placeVolume(v, Position(0,0,0.5*bz));
	printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
      }
    }

    Ref_t create()    {
      xml_dim_t  x_box(x_det.dimensions()); 
      int        max_level = 3;
      double     bx = x_box.x();
      double     by = x_box.y();
      double     bz = x_box.z();
      //Volume     v_det(x_det.nameStr()+"_det",Box(5*bx,5*by,5*bz),description.air());  
      Assembly   v_det(x_det.nameStr()+"_det");
      Box        box(bx,by,bz);
      Volume     vol(x_det.nameStr()+"_box",box,description.air());  
      PlacedVolume pv;
      int cnt = 1;
      
      sensitive.setType("tracker");
      vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),"VisibleGrey");

      place_boxes(max_level-1, vol);
      pv = v_det.placeVolume(vol, Position(0,0,1.1*bz));
      pv.addPhysVolID(_toString(max_level,"lvl%d"), ++cnt);
      pv = nullptr;
      if ( x_det.hasChild(_U(reflect_x)) )   {
	Volume   reflect_vol = vol.reflect(sensitive);
	Position reflect_pos = Position(-1.1*bx,0,0);
	pv = v_det.placeVolume(reflect_vol, reflect_pos);
      }
      else if ( x_det.hasChild(_U(reflect_y)) )   {
	Volume   reflect_vol = vol.reflect(sensitive);
	Position reflect_pos = Position(0,-1.1*by,0);
	pv = v_det.placeVolume(reflect_vol, reflect_pos);
      }
      else if ( x_det.hasChild(_U(reflect_z)) )   {
	Volume   reflect_vol = vol.reflect(sensitive);
	Position reflect_pos = Position(0,0,-1.1*bz);
	pv = v_det.placeVolume(reflect_vol, reflect_pos);
      }
      if ( pv.ptr() )   {
	pv.addPhysVolID(_toString(max_level,"lvl%d"), ++cnt);
      }
      
      if ( x_det.hasChild(_U(reflect)) )   {
	Volume reflect_vol = vol;
	for(xml_coll_t c(x_det,_U(reflect)); c; ++c)   {
	  TGeoCombiTrans* reflect_tr  = transform_reflect(c);
	  pv = v_det.placeVolume(reflect_vol.ptr(), reflect_tr);
	  pv.addPhysVolID(_toString(max_level,"lvl%d"), ++cnt);
	}
      }
  
      // Place the calo inside the world
      pv = description.pickMotherVolume(detector).placeVolume(v_det);
      pv.addPhysVolID("system",x_det.id());
      detector.setPlacement(pv);
      return detector;
    }
  };
}

static Ref_t create_detector(Detector& description, xml_dim_t x_det, SensitiveDetector sens)  {
  NestedBoxReflection builder(description, x_det, sens);
  return builder.create();
}


DECLARE_DETELEMENT(NestedBoxReflection,create_detector)
