#ifndef PARSERS_DETAIL_CHILDVALUE_H
#define PARSERS_DETAIL_CHILDVALUE_H

//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

/** 
 *  Note: Do NEVER include this file directly!
 *
 *  Use the specific include files in the XML or JSON directory!
 *  Also NO header guards!
 */


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class NamedObject;

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace DD4HEP_DIMENSION_NS {

    /// XML Handle_t to xml elements with predefined child access
    /**
     *  Helper class to access any field in a xml tag in a
     *  very easy way.
     *  - You may assign any xml handle to a childvalue object
     *  - Any child with the name and the value attribute set may then be accessed
     *    by its natural way. All possible child names are
     *    reflected by the object's member functions.
     *
     *    IMPORTANT NOTE:
     *    to be understood, the child elements MUST have structure like:
     *    <inner_r value="76*cm" ....other attributes...../>
     *    Other attributes may e.g. be a comment describing the variable.
     *    The 'other attributes' are ignored. The 'value' attribute
     *    is mandatory.
     *
     *  - If an child is requested and not present, a exception
     *    is thrown.
     *  - Functions, which accept a default value do NOT throw
     *    an exception if the child is not present. These
     *    rather return the default value.
     *  - If a often used function is not present - the
     *    implementation thereof is simple.
     *
     *  Such helper classes may be defined locally by any user
     *  since XML element handles may easily be transferred.
     *  Run-time exceptions occur however, if non-exiting child elements
     *  are accessed.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    struct ChildValue: public Element {
      /// Default constructor
      ChildValue()
        : Element(Handle_t(0)) {
      }
      /// Constructor from Handle_t
      ChildValue(Handle_t e)
        : Element(e) {
      }
      /// Constructor from Element
      ChildValue(const Element& e)
        : Element(e) {
      }

      /// Access parameters: id
      int id() const;
      /// Access parameters: id, if not present returns default
      int id(int default_value) const;

      /// Access parameters: type
      int type() const;
      /// Access rotation constants: combineHits
      bool combineHits() const;

      /// Access rotation constants: angle
      double angle() const;
      /// Access rotation constants: angle
      double alpha() const;
      /// Access rotation constants: angle
      double beta() const;
      /// Access rotation constants: angle
      double gamma() const;
      /// Access rotation constants: angle
      double delta() const;
      /// Access rotation constants: angle
      double epsilon() const;
      /// Access rotation constants: theta
      double theta() const;
      /// Access rotation constants: thetaBins
      int thetaBins() const;

      /// Access rotation constants: phi
      double phi() const;
      /// Access rotation constants: phiBins
      int phiBins() const;
      /// Access rotation constants: phi0
      double phi0() const;
      /// Access parameters: phi0, if not present returns default
      double phi0(double default_value) const;
      /// Access rotation constants: phi0_offset
      double phi0_offset() const;
      /// Access parameters: phi0_offset, if not present returns default
      double phi0_offset(double default_value) const;
      /// Access rotation constants: phi1
      double phi1() const;
      /// Access parameters: phi1, if not present returns default
      double phi1(double default_value) const;
      /// Access rotation constants: psi
      double psi() const;

      /// Access Tube parameters: zhalf
      double zhalf() const;
      /// Access Tube parameters: deltaphi
      double deltaphi() const;

      /// Access parameters: b
      double b() const;
      /// Access parameters: B
      double B() const;
      /// Access parameters: g
      double g() const;
      /// Access parameters: G
      double G() const;

      /// Access parameters: r
      double r() const;
      /// Access parameters: r, if not present returns default
      double r(double default_value) const;
      /// Access parameters: R
      double R() const;
      /// Access parameters: dr
      double dr() const;
      /// Access parameters: r0
      double r0() const;
      /// Access parameters: dr, if not present returns default
      double dr(double default_value) const;
      /// Access min/max parameters: rmin
      double rmin() const;
      /// Access min/max parameters: rmax
      double rmax() const;
      /// Access min/max parameters: rmin1
      double rmin1() const;
      /// Access min/max parameters: rmax1
      double rmax1() const;
      /// Access min/max parameters: rmin2
      double rmin2() const;
      /// Access min/max parameters: rmax2
      double rmax2() const;
      /// Access parameters: radius
      double radius() const;
      /// Access attribute values: distance
      double distance() const;
      /// Access attribute values: outer_radius
      double outer_radius() const;
      /// Access attribute values: outer_r
      double outer_r() const;
      /// Access attribute values: inner_radius
      double inner_radius() const;
      /// Access attribute values: inner_r
      double inner_r() const;

      /// Access parameters: x
      double x() const;
      /// Access parameters: x, if not present returns default
      double x(double default_val) const;
      /// Access parameters: X
      double X() const;
      /// Access parameters: x0
      double x0() const;
      /// Access parameters: x1
      double x1() const;
      /// Access parameters: x2
      double x2() const;
      /// Access parameters: dx
      double dx() const;
      /// Access parameters: dx, if not present returns default
      double dx(double default_value) const;
      /// Access min/max parameters: xmax
      double xmin() const;
      /// Access min/max parameters: xmax
      double xmax() const;
      /// Access min/max parameters: x_offset
      double x_offset() const;
      /// Access min/max parameters: dim_x
      double dim_x() const;

      /// Access parameters: y
      double y() const;
      /// Access parameters: y, if not present returns default
      double y(double default_val) const;
      /// Access parameters: Y
      double Y() const;
      /// Access parameters: y0
      double y0() const;
      /// Access parameters: y1
      double y1() const;
      /// Access parameters: y2
      double y2() const;
      /// Access parameters: dy
      double dy() const;
      /// Access parameters: dz, if not present returns default
      double dy(double default_value) const;
      /// Access min/max parameters: ymax
      double ymin() const;
      /// Access min/max parameters: ymax
      double ymax() const;
      /// Access min/max parameters: y_offset
      double y_offset() const;
      /// Access min/max parameters: dim_y
      double dim_y() const;

      /// Access parameters: z
      double z() const;
      /// Access parameters: z, if not present returns default
      double z(double default_val) const;
      /// Access parameters: Z
      double Z() const;
      /// Access parameters: z0
      double z0() const;
      /// Access parameters: z1
      double z1() const;
      /// Access parameters: z2
      double z2() const;
      /// Access parameters: dz
      double dz() const;
      /// Access parameters: dz, if not present returns default
      double dz(double default_value) const;
      /// Access min/max parameters: zmax
      double zmin() const;
      /// Access min/max parameters: zmax
      double zmax() const;
      /// Access attribute values: outer_z
      double outer_z() const;
      /// Access attribute values: inner_z
      double inner_z() const;
      /// Access min/max parameters: z_offset
      double z_offset() const;
      /// Access min/max parameters: dim_z
      double dim_z() const;

      /// Access attribute values: length
      double length() const;
      /// Access attribute values: width
      double width() const;
      /// Access attribute values: height
      double height() const;
      /// Access attribute values: depth
      double depth() const;
      /// Access attribute values: thickness
      double thickness() const;

      /// Access attribute values: z_length
      double z_length() const;
      /// Access attribute values: gap
      double gap() const;
      /// Access attribute values: r_size
      double r_size() const;
      /// Access attribute values: phi_size_max
      double phi_size_max() const;
      /// Access attribute values: reflect
      bool reflect() const;
      /// Access attribute values: reflect
      bool reflect(bool default_value) const;
      /// Access attribute values: crossing_angle
      double crossing_angle() const;
      /// Access attribute values: repeat
      int repeat() const;

      /// Access attribute values: outgoing_r
      double outgoing_r() const;
      /// Access attribute values: incoming_r
      double incoming_r() const;
      /// Access attribute values: offset
      double offset() const;
      /// Access attribute values: offset
      double offset(double default_value) const;
      /// Access attribute values: number
      int number() const;

      /// Access attribute values: nmodules
      int nmodules() const;
      /// Access attribute values: nModules
      int nModules() const;
      /// Access attribute values: RowID
      int RowID() const;
      /// Access attribute values: moduleHeight
      double moduleHeight() const;
      /// Access attribute values: moduleWidth
      double moduleWidth() const;
      /// Access attribute values: modulePitch
      double modulePitch() const;
      /// Access attribute values: modulePosX
      double modulePosX() const;
      /// Access attribute values: modulePosY
      double modulePosY() const;

      /// Access attribute values: nPads
      int nPads() const;
      /// Access attribute values: rowPitch
      double rowPitch() const;
      /// Access attribute values: padPitch
      double padPitch() const;
      /// Access attribute values: rowHeight
      double rowHeight() const;
      /// Access attribute values: padType
      std::string padType() const;

      /// Access attribute values: numsides
      int numsides() const;
      /// Access attribute values: nsides
      int nsides() const;
      /// Access attribute values: nsides_inner
      int nsides_inner() const;
      /// Access attribute values: nsides_outer
      int nsides_outer() const;

      /// Access attribute values: phi_tilt
      double phi_tilt() const;
      /// Access attribute values: nphi
      int nphi() const;
      /// Access attribute values: rc
      double rc() const;

      /// Access attribute values: zstart
      double zstart() const;
      /// Access attribute values: nz
      int nz() const;

      /// Access attribute values: start
      double start() const;
      /// Access attribute values: end
      double end() const;
      /// Access attribute values: inner_field
      double inner_field() const;
      /// Access attribute values: outer_field
      double outer_field() const;

      /// Access attribute values: visible
      bool visible() const;
      /// Access attribute values: show_daughters
      bool show_daughters() const;

      /// Access "name" attribute as STL string
      std::string nameStr() const;
      /// Access "ref" attribute as a string
      std::string refStr() const;
      /// Access "type" attribute as STL string
      std::string typeStr() const;
      /// Access "value" attribute as STL string
      std::string valueStr() const;
      /// Access "module" attribute as STL string
      std::string moduleStr() const;
      /// Access "readout" attribute as STL string
      std::string readoutStr() const;
      /// Access vis attribute as STL string. If not present empty return empty string
      std::string visStr() const;
      /// Access region attribute as STL string. If not present empty return empty string
      std::string regionStr() const;
      /// Access limits attribute as STL string. If not present empty return empty string
      std::string limitsStr() const;
    };

  }       /* End namespace DD4HEP_DIMENSION_NS       */
}         /* End namespace dd4hep                    */

#endif
