#if 0
  G4double crossingAngle = theGeometryEnvironment.GetParameterAsDouble("TUBE_crossing_angle");
  G4double z_begin = theGeometryEnvironment.GetParameterAsDouble("Lcal_z_begin");
  G4double   r_max = theGeometryEnvironment.GetParameterAsDouble("Lcal_outer_radius");
  G4double   r_min = theGeometryEnvironment.GetParameterAsDouble("Lcal_inner_radius");
  G4int    n_layer = theGeometryEnvironment.GetParameterAsInt("Lcal_n_layers");
  G4double tungsten_thickness= theGeometryEnvironment.GetParameterAsDouble("Lcal_tungsten_thickness");
  G4double support_thickness = theGeometryEnvironment.GetParameterAsDouble("Lcal_support_thickness");
  G4double silicon_thickness = theGeometryEnvironment.GetParameterAsDouble("Lcal_silicon_thickness");
  G4double layer_gap = theGeometryEnvironment.GetParameterAsDouble("Lcal_layer_gap");
  G4double z_end = z_begin + (tungsten_thickness + support_thickness + silicon_thickness + layer_gap)*(double)n_layer;

  G4double alpha = crossingAngle / 2.* mrad;
  G4double width = z_end - z_begin;

  G4double r_inner_min = (width + z_begin * cos(alpha )) * tan(2.*alpha);
  G4double lcal_rot_extra_r = width * sin(alpha);
  G4double lcal_rot_extra_z = r_max * sin(alpha);
  G4double opening_angle = r_max/z_end;
  actual_opening_angle = opening_angle;

  //save for lcal
  //check if inner radius change is needed
  dbtmp->exec("SELECT * FROM tube WHERE name=\"lumcal_back\";");
  dbtmp->getTuple();
  G4double r = dbtmp->fetchDouble("rInnerStart");

  if ( (r_min - 0.5 - lcal_rot_extra_r) < r)
  {
	r = r_min - 0.5 - lcal_rot_extra_r;
	G4cout << "   ...change r_min of tube (lcal rotation) r = " << r << G4endl;
  }

  if (dbtmp->fetchDouble("zEnd") * tan(alpha) > r)
  {
    r_min = r_inner_min;
    G4cout << "    ...change r_min of lcal (lateral tubes crossAngle) r = " << r_min << G4endl; 
  }

