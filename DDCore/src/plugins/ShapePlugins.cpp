// $Id: StandardPlugins.cpp 1224 2014-06-19 19:26:59Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Standard plugins necessary for nearly everything.
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoShapeAssembly.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

#if 0
static Ref_t create_Assembly(lcdd_t&, xml_h e)   {
  xml_dim_t dim(e);
  return Ref_t(new TGeoShapeAssembly());
}
DECLARE_XMLELEMENT(Assembly__shape_constructor,create_Assembly)
#endif

static Ref_t create_Box(lcdd_t&, xml_h e)   {
  xml_dim_t dim(e);
  return Box(dim.dx(),dim.dy(),dim.dz());
}
DECLARE_XMLELEMENT(Box__shape_constructor,create_Box)

static Ref_t create_Polycone(lcdd_t&, xml_h element)   {
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
DECLARE_XMLELEMENT(Polycone__shape_constructor,create_Polycone)

static Ref_t create_ConeSegment(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return ConeSegment(e.rmin(0.0),e.rmax(),e.dz(0.0),e.startphi(0.0),e.deltaphi(2*M_PI));
}
DECLARE_XMLELEMENT(ConeSegment__shape_constructor,create_ConeSegment)

static Ref_t create_Tube(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return Tube(e.rmin(0.0),e.rmax(),e.dz(0.0),e.startphi(0.0),e.deltaphi(2*M_PI));
}
DECLARE_XMLELEMENT(Tube__shape_constructor,create_Tube)

static Ref_t create_EllipticalTube(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return EllipticalTube(e.a(),e.b(),e.dz());
}
DECLARE_XMLELEMENT(EllipticalTube__shape_constructor,create_EllipticalTube)

static Ref_t create_Cone(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  double rmi1 = e.rmin1(0.0), rma1 = e.rmax1();
  return Cone(e.z(0.0),rmi1,rma1,e.rmin2(rmi1),e.rmax2(rma1));
}
DECLARE_XMLELEMENT(Cone__shape_constructor,create_Cone)

static Ref_t create_Trap(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  if ( e.hasAttr(_U(dz)) )
    return Trap(e.dz(),e.dy(),e.dx(),_toDouble(_Unicode(pLTX)));
  return Trap(e.z(0.0),e.theta(),e.phi(0),e.y1(),e.x1(),e.x2(),e.alpha(),e.y2(),e.x3(),e.x4(),e.alpha2());
}
DECLARE_XMLELEMENT(Trap__shape_constructor,create_Trap)

static Ref_t create_Trapezoid(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return Trapezoid(e.x1(),e.x2(),e.y1(),e.y2(),e.z(0.0));
}
DECLARE_XMLELEMENT(Trapezoid__shape_constructor,create_Trapezoid)

static Ref_t create_Torus(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return Torus(e.r(),e.rmin(),e.rmax(),e.phi(M_PI),e.deltaphi(2.*M_PI));
}
DECLARE_XMLELEMENT(Torus__shape_constructor,create_Torus)

static Ref_t create_Sphere(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return Sphere(e.rmin(),e.rmax(),e.theta(0e0),e.deltatheta(M_PI),e.phi(0e0),e.deltaphi(2.*M_PI));
}
DECLARE_XMLELEMENT(Sphere__shape_constructor,create_Sphere)

static Ref_t create_Paraboloid(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return Paraboloid(e.rmin(0.0),e.rmax(),e.dz());
}
DECLARE_XMLELEMENT(Paraboloid__shape_constructor,create_Paraboloid)

static Ref_t create_Hyperboloid(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return Hyperboloid(e.rmin(), e.inner_stereo(), e.rmax(), e.outer_stereo(), e.dz());
}
DECLARE_XMLELEMENT(Hyperboloid__shape_constructor,create_Hyperboloid)

static Ref_t create_PolyhedraRegular(lcdd_t&, xml_h element)   {
  xml_dim_t e(element);
  return PolyhedraRegular(e.numsides(),e.rmin(),e.rmax(),e.dz());
}
DECLARE_XMLELEMENT(PolyhedraRegular__shape_constructor,create_PolyhedraRegular)

static Ref_t create_EightPointSolid(lcdd_t&, xml_h element)   {
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
DECLARE_XMLELEMENT(EightPointSolid__shape_constructor,create_EightPointSolid)

