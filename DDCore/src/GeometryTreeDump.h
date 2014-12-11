// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRYTREEDUMP_H
#define DD4HEP_GEOMETRYTREEDUMP_H

#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include <set>
#include <map>
#include <vector>
class TGeoVolume;
class TGeoNode;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Geometry to screen dump action
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class GeometryTreeDump: public GeoHandler {
    public:
      GeometryTreeDump() {
      }
      /// Standard destructor
      virtual ~GeometryTreeDump() {
      }
      /// Main entry point: create required object(s)
      void create(DetElement top);

      /// Dump logical volume in GDML format to output stream
      virtual void* handleVolume(const std::string& name, Volume volume) const;
      /// Dump single volume transformation in GDML format to output stream
      virtual void* handleTransformation(const std::string& name, const TGeoMatrix* matrix) const;
      /// Dump solid in GDML format to output stream
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Dump all constants in GDML format to output stream
      virtual void handleDefines(const LCDD::HandleMap& defs) const;
      /// Dump all visualisation specs in LCDD format to output stream
      void handleVisualisation(const LCDD::HandleMap& vis) const;
      /// Dump all solids in GDML format to output stream
      virtual void handleSolids(const SolidSet& solids) const;
      /// Dump Transformations in GDML format to output stream
      virtual void handleTransformations(const TransformSet& trafos) const;
      /// Dump structure information in GDML format to output stream
      virtual void handleStructure(const VolumeSet& volset) const;
    };
  }    // End namespace Geometry
}      // End namespace DD4hep

#endif // DD4HEP_GEOMETRYTREEDUMP_H