//save for lcal
  actual_lcal_rin = r_min;

  G4double vxd_r = theGeometryEnvironment.GetParameterAsDouble("VXD_inner_radius");
  G4double thickness;

  dbtmp->exec("SELECT * FROM tube WHERE name=\"ip_inner_parallel\";");
  dbtmp->getTuple();
  thickness = dbtmp->fetchDouble("rOuterStart") - dbtmp->fetchDouble("rInnerStart");

  //ip_inner_parallel 0.5mm gap between pipe and vxd
  dbtmp->exec( ("UPDATE tube SET rOuterStart = " + double2str(vxd_r - 0.5) + " WHERE name = \"ip_inner_parallel\";").data() );

  dbtmp->exec( ("UPDATE tube SET rOuterEnd = " + double2str(vxd_r - 0.5) + " WHERE name = \"ip_inner_parallel\";").data() );

  dbtmp->exec( ("UPDATE tube SET rInnerStart = " + double2str(vxd_r - 0.5 - thickness) + " WHERE name = \"ip_inner_parallel\";").data() );

  dbtmp->exec( ("UPDATE tube SET rInnerEnd = " + double2str(vxd_r - 0.5 - thickness) + " WHERE name = \"ip_inner_parallel\";").data() );


  dbtmp->exec("SELECT * FROM tube WHERE name=\"lumcal_front\";");
  dbtmp->getTuple();
  width = dbtmp->fetchDouble("zEnd") - dbtmp->fetchDouble("zStart");
  r = dbtmp->fetchDouble("rInnerStart");

  //ip_outer_bulge
  dbtmp->exec("SELECT * FROM tube WHERE name=\"ip_outer_bulge\";");
  dbtmp->getTuple();
  thickness = dbtmp->fetchDouble("rOuterEnd") - dbtmp->fetchDouble("rInnerEnd");

  dbtmp->exec( ("UPDATE tube SET zEnd = " + double2str(z_begin - width - lcal_rot_extra_z) + " WHERE name = \"ip_outer_bulge\";").data() );

  dbtmp->exec( ("UPDATE tube SET rOuterEnd = " + double2str(r_max) + " WHERE name = \"ip_outer_bulge\";").data() );

  dbtmp->exec( ("UPDATE tube SET rInnerEnd = " + double2str(r_max - thickness) + " WHERE name = \"ip_outer_bulge\";").data() );

  //lumcal_front
  //0.5 cm gap between lcal and tube ?

  dbtmp->exec("SELECT * FROM tube WHERE name=\"lumcal_front\";");
  dbtmp->getTuple();
  r = dbtmp->fetchDouble("rInnerStart");

  dbtmp->exec("SELECT * FROM tube WHERE name=\"lumcal_wall\";");
  dbtmp->getTuple();
  thickness = dbtmp->fetchDouble("rOuterEnd") - dbtmp->fetchDouble("rInnerEnd");

  G4cout << "lumcal_wall thickness = " << thickness << G4endl;

  dbtmp->exec( ("UPDATE tube SET zStart = " + double2str(z_begin - width - lcal_rot_extra_z) + " WHERE name = \"lumcal_front\";").data() );

  dbtmp->exec( ("UPDATE tube SET zEnd = " + double2str(z_begin - lcal_rot_extra_z) + " WHERE name = \"lumcal_front\";").data() );

  dbtmp->exec( ("UPDATE tube SET rOuterStart = " + double2str(r_max) + " WHERE name = \"lumcal_front\";").data() );

  dbtmp->exec( ("UPDATE tube SET rOuterEnd = " + double2str(r_max) + " WHERE name = \"lumcal_front\";").data() );

  if ( (r_min - 0.5 - lcal_rot_extra_r) < r)
  {
    dbtmp->exec( ("UPDATE tube SET rInnerStart = " + double2str(r_min - 0.5 - lcal_rot_extra_r - thickness) + " WHERE name = \"lumcal_front\";").data() );

    dbtmp->exec( ("UPDATE tube SET rInnerEnd = " + double2str(r_min - 0.5 - lcal_rot_extra_r - thickness) + " WHERE name = \"lumcal_front\";").data() );
  }

  //lumcal_wall
  dbtmp->exec("SELECT * FROM tube WHERE name=\"lumcal_wall\";");
  dbtmp->getTuple();
  r = dbtmp->fetchDouble("rOuterStart");

  dbtmp->exec( ("UPDATE tube SET zStart = " + double2str(z_begin - lcal_rot_extra_z) + " WHERE name = \"lumcal_wall\";").data() );

  dbtmp->exec( ("UPDATE tube SET zEnd = " + double2str( z_end + lcal_rot_extra_z) + " WHERE name = \"lumcal_wall\";").data() );

  if ( (r_min - 0.5 - lcal_rot_extra_r) < r)
  {
    dbtmp->exec( ("UPDATE tube SET rOuterStart = " + double2str( r_min - 0.5 - lcal_rot_extra_r ) + " WHERE name = \"lumcal_wall\";").data() );

    dbtmp->exec( ("UPDATE tube SET rOuterEnd = " + double2str( r_min - 0.5 - lcal_rot_extra_r ) + " WHERE name = \"lumcal_wall\";").data() );

    dbtmp->exec( ("UPDATE tube SET rInnerStart = " + double2str(r_min - 0.5 - lcal_rot_extra_r - thickness) + " WHERE name = \"lumcal_wall\";").data() );

    dbtmp->exec( ("UPDATE tube SET rInnerEnd = " + double2str(r_min - 0.5 - lcal_rot_extra_r - thickness) + " WHERE name = \"lumcal_wall\";").data() );
  }

  //lumcal_back
  dbtmp->exec("SELECT * FROM tube WHERE name=\"lumcal_back\";");
  dbtmp->getTuple();
  r = dbtmp->fetchDouble("rInnerStart");

  dbtmp->exec( ("UPDATE tube SET zStart = " + double2str( z_end + lcal_rot_extra_z) + " WHERE name = \"lumcal_back\";").data() );

  if ( (r_min - 0.5 - lcal_rot_extra_r) < r)
  {
    dbtmp->exec( ("UPDATE tube SET rInnerStart = " + double2str(r_min - 0.5 - lcal_rot_extra_r - thickness) + " WHERE name = \"lumcal_back\";").data() );

    dbtmp->exec( ("UPDATE tube SET rInnerEnd = " + double2str(r_min - 0.5 - lcal_rot_extra_r - thickness) + " WHERE name = \"lumcal_back\";").data() );
  }

  return true;
}

G4bool STubeX00::PostLoadScriptAction(Database*  dbtmp, CGAGeometryEnvironment& )
{
  G4cout << "STubeX00 set the following geomerty parameters : " << endl;
  dbtmp->exec("SELECT * FROM tube;");
  while (dbtmp->getTuple())
  {
	G4cout << " " << dbtmp->fetchString("name") << " :  rIn1 = " << dbtmp->fetchDouble("rInnerStart");
	G4cout << " rOut1 = " << dbtmp->fetchDouble("rOuterStart") << " rIn2 = " << dbtmp->fetchDouble("rInnerEnd");
	G4cout << " rOut2 = " << dbtmp->fetchDouble("rOuterEnd") << " zStart = " << dbtmp->fetchDouble("zStart");
	G4cout << " zEnd = " << dbtmp->fetchDouble("zEnd") << endl;
  }

//pass the opening angle to others
  (*Control::globalModelParameters)["TUBE_opening_angle"] = double2str(actual_opening_angle);

//pass updated lcal rin
  (*Control::globalModelParameters)["Lcal_inner_radius"] = double2str(actual_lcal_rin);

  G4cout << "STubeX00 PostLoadScriptAction set opening angle "
	 << actual_opening_angle << G4endl;

  G4cout << "STubeX00 PostLoadScriptAction set LCAL rIn "
	 << actual_lcal_rin << G4endl;

  return true;    
}




// *********************************************************
// *                         Mokka                         *
// *    -- A Detailed Geant 4 Simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id: TubeX01.cc,v 1.5 2008/10/15 16:51:53 steve Exp $
// $Name: mokka-07-00 $
//
// History:
// - first implementation as Tube00: Paulo Mora de Freitas, Sep 2002
// - modified from Tube00 to Tube01DT: Ties Behnke, 2003-02-11
// - modified for a crossing angle as TubeX00: Adrian Vogel, 2005-05-18
// - modified for fancier geometries as TubeX01: Adrian Vogel, 2006-04-20

#include "TubeX01.hh"
#include "MySQLWrapper.hh"
#include "CGAGeometryManager.hh"
#include "CGAGeometryEnvironment.hh"
#include "CGADefs.h"
#include "Control.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#ifdef MOKKA_GEAR
#include "gearimpl/GearParametersImpl.h"
#include "MokkaGear.h"
#include "G4EmCalculator.hh"
#include "G4ParticleTable.hh"
#endif 


