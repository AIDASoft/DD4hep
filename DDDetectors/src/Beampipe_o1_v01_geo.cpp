//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#include "DDDetectors/OtherDetectorHelpers.h"

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Printout.h"

#include "DDRec/DetectorData.h"
#include "DDRec/Surface.h"

#include "XML/Utilities.h"

#include <cmath>
#include <map>
#include <string>

using dd4hep::Transform3D;
using dd4hep::Position;
using dd4hep::RotationY;
using dd4hep::RotateY;
using dd4hep::ConeSegment;
using dd4hep::SubtractionSolid;
using dd4hep::Material;
using dd4hep::Volume;
using dd4hep::Solid;
using dd4hep::Tube;
namespace units = dd4hep;
using dd4hep::rec::Vector3D;
using dd4hep::rec::VolCylinder;
using dd4hep::rec::VolCone;
using dd4hep::rec::SurfaceType;

/// helper class for a simple cylinder surface parallel to z with a given length - used as IP layer
class SimpleCylinderImpl : public  dd4hep::rec::VolCylinderImpl{
  double _half_length ;
public:
  /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
  SimpleCylinderImpl( dd4hep::Volume vol, SurfaceType type,
		      double thickness_inner ,double thickness_outer,  Vector3D origin ) :
    dd4hep::rec::VolCylinderImpl( vol,  type, thickness_inner, thickness_outer,   origin ),
    _half_length(0){
  }
  void setHalfLength( double half_length){
    _half_length = half_length ;
  }
  void setID( dd4hep::FieldID id ) { _id = id ;
  }
  // overwrite to include points inside the inner radius of the barrel
  bool insideBounds(const Vector3D& point, double epsilon) const {
    return ( std::abs( point.rho() - origin().rho() ) < epsilon && std::abs( point.z() ) < _half_length ) ;
  }

  virtual std::vector< std::pair<Vector3D, Vector3D> > getLines(unsigned nMax=100){

    std::vector< std::pair<Vector3D, Vector3D> >  lines ;

    lines.reserve( nMax ) ;

    Vector3D zv( 0. , 0. , _half_length ) ;
    double r = _o.rho() ;

    unsigned n = nMax / 4 ;
    double dPhi = 2.* ROOT::Math::Pi() / double( n ) ;

    for( unsigned i = 0 ; i < n ; ++i ) {

      Vector3D rv0(  r*sin(  i   *dPhi ) , r*cos(  i   *dPhi )  , 0. ) ;
      Vector3D rv1(  r*sin( (i+1)*dPhi ) , r*cos( (i+1)*dPhi )  , 0. ) ;

      Vector3D pl0 =  zv + rv0 ;
      Vector3D pl1 =  zv + rv1 ;
      Vector3D pl2 = -zv + rv1  ;
      Vector3D pl3 = -zv + rv0 ;

      lines.push_back( std::make_pair( pl0, pl1 ) ) ;
      lines.push_back( std::make_pair( pl1, pl2 ) ) ;
      lines.push_back( std::make_pair( pl2, pl3 ) ) ;
      lines.push_back( std::make_pair( pl3, pl0 ) ) ;
    }
    return lines;
  }
};

class SimpleCylinder : public dd4hep::rec::VolSurface{
public:
  SimpleCylinder( dd4hep::Volume vol, dd4hep::rec::SurfaceType type, double thickness_inner ,
		  double thickness_outer,  Vector3D origin ) :
    dd4hep::rec::VolSurface( new SimpleCylinderImpl( vol,  type,  thickness_inner , thickness_outer, origin ) ) {
  }
  SimpleCylinderImpl* operator->() { return static_cast<SimpleCylinderImpl*>( _surf ) ; }
} ;


