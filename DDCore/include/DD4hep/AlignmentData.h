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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTDATA_H
#define DD4HEP_ALIGMENTS_ALIGNMENTDATA_H

// Framework include files
#include "DD4hep/NamedObject.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    // The DetElement is a central object. We alias it here.
    using Geometry::DetElement;
    using Geometry::RotationZYX;
    using Geometry::Transform3D;
    using Geometry::Position;
    using Geometry::LCDD;

    // Forward declarations
    class Alignment;

    /// Class describing an condition to re-adjust an alignment
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Delta   {
    public:
      typedef Position    Pivot;
      Pivot        pivot;
      Position     translation;
      RotationZYX  rotation;
      /// Default constructor
      Delta();
      /// Copy constructor
      Delta(const Delta& c);
      /// Default destructor
      ~Delta();
      /// Assignment operator
      Delta& operator=(const Delta& c);
      /// Reset information to identity
      void clear();
    };

    /// Derived condition data-object definition
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class AlignmentData   {
    public:

      /// Forward definition of the geometry placement
      typedef Geometry::PlacedVolume    PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef std::vector<PlacedVolume> NodeList;
      /// Forward definition of the alignment delta data
      typedef Alignments::Delta         Delta;
      /// Forward declaration of the utility bit mask
      typedef unsigned int              BitMask;
      /// Forward declaration of the utility mask manipulator
      typedef ReferenceBitMask<BitMask> MaskManipulator;
      

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
      NodeList             nodes;
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
      inline AlignmentData& alignmentData()     { return *this; }

      /// Access the ideal/nominal alignment/placement matrix
      Alignment nominal() const;
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation()  const
      {  return worldTrafo;                                                       }
      /// Access the alignment/placement matrix with respect to the world
      const TGeoHMatrix& detectorTransformation() const
      {  return detectorTrafo;                                                    }
      /// Access the currently applied alignment/placement matrix
      const Transform3D& localToWorld() const   
      {  return trToWorld;                                                        }

      /** Aliases for the transformation from local coordinates to the world system  */
      /// Transformation from local coordinates of the placed volume to the world system
      void localToWorld(const Position& local, Position& global) const;
      /// Transformation from local coordinates of the placed volume to the world system
      void localToWorld(const Double_t local[3], Double_t global[3]) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToWorld(const Position& local) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToWorld(const Double_t local[3]) const
      {  return localToWorld(Position(local[0],local[1],local[2]));               }

      /** Aliases for the transformation from world coordinates to the local volume  */
      /// Transformation from world coordinates of the local placed volume coordinates
      void worldToLocal(const Position& global, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      void worldToLocal(const Double_t global[3], Double_t local[3]) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position worldToLocal(const Position& global) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position worldToLocal(const Double_t global[3]) const
      {  return worldToLocal(Position(global[0],global[1],global[2]));            }

      /** Aliases for the transformation from local coordinates to the next DetElement system  */
      /// Transformation from local coordinates of the placed volume to the detector system
      void localToDetector(const Position& local, Position& detector) const;
      /// Transformation from local coordinates of the placed volume to the detector system
      void localToDetector(const Double_t local[3], Double_t detector[3]) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToDetector(const Position& local) const;
      /// Transformation from local coordinates of the placed volume to the world system
      Position localToDetector(const Double_t local[3]) const
      {  return localToDetector(Position(local[0],local[1],local[2]));            }

      /** Aliases for the transformation from the next DetElement to local coordinates */
      /// Transformation from detector element coordinates to the local placed volume coordinates
      void detectorToLocal(const Position& detector, Position& local) const;
      /// Transformation from detector element coordinates to the local placed volume coordinates
      void detectorToLocal(const Double_t detector[3], Double_t local[3]) const;
      /// Transformation from detector element coordinates to the local placed volume coordinates
      Position detectorToLocal(const Position& detector) const;
      /// Transformation from detector element coordinates to the local placed volume coordinates
      Position detectorToLocal(const Double_t det[3]) const
      {  return detectorToLocal(Position(det[0],det[1],det[2]));                  }
    };

    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template <typename T> class AlignmentDecorator : public T  {
    public:
      /// Forward definition of the geometry placement
      typedef AlignmentData::PlacedVolume    PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef AlignmentData::NodeList        NodeList;
      /// Forward definition of the alignment delta data
      typedef AlignmentData::Delta           Delta;
      
      /// Default constructor
      AlignmentDecorator();
      /// Copy constructor
      AlignmentDecorator(const AlignmentDecorator& c);
      /// Special constructors
      template <typename Q> AlignmentDecorator(Q q) : T(q) {}
      /// Default destructor
      ~AlignmentDecorator()  {                                  }

      /// Data accessor
      const AlignmentData& data()   const
      { return T::alignmentData();                              }
      /// Access to the DetElement node
      DetElement detector()   const
      {  return data().detector;                                }
      /// Access the placement of this node
      PlacedVolume placement()   const
      {  return data().placement;                               }
      /// Access the placement of the DetElement node
      PlacedVolume detectorPlacement()   const
      {  return data().detector.placement();                    }

      /// Access the alignment/placement matrix with respect to the world
      const Transform3D& localToWorld() const
      {  return data().trToWorld;  }
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation() const
      {  return data().worldTransformation();                   }
      /// Create cached matrix to transform to detector coordinates
      const TGeoHMatrix& detectorTransformation() const
      {  return data().detectorTransformation();                }

      /// Transformation from local coordinates of the placed volume to the world system
      void localToWorld(const Position& local, Position& global) const
      {  return data().localToWorld(local, global);             }
      /// Transformation from local coordinates of the placed volume to the world system
      void localToWorld(const Double_t local[3], Double_t global[3]) const
      {  return data().localToWorld(local, global);             }

      /// Transformation from local coordinates of the placed volume to the detector system
      void localToDetector(const Position& local, Position& det) const
      {  return data().localToDetector(local, det);             }
      /// Transformation from local coordinates of the placed volume to the detector system
      void localToDetector(const Double_t local[3], Double_t det[3]) const
      {  return data().localToDetector(local, det);             }

      /// Transformation from world coordinates of the local placed volume coordinates
      void worldToLocal(const Position& global, Position& local) const
      {  return data().worldToLocal(global, local);             }
      /// Transformation from world coordinates of the local placed volume coordinates
      void worldToLocal(const Double_t global[3], Double_t local[3]) const
      {  return data().worldToLocal(global, local);             }

      /// Transformation from world coordinates of the local placed volume coordinates
      void detectorToLocal(const Position& det, Position& local) const
      {  return data().detectorToLocal(det, local);             }
      /// Transformation from world coordinates of the local placed volume coordinates
      void detectorToLocal(const Double_t det[3], Double_t local[3]) const
      {  return data().detectorToLocal(det, local);             }

      /// Access the currently applied alignment/placement matrix (mother to daughter)
      const Alignment& nominal() const
      {  return data().detector.ideal();                        }
      /// Access the currently applied correction matrix (delta) (mother to daughter)
      const Delta& delta() const
      {  return data().delta;                                   }

      /// Set the delta alignment information
      void setDelta(const Delta& del)
      {  data().delta = del;                                    }
      /// Set the delta alignment if only a single translation
      void setDelta(const Position& translation)
      {  data().delta.translation = translation;                }
      /// Set the delta alignment if only a single rotation
      void setDelta(const RotationZYX& rotation)
      {  data().delta.rotation = rotation;                      }
      /// Set the delta alignment as a composite of a translation and a rotation
      void setDelta(const Position& translation, RotationZYX& rotation)     {
        AlignmentData& d = data();
        d.delta.rotation = rotation;
        d.delta.translation = translation;
      }
      /// Set the delta alignment if only a single rotation around a pivot point
      void setDeltaPivot(const Position& pivot, const RotationZYX& rotation)  {
        AlignmentData& d = data();
        d.delta.pivot = pivot;
        d.delta.rotation = rotation;
      }
      /// Set the delta alignment if a translation and a rotation around pivot
      void setDeltaPivot(const Position& translation, const Position& pivot, const RotationZYX& rotation)  {
        AlignmentData& d = data();
        d.delta.pivot = pivot;
        d.delta.rotation = rotation;
        d.delta.translation = translation;
      }
    };

    /// Default constructor
    template <typename T> inline 
    AlignmentDecorator<T>::AlignmentDecorator() : T() {}

    /// Copy constructor
    template <typename T> inline 
    AlignmentDecorator<T>::AlignmentDecorator(const AlignmentDecorator& c) : T(c)  {}


    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class NamedAlignmentObject : public NamedObject, public AlignmentData  {
    public:
      /// Default constructor
      NamedAlignmentObject(const std::string& nam, const std::string& tit="")
        : NamedObject(nam,tit), AlignmentData()  {}
      /// Default destructor
      virtual ~NamedAlignmentObject();
    };

  } /* End namespace Aligments                  */
} /* End namespace DD4hep                       */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTDATA_H   */
