// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DIMENSION_H
#define DD4HEP_DDDB_DIMENSION_H

// Framework include files
#include "XML/XMLTags.h"
#include "XML/XMLDimension.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DDDB {

    /// XML Handle to xml elements with predefined attribute access
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
    struct dddb_dim_t : public XML::Dimension {
      /// Default constructor
      dddb_dim_t() : XML::Dimension(0) {
      }
      /// Constructor from Handle
      dddb_dim_t(XML::Handle_t e) : XML::Dimension(e) {
      }
      /// Constructor from Element
      dddb_dim_t(const XML::Element& e) : XML::Dimension(e) {
      }
      double alp1() const;
      double alp1(double default_value) const;
      double alp2() const;
      double alp2(double default_value) const;
      double sizeX() const;
      double sizeX(double default_value) const;
      double sizeX1() const;
      double sizeX1(double default_value) const;
      double sizeX2() const;
      double sizeX2(double default_value) const;
      double sizeX3() const;
      double sizeX3(double default_value) const;
      double sizeX4() const;
      double sizeX4(double default_value) const;
      double sizeY() const;
      double sizeY(double default_value) const;
      double sizeY1() const;
      double sizeY1(double default_value) const;
      double sizeY2() const;
      double sizeY2(double default_value) const;
      double sizeZ() const;
      double sizeZ(double default_value) const;
      double rotX() const;
      double rotX(double default_value) const;
      double rotY() const;
      double rotY(double default_value) const;
      double rotZ() const;
      double rotZ(double default_value) const;

      double phiAngle() const;
      double phiAngle(double default_value) const;
      double startPhiAngle() const;
      double startPhiAngle(double default_value) const;
      double deltaPhiAngle() const;
      double deltaPhiAngle(double default_value) const;
      double thetaAngle() const;
      double thetaAngle(double default_value) const;
      double startThetaAngle() const;
      double startThetaAngle(double default_value) const;
      double deltaThetaAngle() const;
      double deltaThetaAngle(double default_value) const;

      double outerRadius() const;
      double outerRadius(double default_value) const;
      double innerRadius() const;
      double innerRadius(double default_value) const;
      double innerRadiusMZ() const;
      double innerRadiusMZ(double default_value) const;
      double innerRadiusPZ() const;
      double innerRadiusPZ(double default_value) const;
      double outerRadiusMZ() const;
      double outerRadiusMZ(double default_value) const;
      double outerRadiusPZ() const;
      double outerRadiusPZ(double default_value) const;
      double radlen() const;
      double radlen(double default_value) const;
      double lambda() const;
      double lambda(double default_value) const;
      double ionization() const;
      double ionization(double default_value) const;
      double fractionmass() const;
      double fractionmass(double default_value) const;
      int natoms() const;
      int natoms(int default_value) const;
      int number() const;
      int number(int default_value) const;
      int number1() const;
      int number1(int default_value) const;
      int number2() const;
      int number2(int default_value) const;
      int number3() const;
      int number3(int default_value) const;
      std::string href() const;
      std::string state() const;
      std::string symbol() const;
      std::string symbol(std::string default_value) const;
      std::string logvol() const;
    };
  } /* End namespace DDDB    */
} /* End namespace DD4hep    */
#endif    /* DD4HEP_DDDB_XMLDIMENSION_H   */
