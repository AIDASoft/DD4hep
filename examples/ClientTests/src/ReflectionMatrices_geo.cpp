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
#include <iostream>
#include <cmath>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace   {
  Rotation3D makeRotReflect(double thetaX, double phiX, double thetaY, double phiY, double thetaZ, double phiZ) {
    // define 3 unit std::vectors forming the new left-handed axes
    Position x(cos(phiX) * sin(thetaX), sin(phiX) * sin(thetaX), cos(thetaX));
    Position y(cos(phiY) * sin(thetaY), sin(phiY) * sin(thetaY), cos(thetaY));
    Position z(cos(phiZ) * sin(thetaZ), sin(phiZ) * sin(thetaZ), cos(thetaZ));
    printout(INFO,"ReflectionMatrices",
           "+++ Adding rotation: (theta/phi)[rad] X: %6.3f %6.3f Y: %6.3f %6.3f Z: %6.3f %6.3f",
           thetaX,phiX,thetaY,phiY,thetaZ,phiZ);
    Rotation3D rotation(x.x(), y.x(), z.x(), x.y(), y.y(), z.y(), x.z(), y.z(), z.z());
    return rotation;
  }
  Transform3D transform_3d(xml_h element)   {
    xml_dim_t xrot(element.child(_U(rotation)));
    xml_dim_t xpos(element.child(_U(position)));
    double thetaX = xrot.attr<double>(Unicode("thetaX"));
    double phiX   = xrot.attr<double>(Unicode("phiX"));
    double thetaY = xrot.attr<double>(Unicode("thetaY"));
    double phiY   = xrot.attr<double>(Unicode("phiY"));
    double thetaZ = xrot.attr<double>(Unicode("thetaZ"));
    double phiZ   = xrot.attr<double>(Unicode("phiZ"));
    printout(INFO, "ReflectionMatrices",
	     "+++ Adding reflection rotation \"%s\": "
	     "(theta/phi)[rad] X: %6.3f %6.3f Y: %6.3f %6.3f Z: %6.3f %6.3f",
	     element.attr<string>(_U(name)).c_str(), thetaX, phiX, thetaY, phiY, thetaZ, phiZ);
    Rotation3D rot = makeRotReflect(thetaX, phiX, thetaY, phiY, thetaZ, phiZ);
    Position   pos = Position(xpos.x(),xpos.y(),xpos.z());
    return Transform3D(rot, pos);
  }

  struct ReflectionMatrices : public xml::tools::VolumeBuilder  {
    int num_left = 0, num_right = 0;
    
    using VolumeBuilder::VolumeBuilder;
    ///
    void dump(const string& nam, const Transform3D& tr)   {
      stringstream str;
      Position x, y, z, p;
      Rotation3D r;
      tr.GetDecomposition(r, p);
      r.GetComponents(x,y,z);
      double det1 = (x.Cross(y)).Dot(z);
      double det2 = detail::matrix::determinant(r);
      double det3 = detail::matrix::determinant(tr);
      str << "+++ Using rotation: " << nam << "   "
	  << (const char*)((det1 >= 0) ? "RIGHT" : "LEFT") << "-handed  "
	  << " Determinant: " << det1 << " " << det2 << " " << det3 << endl
	  << "Pos: " << p << " Rotation:" << x << " " << y << " " << z
	  << r;
      PrintLevel lvl = det1 != det2 || det1 != det3 ? ERROR : INFO;
      if ( det1 < 0e0 ) ++num_left;
      if ( det1 > 0e0 ) ++num_right;
      if ( det1 < 0e0 && nam.find("LEFT-handed") == string::npos )   {
	lvl = ERROR;
      }
      printout(lvl, "ReflectionMatrices",str.str().c_str());
    }
    /// 
    void dump(xml_elt_t elt, char reflection)   {
      Position    pos;
      RotationZYX rot;
      xml_dim_t   e(elt);
      xml_dim_t   xpos = e.child(_U(position), false);
      xml_dim_t   xrot = e.child(_U(rotation), false);
      if ( !xpos.ptr() ) xpos = e;
      if ( xpos.ptr()  ) pos = Position(xpos.x(0),xpos.y(0),xpos.z(0));
      if ( xrot.ptr()  ) rot = RotationZYX(xrot.x(0),xrot.y(0),xrot.z(0));

      TGeoHMatrix mat;
      detail::matrix::_transform(mat, pos, rot);
      switch(reflection)  {
      case 'X':
	mat.ReflectX(kTRUE, kTRUE);
	break;
      case 'Y':
	mat.ReflectY(kTRUE, kTRUE);
	break;
      case 'Z':
      default:
	mat.ReflectZ(kTRUE, kTRUE);
	break;
      }
      dump(e.nameStr(), detail::matrix::_transform(mat));
    }
    DetElement create()    {
      xml_dim_t x_box(x_det.dimensions()); 
      double    bx = x_box.x();
      double    by = x_box.y();
      double    bz = x_box.z();

      for(xml_coll_t c(x_det,_U(reflect_x)); c; ++c)
	dump(c, 'X');
      for(xml_coll_t c(x_det,_U(reflect_y)); c; ++c)
	dump(c, 'Y');
      for(xml_coll_t c(x_det,_U(reflect_z)); c; ++c)
	dump(c, 'Z');
      for(xml_coll_t c(x_det,_U(reflect)); c; ++c)
	dump(xml_dim_t(c).nameStr(), transform_3d(c));

      auto pv = placeDetector(Volume("envelope",Box(bx, by, bz),description.air()), x_det);
      pv.addPhysVolID("system",x_det.id());
      return detector;
    }
  };
  ///
  Ref_t create_test(Detector& description, xml_dim_t x_det, SensitiveDetector sens)  {
    ReflectionMatrices builder(description, x_det, sens);
    DetElement det = builder.create();
    printout(ALWAYS,"ReflectionMatrices",
	     "+++ Analysed %d right handed and %d left handed matrices.",
	     builder.num_right, builder.num_left);
    return det;
  }
}

DECLARE_DETELEMENT(DD4hep_Test_ReflectionMatrices,create_test)