INSTANTIATE(TubeX01)

G4bool TubeX01::ContextualConstruct(const CGAGeometryEnvironment &env, G4LogicalVolume *worldLog)
{
  // useful values for construction of tubes and cones
  const G4double phi1 =   0.0 * deg; // all cones start at zero...
  const G4double phi2 = 360.0 * deg; // ...and cover the whole 360 degrees

  // some visualization attributes for the tube wall and the vacuum inside
  G4VisAttributes *wallVisAttrib = new G4VisAttributes(G4Colour(1.0, 0.7, 0.5)); // light brown
  //wallVisAttrib->SetForceSolid(true);
  G4VisAttributes *vacuumVisAttrib = new G4VisAttributes(G4Colour(0.0, 0.0, 0.5)); // dark blue
  vacuumVisAttrib->SetVisibility(false); // there isn't anything, so what do you expect?

  const G4double crossingAngle = env.GetParameterAsDouble("ILC_Main_Crossing_Angle") / 2 * mrad; // only half the angle
  const G4String dbName = env.GetDBName() + "_" + env.GetParameterAsString("ILC_Main_Crossing_Angle");
  Database *db = new Database(dbName.c_str());

  G4bool usingOffsets = false;
  TReferenceMap referenceOffsets;
  db->exec("SELECT * FROM `_references`;");
  while (db->getTuple()) {
    const G4String globalName   = db->fetchString("globalName");
    const G4String localName    = db->fetchString("localName");
    const G4double assumedValue = db->fetchDouble("assumption") * mm;
    const G4double currentValue = env.GetParameterAsDouble(globalName);
    const G4double offsetValue  = currentValue - assumedValue;
    referenceOffsets[localName] = offsetValue;

    if (offsetValue != 0) {
      G4cout
        << "TubeX01: Using " << globalName << " = "
        << currentValue / mm << " mm instead of "
        << assumedValue / mm << " mm" << G4endl;
      usingOffsets = true;
    }
  }
  if (usingOffsets) Control::Log("TubeX01: Be sure you know what you're doing!");

  bool firstPiece = true;
  material = "";
  beam_inner_radius = -99999;
  beam_thickness = -99999;
  
  db->exec("SELECT * FROM `tube`;");
  while (db->getTuple()) {
    // reference values for r- and z-values
    const G4String zStartRef         = db->fetchString("zStartRef");
    const G4String zEndRef           = db->fetchString("zEndRef");
    const G4String rInnerStartRef    = db->fetchString("rInnerStartRef");
    const G4String rInnerEndRef      = db->fetchString("rInnerEndRef");
    const G4String rOuterStartRef    = db->fetchString("rOuterStartRef");
    const G4String rOuterEndRef      = db->fetchString("rOuterEndRef");
    
    const G4double zStartOffset      = (zStartRef      == "") ? (0) : (referenceOffsets[zStartRef]);
    const G4double zEndOffset        = (zEndRef        == "") ? (0) : (referenceOffsets[zEndRef]);
    const G4double rInnerStartOffset = (rInnerStartRef == "") ? (0) : (referenceOffsets[rInnerStartRef]);
    const G4double rInnerEndOffset   = (rInnerEndRef   == "") ? (0) : (referenceOffsets[rInnerEndRef]);
    const G4double rOuterStartOffset = (rOuterStartRef == "") ? (0) : (referenceOffsets[rOuterStartRef]);
    const G4double rOuterEndOffset   = (rOuterEndRef   == "") ? (0) : (referenceOffsets[rOuterEndRef]);
  
    // fields in the data tuple
    const ECrossType crossType  = ECrossType(db->fetchInt("crossType")); // positioning of the volume
    const G4double zStart       = db->fetchDouble("zStart")      * mm + zStartOffset;
    const G4double zEnd         = db->fetchDouble("zEnd")        * mm + zEndOffset;
    const G4double rInnerStart  = db->fetchDouble("rInnerStart") * mm + rInnerStartOffset;
    const G4double rInnerEnd    = db->fetchDouble("rInnerEnd")   * mm + rInnerEndOffset;
    const G4double rOuterStart  = db->fetchDouble("rOuterStart") * mm + rOuterStartOffset;
    const G4double thickness    = rOuterStart - rInnerStart;
    const G4double rOuterEnd    = db->fetchDouble("rOuterEnd")   * mm + rOuterEndOffset;
    const G4String materialName = db->fetchString("material");
    const G4String volName      = "tube_" + db->fetchString("name");

    if(volName == "tube_IPOuterTube"){

      std::ostringstream oss1;
      oss1 << zStart;
      (*Control::globalModelParameters)["TUBE_IPOuterTube_start_z"] = oss1.str();
      std::ostringstream oss2;
      oss2 << zEnd;
      (*Control::globalModelParameters)["TUBE_IPOuterTube_end_z"] = oss2.str();
      std::ostringstream oss3;
      oss3 << rOuterStart;
      (*Control::globalModelParameters)["TUBE_IPOuterTube_start_radius"] = oss3.str();
      std::ostringstream oss4;
      oss4 << rOuterEnd;
      (*Control::globalModelParameters)["TUBE_IPOuterTube_end_radius"] = oss4.str();
   	
    }
    
    if(volName == "tube_IPOuterBulge"){

      std::ostringstream oss1;
      oss1 << zEnd;
      (*Control::globalModelParameters)["TUBE_IPOuterBulge_end_z"] = oss1.str();
      std::ostringstream oss2;
      oss2 << rOuterEnd;
      (*Control::globalModelParameters)["TUBE_IPOuterBulge_end_radius"] = oss2.str();

    }
    
    if(firstPiece)
      { 
	firstPiece = false;
	material = materialName;
	beam_inner_radius = rInnerStart;
	beam_thickness = thickness;
      }
    // things which can be calculated immediately
    zHalf        = fabs(zEnd - zStart) / 2; // half z length of the cone
    const G4double zPosition    = fabs(zEnd + zStart) / 2; // middle z position
    G4Material *coreMaterial    = CGAGeometryManager::GetMaterial("beam"); // always the same
    G4Material *wallMaterial    = CGAGeometryManager::GetMaterial(materialName);

    // this could mess up your geometry, so better check it
    if (crossingAngle == 0 && crossType != kCenter) {
      Control::Log("TubeX01: You are trying to build a crossing geometry without a crossing angle.\n"
        "This is probably not what you want - better check your geometry data!");
      return false; // premature exit, Mokka will abort now
    }

    register G4double tmpAngle;
    switch (crossType) {
      case kUpstream:
      case kPunchedUpstream:
      case kUpstreamClippedFront:
      case kUpstreamClippedRear:
      case kUpstreamClippedBoth:
      case kUpstreamSlicedFront:
      case kUpstreamSlicedRear:
      case kUpstreamSlicedBoth:
        tmpAngle = -crossingAngle; break;
      case kDnstream:
      case kPunchedDnstream:
      case kDnstreamClippedFront:
      case kDnstreamClippedRear:
      case kDnstreamClippedBoth:
      case kDnstreamSlicedFront:
      case kDnstreamSlicedRear:
      case kDnstreamSlicedBoth:
        tmpAngle = +crossingAngle; break;
      default:
        tmpAngle = 0; break;
    }
    const G4double rotateAngle = tmpAngle; // for the placement at +z (better make it const now)
    const G4double mirrorAngle = 180 * deg - rotateAngle; // for the "mirrored" placement at -z
    // the "mirroring" in fact is done by a rotation of (almost) 180 degrees around the y-axis
    
    switch (crossType) {
      case kCenter:
      case kUpstream:
      case kDnstream: {
        // a volume on the z-axis, on the upstream branch, or on the downstream branch
      
        // absolute transformations for the placement in the world
        G4Transform3D transformer(G4RotationMatrix().rotateY(rotateAngle), G4ThreeVector(0, 0, zPosition).rotateY(rotateAngle));
        G4Transform3D transmirror(G4RotationMatrix().rotateY(mirrorAngle), G4ThreeVector(0, 0, zPosition).rotateY(mirrorAngle));
        
        // solid for the tube (including vacuum and wall): a solid cone
        G4Cons *tubeSolid = new G4Cons(volName, 0, rOuterStart, 0, rOuterEnd, zHalf, phi1, phi2);
        
        // tube consists of vacuum
        G4LogicalVolume *tubeLog = new G4LogicalVolume(tubeSolid, coreMaterial, volName, 0, 0, 0, true);
        tubeLog->SetVisAttributes(vacuumVisAttrib);
        
        // placement of the tube in the world, both at +z and -z
        new G4PVPlacement(transformer, tubeLog, volName, worldLog, false, 0);
        new G4PVPlacement(transmirror, tubeLog, volName, worldLog, false, 1);
        
        // if inner and outer radii are equal, then omit the tube wall
        if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
          // the wall solid: a tubular cone
          G4Cons *wallSolid = new G4Cons(volName + "_wall", rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, zHalf, phi1, phi2);
        
          // the wall consists of the material given in the database
          G4LogicalVolume *wallLog = new G4LogicalVolume(wallSolid, wallMaterial, volName + "_wall", 0, 0, 0, true);
          wallLog->SetVisAttributes(wallVisAttrib);
        
          // placement as a daughter volume of the tube, will appear in both placements of the tube
          new G4PVPlacement(0, 0, wallLog, volName + "_wall", tubeLog, false, 0);
        }
        break;
      }
      case kPunchedCenter: {
        // a volume on the z-axis with one or two inner holes
        // (implemented as a cone from which tubes are punched out)
      
        const G4double rUpstreamPunch = rInnerStart; // just alias names denoting what is meant here
        const G4double rDnstreamPunch = rInnerEnd; // (the database entries are "abused" in this case)
      
        // relative transformations for the composition of the G4SubtractionVolumes
        G4Transform3D upstreamTransformer(G4RotationMatrix().rotateY(-crossingAngle), G4ThreeVector(zPosition * tan(-crossingAngle), 0, 0));
        G4Transform3D dnstreamTransformer(G4RotationMatrix().rotateY(+crossingAngle), G4ThreeVector(zPosition * tan(+crossingAngle), 0, 0));
  
        // absolute transformations for the final placement in the world (angles always equal zero and 180 deg)
        G4Transform3D placementTransformer(G4RotationMatrix().rotateY(rotateAngle), G4ThreeVector(0, 0, zPosition).rotateY(rotateAngle));
        G4Transform3D placementTransmirror(G4RotationMatrix().rotateY(mirrorAngle), G4ThreeVector(0, 0, zPosition).rotateY(mirrorAngle));
  
        // solid for the tube (including vacuum and wall): a solid cone
        G4Cons *tubeSolid = new G4Cons(volName, 0, rOuterStart, 0, rOuterEnd, zHalf, phi1, phi2);
        
        // tube consists of vacuum (will later have two different daughters)
        G4LogicalVolume *tubeLog0 = new G4LogicalVolume(tubeSolid, coreMaterial, volName + "_0", 0, 0, 0, true);
        G4LogicalVolume *tubeLog1 = new G4LogicalVolume(tubeSolid, coreMaterial, volName + "_1", 0, 0, 0, true);
        tubeLog0->SetVisAttributes(vacuumVisAttrib);
        tubeLog1->SetVisAttributes(vacuumVisAttrib);
        
        // placement of the tube in the world, both at +z and -z
        new G4PVPlacement(placementTransformer, tubeLog0, volName, worldLog, false, 0);
        new G4PVPlacement(placementTransmirror, tubeLog1, volName, worldLog, false, 1);
        
        // the wall solid and placeholders for possible G4SubtractionSolids
        G4Cons *wholeSolid = new G4Cons(volName + "_wall_whole", 0, rOuterStart, 0, rOuterEnd, zHalf, phi1, phi2);
        G4VSolid *tmpSolid0, *tmpSolid1, *wallSolid0, *wallSolid1;
  
        // the punched subtraction solids can be asymmetric and therefore have to be created twice:
        // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
        // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
        if (rUpstreamPunch) { // do we need a hole on the upstream branch?
          G4Tubs *upstreamPunch = new G4Tubs(volName + "_wall_punch_up", 0, rUpstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
          tmpSolid0 = new G4SubtractionSolid(volName + "_wall_tmp_0", wholeSolid, upstreamPunch, upstreamTransformer);
          tmpSolid1 = new G4SubtractionSolid(volName + "_wall_tmp_1", wholeSolid, upstreamPunch, dnstreamTransformer); // [sic]
        } else { // dont't do anything, just pass on the unmodified shape
          tmpSolid0 = wholeSolid;
          tmpSolid1 = wholeSolid;
        }
  
        if (rDnstreamPunch) { // do we need a hole on the downstream branch?
          G4Tubs *dnstreamPunch = new G4Tubs(volName + "_wall_punch_dn", 0, rDnstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
          wallSolid0 = new G4SubtractionSolid(volName + "_wall_0", tmpSolid0, dnstreamPunch, dnstreamTransformer);
          wallSolid1 = new G4SubtractionSolid(volName + "_wall_1", tmpSolid1, dnstreamPunch, upstreamTransformer); // [sic]
        } else { // dont't do anything, just pass on the unmodified shape
          wallSolid0 = tmpSolid0;
          wallSolid1 = tmpSolid1;
        }
  
        // the wall consists of the material given in the database
        G4LogicalVolume *wallLog0 = new G4LogicalVolume(wallSolid0, wallMaterial, volName + "_wall_0", 0, 0, 0, true);
        G4LogicalVolume *wallLog1 = new G4LogicalVolume(wallSolid1, wallMaterial, volName + "_wall_1", 0, 0, 0, true);
        wallLog0->SetVisAttributes(wallVisAttrib);
        wallLog1->SetVisAttributes(wallVisAttrib);
  
        // placement as a daughter volumes of the tube
        new G4PVPlacement(0, 0, wallLog0, volName + "_wall", tubeLog0, false, 0);
        new G4PVPlacement(0, 0, wallLog1, volName + "_wall", tubeLog1, false, 1);
        break;
      }
      case kPunchedUpstream:
      case kPunchedDnstream: {
        // a volume on the upstream or downstream branch with two inner holes
        // (implemented as a cone from which another tube is punched out)
      
        const G4double rCenterPunch = (crossType == kPunchedUpstream) ? (rInnerStart) : (rInnerEnd); // just alias names denoting what is meant here
        const G4double rOffsetPunch = (crossType == kPunchedDnstream) ? (rInnerStart) : (rInnerEnd); // (the database entries are "abused" in this case)
      
        // relative transformations for the composition of the G4SubtractionVolumes
        G4Transform3D punchTransformer(G4RotationMatrix().rotateY(-2 * rotateAngle), G4ThreeVector(zPosition * tan(-2 * rotateAngle), 0, 0));
        G4Transform3D punchTransmirror(G4RotationMatrix().rotateY(+2 * rotateAngle), G4ThreeVector(zPosition * tan(+2 * rotateAngle), 0, 0));
  
        // absolute transformations for the final placement in the world
        G4Transform3D placementTransformer(G4RotationMatrix().rotateY(rotateAngle), G4ThreeVector(0, 0, zPosition).rotateY(rotateAngle));
        G4Transform3D placementTransmirror(G4RotationMatrix().rotateY(mirrorAngle), G4ThreeVector(0, 0, zPosition).rotateY(mirrorAngle));
  
        // solid for the tube (including vacuum and wall): a solid cone
        G4Cons *tubeSolid = new G4Cons(volName, 0, rOuterStart, 0, rOuterEnd, zHalf, phi1, phi2);
        
        // tube consists of vacuum (will later have two different daughters)
        G4LogicalVolume *tubeLog0 = new G4LogicalVolume(tubeSolid, coreMaterial, volName + "_0", 0, 0, 0, true);
        G4LogicalVolume *tubeLog1 = new G4LogicalVolume(tubeSolid, coreMaterial, volName + "_1", 0, 0, 0, true);
        tubeLog0->SetVisAttributes(vacuumVisAttrib);
        tubeLog1->SetVisAttributes(vacuumVisAttrib);
        
        // placement of the tube in the world, both at +z and -z
        new G4PVPlacement(placementTransformer, tubeLog0, volName, worldLog, false, 0);
        new G4PVPlacement(placementTransmirror, tubeLog1, volName, worldLog, false, 1);
        
        // the wall solid and the piece (only a tube, for the moment) which will be punched out
        G4Cons *wholeSolid = new G4Cons(volName + "_wall_whole", rCenterPunch, rOuterStart, rCenterPunch, rOuterEnd, zHalf, phi1, phi2);
        G4Tubs *punchSolid = new G4Tubs(volName + "_wall_punch", 0, rOffsetPunch, 5 * zHalf, phi1, phi2); // a bit longer
  
        // the punched subtraction solids can be asymmetric and therefore have to be created twice:
        // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
        // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
        G4SubtractionSolid *wallSolid0 = new G4SubtractionSolid(volName + "_wall_0", wholeSolid, punchSolid, punchTransformer);
        G4SubtractionSolid *wallSolid1 = new G4SubtractionSolid(volName + "_wall_1", wholeSolid, punchSolid, punchTransmirror);
  
        // the wall consists of the material given in the database
        G4LogicalVolume *wallLog0 = new G4LogicalVolume(wallSolid0, wallMaterial, volName + "_wall_0", 0, 0, 0, true);
        G4LogicalVolume *wallLog1 = new G4LogicalVolume(wallSolid1, wallMaterial, volName + "_wall_1", 0, 0, 0, true);
        wallLog0->SetVisAttributes(wallVisAttrib);
        wallLog1->SetVisAttributes(wallVisAttrib);
  
        // placement as a daughter volumes of the tube
        new G4PVPlacement(0, 0, wallLog0, volName + "_wall", tubeLog0, false, 0);
        new G4PVPlacement(0, 0, wallLog1, volName + "_wall", tubeLog1, false, 1);
        break;
      }
      case kUpstreamClippedFront:
      case kDnstreamClippedFront:
      case kUpstreamSlicedFront:
      case kDnstreamSlicedFront: {
        // a volume on the upstream or donwstream branch, but with the front face parallel to the xy-plane
        // or to a piece tilted in the other direction ("sliced" like a salami with 2 * rotateAngle)
        // (implemented as a slightly longer cone from which the end is clipped off)
      
        // the volume which will be used for clipping: a solid tube
        const G4double clipSize = rOuterStart; // the right order of magnitude for the clipping volume (alias name)
        G4Tubs *clipSolid = new G4Tubs(volName + "_clip", 0, 2 * clipSize, clipSize, phi1, phi2); // should be large enough
        
        // relative transformations for the composition of the G4SubtractionVolumes
        const G4double clipAngle = (crossType == kUpstreamClippedFront || crossType == kDnstreamClippedFront) ? (rotateAngle) : (2 * rotateAngle);
        const G4double clipShift = (zStart - clipSize) / cos(clipAngle) - (zPosition - clipSize / 2); // question: why is this correct?
        G4Transform3D clipTransformer(G4RotationMatrix().rotateY(-clipAngle), G4ThreeVector(0, 0, clipShift));
        G4Transform3D clipTransmirror(G4RotationMatrix().rotateY(+clipAngle), G4ThreeVector(0, 0, clipShift));
  
        // absolute transformations for the final placement in the world
        G4Transform3D placementTransformer(G4RotationMatrix().rotateY(rotateAngle), G4ThreeVector(0, 0, zPosition - clipSize / 2).rotateY(rotateAngle));
        G4Transform3D placementTransmirror(G4RotationMatrix().rotateY(mirrorAngle), G4ThreeVector(0, 0, zPosition - clipSize / 2).rotateY(mirrorAngle));
  
        // solid for the tube (including vacuum and wall): a solid cone
        G4Cons *wholeSolid = new G4Cons(volName + "_whole", 0, rOuterStart, 0, rOuterEnd, zHalf + clipSize / 2, phi1, phi2); // a bit longer
  
        // clip away the protruding end
        G4SubtractionSolid *tubeSolid0 = new G4SubtractionSolid(volName + "_0", wholeSolid, clipSolid, clipTransformer);
        G4SubtractionSolid *tubeSolid1 = new G4SubtractionSolid(volName + "_1", wholeSolid, clipSolid, clipTransmirror);
        
        // tube consists of vacuum (will later have two different daughters)
        G4LogicalVolume *tubeLog0 = new G4LogicalVolume(tubeSolid0, coreMaterial, volName + "_0", 0, 0, 0, true);
        G4LogicalVolume *tubeLog1 = new G4LogicalVolume(tubeSolid1, coreMaterial, volName + "_1", 0, 0, 0, true);
        tubeLog0->SetVisAttributes(vacuumVisAttrib);
        tubeLog1->SetVisAttributes(vacuumVisAttrib);
        
        // placement of the tube in the world, both at +z and -z
        new G4PVPlacement(placementTransformer, tubeLog0, volName, worldLog, false, 0);
        new G4PVPlacement(placementTransmirror, tubeLog1, volName, worldLog, false, 1);
       
        if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
          // the wall solid: a tubular cone
          G4Cons *wallWholeSolid = new G4Cons(volName + "_wall_whole", rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, zHalf + clipSize / 2, phi1, phi2); // a bit longer
        
          // clip away the protruding end
          G4SubtractionSolid *wallSolid0 = new G4SubtractionSolid(volName + "_wall_0", wallWholeSolid, clipSolid, clipTransformer);
          G4SubtractionSolid *wallSolid1 = new G4SubtractionSolid(volName + "_wall_1", wallWholeSolid, clipSolid, clipTransmirror);
        
          // the wall consists of the material given in the database
          G4LogicalVolume *wallLog0 = new G4LogicalVolume(wallSolid0, wallMaterial, volName + "_wall_0", 0, 0, 0, true);
          G4LogicalVolume *wallLog1 = new G4LogicalVolume(wallSolid1, wallMaterial, volName + "_wall_1", 0, 0, 0, true);
          wallLog0->SetVisAttributes(wallVisAttrib);
          wallLog1->SetVisAttributes(wallVisAttrib);
        
          // placement as a daughter volumes of the tube
          new G4PVPlacement(0, 0, wallLog0, volName + "_wall", tubeLog0, false, 0);
          new G4PVPlacement(0, 0, wallLog1, volName + "_wall", tubeLog1, false, 1);
        }
        break;
      }
      case kUpstreamClippedRear:
      case kDnstreamClippedRear:
      case kUpstreamSlicedRear:
      case kDnstreamSlicedRear: {
        // a volume on the upstream or donwstream branch, but with the rear face parallel to the xy-plane
        // or to a piece tilted in the other direction ("sliced" like a salami with 2 * rotateAngle)
        // (implemented as a slightly longer cone from which the end is clipped off)
      
        // the volume which will be used for clipping: a solid tube
        const G4double clipSize = rOuterEnd; // the right order of magnitude for the clipping volume (alias name)
        G4Tubs *clipSolid = new G4Tubs(volName + "_clip", 0, 2 * clipSize, clipSize, phi1, phi2); // should be large enough
        
        // relative transformations for the composition of the G4SubtractionVolumes
        const G4double clipAngle = (crossType == kUpstreamClippedRear || crossType == kDnstreamClippedRear) ? (rotateAngle) : (2 * rotateAngle);
        const G4double clipShift = (zEnd + clipSize) / cos(clipAngle) - (zPosition + clipSize / 2); // question: why is this correct?
        G4Transform3D clipTransformer(G4RotationMatrix().rotateY(-clipAngle), G4ThreeVector(0, 0, clipShift));
        G4Transform3D clipTransmirror(G4RotationMatrix().rotateY(+clipAngle), G4ThreeVector(0, 0, clipShift));
  
        // absolute transformations for the final placement in the world
        G4Transform3D placementTransformer(G4RotationMatrix().rotateY(rotateAngle), G4ThreeVector(0, 0, zPosition + clipSize / 2).rotateY(rotateAngle));
        G4Transform3D placementTransmirror(G4RotationMatrix().rotateY(mirrorAngle), G4ThreeVector(0, 0, zPosition + clipSize / 2).rotateY(mirrorAngle));
  
        // solid for the tube (including vacuum and wall): a solid cone
        G4Cons *wholeSolid = new G4Cons(volName + "_whole", 0, rOuterStart, 0, rOuterEnd, zHalf + clipSize / 2, phi1, phi2); // a bit longer
  
        // clip away the protruding end
        G4SubtractionSolid *tubeSolid0 = new G4SubtractionSolid(volName + "_0", wholeSolid, clipSolid, clipTransformer);
        G4SubtractionSolid *tubeSolid1 = new G4SubtractionSolid(volName + "_1", wholeSolid, clipSolid, clipTransmirror);
        
        // tube consists of vacuum (will later have two different daughters)
        G4LogicalVolume *tubeLog0 = new G4LogicalVolume(tubeSolid0, coreMaterial, volName + "_0", 0, 0, 0, true);
        G4LogicalVolume *tubeLog1 = new G4LogicalVolume(tubeSolid1, coreMaterial, volName + "_1", 0, 0, 0, true);
        tubeLog0->SetVisAttributes(vacuumVisAttrib);
        tubeLog1->SetVisAttributes(vacuumVisAttrib);
        
        // placement of the tube in the world, both at +z and -z
        new G4PVPlacement(placementTransformer, tubeLog0, volName, worldLog, false, 0);
        new G4PVPlacement(placementTransmirror, tubeLog1, volName, worldLog, false, 1);
       
        if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
          // the wall solid: a tubular cone
          G4Cons *wallWholeSolid = new G4Cons(volName + "_wall_whole", rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, zHalf + clipSize / 2, phi1, phi2); // a bit longer
        
          // clip away the protruding end
          G4SubtractionSolid *wallSolid0 = new G4SubtractionSolid(volName + "_wall_0", wallWholeSolid, clipSolid, clipTransformer);
          G4SubtractionSolid *wallSolid1 = new G4SubtractionSolid(volName + "_wall_1", wallWholeSolid, clipSolid, clipTransmirror);
        
          // the wall consists of the material given in the database
          G4LogicalVolume *wallLog0 = new G4LogicalVolume(wallSolid0, wallMaterial, volName + "_wall_0", 0, 0, 0, true);
          G4LogicalVolume *wallLog1 = new G4LogicalVolume(wallSolid1, wallMaterial, volName + "_wall_1", 0, 0, 0, true);
          wallLog0->SetVisAttributes(wallVisAttrib);
          wallLog1->SetVisAttributes(wallVisAttrib);
        
          // placement as a daughter volumes of the tube
          new G4PVPlacement(0, 0, wallLog0, volName + "_wall", tubeLog0, false, 0);
          new G4PVPlacement(0, 0, wallLog1, volName + "_wall", tubeLog1, false, 1);
        }
        break;
      }
      case kUpstreamClippedBoth:
      case kDnstreamClippedBoth:
      case kUpstreamSlicedBoth:
      case kDnstreamSlicedBoth: {
        // a volume on the upstream or donwstream branch, but with both faces parallel to the xy-plane
        // or to a piece tilted in the other direction ("sliced" like a salami with 2 * rotateAngle)
        // (implemented as a slightly longer cone from which the end is clipped off)
      
        // the volume which will be used for clipping: a solid tube
        const G4double clipSize = rOuterEnd; // the right order of magnitude for the clipping volume (alias name)
        G4Tubs *clipSolid = new G4Tubs(volName + "_clip", 0, 2 * clipSize, clipSize, phi1, phi2); // should be large enough
        
        // relative transformations for the composition of the G4SubtractionVolumes
        const G4double clipAngle = (crossType == kUpstreamClippedBoth || crossType == kDnstreamClippedBoth) ? (rotateAngle) : (2 * rotateAngle);
        const G4double clipShiftFrnt = (zStart - clipSize) / cos(clipAngle) - zPosition;
        const G4double clipShiftRear = (zEnd   + clipSize) / cos(clipAngle) - zPosition;
        G4Transform3D clipTransformerFrnt(G4RotationMatrix().rotateY(-clipAngle), G4ThreeVector(0, 0, clipShiftFrnt));
        G4Transform3D clipTransformerRear(G4RotationMatrix().rotateY(-clipAngle), G4ThreeVector(0, 0, clipShiftRear));
        G4Transform3D clipTransmirrorFrnt(G4RotationMatrix().rotateY(+clipAngle), G4ThreeVector(0, 0, clipShiftFrnt));
        G4Transform3D clipTransmirrorRear(G4RotationMatrix().rotateY(+clipAngle), G4ThreeVector(0, 0, clipShiftRear));
  
        // absolute transformations for the final placement in the world
        G4Transform3D placementTransformer(G4RotationMatrix().rotateY(rotateAngle), G4ThreeVector(0, 0, zPosition).rotateY(rotateAngle));
        G4Transform3D placementTransmirror(G4RotationMatrix().rotateY(mirrorAngle), G4ThreeVector(0, 0, zPosition).rotateY(mirrorAngle));
  
        // solid for the tube (including vacuum and wall): a solid cone
        G4Cons *wholeSolid = new G4Cons(volName + "_whole", 0, rOuterStart, 0, rOuterEnd, zHalf + clipSize, phi1, phi2); // a bit longer
  
        // clip away the protruding ends
        G4SubtractionSolid *tmpSolid0  = new G4SubtractionSolid(volName + "_tmp_0", wholeSolid, clipSolid, clipTransformerFrnt);
        G4SubtractionSolid *tmpSolid1  = new G4SubtractionSolid(volName + "_tmp_1", wholeSolid, clipSolid, clipTransmirrorFrnt);
        G4SubtractionSolid *tubeSolid0 = new G4SubtractionSolid(volName + "_0",     tmpSolid0,  clipSolid, clipTransformerRear);
        G4SubtractionSolid *tubeSolid1 = new G4SubtractionSolid(volName + "_1",     tmpSolid1,  clipSolid, clipTransmirrorRear);
        
        // tube consists of vacuum (will later have two different daughters)
        G4LogicalVolume *tubeLog0 = new G4LogicalVolume(tubeSolid0, coreMaterial, volName + "_0", 0, 0, 0, true);
        G4LogicalVolume *tubeLog1 = new G4LogicalVolume(tubeSolid1, coreMaterial, volName + "_1", 0, 0, 0, true);
        tubeLog0->SetVisAttributes(vacuumVisAttrib);
        tubeLog1->SetVisAttributes(vacuumVisAttrib);
        
        // placement of the tube in the world, both at +z and -z
        new G4PVPlacement(placementTransformer, tubeLog0, volName, worldLog, false, 0);
        new G4PVPlacement(placementTransmirror, tubeLog1, volName, worldLog, false, 1);
       
        if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
          // the wall solid: a tubular cone
          G4Cons *wallWholeSolid = new G4Cons(volName + "_wall_whole", rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, zHalf + clipSize, phi1, phi2); // a bit longer
        
          // clip away the protruding ends
          G4SubtractionSolid *wallTmpSolid0 = new G4SubtractionSolid(volName + "_wall_tmp_0", wallWholeSolid, clipSolid, clipTransformerFrnt);
          G4SubtractionSolid *wallTmpSolid1 = new G4SubtractionSolid(volName + "_wall_tmp_1", wallWholeSolid, clipSolid, clipTransmirrorFrnt);
          G4SubtractionSolid *wallSolid0    = new G4SubtractionSolid(volName + "_wall_0",     wallTmpSolid0,  clipSolid, clipTransformerRear);
          G4SubtractionSolid *wallSolid1    = new G4SubtractionSolid(volName + "_wall_1",     wallTmpSolid1,  clipSolid, clipTransmirrorRear);
        
          // the wall consists of the material given in the database
          G4LogicalVolume *wallLog0 = new G4LogicalVolume(wallSolid0, wallMaterial, volName + "_wall_0", 0, 0, 0, true);
          G4LogicalVolume *wallLog1 = new G4LogicalVolume(wallSolid1, wallMaterial, volName + "_wall_1", 0, 0, 0, true);
          wallLog0->SetVisAttributes(wallVisAttrib);
          wallLog1->SetVisAttributes(wallVisAttrib);
        
          // placement as a daughter volumes of the tube
          new G4PVPlacement(0, 0, wallLog0, volName + "_wall", tubeLog0, false, 0);
          new G4PVPlacement(0, 0, wallLog1, volName + "_wall", tubeLog1, false, 1);
        }
        break;
      }
      default: {
        Control::Log("TubeX01: Unimplemented \"crossType\" code.");
        return false; // fatal failure
      }
    } // switch (crossType)
  } // while (db->getTuple())
  delete db;
  return true;
}
#endif
