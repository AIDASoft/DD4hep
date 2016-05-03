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
#ifndef DD4HEP_DDDB_DDDBTAGS_H
#define DD4HEP_DDDB_DDDBTAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE 
#define UNICODE(x)  extern const ::DD4hep::XML::Tag_t Unicode_##x 
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of DDDB conversion namespace
  namespace DDDB  {
    UNICODE(DDDB);
    UNICODE(DDDB_VIS);
    UNICODE(elementref);
    UNICODE(materialref);
    UNICODE(catalog);
    UNICODE(catalogref);
    UNICODE(href);
    UNICODE(npath);
    UNICODE(detelem);
    UNICODE(detelemref);
    UNICODE(condition);
    UNICODE(conditionref);
    UNICODE(conditioninfo);
    UNICODE(geometryinfo);
    UNICODE(lvname);
    UNICODE(logvolref);
    UNICODE(paramVector);
    UNICODE(userParameter);
    UNICODE(fractionmass);
    UNICODE(natoms);
    UNICODE(radlen);
    UNICODE(ionization);
    UNICODE(lambda);
    UNICODE(component);
    UNICODE(address);
    UNICODE(isotoperef);
    UNICODE(symbol);
    UNICODE(state);
    UNICODE(support);
    UNICODE(volumes);
    UNICODE(vismapping);

    UNICODE(posXYZ);
    UNICODE(rotXYZ);
    UNICODE(rotX);
    UNICODE(rotY);
    UNICODE(rotZ);
    UNICODE(cons);
    UNICODE(tubs);
    UNICODE(transformation);
    UNICODE(version);
    UNICODE(author);

    UNICODE(number);
    UNICODE(number1);
    UNICODE(number2);
    UNICODE(number3);
    UNICODE(alp1);
    UNICODE(alp2);
    UNICODE(x);
    UNICODE(y);
    UNICODE(z);
    UNICODE(sizeX);
    UNICODE(sizeX1);
    UNICODE(sizeX2);
    UNICODE(sizeX3);
    UNICODE(sizeX4);
    UNICODE(sizeY);
    UNICODE(sizeY1);
    UNICODE(sizeY2);
    UNICODE(sizeZ);
    UNICODE(outerRadius);
    UNICODE(outerRadiusMZ);
    UNICODE(outerRadiusPZ);
    UNICODE(innerRadius);
    UNICODE(innerRadiusMZ);
    UNICODE(innerRadiusPZ);
    UNICODE(tabprops);
    UNICODE(paramphysvol);
    UNICODE(paramphysvol2D);
    UNICODE(phiAngle);
    UNICODE(startPhiAngle);
    UNICODE(deltaPhiAngle);
    UNICODE(thetaAngle);
    UNICODE(startThetaAngle);
    UNICODE(deltaThetaAngle);
    //UNICODE();
    //UNICODE();

  }   /* End namespace DDDB       */
}     /* End namespace DD4hep     */

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"

#define _LBU(a) ::DD4hep::DDDB::Unicode_##a

#endif /* DD4HEP_DDDB_DDDBTAGS_H  */
