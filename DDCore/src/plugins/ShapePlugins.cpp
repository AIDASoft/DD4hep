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
#include "TGeoShapeAssembly.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Handle<TObject> create_Assembly(Detector&, xml_h e)   {
  xml_dim_t dim(e);
  return Handle<TObject>(new TGeoShapeAssembly());
}
DECLARE_XML_SHAPE(Assembly__shape_constructor,create_Assembly)

static Handle<TObject> create_Box(Detector&, xml_h e)   {
  xml_dim_t dim(e);
  return Box(dim.dx(),dim.dy(),dim.dz());
}
DECLARE_XML_SHAPE(Box__shape_constructor,create_Box)

static Handle<TObject> create_HalfSpace(Detector&, xml_h e)   {
  xml_dim_t dim(e);
  xml_dim_t point  = e.child(_U(point));
  xml_dim_t normal = e.child(_U(normal));
  double p[3] = { point.x(),  point.y(),  point.z()};
  double n[3] = { normal.x(), normal.y(), normal.z()};
  return HalfSpace(p, n);
}
DECLARE_XML_SHAPE(HalfSpace__shape_constructor,create_HalfSpace)

static Handle<TObject> create_Polycone(Detector&, xml_h element)   {
  xml_dim_t e(element);
  int num = 0;
  vector<double> rmin,rmax,z;
  double start = e.startphi(0e0), deltaphi = e.deltaphi(2*M_PI);
  for(xml_coll_t c(e,_U(zplane)); c; ++c, ++num)  {
    xml_comp_t plane(c);
    rmin.push_back(plane.rmin());
    rmax.push_back(plane.rmax());
    z.push_back(plane.z());
  }
  if ( num < 2 )  {
    throw runtime_error("PolyCone Shape> Not enough Z planes. minimum is 2!");
  }
  return Polycone(start,deltaphi,rmin,rmax,z);
}
DECLARE_XML_SHAPE(Polycone__shape_constructor,create_Polycone)

static Handle<TObject> create_ConeSegment(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return ConeSegment(e.rmin(0.0),e.rmax(),e.dz(0.0),e.startphi(0.0),e.deltaphi(2*M_PI));
}
DECLARE_XML_SHAPE(ConeSegment__shape_constructor,create_ConeSegment)

static Handle<TObject> create_Tube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return Tube(e.rmin(0.0),e.rmax(),e.dz(0.0),e.startphi(0.0),e.deltaphi(2*M_PI));
}
DECLARE_XML_SHAPE(Tube__shape_constructor,create_Tube)

static Handle<TObject> create_CutTube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return CutTube(e.rmin(0.0),e.rmax(),e.dz(),
                 e.attr<double>(_U(phi1)),
                 e.attr<double>(_U(phi2)),
                 e.attr<double>(_U(lx)),
                 e.attr<double>(_U(ly)),
                 e.attr<double>(_U(lz)),
                 e.attr<double>(_U(tx)),
                 e.attr<double>(_U(ty)),
                 e.attr<double>(_U(tz)));
}
DECLARE_XML_SHAPE(CutTube__shape_constructor,create_CutTube)

static Handle<TObject> create_EllipticalTube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return EllipticalTube(e.a(),e.b(),e.dz());
}
DECLARE_XML_SHAPE(EllipticalTube__shape_constructor,create_EllipticalTube)

static Handle<TObject> create_Cone(Detector&, xml_h element)   {
  xml_dim_t e(element);
  double rmi1 = e.rmin1(0.0), rma1 = e.rmax1();
  return Cone(e.z(0.0),rmi1,rma1,e.rmin2(rmi1),e.rmax2(rma1));
}
DECLARE_XML_SHAPE(Cone__shape_constructor,create_Cone)

static Handle<TObject> create_Trap(Detector&, xml_h element)   {
  xml_dim_t e(element);
  if ( e.hasAttr(_U(dz)) )
    return Trap(e.dz(),e.dy(),e.dx(),_toDouble(_Unicode(pLTX)));
  return Trap(e.z(0.0),e.theta(),e.phi(0),e.y1(),e.x1(),e.x2(),e.alpha(),e.y2(),e.x3(),e.x4(),e.alpha2());
}
DECLARE_XML_SHAPE(Trap__shape_constructor,create_Trap)

static Handle<TObject> create_PseudoTrap(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return PseudoTrap(e.x1(),e.x2(),e.y1(),e.y2(),e.z(),e.radius(),e.attr<double>(xml_tag_t("minusZ")));
}
DECLARE_XML_SHAPE(PseudoTrap__shape_constructor,create_PseudoTrap)

static Handle<TObject> create_Trapezoid(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return Trapezoid(e.x1(),e.x2(),e.y1(),e.y2(),e.z(0.0));
}
DECLARE_XML_SHAPE(Trapezoid__shape_constructor,create_Trapezoid)

static Handle<TObject> create_Torus(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return Torus(e.r(),e.rmin(),e.rmax(),e.phi(M_PI),e.deltaphi(2.*M_PI));
}
DECLARE_XML_SHAPE(Torus__shape_constructor,create_Torus)

