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
#ifndef DD4HEP_DDCORE_OPTICALSURFACEMANAGER_H
#define DD4HEP_DDCORE_OPTICALSURFACEMANAGER_H

// Framework include files
#include "DD4hep/OpticalSurfaces.h"

// ROOT include files
#include "TGeoManager.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail  {
    class OpticalSurfaceManagerObject;
  }
  
  /// Class to support the handling of optical surfaces.
  /**
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class OpticalSurfaceManager: public Handle<detail::OpticalSurfaceManagerObject> {
  public:
    typedef detail::OpticalSurfaceManagerObject Object;

  public:
    /// Default constructor
    OpticalSurfaceManager() = default;
    /// Copy constructor
    OpticalSurfaceManager(const OpticalSurfaceManager& e) = default;
    /// Constructor taking object pointer
    OpticalSurfaceManager(TGeoManager* obj) : Handle<Object>(obj) { }
    /// Constructor from same-type handle
    OpticalSurfaceManager(const Handle<Object>& e) : Handle<Object>(e) { }
    /// Constructor from arbitrary handle
    template <typename Q>
    OpticalSurfaceManager(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Initializing constructor.
    OpticalSurfaceManager(Detector& description);
    /// Assignment operator
    OpticalSurfaceManager& operator=(const OpticalSurfaceManager& m) = default;

    /// static accessor calling DD4hepOpticalSurfaceManagerPlugin if necessary
    static OpticalSurfaceManager getOpticalSurfaceManager(Detector& description);

#if ROOT_VERSION_CODE > ROOT_VERSION(6,16,0)
    /// Access skin surface by its identifier
    SkinSurface    getSkinSurface(DetElement de, const std::string& nam)  const;
    /// Access border surface by its identifier
    BorderSurface  getBorderSurface(DetElement de, const std::string& nam)  const;
    /// Access optical surface data by its identifier
    OpticalSurface getOpticalSurface(DetElement de, const std::string& nam)  const;
    /// Add skin surface to manager
    void addSkinSurface(DetElement de, SkinSurface surf)  const;
    /// Add border surface to manager
    void addBorderSurface(DetElement de, BorderSurface surf)  const;
    /// Add optical surface data to manager
    void addOpticalSurface(OpticalSurface surf)  const;

    /// Register the temporary surface objects with the TGeoManager
    void registerSurfaces(DetElement subdetector);
#endif
  };
}         /* End namespace dd4hep                  */
#endif    /* DD4HEP_DDCORE_OPTICALSURFACEMANAGER_H */
