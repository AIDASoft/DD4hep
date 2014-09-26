// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_GEOMETRY_FIELDTYPES_H
#define DD4HEP_GEOMETRY_FIELDTYPES_H

// Framework include files
#include "DD4hep/Fields.h"
#include <vector>


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Implementation object of a field with constant strength
    /** 
     *  Generic constant uniform field
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class ConstantField: public CartesianField::Object {
    public:
      /// Field direction
      Direction direction;
    public:
      /// Initializing constructor
      ConstantField()
          : direction() {
      }
      /// Call to access the field components at a given location
      virtual void fieldComponents(const double* /* pos */, double* field);
    };

    /// Implementation object of a solenoidal magnetic field.
    /** 
     *  Generic solonoid magnetic field
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class SolenoidField: public CartesianField::Object {
    public:
      double innerField;
      double outerField;
      double minZ;
      double maxZ;
      double innerRadius;
      double outerRadius;

    public:
      /// Initializing constructor
      SolenoidField();
      /// Call to access the field components at a given location
      virtual void fieldComponents(const double* pos, double* field);
    };

    /// Implementation object of a dipole magnetic field.
    /** 
     *  Generic solonoid magnetic field
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class DipoleField: public CartesianField::Object {
    public:
      typedef std::vector<double> Coefficents;
      double zmax;
      double zmin;
      double rmax;
      Coefficents coefficents;
    public:
      /// Initializing constructor
      DipoleField();
      /// Call to access the field components at a given location
      virtual void fieldComponents(const double* pos, double* field);
    };

  } /* End namespace Geometry           */
} /* End namespace DD4hep             */
#endif    /* DD4HEP_GEOMETRY_FIELDTYPES_H     */