static Handle<TObject> create_Sphere(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return Sphere(e.rmin(),e.rmax(),e.theta(0e0),e.deltatheta(M_PI),e.phi(0e0),e.deltaphi(2.*M_PI));
}
DECLARE_XML_SHAPE(Sphere__shape_constructor,create_Sphere)

static Handle<TObject> create_Paraboloid(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return Paraboloid(e.rmin(0.0),e.rmax(),e.dz());
}
DECLARE_XML_SHAPE(Paraboloid__shape_constructor,create_Paraboloid)

static Handle<TObject> create_Hyperboloid(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return Hyperboloid(e.rmin(), e.inner_stereo(), e.rmax(), e.outer_stereo(), e.dz());
}
DECLARE_XML_SHAPE(Hyperboloid__shape_constructor,create_Hyperboloid)

static Handle<TObject> create_PolyhedraRegular(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return PolyhedraRegular(e.numsides(),e.rmin(),e.rmax(),e.dz());
}
DECLARE_XML_SHAPE(PolyhedraRegular__shape_constructor,create_PolyhedraRegular)

static Handle<TObject> create_Polyhedra(Detector&, xml_h element)   {
  xml_dim_t e(element);
  std::vector<double> z, rmin, rmax;
  for ( xml_coll_t c(e,_U(plane)); c; ++c )  {
    xml_comp_t plane(c);
    rmin.push_back(plane.rmin());
    rmax.push_back(plane.rmax());
    z.push_back(plane.z());
  }
  return Polyhedra(e.numsides(),e.startphi(),e.deltaphi(),z,rmin,rmax);
}
DECLARE_XML_SHAPE(Polyhedra__shape_constructor,create_Polyhedra)

static Handle<TObject> create_ExtrudedPolygon(Detector&, xml_h element)   {
  xml_dim_t e(element);
  std::vector<double> pt_x, pt_y, sec_z, sec_x, sec_y, sec_scale;
  for ( xml_coll_t sec(element, _U(section)); sec; ++sec )   {
    xml_dim_t section(sec);
    sec_z.push_back(section.attr<double>(_U(z)));
    sec_x.push_back(section.attr<double>(_U(x)));
    sec_y.push_back(section.attr<double>(_U(y)));
    sec_scale.push_back(section.attr<double>(_U(scale),1.0));
  }
  for ( xml_coll_t pt(element, _U(point)); pt; ++pt )   {
    xml_dim_t point(pt);
    pt_x.push_back(point.attr<double>(_U(x)));
    pt_y.push_back(point.attr<double>(_U(y)));
  }
  return ExtrudedPolygon(pt_x, pt_y, sec_z, sec_y, sec_x, sec_scale);
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
  return EightPointSolid(e.dz(),&v[0][0]);
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


  std::string op = e.attr<std::string>( dd4hep::xml::Strng_t("operation") ) ;
  std::transform( op.begin(), op.end(), op.begin(), ::tolower);
  
  Solid resultSolid ;

  bool useRot(false), usePos(false) ; 
  Position    pos ;
  RotationZYX rot ;

  if( e.hasChild( _U(position) ) ) {
    usePos = true ;
    xml_comp_t x_pos = e.position();
    pos = Position( x_pos.x(),x_pos.y(),x_pos.z() );  
  }
  if( e.hasChild( _U(rotation) ) ) {
    useRot = true ;
    xml_comp_t  x_rot = e.rotation();
    rot = RotationZYX( x_rot.z(),x_rot.y(),x_rot.x() ) ;
  }

  if( op == "subtraction" ) {

    if( useRot && usePos )
      resultSolid = SubtractionSolid( solid1 , solid2 , Transform3D( rot, pos )  );
    else if( useRot) 
      resultSolid = SubtractionSolid( solid1 , solid2 , rot );
    else if( usePos) 
      resultSolid = SubtractionSolid( solid1 , solid2 , pos );
    else
      resultSolid = SubtractionSolid( solid1 , solid2 ) ;
  }
  else if( op == "union" ) {

    if( useRot && usePos )
      resultSolid = UnionSolid( solid1 , solid2 , Transform3D( rot, pos )  );
    else if( useRot) 
      resultSolid = UnionSolid( solid1 , solid2 , rot );
    else if( usePos) 
      resultSolid = UnionSolid( solid1 , solid2 , pos );
    else
      resultSolid = UnionSolid( solid1 , solid2 ) ;
  }
  else if( op == "intersection" ) {

    if( useRot && usePos )
      resultSolid = IntersectionSolid( solid1 , solid2 , Transform3D( rot, pos )  );
    else if( useRot) 
      resultSolid = IntersectionSolid( solid1 , solid2 , rot );
    else if( usePos) 
      resultSolid = IntersectionSolid( solid1 , solid2 , pos );
    else
      resultSolid = IntersectionSolid( solid1 , solid2 ) ;

  } else{

    throw std::runtime_error(std::string(" create_BooleanShape - unknown operation given: ") + op + 
                             std::string(" - needs to be one of 'subtraction','union' or 'intersection' ") ) ;  
  }
  return resultSolid ;
}
DECLARE_XML_SHAPE(BooleanShape__shape_constructor,create_BooleanShape)
