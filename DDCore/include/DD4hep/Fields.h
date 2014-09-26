// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_GEOMETRY_FIELDS_H
#define DD4HEP_GEOMETRY_FIELDS_H

// Framework include files
#include "DD4hep/NamedObject.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <vector>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    typedef Position Direction;


    /// Handle class describing any field with 3D cartesian vectors for the field strength.
    /**
     *  Generic structure describing any field type (electric or magnetic)
     *  with field components in Cartesian coordinates.
     *
     *  The actual behaviour is solely implemented in the underlying object
     *  class.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
    */
    class CartesianField: public Ref_t {
    public:
      enum FieldType {
        UNKNOWN = 0, ELECTRIC = 0x1, MAGNETIC = 0x2
      };
      typedef std::map<std::string, std::string> PropertyValues;
      typedef std::map<std::string, PropertyValues> Properties;

      /// Internal data class shared by all handles of a given type
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      class Object: public NamedObject {
      public:
        /// Field type
        int type;
        /// Field extensions
        Properties properties;
        /// Default constructor
        Object();
        /// Default destructor
        virtual ~Object();
        /// Call to access the field components at a given location
        virtual void fieldComponents(const double* pos, double* field) = 0;
      };

      /// Default constructor
      CartesianField()
          : Ref_t() {
      }

      /// Constructor to be used when reading the already parsed DOM tree
      CartesianField(const CartesianField& e)
          : Ref_t(e) {
      }

      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> CartesianField(const Handle<Q>& e)
          : Ref_t(e) {
      }

      /// Assignment operator
      CartesianField& operator=(const CartesianField& f) {
        m_element = f.m_element;
        return *this;
      }

      /// Access the field type
      int fieldType() const {
        return data<Object>()->type;
      }

      /// Access the field type (string)
      const char* type() const;

      /// Does the field change the energy of charged particles?
      bool changesEnergy() const;

      /// Returns the 3 field components (x, y, z).
      void value(const Position& pos, Direction& field) const;   // { value(pos,&field.x); }

      /// Returns the 3 field components (x, y, z).
      void value(const Position& pos, double* val) const;   //      { value(&pos.x,val);   }

      /// Returns the 3 field components (x, y, z).
      void value(const double* pos, double* val) const;

      /// Access to properties container
      Properties& properties() const;
    };

    typedef CartesianField ElectricField;
    typedef CartesianField MagneticField;

    /// Class describing a field overlay with several sources
    /**
     *  Generic structure describing any field type (electric or magnetic)
     *  with field components in Cartesian coordinates.
     *
     *  The actual behaviour is solely implemented in the underlying object
     *  class.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class OverlayedField: public Ref_t {
    public:
      enum FieldType {
        ELECTRIC = 0x1, MAGNETIC = 0x2
      };
      typedef std::map<std::string, std::string> PropertyValues;
      typedef std::map<std::string, PropertyValues> Properties;

      /// Internal data class shared by all handles
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      class Object: public NamedObject {
      public:
        int type;
	DD4hep::Geometry::CartesianField electric;
        DD4hep::Geometry::CartesianField magnetic;
        std::vector<DD4hep::Geometry::CartesianField> electric_components;
        std::vector<DD4hep::Geometry::CartesianField> magnetic_components;
        /// Field extensions
        Properties properties;
        /// Default constructor
        Object();
        /// Default destructor
        virtual ~Object();
      };

      /// Default constructor
      OverlayedField()
          : Ref_t() {
      }

      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q>
      OverlayedField(const Handle<Q>& e)
          : Ref_t(e) {
      }

      /// Object constructor
      OverlayedField(const std::string& name);

      /// Access the field type
      int type() const {
        return data<Object>()->type;
      }

      /// Does the field change the energy of charged particles?
      bool changesEnergy() const;

      /// Add a new field component
      void add(CartesianField field);

      /// Returns the 3 electric field components (x, y, z) if many components are present
      void combinedElectric(const Position& pos, double* field) const {
        combinedElectric((const double*) &pos, field);
      }

      /// Returns the 3 electric field components (x, y, z) if many components are present
      void combinedElectric(const double* pos, double* field) const;

      /// Returns the 3 magnetic field components (x, y, z) if many components are present
      void combinedMagnetic(const Position& pos, double* field) const {
        combinedMagnetic((const double*) &pos, field);
      }

      /// Returns the 3 magnetic field components (x, y, z) if many components are present
      void combinedMagnetic(const double* pos, double* field) const;

      /// Returns the 3 electric field components (x, y, z).
      void electricField(const Position& pos, Direction& field) const {
        electricField((const double*) &pos, (double*) &field);
      }

      /// Returns the 3 electric field components (x, y, z).
      void electricField(const Position& pos, double* field) const {
        electricField((double*) &pos, field);
      }

      /// Returns the 3 electric field components (x, y, z).
      void electricField(const double* pos, double* field) const {
        CartesianField f = data<Object>()->electric;
        f.isValid() ? f.value(pos, field) : combinedElectric(pos, field);
      }

      /// Returns the 3 magnetic field components (x, y, z).
      void magneticField(const Position& pos, Direction& field) const {
        magneticField((double*) &pos, (double*) &field);
      }

      /// Returns the 3  magnetic field components (x, y, z).
      void magneticField(const Position& pos, double* field) const {
        magneticField((double*) &pos, field);
      }

      /// Returns the 3  magnetic field components (x, y, z).
      void magneticField(const double* pos, double* field) const {
        CartesianField f = data<Object>()->magnetic;
        f.isValid() ? f.value(pos, field) : combinedMagnetic(pos, field);
      }

      /// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
      void electromagneticField(const Position& pos, double* val) const {
        electromagneticField((double*) &pos, val);
      }

      /// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
      void electromagneticField(const double* pos, double* val) const;

      /// Access to properties container
      Properties& properties() const;
    };

    typedef OverlayedField ElectroMagneticField;

  } /* End namespace Geometry           */
} /* End namespace DD4hep             */
#endif    /* DD4HEP_GEOMETRY_FIELDS_H         */
