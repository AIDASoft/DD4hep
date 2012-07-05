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
#include <PadLayout.h> 

namespace DD4hep {
  
 
  struct TPCModule : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;

    
    PadLayout* padLayout;
    void getExtension();

    //various constructors
    TPCModule() : padLayout(0) {}
    TPCModule(const Ref_t& e) : Geometry::DetElement(e), padLayout(0) {getExtension();}
    TPCModule(const Geometry::DetElement& e) : Geometry::DetElement(e), padLayout(0) {getExtension();}
    TPCModule(const std::string& name, const std::string& type, int id) : Geometry::DetElement(name,type,id), padLayout(0) {getExtension();}
    template<typename T> TPCModule& operator=(const T& h)    {
      m_element = h.m_element;
      getExtension();
      return *this;
    }  

    /** ID of this module.
     */
    int getID() const;
     /**Helper function to get the z position of the module in local coordinates.
     */
    double getModuleZPosition() const;

    //Now all the functionality that depends on specific implementation of padlayout
    /** The type of pad layout (segmentation) on this module.
     */
    virtual std::string getPadType() const { padLayout->getPadType(); } 
    /** The total number of pads on this module.
     */
    virtual int getNPads()  const { padLayout->getNPads(); }
    /** The total number of rows on this module.
     */
    virtual int getNRows() const { padLayout->getNRows(); }
    /** The number of pads in a given row on this module.
     */
    virtual int  getNPadsInRow(int row)const { padLayout->getNPadsInRow(row); }
     /** The height of a given row on this module.
     */
    virtual double getRowHeight (int row) const { padLayout->getRowHeight (row); }
    /** The pad pitch (sensitive plus surroundings = effective size) of a given pad on this module.
     */
    virtual double getPadPitch (int padIndex) const { padLayout->getPadPitch (padIndex); }
    /** The row which contains the given pad on this module.
     */
    virtual int getRowNumber (int padIndex) const { padLayout->getRowNumber (padIndex);} 
    /** The position of a pad within the row on this module.
     */
    virtual int	getPadNumber (int padIndex) const { padLayout->getPadNumber (padIndex); }
    /** The pad id for a pad with padNum in row with rowNum on this module.
     */
    virtual int getPadIndex (int rowNum, int padNum)const { padLayout->getPadIndex (rowNum, padNum); } 
    /** The right neighbour of a pad with the given index on this module.
     */
    virtual int	getRightNeighbour (int padIndex) const { padLayout->getRightNeighbour (padIndex); }
    /** The left neighbour of a pad with the given index on this module.
     */ 
    virtual int	getLeftNeighbour (int padIndex) const { padLayout->getLeftNeighbour (padIndex) ;}
    /** The center of a pad on this module in global coordinates.
     */
    virtual std::vector<double>	getPadCenter (int padIndex) const { padLayout->getPadCenter (padIndex) ;}
    /** Closest pad to a given location.
     */
    virtual int getNearestPad (double c0, double c1) const { padLayout->getNearestPad (c0,c1); }
  };

}
#endif
