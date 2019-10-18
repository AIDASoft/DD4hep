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

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "XML/Utilities.h"
#include "TGeoShapeAssembly.h"
#include "TSystem.h"
#include "TClass.h"

// C/C++ include files
#include <fstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Handle<TObject> create_Assembly(Detector&, xml_h e)   {
  xml_dim_t dim(e);
  Solid solid = Handle<TNamed>(new TGeoShapeAssembly());
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Assembly__shape_constructor,create_Assembly)

static Handle<TObject> create_Box(Detector&, xml_h e)   {
  xml_dim_t dim(e);
  Solid solid = Box(dim.dx(),dim.dy(),dim.dz());
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Box__shape_constructor,create_Box)

static Handle<TObject> create_HalfSpace(Detector&, xml_h e)   {
  xml_dim_t dim(e);
  xml_dim_t point  = e.child(_U(point));
  xml_dim_t normal = e.child(_U(normal));
  double p[3] = { point.x(),  point.y(),  point.z()};
  double n[3] = { normal.x(), normal.y(), normal.z()};
  Solid solid = HalfSpace(p, n);
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(HalfSpace__shape_constructor,create_HalfSpace)

static Handle<TObject> create_Polycone(Detector&, xml_h element)   {
  xml_dim_t e(element);
  int num = 0;
  vector<double> rmin,rmax,z;
  double start = e.startphi(0e0), deltaphi = e.deltaphi(2*M_PI);
  for(xml_coll_t c(e,_U(zplane)); c; ++c, ++num)  {
    xml_comp_t plane(c);
    rmin.emplace_back(plane.rmin());
    rmax.emplace_back(plane.rmax());
    z.emplace_back(plane.z());
  }
  if ( num < 2 )  {
    throw runtime_error("PolyCone Shape> Not enough Z planes. minimum is 2!");
  }
  Solid solid = Polycone(start,deltaphi,rmin,rmax,z);
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Polycone__shape_constructor,create_Polycone)

static Handle<TObject> create_ConeSegment(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = ConeSegment(e.dz(),e.rmin1(0.0),e.rmax1(),e.rmin2(0.0),e.rmax2(),e.phi1(0.0),e.phi2(2*M_PI));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(ConeSegment__shape_constructor,create_ConeSegment)

static Handle<TObject> create_Tube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid;
  xml_attr_t aphi = element.attr_nothrow(_U(phi1));
  if ( aphi )  {
    double phi1 = e.phi1();
    double phi2 = e.phi2(2*M_PI);
    solid = Tube(e.rmin(0.0),e.rmax(),e.dz(0.0),phi1, phi2);
  }
  else  {
    double phi1 = e.startphi(0.0);
    double phi2 = phi1 + e.deltaphi(2*M_PI);
    solid = Tube(e.rmin(0.0),e.rmax(),e.dz(0.0),phi1,phi2);
  }
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Tube__shape_constructor,create_Tube)

static Handle<TObject> create_CutTube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = CutTube(e.rmin(0.0),e.rmax(),e.dz(),
                        e.attr<double>(_U(phi1)),
                        e.attr<double>(_U(phi2)),
                        e.attr<double>(_U(lx)),
                        e.attr<double>(_U(ly)),
                        e.attr<double>(_U(lz)),
                        e.attr<double>(_U(tx)),
                        e.attr<double>(_U(ty)),
                        e.attr<double>(_U(tz)));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(CutTube__shape_constructor,create_CutTube)

static Handle<TObject> create_EllipticalTube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = EllipticalTube(e.a(),e.b(),e.dz());
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(EllipticalTube__shape_constructor,create_EllipticalTube)

static Handle<TObject> create_TruncatedTube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  double sp = e.startphi(0.0), dp = e.deltaphi(2*M_PI);
  Solid solid = TruncatedTube(e.zhalf(), e.rmin(0.0), e.rmax(), sp, dp,
                              e.attr<double>(xml_tag_t("cutAtStart")),
                              e.attr<double>(xml_tag_t("cutAtDelta")),
                              e.attr<bool>(xml_tag_t("cutInside")));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(TruncatedTube__shape_constructor,create_TruncatedTube)

static Handle<TObject> create_Cone(Detector&, xml_h element)   {
  xml_dim_t e(element);
  double rmi1 = e.rmin1(0.0), rma1 = e.rmax1();
  Solid solid = Cone(e.z(0.0),rmi1,rma1,e.rmin2(rmi1),e.rmax2(rma1));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Cone__shape_constructor,create_Cone)

static Handle<TObject> create_Trap(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid;
  if ( e.hasAttr(_U(dz)) )   {
    solid = Trap(e.dz(),e.dy(),e.dx(),_toDouble(_Unicode(pLTX)));
  }
  else   {
    xml_attr_t attr = 0;
    double x1 = e.x1();
    double x2 = e.x2();
    double x3 = (attr=element.attr_nothrow(_U(x3))) ? element.attr<double>(attr) : x1;
    double x4 = (attr=element.attr_nothrow(_U(x4))) ? element.attr<double>(attr) : x2;
    double y1 = e.y1();
    double y2 = (attr=element.attr_nothrow(_U(y2))) ? element.attr<double>(attr) : y1;
    solid = Trap(e.z(0.0),e.theta(0),e.phi(0),y1,x1,x2,e.alpha1(0),y2,x3,x4,e.alpha2(0));
  }
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Trap__shape_constructor,create_Trap)

static Handle<TObject> create_PseudoTrap(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = PseudoTrap(e.x1(),e.x2(),e.y1(),e.y2(),e.z(),e.radius(),e.attr<bool>(xml_tag_t("minusZ")));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(PseudoTrap__shape_constructor,create_PseudoTrap)

static Handle<TObject> create_Trd1(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = Trd1(e.x1(),e.x2(),e.y(),e.z(0.0));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Trd1__shape_constructor,create_Trd1)

static Handle<TObject> create_Trd2(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = Trd2(e.x1(),e.x2(),e.y1(),e.y2(),e.z(0.0));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Trapezoid__shape_constructor,create_Trd2)
DECLARE_XML_SHAPE(Trd2__shape_constructor,create_Trd2)

static Handle<TObject> create_Torus(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = Torus(e.r(),e.rmin(),e.rmax(),e.phi(M_PI),e.deltaphi(2.*M_PI));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Torus__shape_constructor,create_Torus)

static Handle<TObject> create_Sphere(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = Sphere(e.rmin(),e.rmax(),e.theta(0e0),e.deltatheta(M_PI),e.phi(0e0),e.deltaphi(2.*M_PI));
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Sphere__shape_constructor,create_Sphere)

static Handle<TObject> create_Paraboloid(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = Paraboloid(e.rmin(0.0),e.rmax(),e.dz());
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Paraboloid__shape_constructor,create_Paraboloid)

static Handle<TObject> create_Hyperboloid(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = Hyperboloid(e.rmin(), e.inner_stereo(), e.rmax(), e.outer_stereo(), e.dz());
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Hyperboloid__shape_constructor,create_Hyperboloid)

static Handle<TObject> create_PolyhedraRegular(Detector&, xml_h element)   {
  xml_dim_t e(element);
  Solid solid = PolyhedraRegular(e.numsides(),e.rmin(),e.rmax(),e.dz());
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(PolyhedraRegular__shape_constructor,create_PolyhedraRegular)

static Handle<TObject> create_Polyhedra(Detector&, xml_h element)   {
  xml_dim_t e(element);
  std::vector<double> z, rmin, rmax;
  for ( xml_coll_t c(e,_U(plane)); c; ++c )  {
    xml_comp_t plane(c);
    rmin.emplace_back(plane.rmin());
    rmax.emplace_back(plane.rmax());
    z.emplace_back(plane.z());
  }
  Solid solid = Polyhedra(e.numsides(),e.startphi(),e.deltaphi(),z,rmin,rmax);
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(Polyhedra__shape_constructor,create_Polyhedra)

static Handle<TObject> create_ExtrudedPolygon(Detector&, xml_h element)   {
  xml_dim_t e(element);
  std::vector<double> pt_x, pt_y, sec_z, sec_x, sec_y, sec_scale;
  for ( xml_coll_t sec(element, _U(section)); sec; ++sec )   {
    xml_dim_t section(sec);
    sec_z.emplace_back(section.attr<double>(_U(z)));
    sec_x.emplace_back(section.attr<double>(_U(x)));
    sec_y.emplace_back(section.attr<double>(_U(y)));
    sec_scale.emplace_back(section.attr<double>(_U(scale),1.0));
  }
  for ( xml_coll_t pt(element, _U(point)); pt; ++pt )   {
    xml_dim_t point(pt);
    pt_x.emplace_back(point.attr<double>(_U(x)));
    pt_y.emplace_back(point.attr<double>(_U(y)));
  }
  Solid solid = ExtrudedPolygon(pt_x, pt_y, sec_z, sec_x, sec_y, sec_scale);
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(ExtrudedPolygon__shape_constructor,create_ExtrudedPolygon)

static Handle<TObject> create_EightPointSolid(Detector&, xml_h element)   {
  xml_dim_t e(element);
  double v[8][2];
  int num = 0;
  memset(&v[0][0],0,sizeof(v));
  for(xml_coll_t c(e,_Unicode(vertex)); c && num<8; ++c, ++num)  {
    xml_comp_t vtx(c);
    v[num][0] = vtx.x();
    v[num][1] = vtx.y();
  }
  Solid solid = EightPointSolid(e.dz(),&v[0][0]);
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(EightPointSolid__shape_constructor,create_EightPointSolid)


/** Plugin function for creating a boolean solid from an xml element <shape type=\"BooleanShape\"/>. 
 *  Expects exactly two child elements <shape/> and a string attribute 'operation', which is one of
 *  'subtraction', 'union' or 'intersection'. Optionally <position/> and/or <rotation/> can be specified.
 *  More complex boolean solids can be created by nesting the xml elements accordingly.
 *
 * @date 03/2015
 * @author F.Gaede, CERN/DESY
 */
static Handle<TObject> create_BooleanShape(Detector&, xml_h element)   {

  xml_det_t e(element);

  // get the two shape elements
  xml_coll_t c( e ,_U(shape)) ;
  xml_comp_t x_shape1( c ) ;
  ++c ;
  xml_comp_t x_shape2( c ) ;
  
  // and create solids
  Solid solid1( xml_comp_t( x_shape1 ).createShape()) ;
  Solid solid2( xml_comp_t( x_shape2 ).createShape())  ;


  std::string op = e.attr<std::string>(_U(operation)) ;
  std::transform( op.begin(), op.end(), op.begin(), ::tolower);
  
  Solid resultSolid ;

  bool useRot(false), usePos(false), useTrans(false); 
  Position    pos ;
  RotationZYX rot ;

  if( e.hasChild( _U(transformation) ) ) {
    useTrans = true ;
  }
  if( e.hasChild( _U(position) ) ) {
    usePos = true ;
    xml_comp_t x_pos = e.position();
    pos = Position( x_pos.x(0.0),x_pos.y(0.0),x_pos.z(0.0) );  
  }
  if( e.hasChild( _U(rotation) ) ) {
    useRot = true ;
    xml_comp_t  x_rot = e.rotation();
    rot = RotationZYX( x_rot.z(0.0),x_rot.y(0.0),x_rot.x(0.0) ) ;
  }

  if( op == "subtraction" ) {
    if ( useTrans )   {
      Transform3D tr = xml::createTransformation(e.child(_U(transformation)));
      resultSolid = SubtractionSolid(solid1, solid2, tr);
    }
    else if( useRot && usePos )   {
      resultSolid = SubtractionSolid(solid1, solid2, Transform3D(rot, pos));
    }
    else if( useRot ) 
      resultSolid = SubtractionSolid(solid1, solid2, rot);
    else if( usePos) 
      resultSolid = SubtractionSolid(solid1, solid2, pos);
    else
      resultSolid = SubtractionSolid(solid1, solid2);
  }
  else if( op == "union" ) {
    if ( useTrans )  {
      Transform3D tr = xml::createTransformation(e.child(_U(transformation)));
      resultSolid = UnionSolid(solid1, solid2, tr);
    }
    else if( useRot && usePos )
      resultSolid = UnionSolid(solid1, solid2, Transform3D(rot, pos));
    else if( useRot) 
      resultSolid = UnionSolid(solid1, solid2, rot);
    else if( usePos)
      resultSolid = UnionSolid(solid1, solid2, pos);
    else
      resultSolid = UnionSolid(solid1, solid2);
  }
  else if( op == "intersection" ) {
    if ( useTrans )  {
      Transform3D tr = xml::createTransformation(e.child(_U(transformation)));
      resultSolid = IntersectionSolid(solid1, solid2, tr);
    }
    else if( useRot && usePos )
      resultSolid = IntersectionSolid(solid1, solid2, Transform3D(rot, pos));
    else if( useRot) 
      resultSolid = IntersectionSolid(solid1, solid2, rot);
    else if( usePos) 
      resultSolid = IntersectionSolid(solid1, solid2, pos);
    else
      resultSolid = IntersectionSolid(solid1, solid2) ;

  } else  {

    throw std::runtime_error(std::string(" create_BooleanShape - unknown operation given: ") + op + 
                             std::string(" - needs to be one of 'subtraction','union' or 'intersection' ") ) ;  
  }
  Solid solid = resultSolid ;
  if ( e.hasAttr(_U(name)) ) solid->SetName(e.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(BooleanShapeOld__shape_constructor,create_BooleanShape)


static Handle<TObject> create_BooleanMulti(Detector& description, xml_h element)   {
  xml_det_t e(element);
  // get the two shape elements
  Solid tmp, solid, result;
  int flag = 0;
  Transform3D position, rotation, trafo;

  xml_attr_t attr = 0;
  std::string op = e.attr<std::string>(_U(operation)) ;
  std::transform( op.begin(), op.end(), op.begin(), ::tolower);
  //printout(ALWAYS,"","Boolean shape ---> %s",op.c_str());
  for (xml_coll_t i(e ,_U(star)); i; ++i )   {
    xml_comp_t x_elt = i;
    string tag = x_elt.tag();
    if ( tag == "shape" && !result.isValid() )  {
      result = xml::createShape(description, x_elt.typeStr(), x_elt);
      if ( (attr=i.attr_nothrow(_U(name))) ) result->SetName(i.attr<string>(attr).c_str());
      flag = 1;
    }
    else if ( tag == "shape" && !solid.isValid() )  {
      solid = xml::createShape(description,  x_elt.typeStr(), x_elt); 
      if ( (attr=i.attr_nothrow(_U(name))) ) result->SetName(i.attr<string>(attr).c_str());
      flag = 3;
    }
    else if ( result.isValid() && solid.isValid() )  {
      if ( tag == "position" )  {
        if      ( flag == 4 ) trafo = position  * trafo;
        else if ( flag == 5 ) trafo = (position * rotation) * trafo;
        Position pos(x_elt.x(0), x_elt.y(0), x_elt.z(0));
        position = Transform3D(pos);
        rotation = Transform3D();
        flag = 4;
      }
      else if ( tag == "positionRZPhi" )  {
        if      ( flag == 4 ) trafo = position  * trafo;
        else if ( flag == 5 ) trafo = (position * rotation) * trafo;
        ROOT::Math::RhoZPhiVector pos(x_elt.r(0), x_elt.z(0), x_elt.phi(0));
        position = Transform3D(pos);
        rotation = Transform3D();
        flag = 4;
      }
      else if ( tag == "transformation" )   {
        if      ( flag == 4 ) trafo = position  * trafo;
        else if ( flag == 5 ) trafo = (position * rotation) * trafo;
        Transform3D tr = xml::createTransformation(x_elt);
        trafo   = tr * trafo;
        position = rotation = Transform3D();
        flag = 3;
      }
      else if ( tag == "rotation" )  {
        rotation = Transform3D(RotationZYX(x_elt.z(0), x_elt.y(0), x_elt.x(0)));
        flag = 5;
      }
      else if ( tag == "shape" )   {
        //cout << solid.name() << " Flag:" << flag << endl;
        if      ( flag == 4 ) trafo = position  * trafo;
        else if ( flag == 5 ) trafo = (position * rotation) * trafo;
        tmp = Solid();
        if( op == "subtraction" )
          tmp = SubtractionSolid(result, solid, trafo);
        else if( op == "union" )
          tmp = UnionSolid(result, solid, trafo);
        else if( op == "intersection" )
          tmp = IntersectionSolid(result, solid, trafo);
        else  {          // Error!
          throw std::runtime_error(" create_BooleanShape - unknown operation given: " + op + 
                                   " - needs to be one of 'subtraction','union' or 'intersection' ");  
        }
        result = tmp;
        trafo  = position = rotation = Transform3D();
        solid = xml::createShape(description,  x_elt.typeStr(), x_elt); 
        if ( (attr=i.attr_nothrow(_U(name))) ) result->SetName(i.attr<string>(attr).c_str());
        flag = 3;
      }
    }
  }
  if ( flag >= 3 )  {
    //cout << solid.name() << " Flag:" << flag << endl;
    if      ( flag == 4 ) trafo = position  * trafo;
    else if ( flag == 5 ) trafo = (position * rotation) * trafo;
    if( op == "subtraction" )
      tmp = SubtractionSolid(result, solid, trafo);
    else if( op == "union" )
      tmp = UnionSolid(result, solid, trafo);
    else if( op == "intersection" )
      tmp = IntersectionSolid(result, solid, trafo);
    else  {          // Error!
      throw std::runtime_error(" create_BooleanShape - unknown operation given: " + op + 
                               " - needs to be one of 'subtraction','union' or 'intersection' ");  
    }
    result = tmp;
  }
  attr = element.attr_nothrow(_U(name));
  if ( attr )   {
    string nam = element.attr<string>(attr);
    result->SetName(nam.c_str());
  }
  return result;
}
DECLARE_XML_SHAPE(BooleanShape__shape_constructor,create_BooleanMulti)

TGeoCombiTrans* createPlacement(const Rotation3D& iRot, const Position& iTrans) {
  double elements[9];
  iRot.GetComponents(elements);
  TGeoRotation r;
  r.SetMatrix(elements);
  TGeoTranslation t(iTrans.x(), iTrans.y(), iTrans.z());
  return new TGeoCombiTrans(t, r);
}

static Ref_t create_shape(Detector& description, xml_h e, Ref_t /* sens */)  {
  xml_det_t    x_det     = e;
  string       name      = x_det.nameStr();
  xml_dim_t    x_reflect = x_det.child(_U(reflect), false);
  xml_comp_t   x_test    = x_det.child(xml_tag_t("test"), false);
  DetElement   det         (name,x_det.id());
  Assembly     assembly    (name);
  PlacedVolume pv;
  int count = 0;
  for ( xml_coll_t itm(e, _U(check)); itm; ++itm, ++count )   {
    xml_dim_t  x_check = itm;
    xml_comp_t shape    (x_check.child(_U(shape)));
    xml_dim_t  pos      (x_check.child(_U(position), false));
    xml_dim_t  rot      (x_check.child(_U(rotation), false));
    bool       reflect = x_check.hasChild(_U(reflect));
    Solid      solid    (shape.createShape());
    Volume     volume   (name+_toString(count,"_vol_%d"),solid, description.air());

    volume.setVisAttributes(description, x_check.visStr());
    solid->SetName(shape.typeStr().c_str());

    if ( pos.ptr() && rot.ptr() )  {
      Rotation3D  rot3D(RotationZYX(rot.z(0),rot.y(0),rot.x(0)));
      Position    pos3D(pos.x(0),pos.y(0),pos.z(0));
      if ( reflect )
        rot3D = Rotation3D(1., 0., 0., 0., 1., 0., 0., 0., -1.) * rot3D;
      Transform3D tr(rot3D, pos3D);
      pv = assembly.placeVolume(volume,tr);
    }
    else if ( pos.ptr() )  {
      pv = assembly.placeVolume(volume,Position(pos.x(0),pos.y(0),pos.z(0)));
    }
    else if ( rot.ptr() )  {
      Rotation3D rot3D(RotationZYX(rot.z(0),rot.y(0),rot.x(0)));
      if ( reflect )
        rot3D = Rotation3D(1., 0., 0., 0., 1., 0., 0., 0., -1.) * rot3D;
      pv = assembly.placeVolume(volume,rot3D);
    }
    else {
      pv = assembly.placeVolume(volume);
    }

    if ( x_check.hasAttr(_U(id)) )  {
      pv.addPhysVolID("check",x_check.id());
    }

    printout(INFO,"TestShape","Created successfull shape of type: %s",
             shape.typeStr().c_str());
    bool instance_test = false;
    if ( 0 == strcasecmp(solid->GetTitle(),"box") )
      instance_test = isInstance<Box>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Tube") )
      instance_test = isInstance<Tube>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"CutTube") )
      instance_test = isInstance<CutTube>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Cone") )
      instance_test = isInstance<Cone>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Trap") )
      instance_test = isInstance<Trap>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Trd1") )
      instance_test = isInstance<Trd1>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Trd2") )
      instance_test = isInstance<Trd2>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Torus") )
      instance_test = isInstance<Torus>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Sphere") )
      instance_test = isInstance<Sphere>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"HalfSpace") )
      instance_test = isInstance<HalfSpace>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"ConeSegment") )
      instance_test = isInstance<ConeSegment>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Paraboloid") )
      instance_test = isInstance<Paraboloid>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Hyperboloid") )
      instance_test = isInstance<Hyperboloid>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"PolyhedraRegular") )
      instance_test = isInstance<PolyhedraRegular>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Polyhedra") )
      instance_test = isInstance<Polyhedra>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"EllipticalTube") )
      instance_test = isInstance<EllipticalTube>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"ExtrudedPolygon") )
      instance_test = isInstance<ExtrudedPolygon>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"Polycone") )
      instance_test = isInstance<Polycone>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"EightPointSolid") )
      instance_test = isInstance<EightPointSolid>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"SubtractionSolid") )
      instance_test = isInstance<SubtractionSolid>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"UnionSolid") )
      instance_test = isInstance<UnionSolid>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"IntersectionSolid") )
      instance_test = isInstance<IntersectionSolid>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"TruncatedTube") )
      instance_test = isInstance<TruncatedTube>(solid);
    else if ( 0 == strcasecmp(solid->GetTitle(),"PseudoTrap") )
      instance_test = isInstance<PseudoTrap>(solid);

    if ( !instance_test || ::strcasecmp(shape.typeStr().c_str(),solid->GetTitle()) != 0 )   {
      printout(ERROR,"TestShape","BAD shape type: %s <-> %s Instance test: %s",
               shape.typeStr().c_str(), solid->GetTitle(),
               instance_test ? "OK" : "FAILED");
    }
    else   {
      printout(INFO,"TestShape","Correct shape type: %s <-> %s Instance test: %s",
               shape.typeStr().c_str(), solid->GetTitle(), "OK");
    }
  }
  if ( x_reflect )   {
    xml_dim_t   x_pos(x_reflect.child(_U(position), false));
    xml_dim_t   x_rot(x_reflect.child(_U(rotation), false));
    DetElement  full_detector(name+"_full",x_det.id());
    Assembly    full_assembly(name+"_full");
    RotationZYX refl_rot;
    Position    refl_pos;

    if ( x_rot ) refl_rot = RotationZYX(x_rot.z(0),x_rot.y(0),x_rot.x(0));
    if ( x_pos ) refl_pos = Position(x_pos.x(0),x_pos.y(0),x_pos.z(0));
    Transform3D refl_trafo(Rotation3D(refl_rot),refl_pos);

    /// Place the regular detector
    pv = full_assembly.placeVolume(assembly);
    full_detector.add(det);
    det.setPlacement(pv);

    /// Place reflected object
    auto reflected = det.reflect(name+"_reflected",x_det.id());
    pv = full_assembly.placeVolume(reflected.second, refl_trafo);
    full_detector.add(reflected.first);
    reflected.first.setPlacement(pv);

    /// Place mother
    pv = description.worldVolume().placeVolume(full_assembly);
    full_detector.setPlacement(pv);
    
    det = full_detector;
  }
  else  {
    pv = description.worldVolume().placeVolume(assembly);
    det.setPlacement(pv);
  }

  if ( x_test.ptr() )  {
    string typ = x_test.typeStr();
    const void* argv[] = { &e, &pv, 0};
    Ref_t result = (NamedObject*)PluginService::Create<void*>(typ, &description, 2, (char**)argv);
    if ( !result.isValid() )  {
      printout(INFO,"TestShape","+++ Shape verification FAILED. [Plugin not found]");
      except("TestShape","+++ Shape verification FAILED.");
    }
    else if ( ::strcmp(result->GetName(),"SUCCESS") == 0 )  {
      printout(INFO,"TestShape","+++ Shape verification SUCCESSFUL. [type=%s]",name.c_str());
      delete result.ptr();
    }
    else   {
      printout(INFO,"TestShape","+++ Shape verification FAILED [result=%s]",result->GetName());
      printout(INFO,"TestShape","+++ Diagnosis: \n%s",result->GetTitle());
      delete result.ptr();
      except("TestShape","+++ Shape verification FAILED.");
    }
  }
  return det;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(DD4hep_TestShape_Creator,create_shape)

