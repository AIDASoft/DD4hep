// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank, A. Muennich
//
//  Interface to answer all user questions to a TPC.
//  Reproducing the functionality of TPCParameters in GEAR
//
//====================================================================
#ifndef GEARTPC_H
#define GEARTPC_H

#include "DD4hep/Detector.h"
#include "TPCModule.h"
#include <DD4HepExceptions.h> 

namespace DD4hep {
  

  
  struct GearTPC : public Geometry::DetElement {
    GearTPC(const Geometry::Ref_t& e);
    
    /** Inner Radius of the TPC in mm.
     */
    double getInnerRadius() const;
    /** Outer Radius of the TPC in mm.
     */
    double getOuterRadius() const;
    /** Thickness of the Endplate of the TPC in mm.
     */
    double getEndPlateThickness(int endplate) const;
    /** Positon in z of the Endplate of the TPC in mm.
     */
    double getEndPlateZPosition(int endplate) const;
    /** Used as helper function for access to  Endplate of the TPC.
     */
    DetElement getEndPlate(int endplate) const;
     /** For debugging: List all elements and their children belonging to the TPC.
     */
    void listDetElements() const;
    
    /** The maximum drift length in the TPC in mm.
     */
    double getMaxDriftLength() const;
    
    /** Returns number of modules on endplate (default 0 or 1, see compact xml).
     */
    int getNModules(int endplate) const;
    
    /** Returns the module with given id from endplate 0 or 1.
     */
    TPCModule getModule(int ID,int endplate) const;
    
    /** Returns nearest module to given coordinates (3D) on endplate (default 0 or 1, see compact xml).
     */
    TPCModule getNearestModule (double c0, double c1,int endplate) const;
  
    /** Check if coordinate is on a module on given endplate.
     */
    bool isInsideModule(double c0, double c1, int endplate) const;
 
    /** Returns vector of all modules on given endplate.
     */
    std::vector<TPCModule> getModules(int endplate) const;

    /** Returns position of gas volume placement.
     */
    std::vector<double> getPosition() const;
  };
}
#endif
