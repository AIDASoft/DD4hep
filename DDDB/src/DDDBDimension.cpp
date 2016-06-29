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

// Framework includes
#include "XML/XMLDimension.inl"
#include "DDDB/DDDBDimension.h"
#include "DDDB/DDDBTags.h"

using namespace DD4hep::XML;

XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, alp1)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, alp2)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeX)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeX1)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeX2)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeX3)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeX4)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeY)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeY1)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeY2)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, sizeZ)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, rotX)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, rotY)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, rotZ)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, outerRadius)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, outerRadiusPZ)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, outerRadiusMZ)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, innerRadius)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, innerRadiusPZ)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, innerRadiusMZ)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, radlen)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, lambda)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, fractionmass)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, ionization)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, phiAngle)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, startPhiAngle)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, deltaPhiAngle)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, thetaAngle)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, startThetaAngle)
XML_ATTR_NS_ACCESSOR_DOUBLE(DD4hep::DDDB::dddb_dim_t, deltaThetaAngle)

XML_ATTR_NS_ACCESSOR_INT(   DD4hep::DDDB::dddb_dim_t, natoms)
XML_ATTR_NS_ACCESSOR_INT(   DD4hep::DDDB::dddb_dim_t, number)
XML_ATTR_NS_ACCESSOR_INT(   DD4hep::DDDB::dddb_dim_t, number1)
XML_ATTR_NS_ACCESSOR_INT(   DD4hep::DDDB::dddb_dim_t, number2)
XML_ATTR_NS_ACCESSOR_INT(   DD4hep::DDDB::dddb_dim_t, number3)
XML_ATTR_NS_ACCESSOR_STRING(DD4hep::DDDB::dddb_dim_t, symbol)
XML_ATTR_NS_ACCESSOR(std::string,DD4hep::DDDB::dddb_dim_t, href)
XML_ATTR_NS_ACCESSOR(std::string,DD4hep::DDDB::dddb_dim_t, state)
XML_ATTR_NS_ACCESSOR(std::string,DD4hep::DDDB::dddb_dim_t, logvol)
