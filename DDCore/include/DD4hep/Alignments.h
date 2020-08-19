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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTS_H
#define DD4HEP_ALIGMENTS_ALIGNMENTS_H

// Framework include files
#include "DD4hep/IOV.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Conditions.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declarations
  class IOV;
  class AlignmentData;
  class Alignment;
  class Delta;
  class ConditionsMap;

  /// DD4hep internal namespace
  namespace detail  {
    /// Forward declarations
    class ConditionObject;
    class AlignmentObject;
  }

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace align  {
    /// Alignment and Delta item key 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGN
     */
    class Keys  {
    public:
      /// Key name  of a delta condition "alignment_delta".
      static const std::string                         deltaName;
      /// Key value of a delta condition "alignment_delta".
      static const Condition::itemkey_type deltaKey;
      /// Key name  of an alignment condition object "alignment".
      static const std::string                         alignmentName;
      /// Key value of an alignment condition object "alignment".
      static const Condition::itemkey_type alignmentKey;
    };
  }

  
  /// Main handle class to hold an alignment conditions object
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_ALIGN
   */
  class AlignmentCondition : public Handle<detail::AlignmentObject>   {
  public:
    /// Forward definition of the key type
    typedef unsigned long long int    key_type;

  public:
    /// Default constructor
    AlignmentCondition() : Handle<Object>() {}
    /// Default constructor
    AlignmentCondition(Object* p) : Handle<Object>(p) {}
    /// Assignment constructor from condition object
    AlignmentCondition(detail::ConditionObject* p) : Handle<Object>(p) {}
    /// Constructor to be used when reading the already parsed object
    template <typename Q> AlignmentCondition(const Handle<Q>& e) : Handle<Object>(e) {}
    /// Object constructor for pure alignment objects
    AlignmentCondition(const std::string& name);
    /** Interval of validity            */
    /// Access the IOV type
    const IOVType& iovType()  const;
    /// Access the IOV block
    const IOV& iov()  const;
    /// Access the hash identifier
    key_type key()  const;

    /** Data block (bound type)         */
    /// Data accessor for the use of decorators
    AlignmentData& data();
    /// Data accessor for the use of decorators
    const AlignmentData& data() const;
    /// Access the delta value of the object
    const Delta& delta() const;
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
   *  \ingroup DD4HEP_CORE
   *  \ingroup DD4HEP_ALIGN
   */
  class Alignment : public Handle<detail::AlignmentObject>   {
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
    /// Access the delta value of the object
    const Delta& delta() const;
    /// Create cached matrix to transform to world coordinates
    const TGeoHMatrix& worldTransformation()  const;
    /// Access the alignment/placement matrix with respect to the world
    const TGeoHMatrix& detectorTransformation() const;
    /// Access to the node list
    const std::vector<PlacedVolume>& nodes() const;
      
    /** Aliases for the transformation from local coordinates to the world system  */
    /// Transformation from local coordinates of the placed volume to the world system
    void localToWorld(const Position& local, Position& global) const;
    /// Transformation from local coordinates of the placed volume to the world system
    void localToWorld(const Double_t local[3], Double_t global[3]) const;
    /// Transformation from local coordinates of the placed volume to the world system
    Position localToWorld(const Position& local) const;
    /// Transformation from local coordinates of the placed volume to the world system
    Position localToWorld(const Double_t local[3]) const
    {  return localToWorld({local[0],local[1],local[2]});                             }

    /** Aliases for the transformation from world coordinates to the local volume  */
    /// Transformation from world coordinates of the local placed volume coordinates
    void worldToLocal(const Position& global, Position& local) const;
    /// Transformation from world coordinates of the local placed volume coordinates
    void worldToLocal(const Double_t global[3], Double_t local[3]) const;
    /// Transformation from local coordinates of the placed volume to the world system
    Position worldToLocal(const Position& global) const;
    /// Transformation from local coordinates of the placed volume to the world system
    Position worldToLocal(const Double_t global[3]) const
    {  return worldToLocal({global[0],global[1],global[2]});                          }

    /** Aliases for the transformation from local coordinates to the next DetElement system  */
    /// Transformation from local coordinates of the placed volume to the detector system
    void localToDetector(const Position& local, Position& detector) const;
    /// Transformation from local coordinates of the placed volume to the detector system
    void localToDetector(const Double_t local[3], Double_t detector[3]) const;
    /// Transformation from local coordinates of the placed volume to the world system
    Position localToDetector(const Position& local) const;
    /// Transformation from local coordinates of the placed volume to the world system
    Position localToDetector(const Double_t local[3]) const
    {  return localToDetector({local[0],local[1],local[2]});                         }

    /** Aliases for the transformation from the next DetElement to local coordinates */
    /// Transformation from detector element coordinates to the local placed volume coordinates
    void detectorToLocal(const Position& detector, Position& local) const;
    /// Transformation from detector element coordinates to the local placed volume coordinates
    void detectorToLocal(const Double_t detector[3], Double_t local[3]) const;
    /// Transformation from detector element coordinates to the local placed volume coordinates
    Position detectorToLocal(const Position& detector) const;
    /// Transformation from detector element coordinates to the local placed volume coordinates
    Position detectorToLocal(const Double_t det[3]) const
    {  return detectorToLocal({det[0],det[1],det[2]});                              }
  };
}         /* End namespace dd4hep                    */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTS_H           */