void* shape_mesh_verifier(Detector& description, int argc, char** argv)    {
  if ( argc != 2 )   {  }
  xml_det_t    x_det  = *(xml_h*)argv[0];
  PlacedVolume pv     = *(PlacedVolume*)argv[1];
  xml_comp_t   x_test = x_det.child(xml_tag_t("test"));
  int          ref_cr = x_test.hasAttr(_U(create)) ? x_test.attr<int>(_U(create)) : 0;
  int          nseg   = x_test.hasAttr(_U(segmentation)) ? x_test.attr<int>(_U(segmentation)) : -1;
  TString      ref    = x_test.refStr().c_str();
  string       ref_str;  
  stringstream os;

  if ( nseg > 0 )   {
    description.manager().SetNsegments(nseg);
  }
  Volume v = pv.volume();
  for (Int_t ipv=0, npv=v->GetNdaughters(); ipv < npv; ipv++) {
    PlacedVolume place = v->GetNode(ipv);
    os << "ShapeCheck[" << ipv << "] ";
    os << toStringMesh(place, 2);
  }
  gSystem->ExpandPathName(ref);
  if ( ref_cr )   {
    ofstream out(ref, ofstream::out);
    if ( !out.is_open() )   {
      except("Mesh_Verifier","+++ FAILED to open(WRITE) reference file: "+x_test.refStr());
    }
    out << os.str();
    out.close();
    printout(INFO,"Mesh_Verifier","+++ Successfully wrote reference file: "+x_test.refStr());
  }
  else if ( ref.Length() > 0 )  {
    char c;
    ifstream in(ref.Data(), ofstream::in);
    if ( !in.is_open() )   {
      except("Mesh_Verifier","+++ FAILED to access reference file: "+x_test.refStr());
    }
    while (in.get(c))          // loop getting single characters
      ref_str += c;
    in.close();
    printout(INFO,"Mesh_Verifier","+++ Successfully read reference file: "+x_test.refStr());
    if ( ref_str != os.str() )  {
      printout(ERROR,"Mesh_Verifier","+++ Output and reference differ! Please check.");
      return Constant("FAILURE",os.str().c_str()).ptr();
    }
    printout(INFO,"Mesh_Verifier","+++ Successfully checked CREATED shapes.");
    os.str("");
    for (Int_t ipv=0, npv=v->GetNdaughters(); ipv < npv; ipv++) {
      PlacedVolume place = v->GetNode(ipv);
      Solid solid = place.volume().solid();
      if ( isInstance<TruncatedTube>(solid) )   {
        auto params = solid.dimensions();
        solid.setDimensions(params);
      }
      else if ( isInstance<PseudoTrap>(solid) )   {
        auto params = solid.dimensions();
        solid.setDimensions(params);
      }
      else if ( solid->IsA() != TGeoCompositeShape::Class() )   {
        auto params = solid.dimensions();
        solid.setDimensions(params);
      }
      else   {
        printout(INFO,"Mesh_Verifier","+++ Skip re-dimensioning of %s [%s].",
                 solid->IsA()->GetName(), solid->GetTitle());
      }
    }
    for (Int_t ipv=0, npv=v->GetNdaughters(); ipv < npv; ipv++) {
      PlacedVolume place = v->GetNode(ipv);
      os << "ShapeCheck[" << ipv << "] ";
      os << toStringMesh(place, 2);
    }
    if ( ref_str != os.str() )  {
      printout(ERROR,"Mesh_Verifier","+++ Output and reference differ after re-dimension! Please check.");
      return Constant("FAILURE",os.str().c_str()).ptr();
    }
    printout(INFO,"Mesh_Verifier","+++ Successfully checked REDIMENSIONED shapes.");
  }
  return Constant("SUCCESS",os.str().c_str()).ptr();
}

DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_Mesh_Verifier,shape_mesh_verifier)
