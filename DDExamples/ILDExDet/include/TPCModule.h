// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef TPCMODULE_H
#define TPCMODULE_H

#include "DD4hep/Detector.h"
#include <Exceptions.h> 

namespace DD4hep {
  
  
  struct TPCModule : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;
    //various constructors
    TPCModule() {}
    TPCModule(const Ref_t& e) : Geometry::DetElement(e) {}
    TPCModule(const Geometry::DetElement& e) : Geometry::DetElement(e) {}
    TPCModule(const std::string& name, const std::string& type, int id);
    /** ID of this module.
     */
    int getID() const;
    /** The type of pad layout (segmentation) on this module.
     */
    std::string getPadType() const;
    /** The total number of pads on this module.
     */
    int getNPads() const;
    /** The total number of rows on this module.
     */
    int getNRows() const;
    /** The number of pads in a given row on this module.
     */
    int  getNPadsInRow(int row)const;
     /** The height of a given row on this module.
     */
    double getRowHeight (int row) const;
    /** The pad pitch (sensitive plus surroundings = effective size) of a given pad on this module.
     */
    double getPadPitch (int padIndex) const;
    /** The row which contains the given pad on this module.
     */
    int getRowNumber (int padIndex) const;
    /** The position of a pad within the row on this module.
     */
    int	getPadNumber (int padIndex) const;
    /** The pad id for a pad with padNum in row with rowNum on this module.
     */
    int getPadIndex (int rowNum, int padNum)const ;
    /** The right neighbour of a pad with the given index on this module.
     */
    int	getRightNeighbour (int padIndex) const;
    /** The left neighbour of a pad with the given index on this module.
     */
    int	getLeftNeighbour (int padIndex) const;
    /** The center of a pad on this module in global coordinates.
     */
    std::vector<double>	getPadCenter (int padIndex) const;
    /** Closest pad to a given location.
     */
    int getNearestPad (double c0, double c1) const;
    /**Helper function to get the z position of the module.
     */
    double getModuleZPosition() const;
  };

}
#endif
