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
#define UNICODE(x)  extern const ::dd4hep::xml::Tag_t Unicode_##x 
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of DDDB conversion namespace
  namespace DDDB  {
    UNICODE(DDDB);
    UNICODE(DDDB_VIS);

    UNICODE(address);
    UNICODE(alp1);
    UNICODE(alp2);
    UNICODE(author);

    UNICODE(catalog);
    UNICODE(catalogref);
    UNICODE(classID);
    UNICODE(condition);
    UNICODE(conditionref);
    UNICODE(conditioninfo);
    UNICODE(component);
    UNICODE(cons);

    UNICODE(deltaPhiAngle);
    UNICODE(deltaThetaAngle);
    UNICODE(detelem);
    UNICODE(detelemref);

    UNICODE(elementref);

    UNICODE(fractionmass);

    UNICODE(geometryinfo);

    UNICODE(href);

    UNICODE(lambda);

    UNICODE(innerRadius);
    UNICODE(innerRadiusMZ);
    UNICODE(innerRadiusPZ);
    UNICODE(ionization);
    UNICODE(isotoperef);
    UNICODE(item);

    UNICODE(key);
    UNICODE(keytype);

    UNICODE(lvname);
    UNICODE(logvolref);

    UNICODE(map);
    UNICODE(materialref);

    UNICODE(natoms);
    UNICODE(npath);
    UNICODE(number);
    UNICODE(number1);
    UNICODE(number2);
    UNICODE(number3);


    UNICODE(outerRadius);
    UNICODE(outerRadiusMZ);
    UNICODE(outerRadiusPZ);

    UNICODE(paramphysvol);
    UNICODE(paramphysvol2D);
    UNICODE(paramphysvol3D);
    UNICODE(phiAngle);
    UNICODE(paramVector);
    UNICODE(posXYZ);

    UNICODE(radlen);
    UNICODE(rotXYZ);
    UNICODE(rotX);
    UNICODE(rotY);
    UNICODE(rotZ);

    UNICODE(sizeX);
    UNICODE(sizeX1);
    UNICODE(sizeX2);
    UNICODE(sizeX3);
    UNICODE(sizeX4);
    UNICODE(sizeY);
    UNICODE(sizeY1);
    UNICODE(sizeY2);
    UNICODE(sizeZ);
    UNICODE(specific);
    UNICODE(startPhiAngle);
    UNICODE(startThetaAngle);
    UNICODE(state);
    UNICODE(support);
    UNICODE(symbol);


    UNICODE(tabprops);
    UNICODE(tabproperty);
    UNICODE(tabpropertyref);
    UNICODE(thetaAngle);
    UNICODE(transformation);
    UNICODE(tubs);

    UNICODE(userParameter);

    UNICODE(val);
    UNICODE(value);
    UNICODE(valuetype);
    UNICODE(version);
    UNICODE(volumes);
    UNICODE(vismapping);

    UNICODE(x);
    UNICODE(xaxis);
    UNICODE(xunit);

    UNICODE(y);
    UNICODE(yaxis);
    UNICODE(yunit);

    UNICODE(z);
    UNICODE(zaxis);
    UNICODE(zunit);

    //UNICODE();
  }   /* End namespace DDDB       */
}     /* End namespace dd4hep     */

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"

#define _LBU(a) ::dd4hep::DDDB::Unicode_##a

#endif /* DD4HEP_DDDB_DDDBTAGS_H  */
