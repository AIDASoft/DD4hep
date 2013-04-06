// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef VersatileDiskRowLayout_H
#define VersatileDiskRowLayout_H

#include "DD4hep/Detector.h"
#include "VersatileDiskRowLayoutSeg.h"
#include "PadLayout.h"
#include <DD4HepExceptions.h> 

namespace DD4hep {
  
  
  struct VersatileDiskRowLayout : public PadLayout {
   
    Geometry::DetElement                module;
    Geometry::VersatileDiskRowLayoutSeg pads;
    Geometry::VersatileDiskRowLayoutData *padData;
    Geometry::Tube                      tube;
    
    /// Default empty constructor
    VersatileDiskRowLayout() : module(), pads(), tube() {}

    /// Standard constructor with arguments
    VersatileDiskRowLayout(const Geometry::DetElement& d);

    /// Standard extension constructor. det is the NEW detector element e.g. when reflecting a detector
    VersatileDiskRowLayout(const VersatileDiskRowLayout& c,const Geometry::DetElement& det);

       
    /// Default destructor
    virtual ~VersatileDiskRowLayout() {}

    /** The type of pad layout (segmentation) on this module.
     */
    virtual std::string getPadType() const;
    /** The total number of pads on this module.
     */
    virtual int getNPads() const;
    /** The total number of rows on this module.
     */
    virtual int getNRows() const;
    /** The number of pads in a given row on this module.
     */
    virtual int  getNPadsInRow(int row)const;
     /** The height of a given row on this module.
     */
    virtual double getRowHeight (int row) const;
    /** The pad pitch (sensitive plus surroundings = effective size) of a given pad on this module.
     */
    virtual double getPadPitch (int padIndex) const;
    /** The row which contains the given pad on this module.
     */
    virtual int getRowNumber (int padIndex) const;
    /** The position of a pad within the row on this module.
     */
    virtual int	getPadNumber (int padIndex) const;
    /** The pad id for a pad with padNum in row with rowNum on this module.
     */
    virtual int getPadIndex (int rowNum, int padNum)const ;
    /** The right neighbour of a pad with the given index on this module.
     */
    virtual int	getRightNeighbour (int padIndex) const;
    /** The left neighbour of a pad with the given index on this module.
     */
    virtual int	getLeftNeighbour (int padIndex) const;
    /** The center of a pad on this module in global coordinates.
     */
    virtual std::vector<double>	getPadCenter (int padIndex) const;
    /** Closest pad to a given location.
     */
    virtual int getNearestPad (double c0, double c1) const;
  };

}
#endif
