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

namespace   {
  const char* col(int idx)  {
    const char*  cols[5] = {"VisibleRed","VisibleBlue","VisibleGreen","VisibleYellow","VisibleCyan"};
    return cols[idx%(sizeof(cols)/sizeof(cols[0]))];
  }
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
}

namespace   {
  struct TubeReflection : public xml::tools::VolumeBuilder  {
    Material   silicon;
    Material   iron;
    ///
    using VolumeBuilder::VolumeBuilder;
    ///
    void place_quadrants(int /* level */, Volume vol)    {
      Tube         tube = vol.solid();
      double       rmin = tube.rMin(), rmax = tube.rMax(), dz = tube.dZ();
      Tube         tsol1(rmin, rmax, dz*0.05, 0, M_PI*2.);
      Tube         tsol2(rmin, rmax, dz*0.15, 0, M_PI/2.);
      Volume       tvol0("Tube0", tsol1, silicon);
      Volume       tvol1("Tube1", tsol2, silicon);
      Volume       tvol2("Tube2", tsol2, silicon);
      Volume       tvol3("Tube3", tsol2, silicon);
      Volume       tvol4("Tube4", tsol2, silicon);
      TGeoHMatrix* mat;
      Transform3D  tr;
      
      double dz1 = dz-2.0*tsol2.dZ()-tsol1.dZ();
      tvol0.setVisAttributes(description.visAttributes("VisibleCyan"));
      tvol1.setVisAttributes(description.visAttributes("VisibleRed"));
      tvol2.setVisAttributes(description.visAttributes("VisibleBlue"));
      tvol3.setVisAttributes(description.visAttributes("VisibleYellow"));
      tvol4.setVisAttributes(description.visAttributes("VisibleGreen"));

      mat = detail::matrix::_transform(Transform3D(Position(0, 0, dz1)));
      vol.placeVolume(tvol0, mat);

      mat = detail::matrix::_transform(Transform3D(Position(0, 0,-dz1)));
      mat->ReflectZ(kTRUE, kTRUE);
      vol.placeVolume(tvol0, mat);

#if 0
#endif
      double dz2 = dz-tsol2.dZ();
      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0, dz2)));
      vol.placeVolume(tvol1, mat);
      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0, dz2)));
      mat->RotateZ(1.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol2, mat);
      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0, dz2)));
      mat->RotateZ(2.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol3, mat);
      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0, dz2)));
      mat->RotateZ(3.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol4, mat);


      /** Now eflect the quadrants to the other endcap:  */
      
      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0,-dz2)));
      mat->ReflectY(kTRUE,kTRUE);
      //mat->RotateZ(1.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol1, mat);

      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0,-dz2)));
      mat->RotateZ(1.0*M_PI/2.0/dd4hep::degree);
      mat->ReflectY(kTRUE,kTRUE);
      //mat->RotateZ(-1.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol2, mat);

      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0,-dz2)));
      mat->RotateZ(2.0*M_PI/2.0/dd4hep::degree);
      mat->ReflectY(kTRUE,kTRUE);
      //mat->RotateZ(1.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol3, mat);

      // OK
      mat = detail::matrix::_transform(Transform3D(Position(0, 0,-dz2)));
      mat->RotateZ(3.0*M_PI/2.0/dd4hep::degree);
      mat->ReflectY(kTRUE,kTRUE);
      //mat->RotateZ(-1.0*M_PI/2.0/dd4hep::degree);
      vol.placeVolume(tvol4, mat);
    }
    /// 
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
    ///
    DetElement create()    {
      xml_dim_t    x_tube(x_det.dimensions()); 
      double       r      = x_tube.r();
      double       dz     = x_tube.dz();
      int          levels = x_tube.level(2);
      Volume       tube_vol;
      Volume       v_det;
      PlacedVolume pv;

      sensitive.setType("tracker");
      silicon = description.material("Si");
      iron    = description.material("Iron");
      tube_vol = Volume("tube", Tube(0,r,dz,0,M_PI*2.0), silicon);  
      if ( levels != 0 && x_det.hasChild(_U(assembly)) )
	v_det = Assembly("envelope");
      else
	v_det = Volume("envelope",Tube(4.5*r,4.5*r,4.5*r),description.air());

      int cnt = 0;
      Transform3D tr = xml::createTransformation(x_tube);
      tube_vol.setAttributes(description, x_det.regionStr(), x_det.limitsStr(), "VisibleGrey");
      place_quadrants(levels-1, tube_vol);
      pv = v_det.placeVolume(tube_vol, tr);
      pv.addPhysVolID(_toString(levels,"lvl%d"), ++cnt);

      for(xml_coll_t c(x_det,_U(reflect_x)); c; ++c)
	place(v_det, tube_vol, c, levels, ++cnt, 'X');
      for(xml_coll_t c(x_det,_U(reflect_y)); c; ++c)
	place(v_det, tube_vol, c, levels, ++cnt, 'Y');
      for(xml_coll_t c(x_det,_U(reflect_z)); c; ++c)
	place(v_det, tube_vol, c, levels, ++cnt, 'Z');

      if ( x_det.hasChild(_U(reflect)) )   {
	Volume reflect_vol = tube_vol;
	for(xml_coll_t c(x_det,_U(reflect)); c; ++c)   {
	  TGeoCombiTrans* reflect_tr = transform_reflect(c);
	  pv = v_det.placeVolume(reflect_vol.ptr(), reflect_tr);
	  pv.addPhysVolID(_toString(levels,"lvl%d"), ++cnt);
	}
      }
      // Place the calo inside the world
      placeDetector(v_det, x_det).addPhysVolID("system",x_det.id());
      return detector;
    }
  };

  static Ref_t create_refl_tube(Detector& description, xml_dim_t x_det, SensitiveDetector sens)  {
    TubeReflection builder(description, x_det, sens);
    return builder.create();
  }
}
DECLARE_DETELEMENT(DD4hep_Test_TubeReflection,create_refl_tube)


