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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geoemtry namespace declaration
   */
  namespace Geometry  {

    /** @class ConstantField FieldTypes.h
     *  
     *  Generic constant uniform field
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class ConstantField : public CartesianField::Object {
    public:
      /// Field direction
      Direction direction;
    public:
      /// Initializing constructor
      ConstantField() : direction() { }
      /// Call to access the field components at a given location
      virtual void fieldComponents(const double* /* pos */, double* field);
    };

    /** @class SolenoidField FieldTypes.h DD4hep/FieldTypes.h
     *  
     *  Generic solonoid magnetic field
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class SolenoidField : public CartesianField::Object {
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

    /** @class DipoleField FieldTypes.h DD4hep/FieldTypes.h
     *  
     *  Generic solonoid magnetic field
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class DipoleField  : public CartesianField::Object  {
    public:
      typedef std::vector<double> Coefficents;
      double      zmax;
      double      zmin;
      double      rmax;
      Coefficents coefficents;
    public:
      /// Initializing constructor
      DipoleField();
      /// Call to access the field components at a given location
      virtual void fieldComponents(const double* pos, double* field);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DD4hep             */
#endif    /* DD4HEP_GEOMETRY_FIELDTYPES_H     */
