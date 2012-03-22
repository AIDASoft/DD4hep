// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Detector.h"
#include "DDTPCModule.h"

namespace DD4hep {
  

  
  struct GearTPC : public Geometry::DetElement {
    GearTPC(const Geometry::Ref_t& e);
    
    double getInnerRadius() const;
    double getOuterRadius() const;
    double getEndPlateThickness() const;
    
    const DDTPCModule & getModule(int ID) const;
    
    /** Returns number of modules in this TPC (endplate).
     */
     int getNModules() const;
  
    /** The maximum drift length in the TPC in mm.
     */
     double getMaxDriftLength() const;
  
 
 
  
  };
}
