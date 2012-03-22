// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef ILDEXDET_DDTPCPADROWLAYOUT_H
#define ILDEXDET_DDTPCPADROWLAYOUT_H

#include "DD4hep/Detector.h"

namespace DD4hep {
  
  
  class DDTPCPadRowLayout {
    
  public: 
    /// Destructor.
    virtual ~DDTPCPadRowLayout() { /* nop */; }
 
    // Static constants (enums) for the type of the row layout 

    static const int DDTPCFIXEDPADSIZELAYOUT = 1 ;

    static const int CARTESIAN = 1 ;
    static const int POLAR = 2 ;

    /** The type of the row layout implementation:
     *  DDTPCPadRowLayout.RECTANGULARPADROWLAYOUT,
     *  DDTPCPadRowLayout.FIXEDPADSIZEDISKLAYOUT, 
     *  DDTPCPadRowLayout.FIXEDPADANGLEDISKLAYOUT
     */
    virtual int getPadLayoutType() const = 0;
    
    /** The type of the row layouts coordinate system:
      *  DDTPCPadRowLayout.CARTESIAN or
      *  DDTPCPadRowLayout.POLAR.
      */
    virtual int getCoordinateType() const = 0;

     /** The total number of pads.
     */
    virtual int getNPads() const = 0;

    /** The number of rows.
     */
    virtual int getNRows() const = 0;

    /** The row height in mm.
     */
    virtual double getRowHeight(int rowNumber) const = 0;

    /** The width of the pad at padIndex in mm (CARTESIAN) or radians (POLAR). 
     *  This is only the metal of the pad. Use getPadPitch() to get the
     *  effective pitch.
     */
    virtual double getPadWidth(int padIndex) const = 0;

    /** The pitch (i. e. pad width plus gap)
     *  of the pad at padIndex in mm (CARTESIAN) or radians (POLAR). 
     */
    virtual double getPadPitch(int padIndex) const = 0;

    /** The center of the pad in 2d coordinates, (x,y) or (r,phi).
     */
    virtual double* getPadCenter(int padIndex) const = 0;

    /** The height of the pad in mm.
     */
    virtual double getPadHeight(int padIndex) const = 0;

    /** Indices of all pads in row rowNumber (row indices start from 0 at the
     * bottom (CARTESIAN) or at the center (POLAR)). 
     */
    virtual const std::vector<int>  & getPadsInRow(int rowNumber) const = 0;

    /** Extent of the sensitive plane
     */
    virtual const std::vector<double>  & getPlaneExtent() const = 0;

    /** The number of the row that contains the pad at padIndex - numbering starts at r/y==0.
     */
    virtual int getRowNumber(int padIndex) const = 0;

    /** The pad number (column) within the row  - numbering starts at phi/x == 0.
     */
    virtual int getPadNumber(int padIndex) const = 0;

    /** Create a padIndex for the given row and pad ( column ) number 
     */
    virtual int getPadIndex(int rowNum, int padNum) const = 0;

    /** The index of the pad nearest to the given point in 2d coordinates
     * (x,y,) or (r,phi).
     */
    virtual int getNearestPad(double c0, double c1) const = 0;

    /** The index of the right neighbour pad.
     */
    virtual int getRightNeighbour(int padIndex) const = 0;

    /** The index of the left neighbour pad.
     */
    virtual int getLeftNeighbour(int padIndex) const = 0;

    /** True if coordinate (c0,c1) is within the given pad.
     */
    virtual bool isInsidePad(double c0, double c1, int padIndex) const = 0;

    /** True if coordinate (c0,c1) is within any pad.
     */
    virtual bool isInsidePad(double c0, double c1) const = 0;

    /** Returns the closest distance to the edge (outer border) of the pad.
     *  For coordinates inside the pad it returns 0.
     *  ATTENTION: This gives the distance to the effective pad, which is
     *  the relevant one, and not the distance to the metal. I. e. it uses
     *  the pad pitch and the row height instead of pad width and pad height. 
     */
    virtual double getDistanceToPad(double c0, double c1, int padIndex) const = 0;
   
  };
}

#endif // ILDEXDET_DDTPCPADROWLAYOUT_H