// makes sure that the RotationMatrix built is
// LEFT-handed coordinate system (i.e. reflected)
namespace   {
  struct NestedBoxReflection : public xml::tools::VolumeBuilder  {
    using VolumeBuilder::VolumeBuilder;
    Material   silicon;
    Material   iron;
    
    void place_box(int level, int ident, const char* vis, Volume par, Solid sol, TGeoHMatrix* mtx);
    void place_box(int level, int ident, const char* vis, Volume par, Solid sol, const Transform3D& tr)   {
      TGeoHMatrix* mtx = detail::matrix::_transform(tr);
      place_box(level, ident, vis, par, sol, mtx);
    }
    void place_box(int level, int ident, const char* vis, Volume par, Solid sol, Position pos)    {
      place_box(level, ident, vis, par, sol, Transform3D(pos));
    }
    void place_boxes(int level, Volume vol);
    PlacedVolume place(Volume mother, Volume vol, xml_elt_t e, int level, int copyNo, char reflection);
    DetElement   create();
  };
  /// 
  void NestedBoxReflection::place_boxes(int level, Volume vol)    {
    if ( level >= 0 )   {
      Box          box  = vol.solid();
      double       line = 0.015, bx = box.x(), by = box.y(), bz = box.z();
      Box          mbox(bx*0.2, by*0.2, bz*0.2);

      printout(INFO,"NestedBoxReflection","+++ Placing boxes at level %d",level);
      place_box(level, 1, col(0+level), vol, mbox,                           Position(0,0,0));
      place_box(level, 2, col(1+level), vol, Box(bx*0.2,  by*0.2,  bz*0.4),  Position(0.8*bx,0,0));
      place_box(level, 3, col(2+level), vol, mbox,                           Position(0,0.8*by,0));
      place_box(level, 4, col(3+level), vol, mbox,                           Position(0,0,0.8*bz));
      auto mtx = make_unique<TGeoHMatrix>(TGeoTranslation(0,0,-0.9*bz));
      mtx->ReflectZ(kTRUE,kTRUE);
      place_box(level, 5, col(4+level), vol, Box(bx*1.0,  by*1.0,  bz*0.1),  mtx.release());
      place_box(level, 0, col(1+level), vol, Box(bx*0.2,  by*line, bz*line), Position(0.4*bx,0,0));
      place_box(level, 0, col(2+level), vol, Box(bx*line, by*0.2,  bz*line), Position(0,0.4*by,0));
      place_box(level, 0, col(3+level), vol, Box(bx*line, by*line, bz*0.2),  Position(0,0,0.4*bz));
    }
  }
  
  /// 
  void NestedBoxReflection::place_box(int level, int ident, const char* vis, Volume par, Solid sol, TGeoHMatrix* tr)   {
    PlacedVolume pv;
    bool         sens  = level == 2 || level == 0;
    string       idnam = _toString(ident,"box%d");
    Volume       vol   = Volume(idnam, sol, sens ? silicon : iron);
    vol.setRegion(par.region());
    vol.setLimitSet(par.limitSet());
    vol.setVisAttributes(description, vis);
    pv = par.placeVolume(vol, tr);
    if ( ident > 0 )   {
      string vidn  = _toString(level,"lvl%d");
      if ( sens )   {
	vol.setSensitiveDetector(sensitive);
      }
      pv.addPhysVolID(vidn, ident);
      string n = par.name()+string("/")+vol.name();
      printout(INFO,"NestedBoxReflection","++ Level: %3d Volume:%-24s Sensitive:%s Color:%s vid:%s=%d",
	       level, n.c_str(), yes_no(sens), vis, vidn.c_str(), ident);
      place_boxes(level-1, vol);
    }
  }

  /// 
  PlacedVolume NestedBoxReflection::place(Volume mother, Volume vol, xml_elt_t e, int level, int copyNo, char reflection)   {
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
  
  /// 
  DetElement NestedBoxReflection::create()    {
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
    iron    = description.material("Iron");
    silicon = description.material("Si");
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
      int cnt = 0;
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
}

static Ref_t create_nested_box(Detector& description, xml_dim_t x_det, SensitiveDetector sens)  {
  NestedBoxReflection builder(description, x_det, sens);
  return builder.create();
}

DECLARE_DETELEMENT(DD4hep_Test_NestedBoxReflection,create_nested_box)
