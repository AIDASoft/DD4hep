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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTDATA_H
#define DD4HEP_ALIGMENTS_ALIGNMENTDATA_H

// Framework include files
#include "DD4hep/NamedObject.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/Volumes.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Alignment;
  class AlignmentCondition;

  /// Class describing an condition to re-adjust an alignment
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class Delta   {
  public:
    typedef Translation3D Pivot;
    Position      translation;
    Pivot         pivot;
    RotationZYX   rotation;
    unsigned int  flags = 0;

    enum AlignmentFlags {
      HAVE_NONE         = 0,
      HAVE_TRANSLATION  = 1<<2,
      HAVE_ROTATION     = 1<<3,
      HAVE_PIVOT        = 1<<4,
    };

    /// Default constructor
    Delta() = default;
    /// Initializing constructor
    Delta(const Position& tr)
      : translation(tr), flags(HAVE_TRANSLATION) {}
    /// Initializing constructor
    Delta(const RotationZYX& rot)
      : translation(), rotation(rot), flags(HAVE_ROTATION) {}
    /// Initializing constructor
    Delta(const Position& tr, const RotationZYX& rot)
      : translation(tr), rotation(rot), flags(HAVE_ROTATION|HAVE_TRANSLATION) {}
    /// Initializing constructor
    Delta(const Translation3D& piv, const RotationZYX& rot)
      : pivot(piv), rotation(rot), flags(HAVE_ROTATION|HAVE_PIVOT) {}
    /// Initializing constructor
    Delta(const Position& tr, const Translation3D& piv, const RotationZYX& rot)
      : translation(tr), pivot(piv), rotation(rot), flags(HAVE_ROTATION|HAVE_PIVOT|HAVE_TRANSLATION) {}
    /// Copy constructor
    Delta(const Delta& c);
    /// Default destructor
    ~Delta();
    /// Assignment operator
    Delta& operator=(const Delta& c);
    /// Reset information to identity
    void clear();
    /// Check a given flag
    bool checkFlag(unsigned int mask) const {  return (flags&mask) == mask;        }
    /// Check a given flag
    void setFlag(unsigned int mask)         {  flags |= mask;                      }
    /// Access flags: Check if the delta operation contains a translation
    bool hasTranslation() const             {  return checkFlag(HAVE_TRANSLATION); }
    /// Access flags: Check if the delta operation contains a rotation
    bool hasRotation() const                {  return checkFlag(HAVE_ROTATION);    }
    /// Access flags: Check if the delta operation contains a pivot
    bool hasPivot() const                   {  return checkFlag(HAVE_PIVOT);       }
    /// Compute the alignment delta for one detector element and it's alignment condition
    void computeMatrix(TGeoHMatrix& tr_delta)  const;
  };

  /// Derived condition data-object definition
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class AlignmentData   {
  public:
    /// Forward declaration of the utility bit mask
    typedef unsigned int              BitMask;

    enum AlignmentFlags {
      HAVE_NONE = 0,
      HAVE_WORLD_TRAFO  = 1<<0,
      HAVE_PARENT_TRAFO = 1<<1,
      HAVE_OTHER        = 1<<31
    };
    enum DataType  {
      IDEAL   = 1<<10,
      SURVEY  = 1<<11,
      TIME_STAMPED = 1<<12
    };

    /// Alignment changes
    Delta                delta;
    /// Intermediate buffer to store the transformation to the world coordination system
    mutable TGeoHMatrix  worldTrafo;
    /// Intermediate buffer to store the transformation to the parent detector element
    mutable TGeoHMatrix  detectorTrafo;
    /// The list of TGeoNodes (physical placements)
    std::vector<PlacedVolume> nodes;
    /// Transformation from volume to the world
    Transform3D          trToWorld;
    /// Reference to the next hosting detector element
    DetElement           detector;
    /// The subdetector placement corresponding to the actual detector element's volume
    PlacedVolume         placement;
    /// Flag to remember internally calculated quatities
    mutable BitMask      flag;
    /// Magic word to verify object if necessary
    unsigned int         magic;

  public:
    /// Standard constructor
    AlignmentData();
    /// Copy constructor
    AlignmentData(const AlignmentData& copy);
    /// Default destructor
    virtual ~AlignmentData();
    /// Assignment operator necessary due to copy constructor
    AlignmentData& operator=(const AlignmentData& copy);
    /// Data accessor for decorator
    inline AlignmentData& data()                       {  return *this;               }
    /// Access the ideal/nominal alignment/placement matrix
    Alignment nominal() const;
    /// Create cached matrix to transform to world coordinates
    const TGeoHMatrix& worldTransformation()  const    {  return worldTrafo;          }
    /// Access the alignment/placement matrix with respect to the world
    const TGeoHMatrix& detectorTransformation() const  {  return detectorTrafo;       }
    /// Access the currently applied alignment/placement matrix
    const Transform3D& localToWorld() const            {  return trToWorld;           }

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
}         /* End namespace dd4hep               */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTDATA_H   */
