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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTS_H
#define DD4HEP_ALIGMENTS_ALIGNMENTS_H

// Framework include files
#include "DD4hep/IOV.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Conditions.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Forward declarations
  class IOV;

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {
    class Condition;
    class ConditionsKey;
    class ConditionsMap;
    
    /// Conditions internal namespace
    namespace Interna  {
      class ConditionObject;
    }
  }
  
  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    using Geometry::LCDD;
    using Geometry::Position;
    using Geometry::DetElement;
    using Geometry::PlacedVolume;
    using Conditions::ConditionsMap;
    
    /// Alignments internal namespace
    namespace Interna  {
      /// Forward declarations
      class AlignmentObject;
    }
    class AlignmentData;
    class Alignment;
    class Delta;

    /// Main handle class to hold an alignment conditions object
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_ALIGN
     */
    class AlignmentCondition : public Handle<Interna::AlignmentObject>   {
    public:
      /// Forward definition of the geometry placement
      typedef Geometry::PlacedVolume    PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef std::vector<PlacedVolume> NodeList;
      /// Forward definition of the key type
      typedef unsigned long long int    key_type;
      /// Forward definition of the iov type
      typedef IOV                       iov_type;

    public:
      /// Default constructor
      AlignmentCondition() : Handle<Object>() {}
      /// Default constructor
      AlignmentCondition(Object* p) : Handle<Object>(p) {}
      /// Assignment constructor from condition object
      AlignmentCondition(Conditions::Interna::ConditionObject* p) : Handle<Object>(p) {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> AlignmentCondition(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Object constructor for pure alignment objects
      AlignmentCondition(const std::string& name);
      /** Interval of validity            */
      /// Access the IOV type
      const IOVType& iovType()  const;
      /// Access the IOV block
      const iov_type& iov()  const;
      /// Access the hash identifier
      key_type key()  const;

      /** Data block (bound type)         */
      /// Data accessor for the use of decorators
      AlignmentData& data();
      /// Data accessor for the use of decorators
      const AlignmentData& data() const;
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation()  const;
      /// Access the alignment/placement matrix with respect to the world
      const TGeoHMatrix& detectorTransformation() const;
      /// Check if object is already bound....
      bool is_bound()  const;
    };

    /// Main handle class to hold an alignment object
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_ALIGN
     */
    class Alignment : public Handle<Interna::AlignmentObject>   {
    public:
      /// Abstract base for processing callbacks to container objects
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Processor {
      public:
        /// Default constructor
        Processor();
        /// Default destructor
        virtual ~Processor() = default;
        /// Container callback for object processing
        virtual int operator()(Alignment alignment) const = 0;
      };

      /// Forward definition of the geometry placement
      typedef Geometry::PlacedVolume             PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef std::vector<PlacedVolume>          NodeList;
      /// Forward definition of the alignment delta data
      typedef Alignments::Delta                  Delta;

    public:
      /// Default constructor
      Alignment() = default;
      /// Default constructor
      Alignment(Object* p) : Handle<Object>(p)  {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Alignment(const Handle<Q>& e) : Handle<Object>(e)  {}
      /// Object constructor for pure alignment objects
      Alignment(const std::string& name) ;
      /// Data accessor for the use of decorators
      AlignmentData& data();
      /// Data accessor for the use of decorators
      const AlignmentData& data() const;
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation()  const;
      /// Access the alignment/placement matrix with respect to the world
      const TGeoHMatrix& detectorTransformation() const;
      /// Access to the node list
      const NodeList& nodes() const;
      
      /** Aliases for the transformation from local coordinates to the world system  */
      /// Transformation from local coordinates of the placed volume to the world system
      void localToWorld(const Position& local, Position& global) const;
      /// Transformation from local coordinates of the placed volume to the world system
      void localToWorld(const Double_t local[3], Double_t global[3]) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToWorld(const Position& local) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToWorld(const Double_t local[3]) const
      {  return localToWorld(Position(local[0],local[1],local[2]));                     }

      /** Aliases for the transformation from world coordinates to the local volume  */
      /// Transformation from world coordinates of the local placed volume coordinates
      void worldToLocal(const Position& global, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      void worldToLocal(const Double_t global[3], Double_t local[3]) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position worldToLocal(const Position& global) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position worldToLocal(const Double_t global[3]) const
      {  return worldToLocal(Position(global[0],global[1],global[2]));                  }

      /** Aliases for the transformation from local coordinates to the next DetElement system  */
      /// Transformation from local coordinates of the placed volume to the detector system
      void localToDetector(const Position& local, Position& detector) const;
      /// Transformation from local coordinates of the placed volume to the detector system
      void localToDetector(const Double_t local[3], Double_t detector[3]) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToDetector(const Position& local) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToDetector(const Double_t local[3]) const
      {  return localToDetector(Position(local[0],local[1],local[2]));                  }

      /** Aliases for the transformation from the next DetElement to local coordinates */
      /// Transformation from detector element coordinates to the local placed volume coordinates
      void detectorToLocal(const Position& detector, Position& local) const;
      /// Transformation from detector element coordinates to the local placed volume coordinates
      void detectorToLocal(const Double_t detector[3], Double_t local[3]) const;
      /// Transformation from detector element coordinates to the local placed volume coordinates
      Position detectorToLocal(const Position& detector) const;
      /// Transformation from detector element coordinates to the local placed volume coordinates
      Position detectorToLocal(const Double_t det[3]) const
      {  return detectorToLocal(Position(det[0],det[1],det[2]));                        }
    };
  }       /* End namespace Aligments                 */
}         /* End namespace DD4hep                    */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTS_H           */
