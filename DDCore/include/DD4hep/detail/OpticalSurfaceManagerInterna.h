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
// instantiate code. Otherwise the Opticalsurfacemanager include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DETAIL_OPTICALSURFACEMANAGERINTERNA_H
#define DD4HEP_DETAIL_OPTICALSURFACEMANAGERINTERNA_H

/// Framework include files
#include "DD4hep/Detector.h"

/// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// This structure describes the internal data of the volume manager object
    /**
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_CORE
     */
    class OpticalSurfaceManagerObject: public NamedObject {
    public:
      typedef std::pair<DetElement, std::string> LocalKey;

      /// Reference to the main detector description object
      Detector& detector;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
      std::map<LocalKey,    SkinSurface>    skinSurfaces;
      std::map<LocalKey,    BorderSurface>  borderSurfaces;
      std::map<std::string, OpticalSurface> opticalSurfaces;
#endif
      
    public:
      /// Default constructor
      OpticalSurfaceManagerObject() = delete;
      /// No move constructor
      OpticalSurfaceManagerObject(OpticalSurfaceManagerObject&& copy) = delete;
      /// No copy constructor
      OpticalSurfaceManagerObject(const OpticalSurfaceManagerObject& copy) = delete;
      /// Initializing constructor
      OpticalSurfaceManagerObject(Detector& det) : detector(det) {}
      /// Default destructor
      virtual ~OpticalSurfaceManagerObject();
      /// No move assignment
      OpticalSurfaceManagerObject& operator=(OpticalSurfaceManagerObject&& copy) = delete;
      /// No copy assignment
      OpticalSurfaceManagerObject& operator=(const OpticalSurfaceManagerObject& copy) = delete;
    };

  }       /* End namespace detail                          */
}         /* End namespace dd4hep                          */
#endif // DD4HEP_DETAIL_OPTICALSURFACEMANAGERINTERNA_H
