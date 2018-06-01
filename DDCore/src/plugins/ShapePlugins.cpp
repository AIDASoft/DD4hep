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
#include "TGeoShapeAssembly.h"
#include "TSystem.h"
#include "TClass.h"

// C/C++ include files
#include <iomanip>
#include <fstream>

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
  return ConeSegment(e.dz(),e.rmin1(0.0),e.rmax1(),e.rmin2(0.0),e.rmax2(),e.phi1(0.0),e.phi2(2*M_PI));
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

static Handle<TObject> create_TruncatedTube(Detector&, xml_h element)   {
  xml_dim_t e(element);
  double sp = e.startphi(0.0), dp = e.deltaphi(2*M_PI);
  return TruncatedTube(e.zhalf(), e.rmin(0.0), e.rmax(), sp, dp,
                       e.attr<double>(xml_tag_t("cutAtStart")),
                       e.attr<double>(xml_tag_t("cutAtDelta")),
                       e.attr<bool>(xml_tag_t("cutInside")));
}
DECLARE_XML_SHAPE(TruncatedTube__shape_constructor,create_TruncatedTube)

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
  return Trap(e.z(0.0),e.theta(),e.phi(0),e.y1(),e.x1(),e.x2(),e.alpha1(),e.y2(),e.x3(),e.x4(),e.alpha2());
}
DECLARE_XML_SHAPE(Trap__shape_constructor,create_Trap)

static Handle<TObject> create_PseudoTrap(Detector&, xml_h element)   {
  xml_dim_t e(element);
  return PseudoTrap(e.x1(),e.x2(),e.y1(),e.y2(),e.z(),e.radius(),e.attr<bool>(xml_tag_t("minusZ")));
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
  return ExtrudedPolygon(pt_x, pt_y, sec_z, sec_x, sec_y, sec_scale);
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

static Ref_t create_shape(Detector& description, xml_h e, Ref_t /* sens */)  {
  xml_det_t    x_det  = e;
  string       name   = x_det.nameStr();
  xml_comp_t   x_test = x_det.child(xml_tag_t("test"), false);
  DetElement   det     (name,x_det.id());
  Assembly     assembly(name);
  PlacedVolume pv;
  int count = 0;
  for ( xml_coll_t itm(e, _U(check)); itm; ++itm, ++count )   {
    xml_dim_t   x_check = itm;
    xml_comp_t  shape  (x_check.child(_U(shape)));
    xml_dim_t   pos    (x_check.child(_U(position), false));
    xml_dim_t   rot    (x_check.child(_U(rotation), false));
    Solid       solid  (shape.createShape());
    Volume      volume (name+_toString(count,"_vol_%d"),solid, description.air());

    if ( pos.ptr() && rot.ptr() )  {
      Transform3D trafo(Rotation3D(RotationZYX(rot.z(),rot.y(),rot.x())),
                        Position(pos.x(),pos.y(),pos.z()));
      pv = assembly.placeVolume(volume,trafo);
    }
    else if ( pos.ptr() )  {
      pv = assembly.placeVolume(volume,Position(pos.x(),pos.y(),pos.z()));
    }
    else if ( rot.ptr() )  {
      pv = assembly.placeVolume(volume,Rotation3D(RotationZYX(rot.z(),rot.y(),rot.x())));
    }
    else {
      pv = assembly.placeVolume(volume);
    }
    volume.setVisAttributes(description, x_check.visStr());
    if ( x_check.hasAttr(_U(id)) )  {
      pv.addPhysVolID("check",x_check.id());
    }
    solid->SetName(shape.typeStr().c_str());
    printout(INFO,"TestShape","Created successfull shape of type: %s",
             shape.typeStr().c_str());
  }
  pv = description.worldVolume().placeVolume(assembly);
  det.setPlacement(pv);
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

void* shape_mesh_verifier(Detector& /* description */, int argc, char** argv)    {
  if ( argc != 2 )   {  }
  xml_det_t    x_det  = *(xml_h*)argv[0];
  PlacedVolume pv     = *(PlacedVolume*)argv[1];
  xml_comp_t   x_test = x_det.child(xml_tag_t("test"));
  int          ref_cr = x_test.hasAttr(_U(create)) ? x_test.attr<int>(_U(create)) : 0;
  TString      ref    = x_test.refStr().c_str();
  string       ref_str;  
  stringstream os;

  Volume      v = pv.volume();
  for (Int_t ipv=0, npv=v->GetNdaughters(); ipv < npv; ipv++) {
    PlacedVolume place = v->GetNode(ipv);
    Volume       vol   = place->GetVolume();
    TGeoMatrix*  mat   = place->GetMatrix();
    Solid        sol   = vol.solid();
    // Prints shape parameters
    Int_t nvert = 0, nsegs = 0, npols = 0;
    sol->GetMeshNumbers(nvert, nsegs, npols);
    Double_t* points = new Double_t[3*nvert];
    sol->SetPoints(points);
    Box box = sol;
    const Double_t* org = box->GetOrigin();

    os << "ShapeCheck[" << ipv << "] "
       << setw(16) << left << sol->IsA()->GetName()
       << " " << nvert << " Mesh-points:" << endl;
    os << setw(16) << left << sol->IsA()->GetName() << " " << sol->GetName()
       << " N(mesh)=" << sol->GetNmeshVertices()
       << "  N(vert)=" << nvert << "  N(seg)=" << nsegs << "  N(pols)=" << npols << endl;
    
    for(Int_t i=0; i<nvert; ++i)   {
      Double_t* p = points + 3*i;
      Double_t global[3], local[3] = {p[0], p[1], p[2]};
      mat->LocalToMaster(local, global);
      os << setw(16) << left << sol->IsA()->GetName() << " " << setw(3) << left << i
         << " Local  ("  << setw(7) << setprecision(2) << fixed << right << local[0]
         << ", "         << setw(7) << setprecision(2) << fixed << right << local[1]
         << ", "         << setw(7) << setprecision(2) << fixed << right << local[2]
         << ") Global (" << setw(7) << setprecision(2) << fixed << right << global[0]
         << ", "         << setw(7) << setprecision(2) << fixed << right << global[1]
         << ", "         << setw(7) << setprecision(2) << fixed << right << global[2]
         << ")" << endl;
    }
    os << setw(16) << left << sol->IsA()->GetName()
       << " Bounding box: "
       << " dx="        << setw(7) << setprecision(2) << fixed << right << box->GetDX()
       << " dy="        << setw(7) << setprecision(2) << fixed << right << box->GetDY()
       << " dz="        << setw(7) << setprecision(2) << fixed << right << box->GetDZ()
       << " Origin: x=" << setw(7) << setprecision(2) << fixed << right << org[0]
       << " y="         << setw(7) << setprecision(2) << fixed << right << org[1]
       << " z="         << setw(7) << setprecision(2) << fixed << right << org[2]
       << endl;
  
    /// -------------------- DONE --------------------
    delete [] points;
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
  }
  return Constant("SUCCESS",os.str().c_str()).ptr();
}

DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_Mesh_Verifier,shape_mesh_verifier)
