// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef ILDEXDET_DDTPCFIXEDPADSIZELAYOUT_H
#define ILDEXDET_DDTPCFIXEDPADSIZELAYOUT_H

#include <vector>
#include "DD4hep/Detector.h"
#include "DDTPCPadRowLayout.h"

namespace DD4hep {
  
  
  class DDTPCFixedPadSizeLayout : public DDTPCPadRowLayout{
    
     /** Internal helper class for DDTPCFixedPadSizeLayout */
    struct Row{
      int NPad ;
      double PadWidth ;
      double PadHeight ;
      double Height ;
      double LeftOffset ;
      double RightOffset ;
      double Center ;
      double WidthPerPad ; // pad + insensitive ( gap ) 
    } ;
    
    
  protected:
    int _nRow ;
    int _nPad ;
    int _repeatRows ;
    std::vector<Row> _rows ;
    std::vector<double> _extent ;
    mutable std::vector< std::vector<int>* > _padIndices ;
    std::vector<unsigned> _nRows ;  // helper vector to keep track of equal rows

     /** function to copy all internal variables, incl. the objects
     *  pointed to and owned by the GearMgr.
     *  Used by constructor and assigment operator to avoid code duplication
     */
     void copy_and_assign(const DDTPCFixedPadSizeLayout & );

    /** function to delete all the objects
     *  pointed to and owned by the GearMgr.
     *  Used by desctructor and assigment operator to avoid code duplication
     */
     void cleanup();

  public: 
    /** Construct the empty DDTPCFixedPadSizeLayout with the width and x position
     *  specified through  xMin and xMax and an optional yMin - yMax will be computed from 
     *  the rows that are added with addRow().
     */
    DDTPCFixedPadSizeLayout( double xMin, double xMax , double yMin=0.0) ;
    
    /** The copy constructor.
     *	Needed because _padIndices allocates memory dynamically
     */
    DDTPCFixedPadSizeLayout( const DDTPCFixedPadSizeLayout &);
 
    /// The assignment operator
    DDTPCFixedPadSizeLayout & operator = ( const DDTPCFixedPadSizeLayout &);

    /// Destructor.
    virtual ~DDTPCFixedPadSizeLayout() ; 
    
    /* The clone function. Used to access the copy-constructor if this class via the
     * acstract PadRowLayout2D interface.
     */
    DDTPCPadRowLayout* clone() const;

    
    /** Add nRow rows with the given parameters.
     */
    virtual void addRow(  int nRow, int nPad , double padWidth , double padHeight , 
			  double rowHeight =0.0, double leftOffset =0.0 , 
			  double rightOffset =0.0 );
    
    
    /** Repeat the current rows 'count' times - this allows to easily repeat a pattern of
     *  several rows, e.g. 2 rows offset to each other by half a padWidth (staggering).
     *  Can only be called once per layout, i.e. no multiple rpeat patterns are allowed.
     */
    virtual void repeatRows(unsigned count) ;
    
    /** Returns the number number for which a given row pattern has been repaeted 
     */
    int getRepeatRowCount() const { return _repeatRows ; } 
    
    
    /** The type of the row layout implementation:
     *  DDTPCFixedPadSizeLayout.DDTPCRECTANGULARPADROWLAYOUT,
     */
    virtual int getPadLayoutType() const { return  DDTPCPadRowLayout::DDTPCFIXEDPADSIZELAYOUT ; };
    
    /** The type of the row layouts coordinate system:
      *  DDTPCFixedPadSizeLayout.CARTESIAN or
      */
    virtual int getCoordinateType()  const { return DDTPCPadRowLayout::CARTESIAN ; } 

      virtual int getNPads() const { return _nPad ; }
    
    /** The number of rows.
     */
    virtual int getNRows() const ;
    
    /** The row height in mm.
     */
    virtual double getRowHeight(int rowNumber) const ;

    /** The height of the pad in mm.
     */
    virtual double getPadHeight(int padIndex) const ;

    /** The width of the pad at padIndex in mm.
     *  This is only the metal of the pad. Use getPadPitch() to get the
     *  effective pitch.
     */
    virtual double getPadWidth(int padIndex) const ;

    /** The pitch (i. e. pad width plus gap)
     *  of the pad at padIndex in mm.
     */
    virtual double getPadPitch( int padIndex ) const ;

    /** The center of the pad in 2d coordinates, (x,y) or (r,phi).
     */
    virtual double* getPadCenter(int padIndex)  const;


    /** Indices of all pads in row rowNumber (row indices start from 0 at the
     * bottom (CARTESIAN) or at the center (POLAR)). 
     */
    virtual const std::vector<int>& getPadsInRow(int rowNumber) const ;

    /** Extent of the sensitive plane - [xmin,xmax,ymin,ymax] CARTESIAN or 
     *	[rmin,rmax,phimin,phimax] POLAR.
     */
    virtual const std::vector<double>& getPlaneExtent()  const { return _extent ; }

    /** The number of the row that contains the pad at padIndex - numbering starts at y==0 (bottom).
     */
    virtual int getRowNumber(int padIndex)  const ;

    /** The pad number (column) within the row  - numbering starts at x==0 (left).
     */
    virtual int getPadNumber(int padIndex) const { return ( 0x0000ffff & padIndex ) ; }

    /** Create a padIndex for the given row and pad ( column ) number 
     */
    virtual int getPadIndex(int rowNum, int padNum)  const;

    /** The index of the pad nearest to the given point in 2d coordinates
     * (x,y,) or (r,phi).
     */
    virtual int getNearestPad(double x, double y)  const;

    int getNearestPadOld(double x, double y) const;

    /** The index of the right neighbour pad.
     */
    virtual int getRightNeighbour(int padIndex)  const;

    /** The index of the left neighbour pad.
     */
    virtual int getLeftNeighbour(int padIndex)  const;

    /** True if coordinate (x,y) is within the given pad.
     */
    virtual bool isInsidePad(double x, double y, int padIndex)  const;

    /** True if coordinate (x,y) is within any pad.
     */
    virtual bool isInsidePad(double x, double y)  const;

    /** Returns the closest distance to the edge (outer border) of the pad.
     */
    virtual double getDistanceToPad(double c0, double c1, int padIndex) const;
    

    /** Helper method to identify equal row numbers in this layout (as they have been added).*/
    const std::vector<unsigned>& equalRowNumbers() const { return _nRows ; }

    /** Helper method with all row data.*/
    const std::vector<Row>& rows() const { return _rows ; }

    
  };
}

#endif // ILDEXDET_DDTPCFIXEDPADSIZELAYOUT_H
