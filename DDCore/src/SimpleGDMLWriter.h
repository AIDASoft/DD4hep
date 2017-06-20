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

#ifndef DD4HEP_SIMPLEGDMLWRITER_H
#define DD4HEP_SIMPLEGDMLWRITER_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/GeoHandler.h"

// C/C++ include files
#include <set>
#include <map>
#include <vector>

// Forward declarations
class TGeoVolume;
class TGeoNode;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// Simple GDML creator from TGeo geometries in memory
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class SimpleGDMLWriter: public GeoHandler {
    public:
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
      virtual void handleDefines(const Detector::HandleMap& defs) const;
      /// Dump all visualisation specs in Detector format to output stream
      void handleVisualisation(const std::set<VisAttr>& vis) const;
      /// Dump all solids in GDML format to output stream
      virtual void handleSolids(const std::set<TGeoShape*>& solids) const;
      /// Dump Transformations in GDML format to output stream
      virtual void handleTransformations(const std::vector<std::pair<std::string, TGeoMatrix*> >& trafos) const;
      /// Dump structure information in GDML format to output stream
      virtual void handleStructure(const std::vector<Volume>& volset) const;
    };
  }    // End namespace detail
}      // End namespace dd4hep

#endif // DD4HEP_SIMPLEGDMLWRITER_H
