// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XMLDETECTOR_H
#define DD4HEP_XMLDETECTOR_H

// Framework include files
#include "XML/XMLTags.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
  *   XML namespace declaration
  */
  namespace XML  {


    /** @class Dimension XMLDetector.h XML/XMLDetector.h
     * 
     *  Helper class to access any field in a xml tag in a 
     *  very easy way.
     *  - You may assign any xml handle to a dimension object
     *  - Any attribute of this xml element may then be accessed
     *    by it's natural way. All possible attribute names are
     *    reflected by the Dimmension object's member functions.
     *  - If an attribute is requested and not present, a exception
     *    is thrown.
     *  - Functions, which accept a default value do NOT throw 
     *    an exception if the attribute is not present. These
     *    rather return the default value.
     *  - If a often used function is not present - the 
     *    implementation thereof is simple.
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Dimension : public Element  {
      /// Default constructor
      Dimension() : Element(Handle_t(0)) {}
      /// Constructor from Handle
      Dimension(Handle_t e) : Element(e) {}
      /// Constructor from Element
      Dimension(const Element& e) : Element(e) {}

      /// Access rotation constants: theta
      double theta() const;
      /// Access rotation constants: phi
      double phi() const;
      /// Access rotation constants: psi
      double psi() const;

      /// Access Box/Position parameters: x
      double x() const;
      /// Access Box/Position parameters: y
      double y() const;
      /// Access Box/Position parameters: z
      double z() const;

      /// Access Box/Position parameters: x, if not present returns default
      double x(double default_val) const;
      /// Access Box/Position parameters: y, if not present returns default
      double y(double default_val) const;
      /// Access Box/Position parameters: z, if not present returns default
      double z(double default_val) const;

      /// Access Trapezoid parameters: x1
      double x1() const;
      /// Access Trapezoid parameters: x2
      double x2() const;
      /// Access Trapezoid parameters: y1
      double y1() const;
      /// Access Trapezoid parameters: y2
      double y2() const;

      /// Access Tube parameters: rmin
      double rmin() const;
      /// Access Tube parameters: rmax
      double rmax() const;
      /// Access Tube parameters: zhalf
      double zhalf() const;
      /// Access Tube parameters: deltaphi
      double deltaphi() const;

      /// Access attribute values: outer_radius
      double outer_radius() const;
      /// Access attribute values: outer_r
      double outer_r() const;
      /// Access attribute values: outer_z
      double outer_z() const;
      /// Access attribute values: inner_radius
      double inner_radius() const;
      /// Access attribute values: inner_r
      double inner_r() const;
      /// Access attribute values: inner_z
      double inner_z() const;

      /// Access attribute values: z_length
      double z_length() const;
      /// Access attribute values: length
      double length() const;
      /// Access attribute values: width
      double width() const;
      /// Access attribute values: height
      double height() const;
      /// Access attribute values: depth
      double depth() const;

      double gap() const;
      double r_size() const;
      double phi_size_max() const;
      bool   reflect() const;
      bool   reflect(bool default_value) const;
      double outgoing_r() const;
      double incoming_r() const;
      double crossing_angle() const;

      double zmin() const;
      double zmax() const;

      double radius() const;
      double offset() const;
      double offset(double default_value) const;
      int    number() const;

      int    nModules() const;
      int    RowID() const;
      int    nPads() const;
      double moduleHeight() const;
      double moduleWidth() const;
      double modulePitch() const;
      double modulePosX() const;
      double modulePosY() const;
      double rowPitch() const;
      double padPitch() const;
      double rowHeight() const;
      std::string   padType() const;

      int numsides() const;

      double phi0() const;
      double phi0(double default_value) const;
      double phi_tilt() const;
      int    nphi() const;
      double rc()  const;
      double dr()  const;
      double dz()  const;
      double dz(double default_value)  const;
      double r()  const;
      double r(double default_value)  const;
      
      double z0() const;
      double zstart() const;
      int    nz() const;

      double start()  const;
      double end()    const;

    };

    struct DetElement : public Dimension  {
      struct Component : public Dimension {
        Component(Handle_t e) : Dimension(e) {}
        Component(const Element& e) : Dimension(e) {}
        int    id() const;
        int    id(int default_value) const;
        int    repeat()  const;
        double thickness() const;
        bool   isSensitive() const;
        std::string   nameStr() const;
	std::string   typeStr() const;
        std::string   moduleStr() const;
        std::string   materialStr() const;
        std::string   visStr() const;
        std::string   regionStr() const;
        std::string   limitsStr() const;
	Dimension     dimensions()  const;
      };
      DetElement(Handle_t e) : Dimension(e)  {}
      Handle_t handle()  const { return m_element; }
      int  id() const;
      std::string nameStr() const;
      std::string typeStr() const;
      std::string materialStr() const;
      std::string visStr() const;
      std::string regionStr() const;
      std::string limitsStr() const;
      double inner_field() const;
      double outer_field() const;
      Dimension dimensions()  const;
      bool isTracker() const;
      bool isCalorimeter() const;
      bool isInsideTrackingVolume() const;
      void check(bool condition, const std::string& msg) const;
      template <typename T> T attr(const XmlChar* tag) const { return m_element.attr<T>(tag); }
    };
  }
}         /* End namespace DD4hep       */
#endif    /* DD4HEP_XMLDETECTOR_H    */
