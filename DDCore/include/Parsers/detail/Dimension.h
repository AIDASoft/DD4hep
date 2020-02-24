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
 *  Also NO header guards!
 *  Include XML/XMLDimension.h or JSON/Dimension.h !
 */


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace DD4HEP_DIMENSION_NS  {

    /// XML Handle_t to xml elements with predefined attribute access
    /**
     *  Helper class to access any field in a xml tag in a
     *  very easy way.
     *  - You may assign any xml handle to a dimension object
     *  - Any attribute of this xml element may then be accessed
     *    by it's natural way. All possible attribute names are
     *    reflected by the Dimension object's member functions.
     *  - If an attribute is requested and not present, a exception
     *    is thrown.
     *  - Functions, which accept a default value do NOT throw
     *    an exception if the attribute is not present. These
     *    rather return the default value.
     *  - If a often used function is not present - the
     *    implementation thereof is simple.
     *
     *  Such helper classes may be defined locally by any user
     *  since XML element handles may easily be transferred.
     *  Run-time exceptions occur however, if non-exiting attributes
     *  are accessed.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    struct Dimension: public Element {
      /// Default constructor
      Dimension()
        : Element(Handle_t(0)) {
      }
      /// Constructor from Handle_t
      Dimension(Handle_t e)
        : Element(e) {
      }
      /// Constructor from Element
      Dimension(const Element& e)
        : Element(e) {
      }

      /// Access parameters: id
      int id() const;
      /// Access parameters: id, if not present returns default
      int id(int default_value) const;

      /// Access parameters: station
      int station() const;

      /// Access parameters: type
      int type() const;
      /// Access rotation constants: combineHits
      bool combineHits() const;

      /// Access rotation constants: angle
      double angle() const;
      /// Access rotation constants: alpha
      double alpha() const;
      /// Access rotation constants: alpha
      double alpha(double default_value) const;
      /// Access rotation constants: alpha1
      double alpha1() const;
      /// Access rotation constants: alpha1
      double alpha1(double default_value) const;
      /// Access rotation constants: alpha2
      double alpha2() const;
      /// Access rotation constants: alpha2
      double alpha2(double default_value) const;
      /// Access rotation constants: angle
      double beta() const;
      /// Access rotation constants: angle
      double beta(double default_value) const;
      /// Access rotation constants: angle
      double gamma() const;
      /// Access rotation constants: angle
      double gamma(double default_value) const;
      /// Access rotation constants: angle
      double delta() const;
      /// Access rotation constants: angle
      double delta(double default_value) const;
      /// Access rotation constants: angle
      double epsilon() const;
      /// Access rotation constants: angle
      double epsilon(double default_value) const;
      /// Access rotation constants: theta
      double theta() const;
      /// Access rotation constants: theta
      double theta(double default_value) const;
      /// Access rotation constants: deltatheta
      double deltatheta() const;
      /// Access rotation constants: deltatheta
      double deltatheta(double default_value) const;
      /// Access rotation constants: thetaBins
      int thetaBins() const;

      /// Access rotation constants: phi
      double phi() const;
      /// Access rotation constants: phi
      double phi(double default_value) const;
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
      /// Access rotation constants: phi2
      double phi2() const;
      /// Access parameters: phi2, if not present returns default
      double phi2(double default_value) const;
      /// Access rotation constants: psi
      double psi() const;
      /// Access rotation constants: psi
      double psi(double default_value) const;

      /// Access Tube parameters: zhalf
      double zhalf() const;
      /// Access Tube parameters: deltaphi
      double deltaphi() const;
      /// Access Tube parameters: deltaphi
      double deltaphi(double default_value) const;
      /// Access rotation constants: startphi
      double startphi() const;
      /// Access rotation constants: startphi
      double startphi(double default_value) const;
      /// Access rotation constants: twist
      double twist() const;
      /// Access rotation constants: twist
      double twist(double default_value) const;

      /// Access parameters: a
      double a() const;
      /// Access parameters: a
      double a(double default_value) const;
      /// Access parameters: A
      double A() const;
      /// Access parameters: A
      double A(double default_value) const;
      /// Access parameters: b
      double b() const;
      /// Access parameters: b
      double b(double default_value) const;
      /// Access parameters: c
      double c() const;
      /// Access parameters: c
      double c(double default_value) const;
      ///Access parameters: distance
      double distance() const;
      ///Access parameters: distance
      double distance(double default_value) const;
      /// Access parameters: fraction
      double fraction()  const;
      /// Access parameters: fraction
      double fraction(double default_value)  const;
      /// Access parameters: B
      double B() const;
      /// Access parameters: B
      double B(double default_value) const;
      /// Access parameters: g
      double g() const;
      /// Access parameters: g
      double g(double default_value) const;
      /// Access parameters: G
      double G() const;
      /// Access parameters: G
      double G(double default_value) const;
      /// Access parameters: I
      double I() const;
      /// Access parameters: I
      double I(double default_value) const;

      /// Access parameters: r
      double r() const;
      /// Access parameters: r, if not present returns default
      double r(double default_value) const;
      /// Access parameters: R
      double R() const;
      /// Access parameters: R
      double R(double default_value) const;
      /// Access parameters: dr
      double dr() const;
      /// Access parameters: dr, if not present returns default
      double dr(double default_value) const;
      /// Access parameters: r0
      double r0() const;
      /// Access parameters: r0
      double r0(double default_value) const;
      /// Access min/max parameters: rmin
      double rmin() const;
      /// Access min/max parameters: rmin
      double rmin(double default_val) const;
      /// Access min/max parameters: rmax
      double rmax() const;
      /// Access min/max parameters: rmax
      double rmax(double default_val) const;
      /// Access min/max parameters: rmin1
      double rmin1() const;
      /// Access min/max parameters: rmin1
      double rmin1(double default_val) const;
      /// Access min/max parameters: rmax1
      double rmax1() const;
      /// Access min/max parameters: rmax1
      double rmax1(double default_val) const;
      /// Access min/max parameters: rmin2
      double rmin2() const;
      /// Access min/max parameters: rmin2
      double rmin2(double default_val) const;
      /// Access min/max parameters: rmax2
      double rmax2() const;
      /// Access min/max parameters: rmax2
      double rmax2(double default_val) const;
      /// Access parameters: radius
      double radius() const;
      /// Access parameters: radius
      double radius(double default_val) const;
      /// Access attribute values: outer_radius
      double outer_radius() const;
      /// Access attribute values: outer_r
      double outer_r() const;
      /// Access attribute values: inner_radius
      double inner_radius() const;
      /// Access attribute values: inner_r
      double inner_r() const;

      /// Access parameters: v
      double v() const;
      /// Access parameters: v, if not present returns default
      double v(double default_val) const;
      /// Access parameters: V
      double V() const;
      /// Access parameters: V, if not present returns default
      double V(double default_val) const;
      /// Access parameters: dv
      double dv() const;
      /// Access parameters: dv, if not present returns default
      double dv(double default_val) const;
      /// Access parameters: v0
      double v0() const;
      /// Access parameters: v0, if not present returns default
      double v0(double default_val) const;
      /// Access parameters: v1
      double v1() const;
      /// Access parameters: v1, if not present returns default
      double v1(double default_val) const;
      /// Access parameters: v2
      double v2() const;
      /// Access parameters: v2, if not present returns default
      double v2(double default_val) const;
      /// Access parameters: v3
      double v3() const;
      /// Access parameters: v3, if not present returns default
      double v3(double default_val) const;
      /// Access parameters: v4
      double v4() const;
      /// Access parameters: v4, if not present returns default
      double v4(double default_val) const;
      /// Access parameters: vmin
      double vmin() const;
      /// Access parameters: vmin, if not present returns default
      double vmin(double default_val) const;
      /// Access parameters: vmax
      double vmax() const;
      /// Access parameters: vmax, if not present returns default
      double vmax(double default_val) const;
      /// Access parameters: v_offset
      double v_offset() const;
      /// Access parameters: v_offset, if not present returns default
      double v_offset(double default_val) const;
      /// Access parameters: dim_v
      double dim_v() const;
      /// Access parameters: dim_v, if not present returns default
      double dim_v(double default_val) const;

      /// Access parameters: x
      double x() const;
      /// Access parameters: x, if not present returns default
      double x(double default_val) const;
      /// Access parameters: X
      double X() const;
      /// Access parameters: X
      double X(double default_val) const;
      /// Access parameters: x0
      double x0() const;
      /// Access parameters: x0
      double x0(double default_val) const;
      /// Access parameters: x1
      double x1() const;
      /// Access parameters: x1
      double x1(double default_val) const;
      /// Access parameters: x2
      double x2() const;
      /// Access parameters: x2
      double x2(double default_val) const;
      /// Access parameters: x3
      double x3() const;
      /// Access parameters: x3
      double x3(double default_val) const;
      /// Access parameters: x4
      double x4() const;
      /// Access parameters: x4
      double x4(double default_val) const;
      /// Access parameters: dx
      double dx() const;
      /// Access parameters: dx, if not present returns default
      double dx(double default_value) const;
      /// Access min/max parameters: xmax
      double xmin() const;
      /// Access min/max parameters: xmax
      double xmin(double default_value) const;
      /// Access min/max parameters: xmax
      double xmax() const;
      /// Access min/max parameters: xmax
      double xmax(double default_value) const;
      /// Access min/max parameters: x_offset
      double x_offset() const;
      /// Access min/max parameters: x_offset
      double x_offset(double default_value) const;
      /// Access min/max parameters: dim_x
      double dim_x() const;
      /// Access min/max parameters: dim_x
      double dim_x(double default_value) const;

      /// Access parameters: y
      double y() const;
      /// Access parameters: y, if not present returns default
      double y(double default_val) const;
      /// Access parameters: Y
      double Y() const;
      /// Access parameters: Y
      double Y(double default_val) const;
      /// Access parameters: y0
      double y0() const;
      /// Access parameters: y0
      double y0(double default_value) const;
      /// Access parameters: y1
      double y1() const;
      /// Access parameters: y1
      double y1(double default_value) const;
      /// Access parameters: y2
      double y2() const;
      /// Access parameters: y2
      double y2(double default_value) const;
      /// Access parameters: dy
      double dy() const;
      /// Access parameters: dz, if not present returns default
      double dy(double default_value) const;
      /// Access min/max parameters: ymax
      double ymin() const;
      /// Access min/max parameters: ymax
      double ymin(double default_value) const;
      /// Access min/max parameters: ymax
      double ymax() const;
      /// Access min/max parameters: ymax
      double ymax(double default_value) const;
      /// Access min/max parameters: y_offset
      double y_offset() const;
      /// Access min/max parameters: y_offset
      double y_offset(double default_value) const;
      /// Access min/max parameters: dim_y
      double dim_y() const;
      /// Access min/max parameters: dim_y
      double dim_y(double default_value) const;

      /// Access parameters: z
      double z() const;
      /// Access parameters: z, if not present returns default
      double z(double default_val) const;
      /// Access parameters: Z
      double Z() const;
      /// Access parameters: Z
      double Z(double default_val) const;
      /// Access parameters: z0
      double z0() const;
      /// Access parameters: z0
      double z0(double default_val) const;
      /// Access parameters: z1
      double z1() const;
      /// Access parameters: z1
      double z1(double default_val) const;
      /// Access parameters: z2
      double z2() const;
      /// Access parameters: z2
      double z2(double default_val) const;
      /// Access parameters: dz
      double dz() const;
      /// Access parameters: dz, if not present returns default
      double dz(double default_value) const;
      /// Access pos/neg parameters: zpos
      double zpos() const;
      /// Access pos/neg parameters: zpos
      double zpos(double default_value) const;
      /// Access pos/neg parameters: zneg
      double zneg() const;
      /// Access pos/neg parameters: zneg
      double zneg(double default_value) const;
      /// Access min/max parameters: zmin
      double zmin() const;
      /// Access min/max parameters: zmin
      double zmin(double default_value) const;
      /// Access min/max parameters: zmax
      double zmax() const;
      /// Access min/max parameters: zmax
      double zmax(double default_value) const;
      /// Access attribute values: outer_z
      double outer_z() const;
      /// Access attribute values: inner_z
      double inner_z() const;
      /// Access attribute values: outer_stereo
      double outer_stereo() const;
      /// Access attribute values: inner_stereo
      double inner_stereo() const;
      /// Access min/max parameters: z_offset
      double z_offset() const;
      /// Access min/max parameters: z_offset
      double z_offset(double default_value) const;
      /// Access min/max parameters: dim_z
      double dim_z() const;
      /// Access min/max parameters: dim_z
      double dim_z(double default_value) const;

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
      /// Access attribute values: nocore
      bool nocore() const;
      /// Access attribute values: nocore
      bool nocore(bool default_value) const;
      /// Access attribute values: crossing_angle
      double crossing_angle() const;
      /// Access attribute values: repeat
      int repeat() const;

      /// Access attribute values: coefficient
      double coefficient() const;
      /// Access attribute values: coefficient
      double coefficient(double default_value) const;

      /// Access attribute values: scale
      double scale() const;
      /// Access attribute values: scale
      double scale(double default_value) const;

      /// Access attribute values: skew
      double skew() const;
      /// Access attribute values: skew
      double skew(double default_value) const;

      /// Access attribute values: outgoing_r
      double outgoing_r() const;
      /// Access attribute values: incoming_r
      double incoming_r() const;
      /// Access attribute values: offset
      double offset() const;
      /// Access attribute values: offset
      double offset(double default_value) const;

      /// Access attribute values: item
      int item() const;
      /// Access attribute values: items
      int items() const;
      /// Access attribute values: number
      int number() const;
      /// Access attribute values: nmodules
      int nmodules() const;
      /// Access attribute values: nsegments
      int nsegments() const;
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
      int    nphi() const;
      /// Access attribute values: rc
      double rc() const;

      /// Access attribute values: zstart
      double zstart() const;
      /// Access attribute values: nz
      int    nz() const;
      /// Access attribute values: key
      int    key() const;
      /// Access attribute values: key_min
      int    key_min() const;
      /// Access attribute values: key_max
      int    key_max() const;
      /// Access attribute values: key_val
      int    key_val() const;
      /// Access attribute values: key_value
      int    key_value() const;

      /// Access attribute values: start
      double start() const;
      /// Access attribute values: start
      double start(double default_value) const;
      /// Access attribute values: start_x
      double start_x() const;
      /// Access attribute values: start_x
      double start_x(double default_value) const;
      /// Access attribute values: start_y
      double start_y() const;
      /// Access attribute values: start_y
      double start_y(double default_value) const;
      /// Access attribute values: start_z
      double start_z() const;
      /// Access attribute values: start_z
      double start_z(double default_value) const;

      /// Access attribute values: step
      double step() const;
      /// Access attribute values: step
      double step(double default_value) const;
      /// Access attribute values: step_x
      double step_x() const;
      /// Access attribute values: step_x
      double step_x(double default_value) const;
      /// Access attribute values: step_y
      double step_y() const;
      /// Access attribute values: step_y
      double step_y(double default_value) const;
      /// Access attribute values: step_z
      double step_z() const;
      /// Access attribute values: step_z
      double step_z(double default_value) const;

      /// Access attribute values: end
      double end() const;
      /// Access attribute values: end
      double end(double default_value) const;
      /// Access attribute values: inner_field
      double inner_field() const;
      /// Access attribute values: outer_field
      double outer_field() const;

      /// Access attribute values: visible
      bool   visible() const;
      /// Access attribute values: show_daughters
      bool   show_daughters() const;

      /// Access min/max parameters: correction
      double correction() const;
      /// Access min/max parameters: correction
      double correction(double default_value) const;
      /// Access min/max parameters: cut
      double cut() const;
      /// Access min/max parameters: cut
      double cut(double default_value) const;
      /// Access min/max parameters: threshold
      double threshold() const;
      /// Access min/max parameters: threshold
      double threshold(double default_value) const;
      /// Access min/max parameters: eunit
      double eunit() const;
      /// Access min/max parameters: eunit
      double eunit(double default_value) const;
      /// Access min/max parameters: lunit
      double lunit() const;
      /// Access min/max parameters: lunit
      double lunit(double default_value) const;

      /// Access constants: temperature
      double temperature() const;
      /// Access constants: temperature
      double temperature(double default_value) const;
      /// Access constants: pressure
      double pressure() const;
      /// Access constants: pressure
      double pressure(double default_value) const;
      /// Access constants: density
      double density() const;
      /// Access constants: density
      double density(double default_value) const;

      /// Access child element with tag "dimensions" as Dimension object
      Dimension dimensions(bool throw_if_not_present = true) const;
      /// Access child element with tag "params" as Dimension object
      Dimension params(bool throw_if_not_present = true) const;
      /// Access child element with tag "parameters" as Dimension object
      Dimension parameters(bool throw_if_not_present = true) const;
      /// Access child element with tag "position" as Dimension object
      Dimension position(bool throw_if_not_present = true) const;
      /// Access child element with tag "rotation" as Dimension object
      Dimension rotation(bool throw_if_not_present = true) const;
      /// Access child element with tag "cone" as Dimension object
      Dimension cone(bool throw_if_not_present = true) const;
      /// Access child element with tag "sphere" as Dimension object
      Dimension sphere(bool throw_if_not_present = true) const;
      /// Access child element with tag "torus" as Dimension object
      Dimension torus(bool throw_if_not_present = true) const;
      /// Access child element with tag "trap" as Dimension object
      Dimension trap(bool throw_if_not_present = true) const;
      /// Access child element with tag "trapezoid" as Dimension object
      Dimension trapezoid(bool throw_if_not_present = true) const;
      /// Access child element with tag "trd" as Dimension object
      Dimension trd(bool throw_if_not_present = true) const;
      /// Access child element with tag "tubs" as Dimension object
      Dimension tubs(bool throw_if_not_present = true) const;
      /// Access child element with tag "staves" as Dimension object
      Dimension staves(bool throw_if_not_present = true) const;
      /// Access child element with tag "beampipe" as Dimension object
      Dimension beampipe(bool throw_if_not_present = true) const;
      /// Access child element with tag "envelope" as Dimension object
      Dimension envelope(bool throw_if_not_present = true) const;
      /// Access child element with tag "shape" as Dimension object
      Dimension shape(bool throw_if_not_present = true) const;
      /// Access child element with tag "solid" as Dimension object
      Dimension solid(bool throw_if_not_present = true) const;

      /// Access "name" attribute as STL string
      std::string nameStr() const;
      /// Access "ref" attribute as a string
      std::string refStr() const;
      /// Access "type" attribute as STL string
      std::string typeStr() const;
      /// Access "value" attribute as STL string
      std::string valueStr() const;
      /// Access "label" attribute as STL string
      std::string labelStr() const;
      /// Access "symbol" attribute as STL string
      std::string symbolStr() const;
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

  }       /* End namespace DD4HEP_DIMENSION_NS        */
}         /* End namespace dd4hep                     */
