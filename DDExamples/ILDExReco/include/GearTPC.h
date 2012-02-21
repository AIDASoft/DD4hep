// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Detector.h"

namespace DD4hep {
  
  struct TPCData;
  struct TPCModule {};
  struct GlobalPadIndex {};
  struct PadRowLayout2D {};
  
  struct GearTPC : public Geometry::DetElement {
    typedef TPCData Object;
    GearTPC(const Geometry::Ref_t& e);
    
    double innerRadius() const;
    double outerRadius() const;
    double pressure() const;
    
    const TPCModule & getModule(int ID) const;
    
    /** Returns number of modules in this TPC (endplate).
     */
     int getNModules() const;
    
    /** Returns nearest module to given coordinates (2D).
     *  In case of a full TPC with two end caps the first matching module is returned,
     *  although a module from the other end plate might be closer in 3D.
     *  Use the 3D version of getNearestModule in this case 
     *  to get the module from the correct half TPC.
     */
     const TPCModule & getNearestModule(double c0, double c1) const;
    
    /** Returns nearest module to given coordinates (3D).
     *  The z coordinate is only used to determine whether the positive or negative 
     *  half TPC is to be used.
     */
     const TPCModule & getNearestModule(double c0, double c1, double z) const;
    
    /** The maximum drift length in the TPC in mm.
     */
     double getMaxDriftLength() const;
    
    /** True if coordinate (c0,c1) is within any module.
     *  This may or may not be
     *  on a pad, since with resitive films being on the film is enough
     *  to generate signal on pads (see TPCModule::getBorderWidth() ).
     *  This is the 2D version. Use isInsideModule(double c0, double c1, double z)
     *  to limit to correct end plate in case there are two.
     */
     bool isInsideModule(double c0, double c1) const;
    
    /** First determines the correct end plate from the z coordinate and then
     *  calls the same function as the 2D version on the modules from the correct 
     *  end cap.
     */
     bool isInsideModule(double c0, double c1, double z) const;
    
    /** True if coordinate (c0,c1) is within any pad, on any module.
     *  This is the 2D version. Use isInsideModule(double c0, double c1, double z)
     *  to limit to correct end plate in case there are two.
     */
     bool isInsidePad(double c0, double c1) const;
    
    /** First determines the correct end plate from the z coordinate and then
     *  calls the same function as the 2D version on the modules from the correct 
     *  end cap.
     */
     bool isInsidePad(double c0, double c1, double z) const;
    
    /** Returns globalPadindex Object for nearest pad to given coordinates (2D).
     *  In case of a full TPC with two end caps the first matching module ID is returned,
     *  although a module from the other end plate might be closer in 3D.
     *  Use the 3D version of getNearestPad in this case 
     *  to get the pad from a module from the correct half TPC.
     */
     GlobalPadIndex getNearestPad(double c0, double c1) const;
    
    /** Returns globalPadindex Object for nearest pad to given coordinates (3D).
     *  The z coordinate is only used to determine whether the positive or negative 
     *  half TPC is to be used.
     */	
     GlobalPadIndex getNearestPad(double c0, double c1, double z) const;
    
    /** Extent of the sensitive plane - [xmin,xmax,ymin,ymax] CARTESIAN or 
     *	[rmin,rmax,phimin,phimax] POLAR. These are the outermost coordinates
     *  in the respective coordinate system. It does not mean that the complete
     *  plane is sensitive as there might be dead space between the modules.
     */
     const std::vector<double>  & getPlaneExtent() const;
    
    /** Returns coordinate type as an int (see PadRowLayout2D::CARTESIAN,
     *  PadRowLayout2D::POLAR)
     */
     int getCoordinateType() const;
    
    /** Returns vector of all modules in this TPC (endplate).
     */
     const std::vector<TPCModule *>  & getModules() const;
    
    /** Kept for backward compatibility.
     *  In case there is only one module, which has no angle and no
     *  offset wrt. the gobal coordinate systen, it gives back the 
     *  PadRowLayout2D of this module.
     *  Otherwise it throws a gear::Exception.
     *
     *  \deprecated{Please use getModule(moduleID) instead.
     *  The TPCModule is an implementation of PadRowLayout2D}
     */
     const PadRowLayout2D & getPadLayout() const;
    
    /** The electron drift velocity in the TPC in mm/s.
     *  Kept for backwards compatibility. 
     *
     *  \deprecated{This should come from  conditions data.}
     */
     double getDriftVelocity() const;
    
    /** Kept for backward compatibility.
     *  In case there is only one module it gives back the 
     *  redaout frequency of this module.
     *  Otherwise it throws a gear::Exception.
     *  
     *  \deprecated{Use TPCModule::getReadoutFrequency() instead.}
     */
     double getReadoutFrequency() const;
  };
}
