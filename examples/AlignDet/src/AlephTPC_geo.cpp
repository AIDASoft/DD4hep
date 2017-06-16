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
#include "TGeoArb8.h"
#include <iomanip>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static void printCoordinates(char /* sector_type */, const char* /* volume */, double [8][2])  {
}
#if 0
static void printCoordinates(char sector_type, const char* volume, double v[8][2])  {
  cout << "Sector type " << sector_type << " Volume " << volume << endl;
  cout << "Coordinate 1:  x:" << setw(10) << v[0][0] << " y:" << setw(10) << v[0][1] << endl;
  cout << "Coordinate 2:  x:" << setw(10) << v[1][0] << " y:" << setw(10) << v[1][1] << endl;
  cout << "Coordinate 3:  x:" << setw(10) << v[2][0] << " y:" << setw(10) << v[2][1] << endl;
  cout << "Coordinate 4:  x:" << setw(10) << v[3][0] << " y:" << setw(10) << v[3][1] << endl;
}
#endif

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector sens_det)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  DetElement  sdet(name,x_det.id());
  Volume      motherVol   = description.pickMotherVolume(sdet);
  xml_comp_t  x_envelope  = x_det.child(_Unicode(envelope));
  xml_comp_t  x_sectors   = x_det.child(_Unicode(sectors));
  xml_comp_t  x_inner     = x_det.child(_Unicode(inner_wall));
  xml_comp_t  x_outer     = x_det.child(_Unicode(outer_wall));
  xml_comp_t  x_gas       = x_det.child(_Unicode(gas));
  xml_comp_t  x_cathode   = x_det.child(_Unicode(cathode));
  
  PlacedVolume pv;

  struct cylinder_t { double inner, outer, zhalf; };
  cylinder_t env        = { x_envelope.inner_r(), x_envelope.outer_r(), x_envelope.zhalf() };
  cylinder_t inner_wall = { x_inner.inner_r(), x_inner.inner_r()+x_inner.thickness(), env.zhalf };
  cylinder_t outer_wall = { x_outer.outer_r()-x_outer.thickness(), x_outer.outer_r(), env.zhalf };
  cylinder_t gas        = { inner_wall.outer, outer_wall.inner, x_gas.zhalf() };

  // TPC sensitive detector
  sens_det.setType("tracker");

  // the TPC mother volume
  //Tube    envTub(env.inner,env.outer,env.zhalf);
  //Volume  envVol(name+"_envelope",envTub,description.air());
  Assembly  envVol(name+"_envelope");
  //envVol.setVisAttributes(description.visAttributes(x_envelope.visStr()));
  //envVol.setVisAttributes(description.invisible());
  //envVol->SetVisibility(kFALSE);
  //envVol->SetVisDaughters(kTRUE);

  pv = motherVol.placeVolume(envVol);
  pv.addPhysVolID("system",x_det.id());
  sdet.setPlacement(pv);

  // TPC Al inner shield 
  Tube    innerTub(inner_wall.inner, inner_wall.outer, inner_wall.zhalf);
  Volume  innerVol(name+"_inner",innerTub,description.material(x_inner.materialStr()));
  innerVol.setVisAttributes(description.visAttributes(x_inner.visStr()));
  envVol.placeVolume(innerVol);

  // TPC outer shield 
  Tube    outerTub(outer_wall.inner, outer_wall.outer, outer_wall.zhalf);
  Volume  outerVol(name+"_outer",outerTub,description.material(x_outer.materialStr()));
  outerVol.setVisAttributes(description.visAttributes(x_outer.visStr()));
  envVol.placeVolume(outerVol);

#if 0
  // TPC gas chamber envelope
  Material gasMat = description.material(x_gas.materialStr());
  Tube     gasTub(gas.inner,gas.outer,gas.zhalf);
  Volume   gasVol(name+"_chamber",gasTub,gasMat);
  gasVol.setVisAttributes(description.visAttributes(x_gas.visStr()));
  //gasVol.setVisAttributes(description.invisible());
  envVol.placeVolume(gasVol);
