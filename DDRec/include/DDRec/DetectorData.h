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
#ifndef DDREC_DETECTORDATA_H
#define DDREC_DETECTORDATA_H

#include <map>
#include <bitset>
#include <ostream>

#include <boost/variant.hpp>

#include "DD4hep/DetElement.h"

namespace dd4hep {
  namespace rec {
    
    /** Wrapper class for adding structs or pods as extensions to DetElements.
     *  Provides default implementations of the c'tors required by the extension mechamism.
     *  Structs (or classes) can be defined w/o any c'tors relying on the default ones 
     *  created by the compiler.
     * 
     * @author F.Gaede, CERN/DESY
     * @date Sep, 30 2014
     * @version $Id: $
     */
    template< class T>
    struct StructExtension : public T {
      StructExtension() : T()  { } 
      StructExtension(const StructExtension<T>& t) : T(t) {} 
      StructExtension(const T& t) : T(t) {} 
      StructExtension(const DetElement& d) : T( *d.extension<StructExtension<T> >() )  {}
      StructExtension(const StructExtension<T>& t, const DetElement&) : T(t) {}
      std::string toString(){
	std::stringstream s ;
	s << *this ;
	return s.str();
      }
    };



    /** Simple data structure with key parameters for
     *  reconstruction of a cylindrical TPC with a pad plane that
     *  is symmetrically populated with fixed sized pads.
     *  ( Can be used to instantiate a gear::TPCParameter object
     *    with a FixedPadSizeDiskLayout )
     * 
     * @author F.Gaede, CERN/DESY
     * @date Sep, 30 2014
     * @version $Id: $
     */
    struct FixedPadSizeTPCStruct{
      /// half length of the TPC 
      double zHalf ;
      /// inner radius of the TPC
      double rMin ;
      /// outer radius of the TPC
      double rMax ;
      /// driftLength in z (half length of active volume)
      double driftLength ;
      /// start z of active Volume (typically cathode half thickness)
      double zMinReadout ;
      /// inner r of active volume
      double rMinReadout ;
      /// outer r of active volume
      double rMaxReadout ;
      /// thickness of the inner wall (field cage)
      double innerWallThickness ;
      /// thickness of the outer wall (field cage)
      double outerWallThickness ;
      /// pad height of readout rows
      double padHeight ;
      /// fixed pad width ( at middle of row)
      double padWidth ;
      /// maximum number of rows
      double maxRow ;
      /// gap between pads
      double padGap ;
    };
    typedef StructExtension<FixedPadSizeTPCStruct> FixedPadSizeTPCData ;

    std::ostream& operator<<( std::ostream& io , const FixedPadSizeTPCData& d ) ;
    

    /** Simple data structure with key parameters for
     *  reconstruction of a planar silicon tracking detector
     *  with planes parallel to the z-axis.
     *  ( Can be used to instantiate a gear::ZPlanarParameters object )
     * 
     * @author F.Gaede, CERN/DESY
     * @date Oct, 15 2014
     * @version $Id: $
     */
    struct ZPlanarStruct{
      ///  The half length (z) of the support shell (w/o gap) - 0. if no shell exists.
      double zHalfShell ;
      ///  The length of the gap in mm (gap position at z=0).
      double  gapShell ;
      ///  The inner radius of the support shell.
      double rInnerShell ;
      ///  The outer radius of the support shell.
      double rOuterShell ;
      
      /// width of the strips (if applicable )
      double widthStrip ;
      /// length of the strips (if applicable )
      double lengthStrip ;
      /// strip pitch  (if applicable )
      double pitchStrip ;
      /// strip stereo angle  (if applicable )
      double angleStrip ;
      
      /**Internal helper struct for defining the layer layout. Layers are defined 
       * with a sensitive part and a support part.
       */      
      struct LayerLayout{

	// default c'tor with zero initialization
	LayerLayout() :
	  ladderNumber(0),
	  phi0(0), 
	  sensorsPerLadder(0),
	  lengthSensor(0),
	  distanceSupport(0),
	  thicknessSupport(0),
	  offsetSupport(0), 
	  widthSupport(0),
	  zHalfSupport(0),
	  distanceSensitive(0),
	  thicknessSensitive(0),
	  offsetSensitive(0), 
	  widthSensitive(0),
	  zHalfSensitive(0) {
	}


	/// The number of ladders in the layer.
	int ladderNumber ;

