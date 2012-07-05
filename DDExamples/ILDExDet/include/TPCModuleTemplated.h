// $Id:AIDA
//====================================================================
//  $ Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef TPCMODULETEMPLATED_H
#define TPCMODULETEMPLATED_H

#include "DD4hep/Detector.h"
#include <Exceptions.h> 
#include "TPCModuleData.h"

namespace DD4hep {

  struct TPCModuleData;  
  //template for the PadLayouts
  template <class PL>

  struct TPCModuleTemplated : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;
    //template for PadLayout
    PL padlayout;

    TPCModuleData* m_modData;
    void getExtension(){  m_modData = isValid() ? extension<TPCModuleData>() : 0;}

   //various constructors
    TPCModuleTemplated() : m_modData(0) {}
    TPCModuleTemplated(const Ref_t& e) : Geometry::DetElement(e), m_modData(0) {getExtension();}
    TPCModuleTemplated(const Geometry::DetElement& e) : Geometry::DetElement(e), m_modData(0) {getExtension();}
    TPCModuleTemplated(const std::string& name, const std::string& type, int id) : Geometry::DetElement(name,type,id), m_modData(0) {getExtension();}
    template<typename T> TPCModuleTemplated& operator=(const T& h)    {
      m_element = h.m_element;
      getExtension();
      return *this;
    }  

    /** ID of this module.
     */
    int getID() const { return _data().id;  }

    //Now all the functionality that depends on specific implementation of padlayout
    /** The type of pad layout (segmentation) on this module.
     */
    std::string getPadType() const { padlayout.getPadType(); } 
    /** The total number of pads on this module.
     */
    int getNPads()  const { padlayout.getNPads(); }
    /** The total number of rows on this module.
     */
    int getNRows() const { padlayout.getNRows(); }
    /** The number of pads in a given row on this module.
     */
    int  getNPadsInRow(int row)const { padlayout.getNPadsInRow(row); }
     /** The height of a given row on this module.
     */
    double getRowHeight (int row) const { padlayout.getRowHeight (row); }
    /** The pad pitch (sensitive plus surroundings = effective size) of a given pad on this module.
     */
    double getPadPitch (int padIndex) const { padlayout.getPadPitch (padIndex); }
    /** The row which contains the given pad on this module.
     */
    int getRowNumber (int padIndex) const { padlayout.getRowNumber (padIndex);} 
    /** The position of a pad within the row on this module.
     */
    int	getPadNumber (int padIndex) const { padlayout.getPadNumber (padIndex); }
    /** The pad id for a pad with padNum in row with rowNum on this module.
     */
    int getPadIndex (int rowNum, int padNum)const { padlayout.getPadIndex (rowNum, padNum); } 
    /** The right neighbour of a pad with the given index on this module.
     */
    int	getRightNeighbour (int padIndex) const { padlayout.getRightNeighbour (padIndex); }
    /** The left neighbour of a pad with the given index on this module.
     */ 
    int	getLeftNeighbour (int padIndex) const { padlayout.getLeftNeighbour (padIndex) ;}
    /** The center of a pad on this module in global coordinates.
     */
    std::vector<double>	getPadCenter (int padIndex) const { padlayout.getPadCenter (padIndex) ;}
    /** Closest pad to a given location.
     */
    int getNearestPad (double c0, double c1) const { padlayout.getNearestPad (c0,c1); }
    /**Helper function to get the z position of the module in local coordinates.
     */
    double getModuleZPosition() const { padlayout.getModuleZPosition(); }
  };

}
#endif
