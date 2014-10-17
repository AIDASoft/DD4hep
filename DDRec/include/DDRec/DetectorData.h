#ifndef DDRec_DetectorData_H_
#define DDRec_DetectorData_H_


namespace DD4hep {
  namespace DDRec {
    
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
      StructExtension(const Geometry::DetElement& d) : T()  {}
      StructExtension(const StructExtension<T>& t, const Geometry::DetElement& d) : T(t) {}
    };

    /** Simple data structure with key parameters for
     *  reconstruction of a cylindrical TPC with a pad plane that
     *  is symmetrically populated with one size pads.
     *  ( Can be used to instantiate a gear::TPCParameter object
     *    with a FixedPadSizeDiskLayout )
     * 
     * @author F.Gaede, CERN/DESY
     * @date Sep, 30 2014
     * @version $Id: $
     */
    struct FixedPadSizeTPCStruct{
      double zHalf ;
      double rMin ;
      double rMax ;
      double driftLength ;
      double rMinReadout ;
      double rMaxReadout ;
      double innerWallThickness ;
      double outerWallThickness ;
      double padHeight ;
      double padWidth ;
      double maxRow ;
      double padGap ;
    };
    typedef StructExtension<FixedPadSizeTPCStruct> FixedPadSizeTPCData ;



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

      /*
	DDRec::ZPlanarData::LayerLayout thisLayer ;
	thisLayer.sensorsPerLadder =
	thisLayer.lengthSensor =
	
	thisLayer.distanceSupport  = 
	thisLayer.offsetSupport    =
	thisLayer.thicknessSupport =
	thisLayer.zHalfSupport     =
	thisLayer.widthSupport     =
	
	thisLayer.distanceSensitive  = 
	thisLayer.offsetSensitive    =
	thisLayer.thicknessSensitive =
	thisLayer.zHalfSensitive     =
	thisLayer.widthSensitive     =
	
	thisLayer.ladderNumber = 
	thisLayer.phi0 =  
      */
    } ;
    typedef StructExtension<ZPlanarStruct> ZPlanarData ;




  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_DetectorData_H_