	/// Azimuthal angle of the (outward pointing) normal of the first ladder.
	double phi0 ; 

	/// number of sensor per ladder
	int sensorsPerLadder ;
	/// sensor length (if divided in sensors)
	double lengthSensor ;
	
	/// The distance of the ladder support from the origin (IP).
	double distanceSupport ;
	/// The thickness of the ladder support from the origin (IP).
	double thicknessSupport ;
	/// The offset of the ladder support, i.e. the shift in the direction of increasing phi, perpendicular to the ladders's normal.
	double offsetSupport ; 
	///The width of the ladder support.
	double widthSupport ;
	///The half length of the ladder support in z.
	double zHalfSupport ;

	/// The distance of the ladder sensitive from the origin (IP).
	double distanceSensitive ;
	/// The thickness of the ladder sensitive from the origin (IP).
	double thicknessSensitive ;
	/// The offset of the ladder sensitive, i.e. the shift in the direction of increasing phi, perpendicular to the ladders's normal.
	double offsetSensitive ; 
	///The width of the ladder sensitive.
	double widthSensitive ;
	///The half length of the ladder sensitive in z.
	double zHalfSensitive ;
      } ;

      std::vector<LayerLayout> layers ;

    } ;
    typedef StructExtension<ZPlanarStruct> ZPlanarData ;

    std::ostream& operator<<( std::ostream& io , const ZPlanarData& d ) ;



    /** Simple data structure with key parameters for
     *  reconstruction of a silicon tracking detector
     *  with disks (roughly orthogonal to the z-axis) built from petals.
     *  ( Can be used to instantiate a gear::FTDParameters object )
     * 
     * @author F.Gaede, CERN/DESY
     * @date Oct, 20 2014
     * @version $Id: $
     */
    struct ZDiskPetalsStruct{

      /// width of the strips (if applicable )
      double widthStrip ;
      /// length of the strips (if applicable )
      double lengthStrip ;
      /// strip pitch  (if applicable )
      double pitchStrip ;
      /// strip stereo angle  (if applicable )
      double angleStrip ;
      
      /// enum for encoding the sensor type in typeFlags
      struct SensorType{
	enum {
	  DoubleSided=0,
	  Pixel
	};
      } ;

      /** Internal helper struct for defining the layer layout. Layers are defined 
       *  with a sensitive part and a support part.
       */      
      struct LayerLayout{

	LayerLayout() :
	  petalHalfAngle(0),
	  alphaPetal(0),
	  zPosition(0),
	  petalNumber(0),
	  sensorsPerPetal(0),
	  typeFlags(0),
	  phi0(0), 
	  zOffsetSupport(0),
	  distanceSupport(0),
	  thicknessSupport(0),
	  widthInnerSupport(0),
	  widthOuterSupport(0),
	  lengthSupport(0),
	  zOffsetSensitive(0),
	  distanceSensitive(0),
	  thicknessSensitive(0),
	  widthInnerSensitive(0),
	  widthOuterSensitive(0),
	  lengthSensitive(0) {
	}
	
	/// half angle covered by petal 
	double  petalHalfAngle ;
	
	/** angle alpha by which the petal is rotated away from the plane 
	 *  that is orthogonal to the z-axis 
	 */
	double  alphaPetal ;
	
	/// z-position of layer ( z-position of middle axis ) 	
	double  zPosition ;

	/// The number of petals in the layer.
	int petalNumber ;

 	/// number of sensor per petal
	int sensorsPerPetal ;

	/// Bit flag describing sensor type - use enum SensorType to access the bits.
	std::bitset<32> typeFlags ;

	/// azimuthal angle of vector defined by the Z-axis to first petal x-positive, y-positive edge 
	double phi0 ; 

	/** z-offset of support petals from layer z-position - signed for first
	 *  petal, following petals have alternating sign 
	 */
	double  zOffsetSupport ;
	/// The distance of the petal support from the z-axis.
	double distanceSupport ;
	/// The thickness of the petal support.
	double thicknessSupport ;
	/// The inner width of the petal support.
	double widthInnerSupport ;
	/// The outer width of the petal support.
	double widthOuterSupport ;
	/// The radial length of the petal support.
	double lengthSupport ;

