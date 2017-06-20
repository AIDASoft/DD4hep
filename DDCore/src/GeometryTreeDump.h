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

#ifndef DD4HEP_DDCORE_CORETREEDUMP_H
#define DD4HEP_DDCORE_CORETREEDUMP_H

#include "DD4hep/Detector.h"
#include "DD4hep/GeoHandler.h"
#include <set>
#include <map>
#include <vector>
class TGeoVolume;
class TGeoNode;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// Geometry to screen dump action
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class GeometryTreeDump: public GeoHandler {
    public:
      GeometryTreeDump() = default;
      /// Standard destructor
      virtual ~GeometryTreeDump() = default;
      /// Main entry point: create required object(s)
      void create(DetElement top);

      /// Dump logical volume in GDML format to output stream
      virtual void* handleVolume(const std::string& name, Volume volume) const;
      /// Dump single volume transformation in GDML format to output stream
      virtual void* handleTransformation(const std::string& name, const TGeoMatrix* matrix) const;
      /// Dump solid in GDML format to output stream
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Dump all constants in GDML format to output stream
      virtual void handleDefines(const Detector::HandleMap& defs) const;
      /// Dump all visualisation specs in Detector format to output stream
      void handleVisualisation(const Detector::HandleMap& vis) const;
      /// Dump all solids in GDML format to output stream
      virtual void handleSolids(const std::set<TGeoShape*>& solids) const;
      /// Dump Transformations in GDML format to output stream
      virtual void handleTransformations(const std::vector<std::pair<std::string, TGeoMatrix*> >& trafos) const;
      /// Dump structure information in GDML format to output stream
      virtual void handleStructure(const std::set<Volume>& volset) const;
    };
  }    // End namespace detail
}      // End namespace dd4hep
#endif /* DD4HEP_DDCORE_GEOMETRYTREEDUMP_H  */