#endif
  
  // TPC HV plane
  Tube    hvTub(gas.inner,gas.outer,x_cathode.thickness()/2);
  Volume  hvVol(name+"_cathode",hvTub,description.material(x_cathode.materialStr()));
  hvVol.setVisAttributes(description.visAttributes(x_cathode.visStr()));
  envVol.placeVolume(hvVol);

  // TPC Endcap plane to see sectors and misalignments betters
  Tube    endCapPlane(env.inner,env.outer,0.0001);
  Volume  endCapPlaneVol(name+"_plane",endCapPlane,description.material("Copper"));
  endCapPlaneVol.setVisAttributes(description.visAttributes("TPCEndcapVis"));

  double endcap_thickness = 0;
  for(xml_coll_t c(x_sectors,_Unicode(sector)); c; ++c)  {
    //xml_comp_t x_sector = c;
    double thickness = endCapPlane->GetDz();
    for(xml_coll_t l(x_sectors.child(_Unicode(layers)),_Unicode(layer)); l; ++l)  {
      xml_comp_t x_layer = l;
      thickness += 2*x_layer.thickness();
    }
    if ( thickness > endcap_thickness ) endcap_thickness = thickness;
  }

  Tube    endCapTub(env.inner,env.outer,endcap_thickness/2);
  Volume  endCapAVol(name+"_end_A",endCapTub,description.material("Copper"));
  endCapAVol.setVisAttributes(description.invisible());
  //endCapAVol.setVisAttributes(description.visAttributes(x_outer.visStr()));
  endCapAVol.placeVolume(endCapPlaneVol,Position(0,0,-endcap_thickness/2));
  pv = envVol.placeVolume(endCapAVol,Position(0,0,env.zhalf+endcap_thickness/2));
  pv.addPhysVolID("side",1);
  DetElement detA(sdet,name+"_SideA",1);
  detA.setPlacement(pv);

  Volume  endCapBVol(name+"_end_B",endCapTub,description.material("Copper"));
  endCapBVol.setVisAttributes(description.invisible());
  //endCapBVol.setVisAttributes(description.visAttributes(x_outer.visStr()));
  endCapBVol.placeVolume(endCapPlaneVol,Position(0,0,-endcap_thickness/2));

  Transform3D trEndB(RotationY(M_PI),Position(0,0,-env.zhalf-endcap_thickness/2));
  pv = envVol.placeVolume(endCapBVol, trEndB);
  pv.addPhysVolID("side",2);
  DetElement detB(sdet,name+"_SideB",2);
  detB.setPlacement(pv);

  //envVol.setVisAttributes(description.invisible());

  int sector_count = 0;
  for(xml_coll_t c(x_sectors,_Unicode(sector)); c; ++c)  {
    xml_comp_t x_sector = c;
    const char sector_type = x_sector.typeStr()[0];
    const double rmin0 = x_sector.rmin();
    const double rmax0 = x_sector.rmax();
    //const int  padrows = x_sector.attr<int>(_Unicode(padrows));
    //const int  trgrows = x_sector.attr<int>(_Unicode(trgrows));
    //const int  nwires  = x_sector.attr<int>(_Unicode(numwires));
    const int  num_sectors = sector_type == 'K' ? 6 : 12;
    const double shift     = sector_type == 'K' ? 0 : M_PI/num_sectors;
    const double dphi      = 2*M_PI/double(num_sectors);
    string sector_vis      = x_sector.visStr();
    Solid tm;
    double z_start = 0.0;
    Assembly sector(name+"_sector_"+sector_type);
    int i_layer = 0;
    for(xml_coll_t l(x_sectors.child(_Unicode(layers)),_Unicode(layer)); l; ++l, ++i_layer)  {
      xml_comp_t x_layer = l;
      double layer_thickness = x_layer.thickness();
      string layer_vis = x_layer.visStr();
      string layer_mat = x_layer.materialStr();
      double gap_half = 1;
      double rmin = rmin0;
      double rmax = rmax0;

      if ( layer_vis.empty() ) layer_vis = sector_vis;

      if ( sector_type == 'K' )  {
	double angle = M_PI/12.0;
	double angle1 = std::tan(angle);
	double v[8][2];
	v[0][0] = rmin;
	v[0][1] = 0;
	v[1][0] = rmin;
	v[1][1] = rmin*angle1;
	v[2][0] = rmax;
	v[2][1] = rmax*angle1;
	v[3][0] = rmax;
	v[3][1] = 0;
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));
	EightPointSolid upper(layer_thickness/2,&v[0][0]);

	v[0][0] = rmin;
	v[0][1] = 0;
	v[1][0] = rmax;
	v[1][1] = 0;
	v[2][0] = rmax;
	v[2][1] = -rmax*angle1;
	v[3][0] = rmin;
	v[3][1] = -rmin*angle1;
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));
	EightPointSolid lower(layer_thickness/2,&v[0][0]);

	v[0][0] = rmin;
	v[0][1] = gap_half;
	v[1][0] = rmin;
	v[1][1] = rmin*angle1;
	v[2][0] = rmax;
	v[2][1] = rmax*angle1;
	v[3][0] = rmax;
	v[3][1] = gap_half;
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));
	EightPointSolid top(layer_thickness/2,&v[0][0]);

	v[0][0] = rmin;
	v[0][1] = -gap_half;
	v[1][0] = rmax;
	v[1][1] = -gap_half;
	v[2][0] = rmax;
	v[2][1] = -rmax*angle1;
	v[3][0] = rmin;
	v[3][1] = -rmin*angle1;
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));
	EightPointSolid bottom(layer_thickness/2,&v[0][0]);

	UnionSolid u(UnionSolid(upper,lower),top,Rotation3D(RotationZ(2*(angle))));
	tm = UnionSolid(u,bottom,Rotation3D(RotationZ(-2*(angle))));
      }
      else   {
	double overlap = sector_type=='W' ? 20 : -20;
	double angle  = M_PI/12.0;
	double angle1 = std::tan(angle);
	double v[8][2], dr = 0;

	if ( sector_type == 'W' )  {
	  rmax += overlap*std::tan(angle/2);
	  dr    = overlap*std::tan(angle/2);
	}

	v[0][0] = rmin;
	v[0][1] = -rmin*angle1+gap_half;
	v[1][0] = rmin;
	v[1][1] = rmin*angle1-gap_half;
	v[2][0] = rmax;
	v[2][1] = rmax*angle1-gap_half;
	v[3][0] = rmax;
	v[3][1] = -rmax*angle1+gap_half;
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));
	printCoordinates(sector_type,"t2:",v);
	EightPointSolid sectorSolid(layer_thickness/2,&v[0][0]);

	if ( sector_type=='W' )  {
	  v[0][0] = (rmax+rmin)/2-dr;
	  v[0][1] = (rmax+rmin)/2*angle1-gap_half;
	  v[1][0] = rmax+0.0001;
	  v[1][1] = rmax*angle1-gap_half;
	  v[2][0] = rmax+0.0001;
	  v[2][1] = (rmax-overlap)*angle1-gap_half;
	  v[3][0] = (rmax+rmin)/2-dr;
	  v[3][1] = (rmax+rmin-overlap)/2*angle1-gap_half;
	}
	else  {
	  v[0][0] = (rmax+rmin)/2-dr;
	  v[0][1] = (rmax+rmin)/2*angle1-gap_half;
	  v[3][0] = rmax+0.0001;
	  v[3][1] = rmax*angle1-gap_half;
	  v[2][0] = rmax+0.0001;
	  v[2][1] = (rmax-overlap)*angle1-gap_half;
	  v[1][0] = (rmax+rmin)/2-dr;
	  v[1][1] = (rmax+rmin-overlap)/2*angle1-gap_half;
	}
	printCoordinates(sector_type,"upper_boolean:",v);
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));
	EightPointSolid upper_boolean((sector_type == 'W' ? 1.1 : 1.0) * layer_thickness/2,&v[0][0]);

	if ( sector_type=='W' )  {
	  v[0][0] = (rmax+rmin)/2-dr;
	  v[0][1] = -((rmax+rmin)/2*angle1-gap_half);
	  v[1][0] = (rmax+rmin)/2-dr;
	  v[1][1] = -((rmax+rmin-overlap)/2*angle1-gap_half);
	  v[2][0] = rmax+0.0001;
	  v[2][1] = -((rmax-overlap)*angle1-gap_half);
	  v[3][0] = rmax+0.0001;
	  v[3][1] = -(rmax*angle1-gap_half);
	}
	else  {
	  v[0][0] = (rmax+rmin)/2-dr;
	  v[0][1] = -((rmax+rmin)/2*angle1-gap_half);
	  v[3][0] = (rmax+rmin)/2-dr;
	  v[3][1] = -((rmax+rmin-overlap)/2*angle1-gap_half);
	  v[2][0] = rmax+0.0001;
	  v[2][1] = -((rmax-overlap)*angle1-gap_half);
	  v[1][0] = rmax+0.0001;
	  v[1][1] = -(rmax*angle1-gap_half);
	}
	printCoordinates(sector_type,"lower_boolean:",v);
	::memcpy(&v[4][0],&v[0][0],8*sizeof(double));

	// For W sectors make the subtraction solid slightly thicker to ensure everything is cut off
	// and no left-overs from numerical precision are left.
	EightPointSolid lower_boolean((sector_type == 'W' ? 1.1 : 1.0) * layer_thickness/2,&v[0][0]);
	if ( sector_type == 'W' )
	  tm = SubtractionSolid(SubtractionSolid(sectorSolid,upper_boolean),lower_boolean);
	else
	  tm = UnionSolid(UnionSolid(sectorSolid,upper_boolean),lower_boolean);
      }

      Volume secVol(name+"_sector_"+sector_type+_toString(i_layer,"_layer%d"),tm,description.material(layer_mat));
      secVol.setVisAttributes(description.visAttributes(layer_vis));
      if ( x_layer.isSensitive() ) secVol.setSensitiveDetector(sens_det);

      sector.placeVolume(secVol,Position(0,0,z_start+layer_thickness/2));
      z_start += layer_thickness;
    }
    for(int i=0; i<num_sectors;++i) {
      int j = i + (sector_type=='W' ? 1 : 0);
      double phi = dphi*j+shift + (sector_type=='K' ? 0 : M_PI/12.0);
      if ( sector_type == 'K' || (i%2)==0 ) {
	Transform3D trA(RotationZYX(phi,0,0),Position(0,0,0.00001));
	Transform3D trB(RotationZYX(phi,0,0),Position(0,0,0.00001));

	pv = endCapAVol.placeVolume(sector,trA);
	pv.addPhysVolID("type", sector_type=='K' ? 1 : sector_type=='M' ? 2 : 3);
	pv.addPhysVolID("sector",j);
	DetElement sectorA(detA,detA.name()+_toString(sector_count,"_sector%02d"),1);
	sectorA.setPlacement(pv);

	pv = endCapBVol.placeVolume(sector,trB);
	pv.addPhysVolID("type", sector_type=='K' ? 1 : sector_type=='M' ? 2 : 3);
	pv.addPhysVolID("sector",j);
	DetElement sectorB(detB,detB.name()+_toString(sector_count,"_sector%02d"),1);
	sectorB.setPlacement(pv);
	cout << "Placed " << sector_type << " sector at phi=" << phi << endl;
	++sector_count;
      }
    }
  }
  return sdet;
}

DECLARE_SUBDETECTOR(AlephTPC,create_element)
