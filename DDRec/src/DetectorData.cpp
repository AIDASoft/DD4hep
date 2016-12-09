#include "DDRec/DetectorData.h"

namespace DD4hep {
  namespace DDRec {


    std::ostream& operator<<( std::ostream& io , const FixedPadSizeTPCData& d ){
      io <<  " --FixedPadSizeTPCData: "  << std::scientific << std::endl ; 
      io <<  "   zHalf : "              <<  d.zHalf  << std::endl ; 
      io <<  "   rMin : "               <<  d.rMin << std::endl ; 
      io <<  "   rMax : "               <<  d.rMax << std::endl ; 
      io <<  "   driftLength : "        <<  d.driftLength << std::endl ; 
      io <<  "   rMinReadout : "        <<  d.rMinReadout << std::endl ; 
      io <<  "   rMaxReadout : "        <<  d.rMaxReadout << std::endl ; 
      io <<  "   innerWallThickness : " <<  d.innerWallThickness << std::endl ; 
      io <<  "   outerWallThickness : " <<  d.outerWallThickness << std::endl ; 
      io <<  "   padHeight : "          <<  d.padHeight  << std::endl ; 
      io <<  "   padWidth : "           <<  d.padWidth  << std::endl ; 
      io <<  "   maxRow : "             <<  d.maxRow << std::endl ; 
      io <<  "   padGap : "             <<  d.padGap  << std::endl ; 
      //      io <<  "   : " <<  d.  << std::endl ; 
      return io ;
    }
    


    std::ostream& operator<<( std::ostream& io , const ZPlanarData& d ) {

      io <<  " -- ZPlanarData: "  << std::scientific << std::endl ; 
      io <<  " zHalfShell  : " <<  d.zHalfShell  << std::endl ; 
      io <<  " gapShell  : " <<  d.gapShell  << std::endl ; 
      io <<  " rInnerShell  : " <<  d.rInnerShell  << std::endl ; 
      io <<  " rOuterShell  : " <<  d.rOuterShell  << std::endl ; 
      io <<  " widthStrip  : " <<  d.widthStrip  << std::endl ; 
      io <<  " lengthStrip  : " <<  d.lengthStrip  << std::endl ; 
      io <<  " pitchStrip  : " <<  d.pitchStrip  << std::endl ; 
      io <<  " angleStrip  : " <<  d.angleStrip  << std::endl ; 

      std::vector<ZPlanarData::LayerLayout> layers = d.layers ;

      io <<  " Layers : " << std::endl 
	 <<  "  nLadder phi0     nSensors    lengthSensor distSupport  thickSupport  offsetSupport widthSupport zHalfSupport distSense   thickSense   offsetSense   widthSense  zHalfSense" << std::endl ;

      //"ladderNumber phi0 sensorsPerLadder lengthSensor distanceSupport thicknessSupport offsetSupport widthSupport zHalfSupport distanceSensitive thicknessSensitive offsetSensitive widthSensitive zHalfSensitive"  <<  std::endl ; 

      for(unsigned i=0,N=layers.size() ; i<N ; ++i){

	ZPlanarData::LayerLayout l = layers[i] ;

	io << " " << l.ladderNumber
	   << " " << l.phi0
	   << " " << l.sensorsPerLadder
	   << " " << l.lengthSensor
	   << " " << l.distanceSupport
	   << " " << l.thicknessSupport
	   << " " << l.offsetSupport
	   << " " << l.widthSupport
	   << " " << l.zHalfSupport
	   << " " << l.distanceSensitive
	   << " " << l.thicknessSensitive
	   << " " << l.offsetSensitive
	   << " " << l.widthSensitive
	   << " " << l.zHalfSensitive
	   << std::endl ;
      }
   
      io <<  "  mapNeighbours.size() : " <<  d.mapNeighbours.size()  << std::endl ; 
   
      return io ;
    }

