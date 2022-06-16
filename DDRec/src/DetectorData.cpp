//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#include "DDRec/DetectorData.h"

#include <boost/io/ios_state.hpp>

namespace dd4hep {
  namespace rec {


    std::ostream& operator<<( std::ostream& io , const FixedPadSizeTPCData& d ){
      boost::io::ios_base_all_saver ifs(io);

      io <<  " --FixedPadSizeTPCData: "  << std::scientific << std::endl ; 
      io <<  "   zHalf : "              <<  d.zHalf  << std::endl ; 
      io <<  "   rMin : "               <<  d.rMin << std::endl ; 
      io <<  "   rMax : "               <<  d.rMax << std::endl ; 
      io <<  "   driftLength : "        <<  d.driftLength << std::endl ;
      io <<  "   zMinReadout : "        <<  d.zMinReadout << std::endl ;
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
      boost::io::ios_base_all_saver ifs(io);

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
   
      return io ;
    }

    std::ostream& operator<<( std::ostream& io , const ZDiskPetalsData& d ) {
      boost::io::ios_base_all_saver ifs(io);

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

      return io ;
    }

    
    std::ostream& operator<<( std::ostream& io , const ConicalSupportData& d ) {
      boost::io::ios_base_all_saver ifs(io);

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
      boost::io::ios_base_all_saver ifs(io);
      
      io <<  " -- LayeredCalorimeterData : "  << std::scientific << std::endl ; 
      switch (d.layoutType){
      case LayeredCalorimeterStruct::BarrelLayout: { io <<  " LayoutType : BarrelLayout "  << std::endl ;  io <<  "  extent[ rmin, rmax, zmin, zmax ] : " 
							<<  d.extent[0] << " "  << d.extent[1] << " "  << d.extent[2] << " " << d.extent[3]  << std::endl ;  break; }
      case LayeredCalorimeterStruct::EndcapLayout: { io <<  " LayoutType : EndcapLayout "  << std::endl ;  io <<  "  extent[ rmin, rmax, zmin, zmax ] : " 
							<<  d.extent[0] << " "  << d.extent[1] << " "  << d.extent[2] << " " << d.extent[3]  << std::endl ;  break; }
      case LayeredCalorimeterStruct::ConicalLayout: { io <<  " LayoutType : ConicalLayout "  << std::endl ; io <<  "  extent[ rmin, rmax, zmin, zmax, rEndMin, rEndMax ] : " 	                                             <<  d.extent[0] << " "  << d.extent[1] << " "  << d.extent[2] << " " << d.extent[3] << " "  << d.extent[4] << " " << d.extent[5] << std::endl ;   break;}
      }
      io <<  " outer_symmetry : " <<  d.outer_symmetry  << std::endl ; 
      io <<  " inner_symmetry : " <<  d.inner_symmetry  << std::endl ; 
      io <<  " outer_phi0 : " <<  d.outer_phi0  << std::endl ; 
      io <<  " inner_phi0 : " <<  d.inner_phi0  << std::endl ; 
      io <<  " gap1 : " <<  d.gap1  << std::endl ; 
      io <<  " gap2 : " <<  d.gap2  << std::endl ; 
      
      std::vector<LayeredCalorimeterData::Layer> layers = d.layers ;

      io <<  " Layers : " << std::endl 
	 <<  "  distance      inner_nX0   outer_nX0    inner_nInt    outer_nInt  inner_thick outer_thick   sense_thick  cellSize0  cellSize1" 
	 << std::endl ;

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
	   << " " << l.cellSize0
	   << " " << l.cellSize1
	   << std::endl ;
      }

      return io ;
    }



    std::ostream& operator<<( std::ostream& io , const NeighbourSurfacesData& d ){
      boost::io::ios_base_all_saver ifs(io);

      io <<  " --NeighbourSurfacesData: "  << std::scientific << std::endl ; 
      io <<  "   sameLayer.size() : " << d.sameLayer.size() << std::endl ; 
      return io ;
    }


    std::ostream& operator<<(std::ostream& io, const DoubleParameters& d) {
      boost::io::ios_base_all_saver ifs(io);
      io <<  " --DoubleParameters: "  << std::scientific << std::endl ;
      for (auto const& thePair: d.doubleParameters) {
        io <<  "    "
           << std::setw(40) << thePair.first << ":"
           << std::setw(14) << thePair.second
           << std::endl;
      }
      return io ;
    }

    namespace { 
      struct visitor_impl {
        std::ostream& m_io;
        void operator()(std::string value) const {
          m_io << ": str = " << value;
        }

        void operator()(int value) const {
          m_io << ": int = " << value;
        }

        void operator()(double value) const {
          m_io << ": double = " << value;
        }

        void operator()(bool value) const {
          m_io << ": bool = " << (value ? "true" : "false");
        }

      };
    }

    std::ostream& operator<<(std::ostream& io, const VariantParameters& v) {
      boost::io::ios_base_all_saver ifs(io);
      io <<  " --VariantParameters: "  << std::scientific << std::endl ;

      visitor_impl visitor{io};

      for (auto const& thePair: v.variantParameters) {
        io <<  "    "
           << std::setw(40) << thePair.first << "";
        boost::apply_visitor(visitor, thePair.second);
        io << std::endl;
      }
      return io ;
    }
 

  } // namespace
}
