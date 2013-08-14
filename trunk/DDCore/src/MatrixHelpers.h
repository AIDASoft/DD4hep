// $Id: Handle.cpp 575 2013-05-17 20:41:52Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_IMP_MATRIXHELPERS_H
#define DD4HEP_IMP_MATRIXHELPERS_H

// Framework include files
#include "DD4hep/Objects.h"

// Forward declarations
class TGeoTranslation;
class TGeoRotation;
class TGeoHMatrix;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {

    TGeoIdentity*     identityTransform();
    TGeoTranslation* _translation(const Position& pos);
    TGeoRotation*    _rotation(const Rotation& rot);
    TGeoRotation*    _rotation3D(const Rotation3D& rot);
    TGeoHMatrix*     _transform(const Transform3D& trans);

  }       /* End namespace Geometry           */
}         /* End namespace DD4hep            */

#endif // DD4HEP_IMP_MATRIXHELPERS_H
