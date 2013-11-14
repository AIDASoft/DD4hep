// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_SIMPLEGDMLWRITER_H
#define DD4HEP_SIMPLEGDMLWRITER_H

#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include <set>
#include <map>
#include <vector>
class TGeoVolume;
class TGeoNode;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class SimpleGDMLWriter  SimpleGDMLWriter.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct SimpleGDMLWriter: public GeoHandler {

      /// Reference to output stream
      std::ostream& m_output;

      SimpleGDMLWriter(std::ostream& os)
          : m_output(os) {
      }
      /// Standard destructor
      virtual ~SimpleGDMLWriter() {
      }
      /// Main entry point: create required object(s)
      void create(DetElement top);

      /// Dump logical volume in GDML format to output stream
      virtual void* handleVolume(const std::string& name, const TGeoVolume* volume) const;
      /// Dump single volume transformation in GDML format to output stream
      virtual void* handleTransformation(const std::string& name, const TGeoMatrix* matrix) const;
      /// Dump solid in GDML format to output stream
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Dump all constants in GDML format to output stream
      virtual void handleDefines(const DefinitionSet& defs) const;
      /// Dump all visualisation specs in LCDD format to output stream
      void handleVisualisation(const VisRefs& vis) const;
      /// Dump all solids in GDML format to output stream
      virtual void handleSolids(const SolidSet& solids) const;
      /// Dump Transformations in GDML format to output stream
      virtual void handleTransformations(const TransformSet& trafos) const;
      /// Dump structure information in GDML format to output stream
      virtual void handleStructure(const VolumeSet& volset) const;
    };
  }    // End namespace Geometry
}      // End namespace DD4hep

#endif // DD4HEP_SIMPLEGDMLWRITER_H
