// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4CONVERTER_H
#define DD4HEP_GEANT4CONVERTER_H

// Framework include files
#include "DDG4/Geant4Mapping.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    /** @class Geant4Converter Geant4Converter.h DDG4/Geant4Converter.h
     * 
     * Geometry converter from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Converter : public Geometry::GeoHandler, public Geant4Mapping  {
      bool            m_checkOverlaps;

      /// Initializing Constructor
      Geant4Converter( LCDD& lcdd );

      /// Standard destructor
      virtual ~Geant4Converter();

      /// Create geometry conversion
      Geant4Converter& create(DetElement top);

      /// Convert the geometry type material into the corresponding Geant4 object(s).
      virtual void* handleMaterial(const std::string& name, const TGeoMedium* medium) const;

      /// Convert the geometry type element into the corresponding Geant4 object(s).
      virtual void* handleElement(const std::string& name, const TGeoElement* element) const;

      /// Convert the geometry type solid into the corresponding Geant4 object(s).
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;

      /// Convert the geometry type logical volume into the corresponding Geant4 object(s).
      virtual void* handleVolume(const std::string& name, const TGeoVolume* volume) const;
      virtual void* collectVolume(const std::string& name, const TGeoVolume* volume) const;

      /// Convert the geometry type volume placement into the corresponding Geant4 object(s).
      virtual void* handlePlacement(const std::string& name, const TGeoNode* node) const;

      /// Convert the geometry type field into the corresponding Geant4 object(s).
      ///virtual void* handleField(const std::string& name, Ref_t field) const;

      /// Convert the geometry type region into the corresponding Geant4 object(s).
      virtual void* handleRegion(const TNamed* region, const std::set<const TGeoVolume*>& volumes) const;

      /// Convert the geometry visualisation attributes to the corresponding Geant4 object(s).
      virtual void* handleVis(const std::string& name, const TNamed* vis) const;

      /// Convert the geometry type LimitSet into the corresponding Geant4 object(s).
      virtual void* handleLimitSet(const TNamed* limitset, const std::set<const TGeoVolume*>& volumes) const;

      /// Convert the geometry type SensitiveDetector into the corresponding Geant4 object(s).
      virtual void* handleSensitive(const TNamed* sens_det, const std::set<const TGeoVolume*>& volumes) const;

      /// Handle the geant 4 specific properties
      void handleProperties(LCDD::Properties& prp) const;

      /// Print the geometry type SensitiveDetector
      virtual void* printSensitive(const TNamed* sens_det, const std::set<const TGeoVolume*>& volumes) const;
      /// Print Geant4 placement
      virtual void* printPlacement(const std::string& name, const TGeoNode* node) const;
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4CONVERTER_H
