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
class TGeoMatrix;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {
    typedef Position XYZAngles;
    TGeoIdentity*    identityTransform();
    TGeoTranslation* _translation(const Position& pos);
    TGeoRotation*    _rotationZYX(const RotationZYX& rot);
    TGeoRotation*    _rotation3D(const Rotation3D& rot);
    TGeoHMatrix*     _transform(const Transform3D& trans);
    TGeoHMatrix*     _transform(const Position& pos);
    TGeoHMatrix*     _transform(const RotationZYX& rot);
    TGeoHMatrix*     _transform(const Rotation3D& rot3D);
    TGeoHMatrix*     _transform(const Position& pos, const RotationZYX& rot);

    Transform3D      _transform(const TGeoMatrix* matrix);

    void _decompose(const Transform3D& trafo, Position& pos, RotationZYX& rot);
    void _decompose(const Transform3D& trafo, Position& pos, XYZAngles& rot);
    void _decompose(const Transform3D& trafo, Translation3D& pos, RotationZYX& rot);
    void _decompose(const Transform3D& trafo, Translation3D& pos, XYZAngles& rot);

    XYZAngles _XYZangles(const double* matrix);
    XYZAngles _XYZangles(const TGeoMatrix* matrix);

  } /* End namespace Geometry           */
} /* End namespace DD4hep            */

#endif // DD4HEP_IMP_MATRIXHELPERS_H
