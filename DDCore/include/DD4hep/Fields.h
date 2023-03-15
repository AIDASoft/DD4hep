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
#ifndef DD4HEP_FIELDS_H
#define DD4HEP_FIELDS_H

// Framework include files
#include "DD4hep/NamedObject.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <vector>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  typedef Position Direction;

  /// Base class describing any field with 3D cartesian vectors for the field strength.
  /** Abstract base class describing any field (electric or magnetic) with 3D cartesian vectors 
   *  for the field strength and positions.
   *  Implementation classes need to overwrite void fieldComponents(const double* pos, double* field).
   *  The actual behaviour is solely implemented in the underlying object class.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class CartesianField: public Handle<NamedObject> {
  public:
    enum FieldType {
      UNKNOWN = 0, ELECTRIC = 0x1, MAGNETIC = 0x2, OVERLAY  = 0x4,
    };
    typedef std::map<std::string, std::map<std::string, std::string> > Properties;

    /// Internal data class shared by all handles of a given type
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class TypedObject : public NamedObject {
    public:
      /// Field type
      int   field_type { UNKNOWN };
      /// Default constructor
      using NamedObject::NamedObject;
    };

    /// Internal data class shared by all handles of a given type
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class Object: public TypedObject {
    public:
      /// Utility definition for concrete implementations
      typedef std::vector<double> Coefficents;
      /// Field extensions
      Properties properties;
      /// Default constructor
      Object();
      /// Default destructor
      virtual ~Object();

      /** Overwrite to compute the field components at a given location -
       *  NB: The field components have to be added to the provided
       *  field vector in order to allow for superposition of the fields.
       */
      virtual void fieldComponents(const double* pos, double* field) = 0;
    };

    /// Default constructor
    CartesianField() = default;

    /// Constructor to be used when reading the already parsed DOM tree
    CartesianField(const CartesianField& e) = default;

    /// Constructor to be used when reading the already parsed DOM tree
    template <typename Q> CartesianField(const Handle<Q>& e) : Ref_t(e) {
    }

    /// Assignment operator
    CartesianField& operator=(const CartesianField& f) = default;

    /// Access the field type
    int fieldType() const {
      return data<Object>()->field_type;
    }

    /// Access the field type (string)
    const char* type() const;

    /// Does the field change the energy of charged particles?
    bool changesEnergy() const;

    /// Returns the 3 field components (x, y, z).
    void value(const Position& pos, Direction& field) const;

    /// Returns the 3 field components (x, y, z).
    void value(const Position& pos, double* val) const;

    /// Returns the 3 field components (x, y, z).
    void value(const double* pos, double* val) const;

    /// Access to properties container
    Properties& properties() const;
  };

  /// Class describing a field overlay with several sources
  /**
   *  Generic structure describing any field type (electric or magnetic)
   *  with field components in Cartesian coordinates.
   *
   *  The actual behaviour is solely implemented in the underlying object
   *  classes. The overlay field is the sum of several magnetic of electric
   *  field components.
   *
   *  The resulting field vectors are computed by the vector addition
   *  of the individual components.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class OverlayedField: public Handle<NamedObject> {
  public:
    enum FieldType {
      ELECTRIC = CartesianField::ELECTRIC,
      MAGNETIC = CartesianField::MAGNETIC,
      OVERLAY  = CartesianField::OVERLAY,
    };
    typedef std::map<std::string, std::string> PropertyValues;
    typedef std::map<std::string, PropertyValues> Properties;

    /// Internal data class shared by all handles
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class Object: public CartesianField::TypedObject {
    public:
      CartesianField electric;
      CartesianField magnetic;
      std::vector<CartesianField> electric_components;
      std::vector<CartesianField> magnetic_components;
      /// Field extensions
      Properties properties;

    public:
      /// Default constructor
      Object();
      /// Default destructor
      virtual ~Object();
    };

    /// Default constructor
    OverlayedField() = default;

    /// Constructor to be used when reading the already parsed DOM tree
    template <typename Q>  OverlayedField(const Handle<Q>& e) : Ref_t(e) {      }

    /// Object constructor
    OverlayedField(const std::string& name);

    /// Access the field type
    int fieldType() const {
      return data<Object>()->field_type;
    }

    /// Does the field change the energy of charged particles?
    bool changesEnergy() const;

    /// Add a new field component
    void add(CartesianField field);

    /// Returns the 3 electric field components (x, y, z) if many components are present
    void combinedElectric(const Position& pos, double* field) const;

    /// Returns the 3 electric field components (x, y, z) if many components are present
    Direction combinedElectric(const Position& pos) const {
      double field[3] = { 0e0, 0e0, 0e0 };
      combinedElectric(pos, field);
      return {field[0], field[1], field[2]};
    }

    /// Returns the 3 electric field components (x, y, z) if many components are present
    void combinedElectric(const double* pos, double* field) const   {
      combinedElectric(Position(pos[0], pos[1], pos[2]), field);
    }

    /// Returns the 3 magnetic field components (x, y, z) if many components are present
    void combinedMagnetic(const Position& pos, double* field) const;

    /// Returns the 3 magnetic field components (x, y, z) at a given position
    Direction combinedMagnetic(const Position& pos) const {
      double field[3] = { 0e0, 0e0, 0e0 };
      combinedMagnetic({pos.X(), pos.Y(), pos.Z()}, field);
      return { field[0], field[1], field[2] };
    }

    /// Returns the 3 magnetic field components (x, y, z) if many components are present
    void combinedMagnetic(const double* pos, double* field) const   {
      combinedMagnetic(Position(pos[0], pos[1], pos[2]), field);
    }

    /// Returns the 3 electric field components (x, y, z).
    void electricField(const Position& pos, double* field) const;

    /// Returns the 3 electric field components (x, y, z) at a given position
    Direction electricField(const Position& pos) const {
      double field[3] = { 0e0, 0e0, 0e0 };
      electricField(pos, field);
      return { field[0], field[1], field[2] };
    }

    /// Returns the 3 electric field components (x, y, z).
    void electricField(const Position& pos, Direction& field) const {
      double fld[3] = { 0e0, 0e0, 0e0 };
      electricField(Position(pos.X(), pos.Y(), pos.Z()), fld);
      field = { fld[0], fld[1], fld[2] };
    }

    /// Returns the 3 electric field components (x, y, z).
    void electricField(const double* pos, double* field) const {
      field[0] = field[1] = field[2] = 0.0;
      CartesianField f = data<Object>()->electric;
      f.isValid() ? f.value(pos, field) : combinedElectric(pos, field);
    }

    /// Returns the 3  magnetic field components (x, y, z).
    void magneticField(const Position& pos, double* field) const;

    /// Returns the 3  magnetic field components (x, y, z).
    void magneticField(const double* pos, double* field) const   {
      magneticField(Position(pos[0], pos[1], pos[2]), field);
    }

    /// Returns the 3 magnetic field components (x, y, z).
    void magneticField(const double* pos, Direction& field) const {
      double fld[3] = { 0e0, 0e0, 0e0 };
      magneticField(Position(pos[0], pos[1], pos[2]), fld);
      field = { fld[0], fld[1], fld[2] };
    }

    /// Returns the 3 electric field components (x, y, z) at a given position
    Direction magneticField(const Position& pos) const {
      double field[3] = { 0e0, 0e0, 0e0 };
      magneticField(pos, field);
      return { field[0], field[1], field[2] };
    }

    /// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
    void electromagneticField(const Position& pos, double* field) const;

    /// Returns the 3 electric (val[0]-val[2]) and magnetic field components (val[3]-val[5]).
    void electromagneticField(const double* pos, double* val) const   {
      electromagneticField(Position(pos[0], pos[1], pos[2]), val);
    }

    /// Access to properties container
    Properties& properties() const;
  };
}         /* End namespace dd4hep             */
#endif // DD4HEP_FIELDS_H
