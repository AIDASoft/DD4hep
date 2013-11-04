// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKINGPOSTACTION_H
#define DD4HEP_DDG4_GEANT4TRACKINGPOSTACTION_H

// Framework include files
#include "DDG4/Geant4TrackingAction.h"
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    /** @class Geant4TrackingPostAction Geant4TrackingPostAction.h DDG4/Geant4TrackingPostAction.h
     * 
     * Default base class for all geant 4 tracking actions.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackingPostAction : public Geant4TrackingAction   {
    protected:
      typedef std::vector<std::string> StringV;
      StringV m_requiredProcs;
      StringV m_ignoredProcs;
      bool    m_storeMarkedTracks;
    public:
      /// Standard constructor
      Geant4TrackingPostAction(Geant4Context* context, const std::string& name="");
      /// Default destructor
      virtual ~Geant4TrackingPostAction();

      ///
      void saveTrack(const G4Track* track);

      /// Begin-of-tracking callback
      virtual void begin(const G4Track* track);
      /// End-of-tracking callback
      virtual void end(const G4Track* track);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINGPOSTACTION_H
