// $Id: $
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

#ifndef DD4HEP_IMP_MATRIXHELPERS_H
#define DD4HEP_IMP_MATRIXHELPERS_H

// Framework include files
#include "DD4hep/Objects.h"

// Forward declarations
class TGeoTranslation;
class TGeoRotation;
class TGeoHMatrix;
class TGeoMatrix;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    typedef Position XYZAngles;
  
    /// Access the TGeo identity transformation  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoIdentity*    identityTransform();
    /// Convert a Position object to a TGeoTranslation  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoTranslation* _translation(const Geometry::Position& pos);
    /// Convert a RotationZYX object to a TGeoRotation  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoRotation*    _rotationZYX(const Geometry::RotationZYX& rot);
    /// Convert a Rotation3D object to a TGeoRotation  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoRotation*    _rotation3D(const Geometry::Rotation3D& rot);
    /// Convert a Transform3D object to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoHMatrix*     _transform(const Geometry::Transform3D& trans);
    /// Convert a Position object to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoHMatrix*     _transform(const Geometry::Position& pos);
    /// Convert a RotationZYX object to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoHMatrix*     _transform(const Geometry::RotationZYX& rot);
    /// Convert a Rotation3D object to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoHMatrix*     _transform(const Geometry::Rotation3D& rot3D);
    /// Convert a Position followed by a RotationZYX to a TGeoHMatrix  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    TGeoHMatrix*     _transform(const Geometry::Position& pos, const Geometry::RotationZYX& rot);

    /// Convert a TGeoMatrix object to a generic Transform3D  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    Geometry::Transform3D      _transform(const TGeoMatrix* matrix);

    /// Decompose a generic Transform3D into a translation (Position) and a RotationZYX \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    void _decompose(const Geometry::Transform3D& trafo, Geometry::Position& pos, Geometry::RotationZYX& rot);
    /// Decompose a generic Transform3D into a translation (Geometry::Position) and XYZAngles \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    void _decompose(const Geometry::Transform3D& trafo, Geometry::Position& pos, XYZAngles& rot);
    /// Decompose a generic Transform3D into a translation (Translation3D) and a RotationZYX \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    void _decompose(const Geometry::Transform3D& trafo, Geometry::Translation3D& pos, Geometry::RotationZYX& rot);
    /// Decompose a generic Transform3D into a translation (Translation3D) and XYZAngles \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    void _decompose(const Geometry::Transform3D& trafo, Geometry::Translation3D& pos, XYZAngles& rot);

    /// Convert a 3x3 rotation matrix to XYZAngles  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    XYZAngles _XYZangles(const double* matrix);
    /// Convert a the rotation part of a TGeoMatrix to XYZAngles  \ingroup DD4HEP \ingroup DD4HEP_GEOMETRY
    XYZAngles _XYZangles(const TGeoMatrix* matrix);

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */

#endif // DD4HEP_IMP_MATRIXHELPERS_H
