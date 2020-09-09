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
#ifndef DD4HEP_MATRIXHELPERS_H
#define DD4HEP_MATRIXHELPERS_H

// Framework include files
#include "DD4hep/Objects.h"

// Forward declarations
class TGeoTranslation;
class TGeoRotation;
class TGeoHMatrix;
class TGeoMatrix;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description implementation details
  namespace detail   {

    /// Namespace for the AIDA detector description matrix helpers
    namespace matrix {

      /// Access the TGeo identity transformation                                               \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoIdentity*    _identity();
      /// Extract the scale part of a TGeoMatrix                                                \ingroup DD4HEP \ingroup DD4HEP_CORE
      ROOT::Math::XYZVector _scale(const TGeoMatrix* matrix);
      /// Extract the scale part of a TGeoMatrix                                                \ingroup DD4HEP \ingroup DD4HEP_CORE
      ROOT::Math::XYZVector _scale(const TGeoMatrix& matrix);

      /// Convert a Position object to a TGeoTranslation                                        \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoTranslation* _translation(const Position& pos);
      /// Extract the translational component of a TGeoMatrix as a Position                     \ingroup DD4HEP \ingroup DD4HEP_CORE
      Position         _translation(const TGeoMatrix* matrix);
      /// Extract the translational component of a TGeoMatrix as a Position                     \ingroup DD4HEP \ingroup DD4HEP_CORE
      Position         _translation(const TGeoMatrix& matrix);

      /// Convert a RotationZYX object to a newly created TGeoRotation                          \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoRotation*    _rotationZYX(const RotationZYX& rot);
      /// Convert a Rotation3D object to a  newly createdTGeoRotation                           \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoRotation*    _rotation3D(const Rotation3D& rot);
      /// Extract the rotational part of a TGeoMatrix as a Rotation3D                           \ingroup DD4HEP \ingroup DD4HEP_CORE
      Rotation3D       _rotation3D(const TGeoMatrix* matrix);
      /// Extract the rotational part of a TGeoMatrix as a Rotation3D                           \ingroup DD4HEP \ingroup DD4HEP_CORE
      Rotation3D       _rotation3D(const TGeoMatrix& matrix);
      
      /// Convert a Transform3D object to a newly created TGeoHMatrix                           \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix*     _transform(const Transform3D& trans);
      /// Convert a Position object to a newly created TGeoHMatrix                              \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix*     _transform(const Position& pos);
      /// Convert a RotationZYX object to a newly created TGeoHMatrix                           \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix*     _transform(const RotationZYX& rot);
      /// Convert a Rotation3D object to a newly created TGeoHMatrix                            \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix*     _transform(const Rotation3D& rot3D);
      /// Convert a Position followed by a RotationZYX to a newly created TGeoHMatrix           \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix*     _transform(const Position& pos, const RotationZYX& rot);

      /// Set a Transform3D object to a TGeoHMatrix                                             \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix&     _transform(TGeoHMatrix& mat, const Transform3D& trans);
      /// Set a Position object (translation) to a TGeoHMatrix                                  \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix&     _transform(TGeoHMatrix& mat, const Position& pos);
      /// Set a RotationZYX object to a TGeoHMatrix                                             \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix&     _transform(TGeoHMatrix& mat, const RotationZYX& rot);
      /// Set a Rotation3D object to a TGeoHMatrix                                              \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix&     _transform(TGeoHMatrix& mat, const Rotation3D& rot3D);
      /// Set a Position followed by a RotationZYX to a TGeoHMatrix                             \ingroup DD4HEP \ingroup DD4HEP_CORE
      TGeoHMatrix&     _transform(TGeoHMatrix& mat, const Position& pos, const RotationZYX& rot);

      /// Convert a TGeoMatrix object to a generic Transform3D                                  \ingroup DD4HEP \ingroup DD4HEP_CORE
      Transform3D      _transform(const TGeoMatrix* matrix);
      Transform3D      _transform(const TGeoMatrix& matrix);

      /// Decompose a generic ROOT Matrix into a translation (Position) and a Rotation3D        \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const TGeoMatrix& trafo, Position& pos, Rotation3D& rot);
      /// Decompose a generic ROOT Matrix into a translation (Position) and a RotationZYX       \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const TGeoMatrix& trafo, Position& pos, RotationZYX& rot);
      /// Decompose a generic ROOT Matrix into a translation (Position) and XYZAngles           \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const TGeoMatrix& trafo, Position& pos, XYZAngles& rot);
      /// Decompose a generic Transform3D into a translation (Position) and a Rotation3D        \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const Transform3D& trafo, Position& pos, Rotation3D& rot);
      /// Decompose a generic Transform3D into a translation (Position) and a RotationZYX       \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const Transform3D& trafo, Position& pos, RotationZYX& rot);
      /// Decompose a generic Transform3D into a translation (Position) and XYZAngles \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const Transform3D& trafo, Position& pos, XYZAngles& rot);
      /// Decompose a generic Transform3D into a translation (Translation3D) and a RotationZYX  \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const Transform3D& trafo, Translation3D& pos, RotationZYX& rot);
      /// Decompose a generic Transform3D into a translation (Translation3D) and XYZAngles      \ingroup DD4HEP \ingroup DD4HEP_CORE
      void _decompose(const Transform3D& trafo, Translation3D& pos, XYZAngles& rot);

      /// Convert a 3x3 rotation matrix to XYZAngles                                            \ingroup DD4HEP \ingroup DD4HEP_CORE
      XYZAngles _xyzAngles(const double* matrix);
      /// Convert a the rotation part of a TGeoMatrix to XYZAngles                              \ingroup DD4HEP \ingroup DD4HEP_CORE
      XYZAngles _xyzAngles(const TGeoMatrix* matrix);

      enum MatrixEqualityCode  {
        MATRICES_EQUAL = 1,
        MATRICES_DIFFER_TRANSLATION = 2,
        MATRICES_DIFFER_ROTATION = 4
      };
      /// Check matrices for equality
      /** Return codes:
       *  MATRICES_EQUAL:                matrices left and right are equal
       *  MATRICES_DIFFER_TRANSLATION:   matrices differ in translation
       *  MATRICES_DIFFER_ROTATION:      matrices differ in rotation
       *  or combination: MATRICES_DIFFER_TRANSLATION|MATRICES_DIFFER_ROTATION
       */
      int _matrixEqual(const TGeoMatrix& left, const TGeoMatrix& right);
    }
  }
}       /* End namespace dd4hep            */

#endif // DD4HEP_MATRIXHELPERS_H