static dd4hep::Ref_t create_element(dd4hep::Detector& description,
					      xml_h element,
					      dd4hep::SensitiveDetector /*sens*/) {

  printout(dd4hep::DEBUG,"DD4hep_Beampipe", "Creating Beampipe" ) ;

  //Access to the XML File
  xml_det_t xmlBeampipe = element;
  const std::string name = xmlBeampipe.nameStr();
  bool nocore  = xmlBeampipe.nocore(false);

  dd4hep::DetElement tube(  name, xmlBeampipe.id()  ) ;

  // --- create an envelope volume and position it into the world ---------------------

  Volume envelope = dd4hep::xml::createPlacedEnvelope( description,  element , tube ) ;

  dd4hep::xml::setDetectorTypeFlag( element, tube ) ;

  if( description.buildType() == dd4hep::BUILD_ENVELOPE ) return tube ;

  //-----------------------------------------------------------------------------------


  dd4hep::rec::ConicalSupportData* beampipeData = new dd4hep::rec::ConicalSupportData ;

  //DD4hep/TGeo seems to need rad (as opposed to the manual)
  const double phi1 = 0 ;
  const double phi2 = 360.0*units::degree;

  //Parameters we have to know about
  dd4hep::xml::Component xmlParameter = xmlBeampipe.child(_Unicode(parameter));
  const double crossingAngle  = xmlParameter.attr< double >(_Unicode(crossingangle))*0.5; //  only half the angle


  double min_radius = 1.e99 ;

  for(xml_coll_t c( xmlBeampipe ,Unicode("section")); c; ++c) {

    xml_comp_t xmlSection( c );

    ODH::ECrossType crossType = ODH::getCrossType(xmlSection.attr< std::string >(_Unicode(type)));
    const double zStart       = xmlSection.attr< double > (_Unicode(start));
    const double zEnd         = xmlSection.attr< double > (_Unicode(end));
    const double rInnerStart  = xmlSection.attr< double > (_Unicode(rMin1));
    const double rInnerEnd    = xmlSection.attr< double > (_Unicode(rMin2));
    const double rOuterStart  = xmlSection.attr< double > (_Unicode(rMax1));
    const double rOuterEnd    = xmlSection.attr< double > (_Unicode(rMax2));
    const double thickness    = rOuterStart - rInnerStart;
    Material sectionMat  = description.material(xmlSection.materialStr());
    const std::string volName      = "tube_" + xmlSection.nameStr();

    std::stringstream pipeInfo;
    pipeInfo << std::setw(8) << zStart      /units::mm
	     << std::setw(8) << zEnd        /units::mm
	     << std::setw(8) << rInnerStart /units::mm
	     << std::setw(8) << rInnerEnd   /units::mm
	     << std::setw(8) << rOuterStart /units::mm
	     << std::setw(8) << rOuterEnd   /units::mm
	     << std::setw(8) << thickness   /units::mm
	     << std::setw(8) << crossType
	     << std::setw(35) << volName
	     << std::setw(15) << sectionMat.name();

    printout(dd4hep::INFO, "DD4hep_Beampipe", pipeInfo.str() );

    if( crossType == ODH::kCenter ) { // store only the central sections !
      dd4hep::rec::ConicalSupportData::Section section ;
      section.rInner = rInnerStart ;
      section.rOuter = rOuterStart ;
      section.zPos   = zStart ;
      beampipeData->sections.push_back( section ) ;
    }

    // things which can be calculated immediately
    const double zHalf       = fabs(zEnd - zStart) * 0.5; // half z length of the cone
    const double zPosition   = fabs(zEnd + zStart) * 0.5; // middle z position
    Material coreMaterial    = description.material("beam"); // always the same
    Material wallMaterial    = sectionMat;

    // this could mess up your geometry, so better check it
    if (not checkForSensibleGeometry(crossingAngle, crossType)){

      throw std::runtime_error( " Beampipe_o1_v01_geo.cpp : checkForSensibleGeometry() failed " ) ;
      //      return false;
    }
    const double rotateAngle = getCurrentAngle(crossingAngle, crossType); // for the placement at +z (better make it const now)
    const double mirrorAngle = M_PI - rotateAngle; // for the "mirrored" placement at -z
    // the "mirroring" in fact is done by a rotation of (almost) 180 degrees around the y-axis

    switch (crossType) {
    case ODH::kCenter:
    case ODH::kUpstream:
    case ODH::kDnstream: {
      // a volume on the z-axis, on the upstream branch, or on the downstream branch

      // absolute transformations for the placement in the world
      Transform3D transformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition), rotateAngle) );
      Transform3D transmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition), mirrorAngle) );

      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment tubeSolid( zHalf, 0, rOuterStart, 0, rOuterEnd , phi1, phi2);

      // tube consists of vacuum
      // place tube twice explicitely so we can attach surfaces to each one
      Volume tubeLog( volName, tubeSolid, coreMaterial ) ;
      Volume tubeLog2( volName, tubeSolid, coreMaterial ) ;

      // if inner and outer radii are equal, then omit the tube wall
      if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {

	// the wall solid: a tubular cone
	ConeSegment wallSolid( zHalf, rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, phi1, phi2);

	// the wall consists of the material given in the XML
	Volume wallLog ( volName + "_wall", wallSolid, wallMaterial);
	Volume wallLog2( volName + "_wall2", wallSolid, wallMaterial);

	if( crossType == ODH::kCenter ) {

	  // add surface for tracking ....
	  const bool isCylinder = ( rInnerStart == rInnerEnd );


	  if (isCylinder) {  // cylinder

	    Vector3D ocyl(  rInnerStart + thickness/2.  , 0. , 0. ) ;

	    VolCylinder cylSurf1( wallLog , SurfaceType( SurfaceType::Helper ) , 0.5*thickness  , 0.5*thickness , ocyl );
	    VolCylinder cylSurf2( wallLog2, SurfaceType( SurfaceType::Helper ) , 0.5*thickness  , 0.5*thickness , ocyl );

	    dd4hep::rec::volSurfaceList( tube )->push_back( cylSurf1 );
	    dd4hep::rec::volSurfaceList( tube )->push_back( cylSurf2 );

	  }else{   // cone

	    const double dr    = rInnerEnd - rInnerStart ;
	    const double theta = atan2( dr , 2.* zHalf ) ;

	    Vector3D ocon( rInnerStart + 0.5 * ( dr + thickness ), 0. , 0. );

	    Vector3D v( 1. , 0. , theta, Vector3D::spherical ) ;

	    VolCone conSurf1( wallLog , SurfaceType( SurfaceType::Helper ) , 0.5*thickness  , 0.5*thickness , v, ocon );
	    VolCone conSurf2( wallLog2, SurfaceType( SurfaceType::Helper ) , 0.5*thickness  , 0.5*thickness , v, ocon );

	    dd4hep::rec::volSurfaceList( tube )->push_back( conSurf1 );
	    dd4hep::rec::volSurfaceList( tube )->push_back( conSurf2 );

	  }

	  if( rInnerStart < min_radius ) min_radius = rInnerStart ;
	  if( rOuterStart < min_radius ) min_radius = rOuterStart ;
	}

	wallLog.setVisAttributes(description, "TubeVis");
	wallLog2.setVisAttributes(description, "TubeVis");
	tubeLog.setVisAttributes(description, "VacVis");
	tubeLog2.setVisAttributes(description, "VacVis");

	if (nocore){
	  
	  //placement of the wall only
	  envelope.placeVolume( wallLog,  transformer );
	  envelope.placeVolume( wallLog2,  transmirror );
	  
	}else{
	  
	  // placement of the tube in the world, both at +z and -z
	  envelope.placeVolume( tubeLog,  transformer );
	  envelope.placeVolume( tubeLog2,  transmirror );
	  
	  // placement as a daughter volume of the tube, will appear in both placements of the tube
	  tubeLog.placeVolume( wallLog,  Transform3D() );
	  tubeLog2.placeVolume( wallLog2,  Transform3D() );
	}
	
      }
    }
      break;

    case ODH::kPunchedCenter: {
      // a volume on the z-axis with one or two inner holes
      // (implemented as a cone from which tubes are punched out)

      const double rUpstreamPunch = rInnerStart; // just alias names denoting what is meant here
      const double rDnstreamPunch = rInnerEnd; // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D upstreamTransformer(RotationY(-crossingAngle), Position(zPosition * tan(-crossingAngle), 0, 0));
      Transform3D dnstreamTransformer(RotationY(+crossingAngle), Position(zPosition * tan(+crossingAngle), 0, 0));

      // absolute transformations for the final placement in the world (angles always equal zero and 180 deg)
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment tubeSolid( zHalf, 0, rOuterStart, 0, rOuterEnd, phi1, phi2);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", tubeSolid, coreMaterial );
      Volume tubeLog1( volName + "_1", tubeSolid, coreMaterial );

      // the wall solid and placeholders for possible SubtractionSolids
      ConeSegment wholeSolid(  zHalf, 0, rOuterStart, 0, rOuterEnd, phi1, phi2);

      Solid tmpSolid0, tmpSolid1, wallSolid0, wallSolid1;

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x

      if ( rUpstreamPunch > 1e-6 ) { // do we need a hole on the upstream branch?
	Tube upstreamPunch( 0, rUpstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
	tmpSolid0 = SubtractionSolid( wholeSolid, upstreamPunch, upstreamTransformer);
	tmpSolid1 = SubtractionSolid( wholeSolid, upstreamPunch, dnstreamTransformer); // [sic]
      } else { // dont't do anything, just pass on the unmodified shape
	tmpSolid0 = wholeSolid;
	tmpSolid1 = wholeSolid;
      }

      if (rDnstreamPunch > 1e-6 ) { // do we need a hole on the downstream branch?
	Tube dnstreamPunch( 0, rDnstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
	wallSolid0 = SubtractionSolid( tmpSolid0, dnstreamPunch, dnstreamTransformer);
	wallSolid1 = SubtractionSolid( tmpSolid1, dnstreamPunch, upstreamTransformer); // [sic]
      } else { // dont't do anything, just pass on the unmodified shape
	wallSolid0 = tmpSolid0;
	wallSolid1 = tmpSolid1;
      }

      // the wall consists of the material given in the XML
      Volume wallLog0( volName + "_wall_0", wallSolid0, wallMaterial );
      Volume wallLog1( volName + "_wall_1", wallSolid1, wallMaterial );

      wallLog0.setVisAttributes(description, "TubeVis");
      wallLog1.setVisAttributes(description, "TubeVis");
      tubeLog0.setVisAttributes(description, "VacVis");
      tubeLog1.setVisAttributes(description, "VacVis");

      if (nocore){
	
	// placement of the wall only
	envelope.placeVolume( wallLog0, placementTransformer );
	envelope.placeVolume( wallLog1, placementTransmirror );
	
      }else{
	
	// placement of the tube in the world, both at +z and -z
	envelope.placeVolume( tubeLog0, placementTransformer );
	envelope.placeVolume( tubeLog1, placementTransmirror );
	
	// placement as a daughter volumes of the tube
	tubeLog0.placeVolume( wallLog0, Position() );
	tubeLog1.placeVolume( wallLog1, Position() );
      }

      break;
    }

    case ODH::kPunchedUpstream:
    case ODH::kPunchedDnstream: {
      // a volume on the upstream or downstream branch with two inner holes
      // (implemented as a cone from which another tube is punched out)

      const double rCenterPunch = (crossType == ODH::kPunchedUpstream) ? (rInnerStart) : (rInnerEnd); // just alias names denoting what is meant here
      const double rOffsetPunch = (crossType == ODH::kPunchedDnstream) ? (rInnerStart) : (rInnerEnd); // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D punchTransformer(RotationY(-2 * rotateAngle), Position(zPosition * tan(-2 * rotateAngle), 0, 0));
      Transform3D punchTransmirror(RotationY(+2 * rotateAngle), Position(zPosition * tan(+2 * rotateAngle), 0, 0));

      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment tubeSolid( zHalf, 0, rOuterStart, 0, rOuterEnd, phi1, phi2);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", tubeSolid, coreMaterial );
      Volume tubeLog1( volName + "_1", tubeSolid, coreMaterial );

      // the wall solid and the piece (only a tube, for the moment) which will be punched out
      ConeSegment wholeSolid( zHalf, rCenterPunch , rOuterStart, rCenterPunch, rOuterEnd, phi1, phi2);

      Tube punchSolid( 0, rOffsetPunch, 5 * zHalf, phi1, phi2); // a bit longer

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
      SubtractionSolid wallSolid0( wholeSolid, punchSolid, punchTransformer);
      SubtractionSolid wallSolid1( wholeSolid, punchSolid, punchTransmirror);

      // the wall consists of the material given in the database
      Volume wallLog0( volName + "_wall_0", wallSolid0, wallMaterial );
      Volume wallLog1( volName + "_wall_1", wallSolid1, wallMaterial );

      wallLog0.setVisAttributes(description, "TubeVis");
      wallLog1.setVisAttributes(description, "TubeVis");

      tubeLog0.setVisAttributes(description, "VacVis");
      tubeLog1.setVisAttributes(description, "VacVis");


      if (nocore){

	// placement of the wall only
	envelope.placeVolume( wallLog0, placementTransformer );
	envelope.placeVolume( wallLog1, placementTransmirror );

      }else{

	// placement of the tube in the world, both at +z and -z
	envelope.placeVolume( tubeLog0, placementTransformer );
	envelope.placeVolume( tubeLog1, placementTransmirror );

	// placement as a daughter volumes of the tube
	tubeLog0.placeVolume( wallLog0 , Position() );
	tubeLog1.placeVolume( wallLog1 , Position() );
      }

      break;
    }

    case ODH::kUpstreamClippedFront:
    case ODH::kDnstreamClippedFront:
    case ODH::kUpstreamSlicedFront:
    case ODH::kDnstreamSlicedFront: {
      // a volume on the upstream or donwstream branch, but with the front face parallel to the xy-plane
      // or to a piece tilted in the other direction ("sliced" like a salami with 2 * rotateAngle)
      // (implemented as a slightly longer cone from which the end is clipped off)

      // the volume which will be used for clipping: a solid tube
      const double clipSize = rOuterStart; // the right order of magnitude for the clipping volume (alias name)
      Tube clipSolid( 0, 2 * clipSize, clipSize, phi1, phi2); // should be large enough

      // relative transformations for the composition of the SubtractionVolumes
      const double clipAngle = (crossType == ODH::kUpstreamClippedFront || crossType == ODH::kDnstreamClippedFront) ? (rotateAngle) : (2 * rotateAngle);
      const double clipShift = (zStart - clipSize) / cos(clipAngle) - (zPosition - clipSize / 2); // question: why is this correct?
      Transform3D clipTransformer(RotationY(-clipAngle), Position(0, 0, clipShift));
      Transform3D clipTransmirror(RotationY(+clipAngle), Position(0, 0, clipShift));

      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition - clipSize / 2) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition - clipSize / 2) , mirrorAngle) );

      // solid for the tube (including vacuum and wall): a solid cone

      ConeSegment wholeSolid(  zHalf + clipSize / 2, 0, rOuterStart, 0, rOuterEnd,  phi1, phi2); // a bit longer

      // clip away the protruding end
      SubtractionSolid tubeSolid0( wholeSolid, clipSolid, clipTransformer);
      SubtractionSolid tubeSolid1( wholeSolid, clipSolid, clipTransmirror);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", tubeSolid0, coreMaterial );
      Volume tubeLog1( volName + "_1", tubeSolid1, coreMaterial );

      if (nocore==false){
	// placement of the tube in the world, both at +z and -z
	envelope.placeVolume( tubeLog0, placementTransformer );
	envelope.placeVolume( tubeLog1, placementTransmirror );
      }

      if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
	// the wall solid: a tubular cone
	ConeSegment wallWholeSolid(  zHalf + clipSize / 2, rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, phi1, phi2); // a bit longer

	// clip away the protruding end
	SubtractionSolid wallSolid0( wallWholeSolid, clipSolid, clipTransformer);
	SubtractionSolid wallSolid1( wallWholeSolid, clipSolid, clipTransmirror);

	// the wall consists of the material given in the database
	Volume wallLog0( volName + "_wall_0", wallSolid0, wallMaterial );
	Volume wallLog1( volName + "_wall_1", wallSolid1, wallMaterial );

	wallLog0.setVisAttributes(description, "TubeVis");
	wallLog1.setVisAttributes(description, "TubeVis");

	tubeLog0.setVisAttributes(description, "VacVis");
	tubeLog1.setVisAttributes(description, "VacVis");
	if (nocore==false){
	  // placement as a daughter volumes of the tube
	  tubeLog0.placeVolume( wallLog0, Position() );
	  tubeLog1.placeVolume( wallLog1, Position() );
	}else{    // placement of the wall only
	  envelope.placeVolume( wallLog0, placementTransformer );
	  envelope.placeVolume( wallLog1, placementTransmirror );
	}
      }
    }
      break;

    case ODH::kUpstreamClippedRear:
    case ODH::kDnstreamClippedRear:
    case ODH::kUpstreamSlicedRear:
    case ODH::kDnstreamSlicedRear: {
      // a volume on the upstream or donwstream branch, but with the rear face parallel to the xy-plane
      // or to a piece tilted in the other direction ("sliced" like a salami with 2 * rotateAngle)
      // (implemented as a slightly longer cone from which the end is clipped off)

      // the volume which will be used for clipping: a solid tube
      const double clipSize = rOuterEnd; // the right order of magnitude for the clipping volume (alias name)
      Tube clipSolid( 0, 2 * clipSize, clipSize, phi1, phi2); // should be large enough

      // relative transformations for the composition of the SubtractionVolumes
      const double clipAngle = (crossType == ODH::kUpstreamClippedRear || crossType == ODH::kDnstreamClippedRear) ? (rotateAngle) : (2 * rotateAngle);
      const double clipShift = (zEnd + clipSize) / cos(clipAngle) - (zPosition + clipSize / 2); // question: why is this correct?
      Transform3D clipTransformer(RotationY(-clipAngle), Position(0, 0, clipShift));
      Transform3D clipTransmirror(RotationY(+clipAngle), Position(0, 0, clipShift));

      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition + clipSize / 2) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition + clipSize / 2) , mirrorAngle) );

      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment wholeSolid( 0, rOuterStart, 0, rOuterEnd, zHalf + clipSize / 2, phi1, phi2); // a bit longer

      // clip away the protruding end
      SubtractionSolid tubeSolid0( wholeSolid, clipSolid, clipTransformer);
      SubtractionSolid tubeSolid1( wholeSolid, clipSolid, clipTransmirror);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", tubeSolid0, coreMaterial );
      Volume tubeLog1( volName + "_1", tubeSolid1, coreMaterial );

      if (nocore==false){
	// placement of the tube in the world, both at +z and -z
	envelope.placeVolume( tubeLog0, placementTransformer );
	envelope.placeVolume( tubeLog1, placementTransmirror );
      }

      if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
	// the wall solid: a tubular cone
	ConeSegment wallWholeSolid( rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, zHalf + clipSize / 2, phi1, phi2); // a bit longer

	// clip away the protruding end
	SubtractionSolid wallSolid0( wallWholeSolid, clipSolid, clipTransformer);
	SubtractionSolid wallSolid1( wallWholeSolid, clipSolid, clipTransmirror);

	// the wall consists of the material given in the database
	Volume wallLog0( volName + "_wall_0", wallSolid0, wallMaterial );
	Volume wallLog1( volName + "_wall_1", wallSolid1, wallMaterial );

	wallLog0.setVisAttributes(description, "TubeVis");
	wallLog1.setVisAttributes(description, "TubeVis");

	tubeLog0.setVisAttributes(description, "VacVis");
	tubeLog1.setVisAttributes(description, "VacVis");

	if (nocore==false){
	// placement as a daughter volumes of the tube
	  tubeLog0.placeVolume( wallLog0, Transform3D() );
	  tubeLog1.placeVolume( wallLog1, Transform3D() );
	}else{   // placement of the wall only
	  envelope.placeVolume( wallLog0, placementTransformer );
	  envelope.placeVolume( wallLog1, placementTransmirror );
	}
      }
      break;
    }

    case ODH::kUpstreamClippedBoth:
    case ODH::kDnstreamClippedBoth:
    case ODH::kUpstreamSlicedBoth:
    case ODH::kDnstreamSlicedBoth: {
      // a volume on the upstream or donwstream branch, but with both faces parallel to the xy-plane
      // or to a piece tilted in the other direction ("sliced" like a salami with 2 * rotateAngle)
      // (implemented as a slightly longer cone from which the end is clipped off)

      // the volume which will be used for clipping: a solid tube
      const double clipSize = rOuterEnd; // the right order of magnitude for the clipping volume (alias name)
      Tube clipSolid( 0, 2 * clipSize, clipSize, phi1, phi2); // should be large enough

      // relative transformations for the composition of the SubtractionVolumes
      const double clipAngle = (crossType == ODH::kUpstreamClippedBoth || crossType == ODH::kDnstreamClippedBoth) ? (rotateAngle) : (2 * rotateAngle);
      const double clipShiftFrnt = (zStart - clipSize) / cos(clipAngle) - zPosition;
      const double clipShiftRear = (zEnd   + clipSize) / cos(clipAngle) - zPosition;
      Transform3D clipTransformerFrnt(RotationY(-clipAngle), Position(0, 0, clipShiftFrnt));
      Transform3D clipTransformerRear(RotationY(-clipAngle), Position(0, 0, clipShiftRear));
      Transform3D clipTransmirrorFrnt(RotationY(+clipAngle), Position(0, 0, clipShiftFrnt));
      Transform3D clipTransmirrorRear(RotationY(+clipAngle), Position(0, 0, clipShiftRear));

      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment wholeSolid( 0, rOuterStart, 0, rOuterEnd, zHalf + clipSize, phi1, phi2); // a bit longer

      // clip away the protruding ends
      SubtractionSolid tmpSolid0 ( wholeSolid, clipSolid, clipTransformerFrnt);
      SubtractionSolid tmpSolid1 ( wholeSolid, clipSolid, clipTransmirrorFrnt);
      SubtractionSolid tubeSolid0( tmpSolid0,  clipSolid, clipTransformerRear);
      SubtractionSolid tubeSolid1( tmpSolid1,  clipSolid, clipTransmirrorRear);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", tubeSolid0, coreMaterial );
      Volume tubeLog1( volName + "_1", tubeSolid1, coreMaterial );

      if (nocore==false){
	// placement of the tube in the world, both at +z and -z
	envelope.placeVolume( tubeLog0, placementTransformer );
	envelope.placeVolume( tubeLog1, placementTransmirror );
      }

      if (rInnerStart != rOuterStart || rInnerEnd != rOuterEnd) {
	// the wall solid: a tubular cone
	ConeSegment wallWholeSolid( rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, zHalf + clipSize, phi1, phi2); // a bit longer

	// clip away the protruding ends
	SubtractionSolid wallTmpSolid0( wallWholeSolid, clipSolid, clipTransformerFrnt);
	SubtractionSolid wallTmpSolid1( wallWholeSolid, clipSolid, clipTransmirrorFrnt);
	SubtractionSolid wallSolid0   ( wallTmpSolid0,  clipSolid, clipTransformerRear);
	SubtractionSolid wallSolid1   ( wallTmpSolid1,  clipSolid, clipTransmirrorRear);

	// the wall consists of the material given in the database
	Volume wallLog0(volName + "_wall_0", wallSolid0, wallMaterial );
	Volume wallLog1(volName + "_wall_1", wallSolid1, wallMaterial );

	wallLog0.setVisAttributes(description, "TubeVis");
	wallLog1.setVisAttributes(description, "TubeVis");

	tubeLog0.setVisAttributes(description, "VacVis");
	tubeLog1.setVisAttributes(description, "VacVis");

	if (nocore==false){
	  // placement as a daughter volumes of the tube
	  tubeLog0.placeVolume( wallLog0, Transform3D() );
	  tubeLog1.placeVolume( wallLog1, Transform3D() );
	}else{  // placement of the wall only
	  envelope.placeVolume( wallLog0, placementTransformer );
	  envelope.placeVolume( wallLog1, placementTransmirror );
	}
      }
      break;
    }
    default: {
      throw std::runtime_error( " Beampipe_o1_v01_geo.cpp : fatal failure !! ??  " ) ;

      //      return false; // fatal failure
    }

    }//end switch
  }//for all xmlSections

  //######################################################################################################################################################################


  // add a surface just inside the beampipe for tracking:
  if (nocore==false){
    Vector3D oIPCyl( (min_radius-1.e-3)  , 0. , 0.  ) ;
    SimpleCylinder ipCylSurf( envelope , SurfaceType( SurfaceType::Helper ) , 1.e-5  , 1e-5 , oIPCyl ) ;
    // the length does not really matter here as long as it is long enough for all tracks ...
    ipCylSurf->setHalfLength(  100*units::cm ) ;
    dd4hep::rec::volSurfaceList( tube )->push_back( ipCylSurf ) ;
  }

  tube.addExtension< dd4hep::rec::ConicalSupportData >( beampipeData ) ;

  //--------------------------------------

  tube.setVisAttributes( description, xmlBeampipe.visStr(), envelope );

  // // tube.setPlacement(pv);

  return tube;
}
DECLARE_DETELEMENT(DD4hep_Beampipe_o1_v01,create_element)
