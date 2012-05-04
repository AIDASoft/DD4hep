// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef GEARTPC_H
#define GEARTPC_H

#include "DD4hep/Detector.h"
#include "DDTPCModule.h"
#include <Exceptions.h> 

namespace DD4hep {
  

  
  struct GearTPC : public Geometry::DetElement {
    GearTPC(const Geometry::Ref_t& e);
    
    double getInnerRadius() const;
    double getOuterRadius() const;
    double getEndPlateThickness(int endplate) const;
    double getEndPlateZPosition(int endplate) const;
    DetElement getEndPlate(int endplate) const;
    void listDetElements() const;
    
    /** The maximum drift length in the TPC in mm.
     */
    double getMaxDriftLength() const;
    
    /** Returns number of modules on endplate (default 0 or 1, see compact xml).
     */
    int getNModules(int endplate) const;
    
    /** Returns the module with given id from endplate 0 or 1.
     */
    DDTPCModule getModule(int ID,int endplate) const;
    
    /** Returns nearest module to given coordinates (3D) on endplate (default 0 or 1, see compact xml).
     */
    DDTPCModule getNearestModule (double c0, double c1,int endplate) const;
  
    /** Check if coordinate is on a module on given endplate.
     */
    bool isInsideModule(double c0, double c1, int endplate) const;
 
    /** Returns vector of all modules on given endplate.
     */
    std::vector<DDTPCModule> getModules(int endplate) const;


  };
}
#endif
