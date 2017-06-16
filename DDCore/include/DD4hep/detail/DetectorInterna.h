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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the Detector include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_GEOMETRY_DETECTORINTERNA_H
#define DD4HEP_GEOMETRY_DETECTORINTERNA_H

// Framework include files
#include "DD4hep/Callback.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/World.h"
#include "DD4hep/Objects.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/ObjectExtensions.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class WorldObject;
  class DetElementObject;
  class SensitiveDetectorObject;
  class VolumeManager_Populator;
    
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
    int          verbose;
    int          combineHits;
    double       ecut;
    Readout      readout;
    Region       region;
    LimitSet     limits;
    std::string  hitsCollection;

    /// Default constructor
    SensitiveDetectorObject();
    /// Initializing constructor
    SensitiveDetectorObject(const std::string& nam);
    /// detaill object destructor: release extension object(s)
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
  class DetElementObject : public NamedObject, public ObjectExtensions {
  public:
    // Type definitions.
    // The full namespace declaration is required by cint....
    typedef std::pair<Callback,unsigned long>    UpdateCall;
    typedef std::vector<UpdateCall>              UpdateCallbacks;

    enum DetFlags {
      HAVE_WORLD_TRAFO        = 1<<0,
      HAVE_PARENT_TRAFO       = 1<<1,
      HAVE_REFERENCE_TRAFO    = 1<<2,
      HAVE_SENSITIVE_DETECTOR = 1<<29,
      IS_TOP_LEVEL_DETECTOR   = 1<<30,
      HAVE_OTHER              = 1<<31
    };

    /// Magic number to ensure data integrity
    unsigned int        magic;
    /// Flag to remember internally calculated quatities
    unsigned int        flag;
    /// Unique integer identifier of the detector instance
    int                 id;
    /// Flag to process hits
    int                 combineHits;
    /// Flag to encode detector types
    unsigned int        typeFlag;
    /// Hierarchical level within the detector description
    int                 level;
    /// Access hash key of this detector element (Only valid once geometry is closed!)
    unsigned int        key;
    /// Full path to this detector element. May be invalid
    std::string         path;
    /// The path to the placement of the detector element (if placed)
    std::string         placementPath;

    /// The subdetector placement corresponding to the ideal detector element's volume
    PlacedVolume        idealPlace;
    /// The subdetector placement corresponding to the actual detector element's volume
    PlacedVolume        placement;
    /// The cached VolumeID of this subdetector element
    /**  Please note:
     *  These values are set when populating the volume manager.
     *  There are restrictions: e.g. only sensitive subdetectors are present.
     */
    VolumeID             volumeID;
    /// Reference to the parent element
    World                privateWorld;
    /// Reference to the parent element
    DetElement           parent;
    /// The array of children
    DetElement::Children children;
    /// Placeholder for structure with update callbacks
    UpdateCallbacks      updateCalls;

    //@{ Additional information set externally to facilitate the processing of event data */
    /// Basic ideal/nominal detector element alignment entry
    AlignmentCondition  nominal;
    /// Basic detector element alignment entry containing the survey data
    AlignmentCondition  survey;

    /// Global alignment data
    Ref_t               global_alignment;
    //@}

#if 0      
    // To be removed!
    /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
    std::vector<Alignment> volume_alignments;
    /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
    std::vector<Alignment> volume_surveys;
#endif

  private:
    friend class VolumeManager_Populator;

  private:
    //@{ Private methods used internally by the object itself. */
    /// Resolve the world object. detaill use ONLY.
    World i_access_world();

  public:
    //@{ Public methods to ease the usage of the data. */
    /// Initializing constructor
    DetElementObject(const std::string& nam, int ident);
    /// Default constructor
    DetElementObject();
    /// detaill object destructor: release extension object(s)
    virtual ~DetElementObject();
    /// Deep object copy to replicate DetElement trees e.g. for reflection
    virtual DetElementObject* clone(int new_id, int flag) const;
    /// Access to the world object. Only possible once the geometry is closed.
    World world()
    {  return privateWorld.isValid() ? privateWorld : i_access_world(); }
    //@}
    /// Remove callback from object
    void removeAtUpdate(unsigned int type, void* pointer);
    /// Trigger update callbacks
    void update(unsigned int tags, void* param);
    /// Revalidate the caches
    void revalidate();
  };

  /// Data class with properties of a detector element
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *
   *  \ingroup DD4HEP DD4HEP_GEOMETRY
   */
  class WorldObject: public DetElementObject {
  public:
    /// Reference to the Detector instance object
    Detector* description;

  public:
    //@{ Public methods to ease the usage of the data. */
    /// Default constructor
    WorldObject();
#ifndef __CINT__
    /// Initializing constructor
    WorldObject(Detector& description, const std::string& nam);
#endif
    /// detaill object destructor: release extension object(s)
    virtual ~WorldObject();
  };

  /// Default constructor
  inline WorldObject::WorldObject() : DetElementObject(), description(0)      {
  }

}         /* End namespace dd4hep        */
#endif    /* DD4HEP_GEOMETRY_DETECTORINTERNA_H      */
