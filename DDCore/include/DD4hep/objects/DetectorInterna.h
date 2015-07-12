// $Id: $
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
#ifndef DD4HEP_GEOMETRY_DETECTORINTERNA_H
#define DD4HEP_GEOMETRY_DETECTORINTERNA_H

// Framework include files
#include "DD4hep/Callback.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Alignment.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/ObjectExtensions.h"
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    class LCDD;
    class DetElementObject;
    class SensitiveDetectorObject;

    /// Data class with properties of sensitive detectors
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *
     *  \ingroup DD4HEP DD4HEP_GEOMETRY
     */
    class SensitiveDetectorObject: public NamedObject, public ObjectExtensions {
    public:
      unsigned int magic;
      int verbose;
      int combineHits;
      double ecut;
      Readout readout;
      Region region;
      LimitSet limits;
      std::string hitsCollection;

      /// Default constructor
      SensitiveDetectorObject();
      /// Initializing constructor
      SensitiveDetectorObject(const std::string& nam);
      /// Internal object destructor: release extension object(s)
      virtual ~SensitiveDetectorObject();
    };

    /// Data class with properties of a detector element
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *
     *  \ingroup DD4HEP DD4HEP_GEOMETRY
     */
    class DetElementObject: public NamedObject, public ObjectExtensions {
    public:
      // Type definitions.
      // The full namespace declaration is required by cint....
      typedef /* DD4hep::Geometry:: */  DetElement::destruct_t destruct_t;
      typedef /* DD4hep::Geometry:: */  DetElement::copy_t copy_t;

      typedef /* DD4hep::Geometry:: */  DetElement::Children Children;
      typedef /* DD4hep::Geometry:: */  DetElement::Extensions Extensions;
      typedef std::pair<Callback,unsigned long> UpdateCall;
      typedef std::vector<UpdateCall> UpdateCallbacks;

      enum DetFlags {
        HAVE_WORLD_TRAFO = 1<<0,
        HAVE_PARENT_TRAFO = 1<<1,
        HAVE_REFERENCE_TRAFO = 1<<2,
        HAVE_OTHER = 1<<31
      };

      /// Magic number to ensure data integrity
      unsigned int magic;
      /// Flag to remember internally calculated quatities
      unsigned int flag;
      /// Unique integer identifier of the detector instance
      int id;
      /// Flag to process hits
      int combineHits;
      /// Full path to this detector element. May be invalid
      std::string path;
      /// The path to the placement of the detector element (if placed)
      std::string placementPath;

      /// The subdetector placement corresponding to the ideal detector element's volume
      PlacedVolume idealPlace;
      /// The subdetector placement corresponding to the actual detector element's volume
      PlacedVolume placement;
      /// The cached VolumeID of this subdetector element
      /**  Please note:
       *  These values are set when populating the volume manager.
       *  There are restrictions: e.g. only sensitive subdetectors are present.
       */
      VolumeID volumeID;
      /// Reference to the parent element
      DetElement parent;
      /// Reference element for stored transformations
      DetElement reference;
      /// The array of children
      Children children;
      /// Placeholder for structure with update callbacks
      UpdateCallbacks updateCalls;

      //@{ Additional information set externally to facilitate the processing of event data */
      /// Basic detector element alignment entry
      Alignment alignment;
      /// Basic detector element alignment entry containing the survey data
      Alignment survey;
      /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
      std::vector<Alignment> volume_alignments;
      /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
      std::vector<Alignment> volume_surveys;
      /// The detector elements condition entry
      Conditions conditions;
      //@}
      //@{ Cached information of the detector element
      /// Intermediate buffer to store the transformation to the world coordination system
      TGeoHMatrix worldTrafo;
      /// Intermediate buffer to store the transformation to the parent detector element
      TGeoHMatrix parentTrafo;
      /// Intermediate buffer for the transformation to an arbitrary DetElement
      TGeoHMatrix* referenceTrafo;
      //@}
      //@{ Public methods to ease the usage of the data. */
      /// Initializing constructor
      DetElementObject(const std::string& nam, int ident);
      /// Default constructor
      DetElementObject();
      /// Internal object destructor: release extension object(s)
      virtual ~DetElementObject();
      /// Deep object copy to replicate DetElement trees e.g. for reflection
      virtual DetElementObject* clone(int new_id, int flag) const;
      //@}
      /// Create cached matrix to transform to world coordinates
      const TGeoHMatrix& worldTransformation();
      /// Create cached matrix to transform to parent coordinates
      const TGeoHMatrix& parentTransformation();
      /// Create cached matrix to transform to reference coordinates
      const TGeoHMatrix& referenceTransformation();
      /// Remove callback from object
      void removeAtUpdate(unsigned int type, void* pointer);
      /// Trigger update callbacks
      void update(unsigned int tags, void* param);
      /// Revalidate the caches
      void revalidate(TGeoHMatrix* parent_world_trafo);
    };

  } /* End namespace Geometry      */
} /* End namespace DD4hep        */
#endif    /* DD4HEP_GEOMETRY_DETECTORINTERNA_H      */