    std::ostream& operator<<( std::ostream& io , const ZDiskPetalsData& d ) {

      io <<  " -- ZDiskPetalsData: "  << std::scientific << std::endl ; 
      io <<  "  widthStrip : " <<  d.widthStrip  << std::endl ; 
      io <<  "  lengthStrip : " <<  d.lengthStrip  << std::endl ; 
      io <<  "  pitchStrip : " <<  d.pitchStrip  << std::endl ; 
      io <<  "  angleStrip : " <<  d.angleStrip  << std::endl ; 

      std::vector<ZDiskPetalsData::LayerLayout> layers = d.layers ;

      io <<  " Layers : " << std::endl 
	 <<  " petHalfAngle  alphaPetal  zPosition    nP n d p    phi0        zOffsetSup  distSupport  thickSupport widthInSup  widthOutSup   lengthSup   zOffsetSense  distSense    thickSense  widthInSense widthOutSense lenSense" << std::endl ;

      //"petalHalfAngle alphaPetal zPosition petalNumber sensorsPerPetal DoubleSided Pixel phi0 zOffsetSupport distanceSupport thicknessSupport widthInnerSupport widthOuterSupport lengthSupport zOffsetSensitive distanceSensitive thicknessSensitive widthInnerSensitive widthOuterSensitive lengthSensitive" << std::endl ;

      for(unsigned i=0,N=layers.size() ; i<N ; ++i){

	ZDiskPetalsData::LayerLayout l = layers[i] ;

	io << " " << l.petalHalfAngle
	   << " " << l.alphaPetal
	   << " " << l.zPosition
	   << " " << l.petalNumber
	   << " " << l.sensorsPerPetal
	   << " " << l.typeFlags[ ZDiskPetalsData::SensorType::DoubleSided ]
	   << " " << l.typeFlags[ ZDiskPetalsData::SensorType::Pixel ]
	   << " " << l.phi0
	   << " " << l.zOffsetSupport
	   << " " << l.distanceSupport
	   << " " << l.thicknessSupport
	   << " " << l.widthInnerSupport
	   << " " << l.widthOuterSupport
	   << " " << l.lengthSupport 
	   << " " << l.zOffsetSensitive
	   << " " << l.distanceSensitive
	   << " " << l.thicknessSensitive
	   << " " << l.widthInnerSensitive
	   << " " << l.widthOuterSensitive
	   << " " << l.lengthSensitive
	   << std::endl ;
      }

      io <<  " nP:petalNumber  n:sensorsPerPetal  d:DoubleSided  p: Pixel "  << std::endl ;

      io <<  "  mapNeighbours.size() : " <<  d.mapNeighbours.size()  << std::endl ; 

      return io ;
    }

    
    std::ostream& operator<<( std::ostream& io , const ConicalSupportData& d ) {

      io <<  " -- ConicalSupportData : "  << std::scientific << std::endl ; 
      io <<  "  isSymmetricInZ : " <<  d.isSymmetricInZ  << std::endl ; 
      
      std::vector<ConicalSupportData::Section> layers = d.sections ;

      io <<  " Sections : " << std::endl 
	 <<  "rInner rOuter zPos " << std::endl ;
      
      for(unsigned i=0,N=layers.size() ; i<N ; ++i){
	
	ConicalSupportData::Section l = layers[i] ;
	
	io << " " << l.rInner
	   << " " << l.rOuter
	   << " " << l.zPos
	   << std::endl ;
      }
      return io ;
    }

    
    std::ostream& operator<<( std::ostream& io , const LayeredCalorimeterData& d ) {

      io <<  " -- LayeredCalorimeterData : "  << std::scientific << std::endl ; 
      io <<  "  LayoutType : " <<  ( d.layoutType == LayeredCalorimeterStruct::BarrelLayout ?
				     "BarrelLayout" : "EndcapLayout" ) << std::endl ; 
      io <<  "  extent[ rmin, rmax, zmin, zmax ] : " 
	 <<  d.extent[0] << " "  << d.extent[1] << " "  << d.extent[2] << " " << d.extent[3]  << std::endl ; 
      io <<  " outer_symmetry : " <<  d.outer_symmetry  << std::endl ; 
      io <<  " inner_symmetry : " <<  d.inner_symmetry  << std::endl ; 
      io <<  " outer_phi0 : " <<  d.outer_phi0  << std::endl ; 
      io <<  " inner_phi0 : " <<  d.inner_phi0  << std::endl ; 
      io <<  " gap1 : " <<  d.gap1  << std::endl ; 
      io <<  " gap2 : " <<  d.gap2  << std::endl ; 
      
      std::vector<LayeredCalorimeterData::Layer> layers = d.layers ;

      io <<  " Layers : " << std::endl 
	 <<  "  distance      inner_nX0   outer_nX0    inner_nInt    outer_nInt  inner_thick outer_thick   sense_thick" 
	 << std::endl ;
      //"distance inner_nX0   outer_nX0 inner_nLambda outer_nLambda inner_thick outer_thick sensitive_thick" << std::endl ;

      for(unsigned i=0,N=layers.size() ; i<N ; ++i){

	LayeredCalorimeterData::Layer l = layers[i] ;

	io << " " << l.distance
	   << " " << l.inner_nRadiationLengths
	   << " " << l.outer_nRadiationLengths
	   << " " << l.inner_nInteractionLengths 
	   << " " << l.outer_nInteractionLengths
	   << " " << l.inner_thickness
	   << " " << l.outer_thickness
	   << " " << l.sensitive_thickness
	   << std::endl ;
      }

      return io ;
    }


  } // namespace
}