	/** z-offset of sensitive petals from layer z-position - signed for first
	 *  petal, following petals have alternating sign 
	 */
	double  zOffsetSensitive ;
	/// The distance of the petal sensitive from the z-axis.
	double distanceSensitive ;
	/// The thickness of the petal sensitive.
	double thicknessSensitive ;
	/// The inner width of the petal sensitive.
	double widthInnerSensitive ;
	/// The outer width of the petal sensitive.
	double widthOuterSensitive ;
	/// The radial length of the petal sensitive.
	double lengthSensitive ;
      } ;

      std::vector<LayerLayout> layers ;

    } ;
    typedef StructExtension<ZDiskPetalsStruct> ZDiskPetalsData ;

    std::ostream& operator<<( std::ostream& io , const ZDiskPetalsData& d ) ;

    /** Simple data structure defining a support
     *  structure built from consecutive conical
     *  sections. Could be used for example to 
     *  describe the central beam pipe as needed
     *  for track reconstruction.
     * 
     * @author F.Gaede, CERN/DESY
     * @date Oct, 29 2014
     * @version $Id: $
     */
    struct ConicalSupportStruct{

      /// if true the sections are repeated at negative z
      bool isSymmetricInZ ;

      struct Section{

	Section() :
	  rInner(0),
	  rOuter(0),
	  zPos(0){
	}

	/// inner r at start of section
	double rInner ;
	/// outer r at start of section
	double rOuter ;
	/// z position at start of section
	double zPos ;
      } ;

      /** The consecutive sections of the structure.
       *  The end of one sections is defined by the start
       *  the next.
       */
      std::vector<Section> sections ;	

    } ;

    typedef StructExtension<ConicalSupportStruct> ConicalSupportData ;

    std::ostream& operator<<( std::ostream& io , const ConicalSupportData& d ) ;


    /** Simple data structure defining a layered calorimeter layout for
     *  reconstruction. The implicit assumption is that the shape
     *  of the calorimter in the x-y-plane is a regular polygon.
     * 
     * @author F.Gaede, CERN/DESY
     * @date Nov, 05 2014
     * @version $Id: $
     */
    struct LayeredCalorimeterStruct{
      
      /// enum for encoding the sensor type in typeFlags
      enum LayoutType{
	BarrelLayout=0,
	EndcapLayout,
	ConicalLayout
      };

      /// type of layout: BarrelLayout or EndcapLayout
      LayoutType layoutType ;

      /** extent of the calorimeter in the r-z-plane [ rmin, rmax, zmin, zmax, rmin2, rmax2 ] in mm.
       * where rmin2, rmax2 are the radii at zmax for the ConicalLayout and not used else.
       */
      double extent[6] ;

      /** the order of the rotational symmetry at the outside, e.g.
       *  8 for an octagonal barrel calorimeter.
       */
      int outer_symmetry ;

      /** the order of the rotational symmetry at the inside, e.g.
       *  4 for an endcap with a rectangular cout out at the inside.
       */
      int inner_symmetry ;

      /** Angle between the normal to the first outer face of the 
       *  calorimeter and the x-axis, where the first face is defined
       *  to be the one with the smallest positve angle.
       *  Example:
       *  outer_phi0=0 corresponds to the first face beeing vertical and 
       *  thus the bottom face being parallel to the floor for a 
       *  symmetry that is a multiple of 4.
       */      
      double  outer_phi0  ;

      /** Same as outer_phi for the first inner face.
       */
      double  inner_phi0  ;
      

       /** Azimuthal angle of the first module in barrel layout
       *  DEPRECATED! PLEASE POPULATE INNER/OUTER PHI0 INSTEAD
       */
      double  phi0  ;
           
      
      /// Gap between modules(eg. stave gap) in the phi-direction
      double gap0;
      
      /// Gap between modules(eg. middle stave gap) in the z-direction
      double gap1;
      
      /// Gap between modules(reserved for future use) e.g in the r-direction
      double gap2;
      

      struct Layer {
	
	Layer() :
	  distance(0),
	  phi0(0),
	  absorberThickness(0),
	  inner_nRadiationLengths(0),
	  inner_nInteractionLengths(0),        
	  outer_nRadiationLengths(0),
	  outer_nInteractionLengths(0), 
	  inner_thickness(0),
	  outer_thickness(0),
	  sensitive_thickness(0),
	  cellSize0(0),
	  cellSize1(0) {
	}
	
	/// distance from Origin (or the z-axis) to the inner-most face of the layer
	double distance;
        
	/// phi0 of layer: potential rotation around normal to absorber plane, e.g. if layers are 'staggered' in phi in fwd. calos 
	double phi0 ;
	
