// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
// This subdetector is rather simple: Just a set of disk
// perpendicular to the beam.
// - No special user limits required for simulation.
// - 
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"
#include <limits>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens_det) {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  Rotation    reflect_rot(0,M_PI,0);
  DetElement  sdet(name,x_det.id());
  Assembly    assembly(name);

  xml_comp_t  x_disks = x_det.child(_U(disks));
  xml_comp_t  x_par   = x_det.child(_U(params));
  VisAttr  supportVis  = lcdd.visAttributes("FtdSupportVis");
  VisAttr  cablesVis   = lcdd.visAttributes("FtdCablesVis");
  VisAttr  cylinderVis = lcdd.visAttributes("FtdCylinderVis");
  VisAttr  diskVis     = lcdd.visAttributes("FtdDiskVis");
  Material copperMat   = lcdd.material("Copper");
  Material kaptonMat   = lcdd.material("Kapton");
  Material siliconMat  = lcdd.material("silicon_2.33gccm");
  
  // Now get the Globals from the surrounding environment TPC ECAL SIT VTX and Beam-Pipe  
  double TPC_Ecal_Hcal_barrel_halfZ   = lcdd.constant<double>("TPC_Ecal_Hcal_barrel_halfZ");
  double Ecal_endcap_zmin             = lcdd.constant<double>("Ecal_endcap_zmin");
  double TPC_inner_radius             = lcdd.constant<double>("TPC_inner_radius");

  double SIT1_Half_Length_Z           = lcdd.constant<double>("SIT1_Half_Length_Z");
  double SIT2_Half_Length_Z           = lcdd.constant<double>("SIT2_Half_Length_Z");
  double SIT1_Radius                  = lcdd.constant<double>("SIT1_Radius");
  double SIT2_Radius                  = lcdd.constant<double>("SIT2_Radius");
  double VXD_layer3_maxZ              = lcdd.constant<double>("VXD_length_r3");
  double TUBE_IPOuterTube_end_z       = lcdd.constant<double>("TUBE_IPOuterTube_end_z");
  double TUBE_IPOuterTube_end_radius  = lcdd.constant<double>("TUBE_IPOuterTube_end_radius");
  double TUBE_IPOuterBulge_end_z      = lcdd.constant<double>("TUBE_IPOuterBulge_end_z");
  double TUBE_IPOuterBulge_end_radius = lcdd.constant<double>("TUBE_IPOuterBulge_end_radius");  
  double beamTangent = (TUBE_IPOuterBulge_end_radius-TUBE_IPOuterTube_end_radius) / 
    (TUBE_IPOuterBulge_end_z-TUBE_IPOuterTube_end_z);

  // Now get the variables global to the FTD cables_thickness, ftd1_vtx3_distance_z, etc
  double beamTubeClearance            = x_par.attr<double>(_Unicode(beamtube_clearance));
  double outer_cyl_thickness          = x_par.attr<double>(_Unicode(outer_cylinder_total_thickness));
  double inner_cyl_thickness          = outer_cyl_thickness;
  double cable_shield_thickness       = x_par.attr<double>(_Unicode(cable_shield_thickness));
  double cables_thickness             = x_par.attr<double>(_Unicode(cables_thickness));
  double ftd1_vtx3_distance_z         = x_par.attr<double>(_Unicode(ftd1_vtx3_distance_z)); 
  double ftd7_ecal_distance_z         = x_par.attr<double>(_Unicode(ftd7_ecal_distance_z)); 
  double ftd1_sit1_radial_diff        = x_par.attr<double>(_Unicode(ftd1_sit1_radial_diff)); 
  double ftd2_sit1_radial_diff        = x_par.attr<double>(_Unicode(ftd2_sit1_radial_diff)); 
  double ftd3_sit2_radial_diff        = x_par.attr<double>(_Unicode(ftd3_sit2_radial_diff)); 
  double ftd4to7_tpc_radial_gap       = x_par.attr<double>(_Unicode(ftd4to7_tpc_radial_gap)); 

  // Helper class for parameter transformations
  struct cone_t { double rmin1, rmax1, rmin2, rmax2, zhalf;
    // 1st. constructor. Note the functionality!
    cone_t(double rm1,double rm2,double dr,double z) 
      : rmin1(rm1-dr),rmax1(rm1), rmin2(rm2-dr), rmax2(rm2), zhalf(z){}
    // 2nd. constructor. Note the different functionality!
    cone_t(double rm1,double rm2,double dr,double z,int) 
      : rmin1(rm1),rmax1(rm1+dr), rmin2(rm2), rmax2(rm2+dr), zhalf(z){}
    // 3rd. constructor. Note the different functionality!
    cone_t(double rmi1,double rma1,double rmi2,double rma2,double dr,double z) 
      : rmin1(rmi1+dr),rmax1(rma1+dr), rmin2(rmi2+dr), rmax2(rma2+dr), zhalf(z){}
  };
  struct cylinder_t { double start, stop, r1, r2;
    cylinder_t() : start(0), stop(0), r1(0), r2(0) {}
    cylinder_t(double _start, double _stop) : start(_start),stop(_stop), r1(0), r2(0) {}
    double zhalf() const    { return (stop-start)/2; }
    double position() const { return (start+stop)/2; }
  };


  // --- check that there is enough space for the cables and support
  if(beamTubeClearance < (cables_thickness + (2.0*cable_shield_thickness) + 0.5) ) {
    throw runtime_error("SFtd05:Stop: Not enough space for inner support structure and cables: increase beamTubeClearance");
  }

  // --- Now we can start to build the disks 
