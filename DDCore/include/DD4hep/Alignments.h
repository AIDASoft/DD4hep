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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Forward declarations
  class IOV;

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    class UserPool;
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
    using Conditions::UserPool;
    
    /// Alignments internal namespace
    namespace Interna  {
      /// Forward declarations
      class AlignmentContainer;
      class AlignmentNamedObject;
      class AlignmentConditionObject;
    }
    class AlignmentsManagerObject;
    class AlignmentsLoader;
    class AlignmentData;
    class Alignment;
    class Delta;

    /// Main handle class to hold an alignment object
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_ALIGN
     */
    class Alignment : public Handle<AlignmentData>   {
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
        virtual int operator()(Alignment container) = 0;
      };

      /// Forward definition of the base data object containing alignment data
      typedef AlignmentData             Object;
      /// Forward definition of the base data object containing alignment data
      typedef AlignmentData             Data;
      /// Forward definition of the geometry placement
      typedef Geometry::PlacedVolume    PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef std::vector<PlacedVolume> NodeList;
      /// Forward definition of the alignment delta data
      typedef Alignments::Delta         Delta;
      /// Forward definition of the key type
      typedef unsigned long long int    key_type;
      /// Forward definition of the iov type
      typedef IOV                       iov_type;

    public:
      /// Default constructor
      Alignment() = default;
      /// Default constructor
      Alignment(Object* p) : Handle<Object>(p)  {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Alignment(const Handle<Q>& e) : Handle<Object>(e)  {}
      /// Object constructor for pure alignment objects
#if defined(__APPLE__) || defined(__clang__)
      Alignment(const std::string& name) ;
#else
      template <typename Q=Interna::AlignmentNamedObject> Alignment(const std::string& name);
#endif
      /// Hash code generation from input string
      static key_type hashCode(const char* value);
      /// Hash code generation from input string
      static key_type hashCode(const std::string& value);

      /// Data accessor for the use of decorators
      Data& data()              {   return (*access()); }
      /// Data accessor for the use of decorators
      const Data& data() const  {   return (*access()); }
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation()  const;
      /// Access the alignment/placement matrix with respect to the world
      const TGeoHMatrix& detectorTransformation() const;

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

    /// Hash code generation from input string
    inline Alignment::key_type Alignment::hashCode(const char* value)
    {   return hash64(value);    }

    /// Hash code generation from input string
    inline Alignment::key_type Alignment::hashCode(const std::string& value)
    {   return hash64(value);    }

    /// Main handle class to hold an alignment conditions object
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_ALIGN
     */
    class AlignmentCondition : public Handle<Interna::AlignmentConditionObject>   {
    public:
      /// Forward definition of the base data object containing alignment data
      typedef Interna::AlignmentConditionObject Object;
      /// Forward definition of the base data object containing alignment data
      typedef AlignmentData             Data;
      /// Forward definition of the geometry placement
      typedef Geometry::PlacedVolume    PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef std::vector<PlacedVolume> NodeList;
      /// Forward definition of the alignment delta data
      typedef Alignments::Delta         Delta;
      /// Forward definition of the key type
      typedef unsigned int              key_type;
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
#if defined(__APPLE__) || defined(__clang__)
      AlignmentCondition(const std::string& name) ;
#else
      template <typename Q=Object> AlignmentCondition(const std::string& name);
#endif
      /** Interval of validity            */
      /// Access the IOV type
      const IOVType& iovType()  const;
      /// Access the IOV block
      const iov_type& iov()  const;
      /// Access the hash identifier
      key_type key()  const;

      /** Data block (bound type)         */
      /// Data accessor for the use of decorators
      Data& data();
      /// Data accessor for the use of decorators
      const Data& data() const;
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation()  const;
      /// Access the alignment/placement matrix with respect to the world
      const TGeoHMatrix& detectorTransformation() const;
      /// Check if object is already bound....
      bool is_bound()  const;
    };


    /// Container class for alignment handles aggregated by a detector element
    /**
     *  Note: The alignments container is owner by the detector element
     *        On deletion the detector element will destroy the container
     *        and all associated entries.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class Container : public Handle<Interna::AlignmentContainer> {
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
        virtual int operator()(Container container) = 0;
      };

      /// Standard object type
      typedef Interna::AlignmentContainer      Object;
      /// Forward definition of the key type
      typedef Alignment::key_type              key_type;
      /// Forward definition of the iov type
      typedef Alignment::iov_type              iov_type;
      /// Forward definition of the mapping type
      typedef std::pair<key_type, std::string> key_value;
      /// Definition of the keys
      typedef std::map<key_type, key_value>    Keys;

    public:
      /// Default constructor
      Container();

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Container& c) : Handle<Object>(c) {}

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Handle<Q>& e) : Handle<Object>(e) {}

      /// Access the number of conditons keys available for this detector element
      size_t numKeys() const;

      /// Known keys of conditions in this container
      const Keys&  keys()  const;

      /// Add a new key to the conditions access map.
      /**  Caution: This is not thread protected!  */
      void addKey(const std::string& key_val);

      /// Add a new key to the conditions access map: Allow for alias if key_val != data_val
      /**  Caution: This is not thread protected!  */
      void addKey(const std::string& key_val, const std::string& data_val);

      /// Access to alignment objects by key and IOV. 
      Alignment get(const std::string& alignment_key, const iov_type& iov);

      /// Access to alignment objects directly by their hash key. 
      Alignment get(key_type alignment_key, const iov_type& iov);

      /// Access to alignment objects. Only alignments in the pool are accessed.
      Alignment get(const std::string& alignment_key, const UserPool& iov);

      /// Access to alignment objects. Only alignments in the pool are accessed.
      Alignment get(key_type alignment_key, const UserPool& iov);
    };

    /// Default constructor
    inline Container::Container() : Handle<Object>() {
    }

  } /* End namespace Aligments               */
} /* End namespace DD4hep                    */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTS_H   */