        /// thickness of the absorber part of the layer. Consider using inner/outer_nRadiationLengths and inner/outer_nInteractionLengths
	double absorberThickness ;
        
        ///Absorber material in front of sensitive element in the layer, units of radiation lengths
        double inner_nRadiationLengths ;
        ///Absorber material in front of sensitive element in the layer, units of radiation lengths
        double inner_nInteractionLengths ;        
        
        ///Absorber material in behind of sensitive element in the layer, units of radiation lengths
        double outer_nRadiationLengths ;
        ///Absorber material in behind of sensitive element in the layer, units of radiation lengths
        double outer_nInteractionLengths ; 
        
        ///Distance between the innermost face of the layer (closest to IP) and the center of the sensitive element
        double inner_thickness;
        
        ///Distance between the center of the sensitive element and the outermost face of the layer
        double outer_thickness;
        
        ///Thickness of the sensitive element (e.g. scintillator)
        double sensitive_thickness;
        
        
        
	/// cell size along the first axis where first is either along the beam (BarrelLayout) or up (EndcapLayout) or the direction closest to that. 
	double cellSize0 ;
	/// second cell size, perpendicular to the first direction cellSize0 and the depth of the layers. 
	double cellSize1 ;
      } ;   

      std::vector<Layer> layers ;	
    } ;

    typedef StructExtension<LayeredCalorimeterStruct> LayeredCalorimeterData ;

    std::ostream& operator<<( std::ostream& io , const LayeredCalorimeterData& d ) ;




    /** Simple data strucuture that holds maps of ids of the nearest neighbour surfaces in the same, next and previous layers
     *  of a tracking detector. Could be used as extension object for tracking DetectorElements and used in 
     *  pattern recognition. The exact details of the neighbouring criteria depend on the algorithm that is used.
     *
     * @author F.Gaede, DESY, R. Simoniello, CERN
     * @date Dec, 15 2016
     */
    struct NeighbourSurfacesStruct {

      /// map of all neighbours in the same layer
      std::map<dd4hep::long64 , std::vector<dd4hep::long64 > > sameLayer ; 

      /// map of all neighbours in the previous layer
      std::map<dd4hep::long64 , std::vector<dd4hep::long64 > > prevLayer ;

      /// map of all neighbours in the next layer
      std::map<dd4hep::long64 , std::vector<dd4hep::long64 > > nextLayer ;

    } ;
    typedef StructExtension<NeighbourSurfacesStruct> NeighbourSurfacesData ;

    std::ostream& operator<<( std::ostream& io , const NeighbourSurfacesData& d ) ;

    struct MapStringDoubleStruct {
      std::map<std::string, double> doubleParameters{};
    };
    using DoubleParameters = StructExtension<MapStringDoubleStruct>;
    std::ostream& operator<<( std::ostream& io , const DoubleParameters& d ) ;

    /** Data structure that holds a map of string keys to a typesafe union of double, int, string or bool.
     *  It can be used as an extension object for detector elements to attach arbitrary information 
     *  @author P.Gessinger, CERN
     *  @date May, 25 2022
     */
    struct MapStringVariantStruct {
      std::map<std::string, boost::variant<double, int, std::string, bool>> variantParameters{};

      template <typename T>
      const T& get(const std::string& key) const {
        auto it = variantParameters.find(key);
        if(it == variantParameters.end()) {
          throw std::runtime_error{"Key "+key+" not found"};
        }
        return boost::get<T>(it->second);
      }

      template <typename T>
      T& get(const std::string& key) {
        auto it = variantParameters.find(key);
        if(it == variantParameters.end()) {
          throw std::runtime_error{"Key "+key+" not found"};
        }
        return boost::get<T>(it->second);
      }

      template <typename T>
      T value_or(const std::string& key, T alternative) const {
        auto it = variantParameters.find(key);
        if(it == variantParameters.end()) {
          return alternative;
        }
        return boost::get<T>(it->second);
      }

      template <typename T>
      void set(const std::string& key, T value) {
        variantParameters[key] = value;
      }

      bool contains(const std::string& key) const {
        return variantParameters.find(key) != variantParameters.end();
      }
    };
    using VariantParameters = StructExtension<MapStringVariantStruct>;
    std::ostream& operator<<( std::ostream& io , const VariantParameters& d ) ;


  } /* namespace rec */
} /* namespace dd4hep */

#endif // DDREC_DETECTORDATA_H
