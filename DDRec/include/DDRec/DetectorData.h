#ifndef DDRec_DetectorData_H_
#define DDRec_DetectorData_H_


namespace DD4hep {
  namespace DDRec {
    
    /** Wrapper class for adding structs or pods as extensions to DetElements.
     *  Provides default implementations of the c'tors required by the extension mechamism.
     *  Structs (or classes) can be defined w/o any c'tors relying in the default ones 
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

    /** Simple data structure holding data that is relevant for
     *  reconstruction of a cylindrical TPC with a pad plane that
     *  is symmetrically populated with one size pads.
     *  ( Can be used to instantiate a gear::TPCParmeter object
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


  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_DetectorData_H_
