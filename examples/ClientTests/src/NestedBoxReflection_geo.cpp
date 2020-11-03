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
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DetFactoryHelper.h"
#include "XML/VolumeBuilder.h"
#include "XML/Utilities.h"
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
        Box          box  = vol.solid();
        double       line = 0.015;
        double       bx   = box.x();
        double       by   = box.y();
        double       bz   = box.z();
        Material     mat  = description.material("Si");
	bool         sens = level == 2 || level == 0;
        Box          long_box(bx*0.2, by*0.2, bz*0.4);
        Box          flat_box(bx*1.0, by*1.0, bz*0.1);
        Box          mini_box(bx*0.2, by*0.2, bz*0.2);
        const char*  cols[5] = {"VisibleRed","VisibleBlue","VisibleGreen","VisibleYellow","VisibleCyan"};
        const char*  c;
        PlacedVolume pv;
        Volume       v;

        c = cols[(0+level)%5];
        v = Volume(_toString(1,"box%d"), mini_box, mat);
        v.setRegion(vol.region());
        v.setLimitSet(vol.limitSet());
        if ( !sens ) v.setSensitiveDetector(sensitive);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0,0,0));
        pv.addPhysVolID(_toString(level,"lvl%d"), 1);
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
        place_boxes(level-1, v);

        c = cols[(1+level)%5];
        v = Volume(_toString(2,"box%d"), long_box, mat);
        v.setRegion(vol.region());
        v.setLimitSet(vol.limitSet());
        if ( !sens ) v.setSensitiveDetector(sensitive);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0.8*bx,0,0));
        pv.addPhysVolID(_toString(level,"lvl%d"), 2);
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
        place_boxes(level-1, v);
    
        v = Volume(_toString(1,"axis_x"), Box(bx*0.2, by*line, bz*line), mat);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0.4*bx,0,0));
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);

        c = cols[(2+level)%5];
        v = Volume(_toString(3,"box%d"), mini_box, mat);
        v.setRegion(vol.region());
        v.setLimitSet(vol.limitSet());
        if ( !sens ) v.setSensitiveDetector(sensitive);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0,0.8*by,0));
        pv.addPhysVolID(_toString(level,"lvl%d"), 3);
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
        place_boxes(level-1, v);

        v = Volume(_toString(1,"axis_y"), Box(bx*line, by*0.2, bz*line), mat);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0,0.4*by,0));
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);

        c = cols[(3+level)%5];
        v = Volume(_toString(4,"box%d"), mini_box, mat);
        v.setRegion(vol.region());
        v.setLimitSet(vol.limitSet());
        if ( !sens ) v.setSensitiveDetector(sensitive);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0,0,0.8*bz));
        pv.addPhysVolID(_toString(level,"lvl%d"), 4);
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
        place_boxes(level-1, v);

        v = Volume(_toString(1,"axis_z"), Box(bx*line, by*line, bz*0.2), mat);
        v.setVisAttributes(description, c);
        pv = vol.placeVolume(v, Position(0,0,0.4*bz));
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);

        c = cols[(4+level)%5];
        v = Volume(_toString(5,"box%d"), flat_box, mat);
        v.setRegion(vol.region());
        v.setLimitSet(vol.limitSet());
        v.setVisAttributes(description, c);
        if ( !sens ) v.setSensitiveDetector(sensitive);
	TGeoHMatrix* mtx = detail::matrix::_transform(Position(0,0,0.9*bz));
	mtx->ReflectZ(kTRUE, kFALSE);
        pv = vol.placeVolume(v, mtx);
        pv.addPhysVolID(_toString(level,"lvl%d"), 5);
        printout(INFO,"NestedBoxReflection","++ Volume: %s  Color: %s", v.name(), c);
        place_boxes(level-1, v);
      }
    }
    
    PlacedVolume place(Volume mother, Volume vol, xml_elt_t e, int level, int copyNo, char reflection)   {
      Position    pos;
      RotationZYX rot;
      xml_dim_t   xpos = xml_dim_t(e).child(_U(position), false);
      xml_dim_t   xrot = xml_dim_t(e).child(_U(rotation), false);
      if ( !xpos.ptr() ) xpos = e;
      if ( xpos.ptr()  ) pos = Position(xpos.x(0),xpos.y(0),xpos.z(0));
      if ( xrot.ptr()  ) rot = RotationZYX(xrot.x(0),xrot.y(0),xrot.z(0));

      TGeoHMatrix* mat = detail::matrix::_transform(pos, rot);
      switch(reflection)  {
      case 'X':
	mat->ReflectX(kTRUE, kTRUE);
	break;
      case 'Y':
	mat->ReflectY(kTRUE, kTRUE);
	break;
      case 'Z':
      default:
	mat->ReflectZ(kTRUE, kTRUE);
	break;
      }
      PlacedVolume pv = mother.placeVolume(vol, mat);
      pv.addPhysVolID(_toString(level,"lvl%d"), copyNo);
      return pv;
    }

    Ref_t create()    {
      xml_dim_t    x_box(x_det.dimensions()); 
      int          levels = x_box.level(2);
      double       bx = x_box.x();
      double       by = x_box.y();
      double       bz = x_box.z();
      Volume       v_det;
      Box          box_solid(bx,by,bz);
      Volume       box_vol("nested_box",box_solid,description.air());  
      PlacedVolume pv;

      sensitive.setType("tracker");
      if ( levels != 0 && x_det.hasChild(_U(assembly)) )
        v_det = Assembly("envelope");
      else
        v_det = Volume("envelope",Box(4.5*bx,4.5*by,4.5*bz),description.air());

      if ( levels == 0 )   {
	v_det.setSensitiveDetector(sensitive);
      }
      else if ( levels == 1 )   {
	box_vol.setSensitiveDetector(sensitive);
	box_vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),"VisibleGrey");
      }
      else  {
	int cnt = 1;
	Transform3D tr = xml::createTransformation(x_box);
	box_vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),"VisibleGrey");
	place_boxes(levels-1, box_vol);
	pv = v_det.placeVolume(box_vol, tr);
	pv.addPhysVolID(_toString(levels,"lvl%d"), ++cnt);
	
	for(xml_coll_t c(x_det,_U(reflect_x)); c; ++c)
	  place(v_det, box_vol, c, levels, ++cnt, 'X');
	for(xml_coll_t c(x_det,_U(reflect_y)); c; ++c)
	  place(v_det, box_vol, c, levels, ++cnt, 'Y');
	for(xml_coll_t c(x_det,_U(reflect_z)); c; ++c)
	  place(v_det, box_vol, c, levels, ++cnt, 'Z');

	if ( x_det.hasChild(_U(reflect)) )   {
	  Volume reflect_vol = box_vol;
	  for(xml_coll_t c(x_det,_U(reflect)); c; ++c)   {
	    TGeoCombiTrans* reflect_tr = transform_reflect(c);
	    pv = v_det.placeVolume(reflect_vol.ptr(), reflect_tr);
	    pv.addPhysVolID(_toString(levels,"lvl%d"), ++cnt);
	  }
	}
      }
      // Place the calo inside the world
      placeDetector(v_det, x_det).addPhysVolID("system",x_det.id());
      return detector;
    }
  };
}

static Ref_t create_detector(Detector& description, xml_dim_t x_det, SensitiveDetector sens)  {
  NestedBoxReflection builder(description, x_det, sens);
  return builder.create();
}

DECLARE_DETELEMENT(NestedBoxReflection,create_detector)
