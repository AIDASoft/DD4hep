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
#ifndef DD4HEP_DDCORE_OPTICALSURFACES_H
#define DD4HEP_DDCORE_OPTICALSURFACES_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/DetElement.h"

#if ROOT_VERSION_CODE > ROOT_VERSION(6,16,0)

// ROOT include files
#include "TGeoOpticalSurface.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Class to support the handling of optical surfaces.
  /**
   *   Optical surfaces are "global" objects ie. they are attached to the 
   *   TGeoManager of the Detector object.
   *
   *   IF the optical surface is created locally in a detector constructor,
   *   and registered to the OpticalSurfaceManager, the surfaces are ALSO
   *   attached to the currently built detector.
   *   Alternatively the call 
   *   OpticalSurfaceManager::registerSurfaces(DetElement subdetector)
   *   may be issued to attach then to the subdetector DetElement.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class OpticalSurface : public Handle<TGeoOpticalSurface> {
  public:
    typedef TGeoOpticalSurface     Object;
    typedef Object Finish;
    typedef Object Model;
    typedef Object Type;
    typedef Object::ESurfaceModel  EModel;
    typedef Object::ESurfaceFinish EFinish;
    typedef Object::ESurfaceType   EType;
    
  public:
    /// Default constructor
    OpticalSurface() = default;
    /// Copy constructor
    OpticalSurface(const OpticalSurface& e) = default;
    /// Constructor taking object pointer
    OpticalSurface(Object* obj) : Handle<Object>(obj) { }
    /// Constructor from same-type handle
    OpticalSurface(const Handle<Object>& e) : Handle<Object>(e) {  }
    /// Constructor from arbitrary handle
    template <typename Q>
    OpticalSurface(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Initializing constructor.
    OpticalSurface(Detector& detector,
                   const std::string& name,
                   EModel  model    = Model::kMglisur,
                   EFinish finish   = Finish::kFpolished,
                   EType   type     = Type::kTdielectric_dielectric,
                   double  value    = 1.0);

    /// Assignment operator
    OpticalSurface& operator=(const OpticalSurface& m) = default;
  };

  /// Class to support the handling of optical surfaces.
  /**
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class SkinSurface : public Handle<TGeoSkinSurface> {
  public:
    typedef TGeoSkinSurface Object;

  public:
    /// Default constructor
    SkinSurface() = default;
    /// Copy constructor
    SkinSurface(const SkinSurface& e) = default;
    /// Constructor taking object pointer
    SkinSurface(Object* obj) : Handle<Object>(obj) { }
    /// Constructor from same-type handle
    SkinSurface(const Handle<Object>& e) : Handle<Object>(e) {  }
    /// Constructor from arbitrary handle
    template <typename Q>
    SkinSurface(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Initializing constructor.
    SkinSurface(Detector&          detector,
                DetElement         de,
                const std::string& nam,
                OpticalSurface     surf,
                Volume             vol);

    /// Assignment operator
    SkinSurface& operator=(const SkinSurface& m) = default;

    /// Access surface data
    OpticalSurface surface()  const;
    /// Access the node of the skin surface
    Volume  volume()   const;
  };

  /// Class to support the handling of optical surfaces.
  /**
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class BorderSurface : public Handle<TGeoBorderSurface> {
  public:
    typedef TGeoBorderSurface Object;

  public:
    /// Default constructor
    BorderSurface() = default;
    /// Copy constructor
    BorderSurface(const BorderSurface& e) = default;
    /// Constructor taking object pointer
    BorderSurface(Object* obj) : Handle<Object>(obj) { }
    /// Constructor from same-type handle
    BorderSurface(const Handle<Object>& e) : Handle<Object>(e) {  }
    /// Constructor from arbitrary handle
    template <typename Q>
    BorderSurface(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Initializing constructor: Creates the object and registers it to the manager
    BorderSurface(Detector&          detector,
                  DetElement         de,
                  const std::string& nam,
                  OpticalSurface     surf,
                  PlacedVolume       left,
                  PlacedVolume       right);

    /// Assignment operator
    BorderSurface& operator=(const BorderSurface& m) = default;
    /// Access surface data
    OpticalSurface surface()  const;
    /// Access the left node of the border surface
    PlacedVolume   left()   const;
    /// Access the right node of the border surface
    PlacedVolume   right()  const;
  };


}         /* End namespace dd4hep              */
#endif    /* ROOT_VERSION                      */
#endif    /* DD4HEP_DDCORE_OPTICALSURFACES_H   */
