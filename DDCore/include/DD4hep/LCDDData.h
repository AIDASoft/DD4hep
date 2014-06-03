// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_DDCORE_LCDDDATA_H
#define DD4HEP_DDCORE_LCDDDATA_H

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/ObjectExtensions.h"

// C/C++ include files
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Foward declarations
  class NamedObject;

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class LCDDData   LCDDData.h  DD4hep/LCDDData.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class LCDDData  {

    public:
      struct InvalidObjectError: public std::runtime_error {
      InvalidObjectError(const std::string& msg)
	: std::runtime_error("DD4hep: " + msg) {
        }
      };

    protected:
      struct ObjectHandleMap: public LCDD::HandleMap {
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

      // GDML fields
      ObjectHandleMap m_define;

      DetElement m_world;
      DetElement m_trackers;
      Volume m_worldVol;
      Volume m_trackingVol;

      Material m_materialAir;
      Material m_materialVacuum;
      VisAttr m_invisibleVis;
      OverlayedField m_field;
      Header m_header;
      LCDD::Properties m_properties;
      LCDDBuildType m_buildType;

      /// Definition of the extension type
      ObjectExtensions m_extensions;
    protected:
      /// Default constructor
      LCDDData();
      /// Default destructor
      virtual ~LCDDData();
    public:
      /// Clear data content: releases all allocated resources
      void destroyData();
      /// Clear data content: DOES NOT RELEASEW ALLOCATED RESOURCES!
      void clearData();
      /// Adopt all data from source structure.
      void adoptData(LCDDData& source);
    };

  } /* End namespace Geometry  */
} /* End namespace DD4hep   */
#endif    /* DD4HEP_DDCORE_LCDDDATA_H */