#if 0
  double minDiskThickness = numeric_limits<double>::max();
  for(xml_coll_t c(x_disks,_U(disk)); c; ++c) {
    double val = c.attr<double>(_Unicode(si_thickness));
    if ( minDiskThickness > val ) minDiskThickness = val;
  }
  //... The sensitive layer: Threshold is 20% of a MIP. For Si we have 340 KeV/mm as MIP.
  theFTDSD = new TRKSD00("FTD", minDiskThickness * 340 * keV/mm * 0.2);     
  RegisterSensitiveDetector(theFTDSD);
#endif

  //... Disks
  int cnt = 0;
  cylinder_t outer, inner;
  //... assembling detector
  for(xml_coll_t c(x_disks,_U(disk)); c; ++c, ++cnt) {
    //... Get the disk parameters
    xml_comp_t x_disk(c);
    int id = x_disk.id();
    string disk_name = name+_toString(id,"_disk%d");
    double inner_radius = 0;
    double outer_radius = 0;
    double beamTubeRadius = 0;
    double z_pos = 0;
    double z_end = 0;

    double si_thickness      = x_disk.attr<double>(_Unicode(si_thickness));
    double support_thickness = x_disk.attr<double>(_Unicode(support_thickness));
    double z_rel             = x_disk.attr<double>(_Unicode(z_ReltoTPCLength));

    switch (id) {
    case 1:
      // z defined by distance from end of VTX layer 3
      z_pos = VXD_layer3_maxZ + ftd1_vtx3_distance_z;
      // beam tube radius at backside of disk
      z_end = z_pos + si_thickness/2 + support_thickness;
      // outer r defined by radial difference to SIT layer 1
      outer_radius = SIT1_Radius + ftd1_sit1_radial_diff; 
      // check which part of the beam tube this disk lies above
      beamTubeRadius = (z_end < TUBE_IPOuterTube_end_z ) 
	? TUBE_IPOuterTube_end_radius 
	: TUBE_IPOuterTube_end_radius + (z_end-TUBE_IPOuterTube_end_z)*beamTangent;
      inner_radius = beamTubeRadius + beamTubeClearance;
      // check that there is no overlap with SIT1
      if( z_pos <= SIT1_Half_Length_Z && outer_radius>=SIT1_Radius) {
	cout << "SFtd05: Overlap between FTD1 and SIT1" << endl;
	cout << "SFtd05:FTD1 Radius = " << outer_radius << "SIT1 Radius = " << SIT1_Radius << endl;
	throw runtime_error("SFtd05: Overlap between FTD1 and SIT1");
      }
      if( z_rel != 0.0) {
	cout << "SFtd05: The z position of FTD1 is not relative." << endl;
	cout << "SFtd05: The z position of FTD1 is set by the distance between the centre of the sensitive layer and the max z of VTX layer 3." << endl;
	throw runtime_error("SFtd05: The z position of FTD1 is not relative.");
      }
      break;
      
    case 2:
      // z defined relative to TPC half-length: to ensure positioning with SIT set these numbers to the same value in DB
      z_pos = TPC_Ecal_Hcal_barrel_halfZ * z_rel;
      // outer r defined by radial difference to SIT layer 1
      outer_radius = SIT1_Radius + ftd2_sit1_radial_diff; 
      // beam tube radius at backside of disk
      z_end = z_pos + (si_thickness*0.5) + support_thickness;
      // check which part of the beam tube this disk lies above
      beamTubeRadius = (z_end < TUBE_IPOuterTube_end_z ) 
	? TUBE_IPOuterTube_end_radius 
	: TUBE_IPOuterTube_end_radius + (z_end-TUBE_IPOuterTube_end_z)*beamTangent;
      inner_radius = beamTubeRadius + beamTubeClearance;
      //... keep information for inner support cylinder with 0.5mm saftey clearance from inner radius of disks
      inner.start = TUBE_IPOuterTube_end_z;
      inner.r1 = inner_radius - (z_end-TUBE_IPOuterTube_end_z)*beamTangent - 0.5; 

      // check that there is no overlap with SIT1
      if( z_pos <= SIT1_Half_Length_Z && outer_radius >= SIT1_Radius ) {
	cout << "SFtd05:Overlap between FTD2 and SIT1" << endl;
	cout << "SFtd05:FTD2 Radius = " << outer_radius << "SIT1 Radius = " << SIT1_Radius << endl;
	throw runtime_error("SFtd05:Overlap between FTD2 and SIT1");
      }
      break;

    case 3:
      // z defined relative to TPC half-length: to ensure positioning with SIT set these numbers to the same value in DB
      z_pos = TPC_Ecal_Hcal_barrel_halfZ * z_rel;
      // beam tube radius at backside of disk
      z_end  = z_pos + si_thickness/2 + support_thickness;
      // outer r defined by radial difference to SIT layer 2
      outer_radius = SIT2_Radius + ftd3_sit2_radial_diff; 
      // check which part of the beam tube this disk lies above
      beamTubeRadius = (z_end < TUBE_IPOuterTube_end_z ) 
	? TUBE_IPOuterTube_end_radius 
	: TUBE_IPOuterTube_end_radius + (z_end-TUBE_IPOuterTube_end_z)*beamTangent;
      inner_radius = beamTubeRadius + beamTubeClearance;
      // check that there is no overlap with SIT1
      if( z_pos <= SIT2_Half_Length_Z && outer_radius >= SIT2_Radius ) {
	cout << "SFtd05:Overlap between FTD3 and SIT2" <<  endl;
	cout << "SFtd05:FTD3 Radius = " << outer_radius << "SIT2 Radius = " << SIT2_Radius << endl;
	throw runtime_error("SFtd05:Overlap between FTD3 and SIT2");
      }
      break;

    case 4:
    case 5:
    case 6:
      // z defined relative to TPC half-length
      z_pos = (TPC_Ecal_Hcal_barrel_halfZ * z_rel);
      // outer r defined by gap between TPC inner radius and FTD disks
      outer_radius = TPC_inner_radius - ftd4to7_tpc_radial_gap; 
      // beam tube radius at backside of disk
      z_end = (z_pos + (si_thickness*0.5) + support_thickness );
      // check which part of the beam tube this disk lies above
      beamTubeRadius = (z_end < TUBE_IPOuterTube_end_z ) 
	? TUBE_IPOuterTube_end_radius 
	: TUBE_IPOuterTube_end_radius + (z_end-TUBE_IPOuterTube_end_z)*beamTangent;
      inner_radius = beamTubeRadius + beamTubeClearance;
	
      // keep the information for outer cylinder
      if(id==4) outer.start = z_pos;
      break;

    case 7:
      // z defined by distance from front of ECal endcap
      z_pos = Ecal_endcap_zmin - ftd7_ecal_distance_z;
      // outer r defined by gap between TPC inner radius and FTD disks
      outer_radius = TPC_inner_radius - ftd4to7_tpc_radial_gap; 
      // beam tube radius at backside of disk
      z_end = (z_pos + (si_thickness*0.5) + support_thickness );
      // check which part of the beam tube this disk lies above
      beamTubeRadius = (z_end < TUBE_IPOuterTube_end_z ) 
	? TUBE_IPOuterTube_end_radius 
	: TUBE_IPOuterTube_end_radius + (z_end-TUBE_IPOuterTube_end_z)*beamTangent;
      inner_radius = beamTubeRadius + beamTubeClearance;
      // End of Support Structure: 0.5 mm clearance from disks
      outer.stop = z_end;
      inner.stop = z_end;
      outer.r1 = outer_radius + 0.5;
      inner.r2 = inner_radius - 0.5;

      if( z_rel != 0.0 )   {
	cout << "SFtd05: The z position of FTD7 is not relative. "
	     << "The relative value will not be used. It should be set to 0.0 in the DB." << endl;
	cout << "SFtd05: The z position of FTD7 is set by the distance "
	     << "between the centre of the sensitive layer and the min z of the ECal Endcap." << endl;
	throw runtime_error("SFtd05: The z position of FTD7 is not relative.");
      }
      break;

    default:
      cout << "SFtd05: Error disk number must be between 1-7: disk number = " << id << endl;
      throw runtime_error("SFtd05: Error disk number must be between 1-7.");
    }

    //... Si sensitive
    Tube  diskTube(inner_radius,outer_radius,si_thickness/2);
    Volume diskVol(disk_name+_toString(cnt,"_silicon_%d"),diskTube,siliconMat);
    diskVol.setSensitiveDetector(sens_det);
    diskVol.setVisAttributes(diskVis);
    assembly.placeVolume(diskVol,Position(0,0, z_pos)).addPhysVolID("disk", id);
    assembly.placeVolume(diskVol,Position(0,0,-z_pos)).addPhysVolID("disk",-id);

    //... Support
    Tube  suppTube(inner_radius,outer_radius,support_thickness/2);
    Volume suppVol(disk_name+_toString(cnt,"_support_%d"),suppTube,kaptonMat);
    suppVol.setVisAttributes(supportVis);     
    assembly.placeVolume(diskVol,Position(0,0,  z_pos+si_thickness/2+support_thickness/2));
    assembly.placeVolume(diskVol,Position(0,0,-(z_pos+si_thickness/2+support_thickness/2)));
  }
  
  //... Outer cylinder
  assert(outer.stop>0 && outer.start>0 && outer.zhalf()>0);  
  Tube outerCylSolid(outer.r1,outer.r1+outer_cyl_thickness,outer.zhalf());
  Volume outerCylVol(name+"_outer_cylinder",outerCylSolid,kaptonMat);
  outerCylVol.setVisAttributes(cylinderVis);

  assembly.placeVolume(outerCylVol,Position(0,0, outer.position()));  
  assembly.placeVolume(outerCylVol,Position(0,0,-outer.position()));

  //... Inner cylinder (cone)
  assert(inner.stop>0 && inner.start>0 && inner.zhalf()>0);
  cone_t inner_cone(inner.r1,inner.r2,
		    inner_cyl_thickness + 2*cable_shield_thickness + cables_thickness,inner.zhalf());
  cone_t shield    (inner_cone.rmin1,inner_cone.rmin2,
		    2*cable_shield_thickness + cables_thickness, inner.zhalf(),0);
  cone_t cables    (shield.rmin1,shield.rmin1+cables_thickness,
		    shield.rmin2,shield.rmin2+cables_thickness,
		    cable_shield_thickness,inner.zhalf());

  Cone cablesSolid(cables.zhalf,cables.rmin1,cables.rmax1,cables.rmin2,cables.rmax2);
  Volume cablesVol(name+"_cables",cablesSolid,copperMat);
  cablesVol.setVisAttributes(cablesVis);
  
  Cone shieldSolid(shield.zhalf,shield.rmin1,shield.rmax1,shield.rmin2,shield.rmax2);
  Volume shieldVol(name+"_shield",shieldSolid,kaptonMat);
  shieldVol.setVisAttributes(cablesVis);
  shieldVol.placeVolume(cablesVol);

  Cone cylinderSolid(inner_cone.zhalf,inner_cone.rmin1,inner_cone.rmax1,inner_cone.rmin2,inner_cone.rmax2);
  Volume cylinderVol(name+"_cylinder",cylinderSolid,kaptonMat);
  cylinderVol.setVisAttributes(cylinderVis);
  cylinderVol.placeVolume(shieldVol);

  assembly.placeVolume(cylinderVol,Position(0,0,inner.position()));
  assembly.placeVolume(cylinderVol,reflect_rot,Position(0,0,-inner.position()));
  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));

  PlacedVolume pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_SFtd05,create_element);
