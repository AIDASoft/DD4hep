// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef PadLayout_H
#define PadLayout_H

#include <string>
#include <vector> 

namespace DD4hep {
  
  
  struct PadLayout {
    /// Default destructir
    virtual ~PadLayout() {}

    /** The type of pad layout (segmentation) on this module.
     */
    virtual std::string getPadType() const=0;
    /** The total number of pads on this module.
     */
    virtual int getNPads() const=0;
    /** The total number of rows on this module.
     */
    virtual int getNRows() const=0;
    /** The number of pads in a given row on this module.
     */
    virtual int  getNPadsInRow(int row)const=0;
     /** The height of a given row on this module.
     */
    virtual double getRowHeight (int row) const=0;
    /** The pad pitch (sensitive plus surroundings = effective size) of a given pad on this module.
     */
    virtual double getPadPitch (int padIndex) const=0;
    /** The row which contains the given pad on this module.
     */
    virtual int getRowNumber (int padIndex) const=0;
    /** The position of a pad within the row on this module.
     */
    virtual int	getPadNumber (int padIndex) const=0;
    /** The pad id for a pad with padNum in row with rowNum on this module.
     */
    virtual int getPadIndex (int rowNum, int padNum)const=0;
    /** The right neighbour of a pad with the given index on this module.
     */
    virtual int	getRightNeighbour (int padIndex) const=0;
    /** The left neighbour of a pad with the given index on this module.
     */
    virtual int	getLeftNeighbour (int padIndex) const=0;
    /** The center of a pad on this module in global coordinates.
     */
    virtual std::vector<double>	getPadCenter (int padIndex) const=0;
    /** Closest pad to a given location.
     */
    virtual int getNearestPad (double c0, double c1) const=0;
  };

}
#endif
