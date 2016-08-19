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
#ifndef DD4HEP_GEOMETRY_ALIGNMENTCONDITION_H
#define DD4HEP_GEOMETRY_ALIGNMENTCONDITION_H

// Framework include files
#include "DD4hep/ConditionsData.h"
#include "DD4hep/ConditionDerived.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    /// Derived condition data-object definition
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class AlignmentConditionData   {
    public:
      typedef Geometry::PlacedVolume  PlacedVolume;
      typedef Geometry::DetElement    DetElement;
      typedef Geometry::Position      Position;
      typedef Geometry::Position      Pivot;
      typedef Geometry::RotationZYX   Rotation;

      enum AlignmentFlags {
	HAVE_NONE = 0,
	HAVE_WORLD_TRAFO = 1<<0,
	HAVE_PARENT_TRAFO = 1<<1,
	HAVE_OTHER = 1<<31
      };

      AlignmentDelta       delta;
      /// Intermediate buffer to store the transformation to the world coordination system
      mutable TGeoHMatrix  worldTrafo;
      /// Intermediate buffer to store the transformation to the parent detector element
      mutable TGeoHMatrix  parentTrafo;
      /// The subdetector placement corresponding to the actual detector element's volume
      PlacedVolume         placement;
      /// Parent condition reference
      Condition            condition;
      /// Flag to remember internally calculated quatities
      mutable unsigned int flag;
      /// Magic word to verify object if necessary
      unsigned int         magic;

    public:
      /// Standard constructor
      AlignmentConditionData();
      /// Default destructor
      virtual ~AlignmentConditionData();
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation()  const;
      /// Create cached matrix to transform to parent coordinates
      const TGeoHMatrix& parentTransformation()  const;
    };

    /// Class describing an alignment condition 
    /**
     *  The Alignment condition describes an alignment delta applied
     *  to a given volume. An instance contains all necessary
     *  alignment data and the corresponding transformations.
     *
     *  Typically this object is derived condition ie. the raw conditions data
     *  are stored as a 'delta' and then get reformatted and re-computed
     *  in the derived quantity.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class AlignmentCondition : public Condition   {
    public:
      typedef Geometry::PlacedVolume  PlacedVolume;
      typedef Geometry::DetElement    DetElement;
      typedef Geometry::Position      Position;
      typedef Geometry::Position      Pivot;
      typedef Geometry::RotationZYX   Rotation;

     public:
      /// Default constructor
      AlignmentCondition();
      /// Copy constructor
      AlignmentCondition(const Condition& c);
      /// Copy constructor
      AlignmentCondition(const AlignmentCondition& c);
      /// Initializing constructor
      AlignmentCondition(Object* p);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> AlignmentCondition(const Handle<Q>& e)
        : Condition(e) {
      }
      /// Assignment operator
      AlignmentCondition& operator=(const AlignmentCondition& c);
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation() const;
      /// Create cached matrix to transform to parent coordinates
      const TGeoHMatrix& parentTransformation() const;

      /// Transformation from local coordinates of the placed volume to the world system
      bool localToWorld(const Position& local, Position& global) const;
      /// Transformation from local coordinates of the placed volume to the parent system
      bool localToParent(const Position& local, Position& parent) const;

      /// Transformation from world coordinates of the local placed volume coordinates
      bool worldToLocal(const Position& global, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      bool parentToLocal(const Position& parent, Position& local) const;
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_ALIGNMENTCONDITION_H */
