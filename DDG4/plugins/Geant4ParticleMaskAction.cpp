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

// Framework include files
#include <DD4hep/Primitives.h>
#include <DD4hep/InstanceCount.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4SensDetAction.h>
#include <DDG4/Geant4UserParticleHandler.h>

// C/C++ include files
#include <cstdint>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Geant4ParticleHandler user extension action to modify the particle reason mask
    /**
     * This action marks the 'reason' bitmask of a Geant4Particle with a given bitmask
     * if a sub-detector created a hit. Multiple subdetectors can be set to mask
     * different values by the options <action>.Masks = {'det1': 4, 'det2': 1024, ... }
     *
     * Please note:
     *
     * This is a simple example of a user supplied Geant4UserParticleHandler
     * Do not artificially complicate it. Enhanced functionality can also be achieved
     * by chaining other sub-classes of Geant4UserParticleHandler instances.
     *
     *
     * @author  M.Frank
     * @version 1.0
     */       
    class Geant4ParticleMaskAction : public Geant4UserParticleHandler  {
    protected:
      /// Map of Subdetector names to trigger setting of the mask : Bit mask to be set to particle 
      std::map<std::string, int32_t>  m_nameMasks;
      /// Map of Subdetector types to trigger setting of the mask : Bit mask to be set to particle 
      std::map<std::string, int32_t>  m_typeMasks;
      
    public:
      /// Standard constructor
      Geant4ParticleMaskAction(Geant4Context* ctxt, const std::string& nam)
        : Geant4UserParticleHandler(ctxt,nam)
      {
        declareProperty("DetectorNameMasks", m_nameMasks );
        declareProperty("DetectorTypeMasks", m_typeMasks );
        m_needsControl = true;
      }

      /// User overload to handle particle settings when processing the track in the Geant4ParticleHandler.
      void mark_track(const G4Track*            /* track */,
                      Particle*                    curr_track,
                      Geant4SensDetActionSequence* sens_det )  {
        using PropertyMask = dd4hep::detail::ReferenceBitMask<int>;
        PropertyMask mask(curr_track->reason);
        if( !m_nameMasks.empty() )  {
          auto it = m_nameMasks.begin();
          auto nam = sens_det->name();
          for( ; it != m_nameMasks.end(); ++it )  {
            if( it->first == nam || it->first == "*" )  {
              print("+++ Detector: name: %s Masking track %d with mask: %08X",
                    nam.c_str(), curr_track->id, it->second);
              mask.set(it->second);
            }
          }
        }
        if( !m_typeMasks.empty() )  {
          auto it = m_typeMasks.begin();          
          auto nam = sens_det->sensitiveType();
          for( ; it != m_typeMasks.end(); ++it )  {
            if( it->first == nam || it->first == "*" )  {
              print("+++ Detector: type: %s Masking track %d with mask: %08X",
                    nam.c_str(), curr_track->id, it->second);
              mask.set(it->second);
            }
          }
        }
      }
    }; // End class Geant4ParticleMaskAction
  }    // End namespace sim
}      // End namespace dd4hep

#include <DDG4/Geant4Particle.h>
#include <DDG4/Factories.h>
using namespace dd4hep::sim;
DECLARE_GEANT4ACTION(Geant4ParticleMaskAction)
