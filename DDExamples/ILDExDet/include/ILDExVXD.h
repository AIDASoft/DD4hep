// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_ILDEXVXD_H
#define DD4hep_ILDEXVXD_H

#include "DD4hep/Detector.h"

namespace DD4hep {
  
  // Forward declarations
  struct VXDData;

  /**@class ILDExVXD  ILDExVXD.h ILDExVXD.h
   * 
   *  An example how to implement a gear like interface of the ILD vertex detector
   *  using the DD4hep geometry package.
   *  In this example we use the technique of **extending** the DetElement::Object
   *  structure using inheritance. The data extension allow to store the additional
   *  for the reconstruction. The data were filled, when the geometry was instantiated.
   *  (See: compact/ILDExVXD_geo.cpp)
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct ILDExVXD : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;
    /// Internally kept pointer to object data - for optimization purposes only
    VXDData* m_data;

    public:
    /// Constructor from another handle. Checks validity of assignment internally
    ILDExVXD(const Ref_t& e);

    /// Assignment from same type of object
    ILDExVXD(const ILDExVXD& e) : Geometry::DetElement(e), m_data(e.m_data) {}

    /// Assignment from another handle. Checks validity of assignment internally
    ILDExVXD& operator=(const Ref_t& e);

    /// Assignment from same type of object
    ILDExVXD& operator=(const ILDExVXD& e);

    /** The total number of layers.
     */
    int getNLayers() const;

    /** The number of ladders in the layer layerIndex - layer indexing starts at 0
     *  for the layer closest to IP.
     */
    int getNLadders(int layerIndex) const;

    /** Azimuthal angle of the (outward pointing) normal of the first ladder.
     *  Phi0==0 corresponds to the first ladder's normal coinciding (if offset==0) with the x-axis.
     *  The layerIndex starts at 0 for the layer closest to IP.
     */
    double getPhi0(int layerIndex) const;

    /** The radiation length in the support structure ladders of layer layerIndex - layer indexing starts at 0
     *  for the layer closest to IP.
     */
    double getLadderRadLength(int layerIndex) const;

    /** The radiation length in sensitive volumes in layer layerIndex - layer indexing starts at 0
     *  for the layer closest to IP.
     */
    double getSensitiveRadLength(int layerIndex) const;

    /** The distance of ladders in layer layerIndex from the IP - layer indexing starts at 0
     *  for the layer closest to IP.
     */
    double getLadderDistance(int layerIndex) const;

    /** The thickness in mm of the ladders in layerIndex - layer indexing starting at 0
     *  for the layer closest to IP.
     */
    double getLadderThickness(int layerIndex) const;

    /** The offset of the ladder in mm defines the shift of the ladder in the direction of increasing phi
     *  perpendicular to the ladder's normal. For example if the first ladder is at phi0==0 then the offset 
     *  defines an upward shift of the ladder (parallel to the y-axis).  
     *  Layer indexing starts at 0 for the layer closest to IP.
     *  @see getPhi0
     *  @see getSensitiveOffset
     */
    double getLadderOffset(int layerIndex) const;

    /** The width of the ladder in layer in mm for ladders in layer layerIndex -
     *  layer indexing starting at 0 from the layer closest to IP.
     */
    double getLadderWidth(int layerIndex) const;

    /** The length of the ladder in z direction in mm for ladders in layer layerIndex -
     *  layer indexing starting at 0 from the layer closest to IP.
     */
    double getLadderLength(int layerIndex) const;

    /** The distance of sensitive area in ladders in layer layerIndex from the IP.
     */
    double getSensitiveDistance(int layerIndex) const;

    /** The thickness in mm of the sensitive area in ladders in layer layerIndex.
     */
    double getSensitiveThickness(int layerIndex) const;

    /** Same as getLadderOffset() except for the sensitive part of the ladder.
     * @see getLadderOffset
     */
    double getSensitiveOffset(int layerIndex) const;

    /** The width of the sensitive area in ladders in layer layerIndex in mm.
     */
    double getSensitiveWidth(int layerIndex) const;

    /** The length of the sensitive area in ladders in z direction in mm for ladders in layer layerIndex.
     */
    double getSensitiveLength(int layerIndex) const;


    /** returns maximum radius for a given layer
     */
    virtual double getMaxRadius( int layerIndex , bool sensitive=false ) const; 
  
    /** returns starting phi for first ladder in layer layerIndex (on side facing IP)
     */
    virtual double getStartInnerPhi( int layerIndex , bool sensitive=false ) const;

    /** returns ending phi for first ladder in layer layerIndex (on side facing IP)
     */
    virtual double getEndInnerPhi( int layerIndex , bool sensitive=false ) const;

    /** returns starting phi for first ladder in layer layerIndex (on side away from IP)
     */
    virtual double getStartOuterPhi( int layerIndex , bool sensitive=false ) const;

    /** returns ending phi for first ladder in layer layerIndex (on side away from IP)
     */
    virtual double getEndOuterPhi( int layerIndex , bool sensitive=false ) const;
  
    /** returns thickness as viewed under the angle phi only for the first ladder
     *  in layer layerIndex.
     *  returns -1 if phi is not covered by ladder
     */
    virtual double getThicknessForPhi( int layerIndex, double phi , bool sensitive=false ) const;
  };
}
#endif // DD4hep_ILDEXVXD_H
