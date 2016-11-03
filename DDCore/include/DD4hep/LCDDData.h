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

#ifndef DD4HEP_DDCORE_LCDDDATA_H
#define DD4HEP_DDCORE_LCDDDATA_H

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/ObjectExtensions.h"
#include "DD4hep/objects/VolumeManagerInterna.h"

// C/C++ include files
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Foward declarations
  class NamedObject;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Data implementation class of the LCDD interface
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class LCDDData  {

    public:
      /// Specialized exception class
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      struct InvalidObjectError: public std::runtime_error {
        InvalidObjectError(const std::string& msg)
          : std::runtime_error("DD4hep: " + msg) {
        }
      };

      /// Implementation of a map of named DD4hep Handles
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      class ObjectHandleMap: public LCDD::HandleMap {
      public:
        /// Default constructor
        ObjectHandleMap() {
        }
        void append(const Ref_t& e, bool throw_on_doubles = true) {
          if (e.isValid()) {
            std::string n = e.name();
            std::pair<iterator, bool> r = this->insert(std::make_pair(n, e.ptr()));
            if (!throw_on_doubles || r.second)
              return;
            throw InvalidObjectError("Attempt to add an already existing object:" + std::string(e.name()) + ".");
          }
          throw InvalidObjectError("Attempt to add an invalid object.");
        }

        template <typename T> void append(const Ref_t& e, bool throw_on_doubles = true) {
          T* obj = dynamic_cast<T*>(e.ptr());
          if (obj) {
            this->append(e, throw_on_doubles);
            return;
          }
          throw InvalidObjectError("Attempt to add an object, which is of the wrong type.");
        }
      };

    protected:
      TGeoManager* m_manager;
      ObjectHandleMap m_readouts;
      ObjectHandleMap m_idDict;
      ObjectHandleMap m_limits;
      ObjectHandleMap m_regions;
      ObjectHandleMap m_detectors;
      ObjectHandleMap m_alignments;

      ObjectHandleMap m_sensitive;
      ObjectHandleMap m_display;
      ObjectHandleMap m_fields;

      ObjectHandleMap  m_motherVolumes;

      // GDML fields
      ObjectHandleMap  m_define;

      DetElement       m_world;
      DetElement       m_trackers;
      Volume           m_worldVol;
      Volume           m_trackingVol;

      Material         m_materialAir;
      Material         m_materialVacuum;
      VisAttr          m_invisibleVis;
      OverlayedField   m_field;
      Header           m_header;
      LCDD::Properties m_properties;
      LCDDBuildType    m_buildType;

      /// Definition of the extension type
      ObjectExtensions m_extensions;
      /// Volume manager reference
      VolumeManager m_volManager;

      /// Flag to inhibit the access to global constants. Value set by constants section 'LCDD_InhibitConstants'
      bool m_inhibitConstants;

    protected:
      /// Default constructor
      LCDDData();
      /// Default destructor
      virtual ~LCDDData();
      /// Copy constructor
      LCDDData(const LCDDData& copy) = delete;
      /// Assignment operator
      //LCDDData& operator=(const LCDDData& copy) = delete;
    public:
      /// Clear data content: releases all allocated resources
      void destroyData(bool destroy_mgr=true);
      /// Clear data content: DOES NOT RELEASEW ALLOCATED RESOURCES!
      void clearData();
      /// Adopt all data from source structure.
      void adoptData(LCDDData& source);
    };

  } /* End namespace Geometry  */
} /* End namespace DD4hep   */
#endif    /* DD4HEP_DDCORE_LCDDDATA_H */
