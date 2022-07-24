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
#ifndef DDG4_GEANT4PLACEMENTPARAMETERISATION_H
#define DDG4_GEANT4PLACEMENTPARAMETERISATION_H

/// Geant4 include files
#include <G4PVParameterised.hh>
#include <G4VPVParameterisation.hh>

/// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to dump the records of the intrinsic Geant4 event model.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PlacementParameterisation : public G4VPVParameterisation  {
    public:

      /// Helper structure to cache dimension variabled from setup parameters
      struct Dimension  {
	G4Transform3D delta        {   };
	G4ThreeVector translation  {   };
	std::size_t   count        { 0 };

      public:
	/// Initializing constructor
	Dimension(const G4Transform3D& d, std::size_t c)
	  : delta(d), translation(d.getTranslation()), count(c) {}
	/// Default constructor
	Dimension() = default;
	/// Move Constructor
	Dimension(Dimension&&) = default;
	/// Copy Constructor
	Dimension(const Dimension&) = default;
	/// Assignment operator
	Dimension& operator=(Dimension&&) = default;
	/// Assignment operator
	Dimension& operator=(const Dimension&) = default;
      };

      using Parameters   = PlacedVolume::Object::Parameterisation;
      using Rotations    = std::vector<G4RotationMatrix>;
      using Translations = std::vector<G4ThreeVector>;
      using Dimensions   = std::vector<Dimension>;

      /// Setup parameters: Reference to the DD4hep placement handle
      PlacedVolume         m_placement;
      /// Setup parameters: Reference to the DD4hep placement parameters
      const Parameters&    m_params;
      /// Setup parameters: Initial parameterisation position
      Dimension            m_start          {  };
      /// Setup parameters: Dimensional parameter setup
      Dimensions           m_dimensions     {  };

      /// Axis definition
      EAxis                m_axis           { kUndefined };
      /// Total number of parameterized cells
      size_t               m_num_cells      { 0UL };
      /// Optimization flag for simple parameteristions
      bool                 m_have_rotation  { false };

      /// Cached rotations and translations for complex parameterisations
      mutable Rotations    m_rotations;
      mutable Translations m_translations;

      /// Callback to store resulting rotation
      void operator()(const G4Transform3D& transform);

    public:
      /// Initializing constructor
      Geant4PlacementParameterisation(PlacedVolume pv);
      /// Standard destructor
      virtual ~Geant4PlacementParameterisation() = default;
      /// Access Axis direction
      EAxis axis()  const     {   return m_axis;  }
      /// Access number of replicas
      std::size_t count()  const;
      /// G4VPVParameterisation overload: Compute copy transformation
      virtual void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* vol) const override;
    };
  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4PLACEMENTPARAMETERISATION_H
