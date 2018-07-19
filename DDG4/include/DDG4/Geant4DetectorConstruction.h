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

#ifndef DD4HEP_GEANT4DETECTORCONSTRUCTION_H
#define DD4HEP_GEANT4DETECTORCONSTRUCTION_H

// Framework include files
#include "DD4hep/DetElement.h"
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4VUserDetectorConstruction;
class G4VSensitiveDetector;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4UserLimits;
class G4Material;
class G4Element;
class G4VSolid;
class G4Region;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4Kernel;
    class Geant4GeometryInfo;
    class Geant4AssemblyVolume;
    class Geant4DetectorConstruction;
    class Geant4DetectorConstructionContext;
    class Geant4DetectorConstructionSequence;

    /// Geant4 detector construction context definition.
    /**
     * Detector construction context to allow the workers a simplified
     * access to the object created.
     *
     * The context is a stack based object. Do not keep a pointer to
     * The object scope does not span beyond the actional function call.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorConstructionContext  {
    public:
      /// Reference to geometry object
#ifdef __CINT__
      Detector*     description;
#else
      Detector&     description;
#endif
      /// Reference to the world after construction
      G4VPhysicalVolume*  world;
      /// The cached geometry information
      Geant4GeometryInfo* geometry;
      /// G4 User detector initializer
      G4VUserDetectorConstruction* detector;
      /// Initializing Constructor
      Geant4DetectorConstructionContext(Detector& l,G4VUserDetectorConstruction* d)
        : description(l), world(0), geometry(0), detector(d)  { }
      /// Default destructor
      ~Geant4DetectorConstructionContext()             { }
      /// Helper: Assign sensitive detector to logical volume
      void setSensitiveDetector(G4LogicalVolume* vol, G4VSensitiveDetector* sd);
    };

    /// Basic implementation of the Geant4 detector construction action.
    /**
     * Concrete implementation of the Geant4 detector construction action.
     * The sequences as the object's master dispatches the callbacks for 
     * the "constructGeo()" method to create a detector geometry and the
     * "constructSensitives()" call to create the sensitive detectors
     * and the "constructField" to each worker subclassing
     * Geant4DetectorConstruction.
     *
     * Please note:
     * constructField() and constructSensitives() are executed in a 
     * thread-local context. See the Geant4 documentation for details:
     * https://twiki.cern.ch/twiki/bin/view/Geant4/QuickMigrationGuideForGeant4V10
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorConstruction : public Geant4Action {
    public:
      /// Standard Constructor
      Geant4DetectorConstruction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4DetectorConstruction();
      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt);
      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt);
      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt);
    };

    /// Concrete basic implementation of the Geant4 detector construction sequencer.
    /**
     * Concrete implementation of the Geant4 detector construction sequence.
     * The sequence dispatches the callbacks for the "Construct()" method to
     * create a detector geometry and to dispatch the "ConstructSDandField()"
     * calls for each worker.
     *
     * Please note:
     * constructField() and constructSensitives() are executed in a 
     * thread-local context. See the Geant4 documentation for details:
     * https://twiki.cern.ch/twiki/bin/view/Geant4/QuickMigrationGuideForGeant4V10
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorConstructionSequence : public Geant4Action {
    protected:
      /// The list of action objects to be called
      Actors<Geant4DetectorConstruction> m_actors;

    public:
      /// Standard Constructor
      Geant4DetectorConstructionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4DetectorConstructionSequence();
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt);
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4DetectorConstruction* action);
      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt);
      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt);
      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt);

#if !defined(__CINT__)
      //@{ Accessor to the various geant4 maps after construction

      /// Access to the converted materials
      const std::map<Material, G4Material*>& materials() const;
      /// Access to the converted elements
      const std::map<Atom, G4Element*>& elements() const;
      /// Access to the converted shapes
      const std::map<const TGeoShape*, G4VSolid*>& shapes() const;
      /// Access to the converted volumes
      const std::map<Volume, G4LogicalVolume*>& volumes() const;
      /// Access to the converted placements
      const std::map<PlacedVolume, G4VPhysicalVolume*>& placements() const;
      /// Access to the converted assemblys
      const std::map<PlacedVolume, Geant4AssemblyVolume*>& assemblies() const;

      /// Access to the converted limit sets
      const std::map<LimitSet, G4UserLimits*>& limits() const;
      /// Access to the converted regions
      const std::map<Region, G4Region*>& regions() const;
#endif

      //@}
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4DETECTORCONSTRUCTION_H
