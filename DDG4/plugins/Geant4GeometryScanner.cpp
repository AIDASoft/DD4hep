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
#include "DD4hep/Objects.h"
#include "DDG4/Defs.h"
#include "DDG4/Geant4SteppingAction.h"

// Forward declarations
class G4LogicalVolume;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    /// Class to perform directional material scans using Geantinos.
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GeometryScanner : public Geant4SteppingAction  {
    protected:
      /// Structure to hold the information of one simulation step.
      class StepInfo {
      public:
        /// Pre-step and Post-step position
        Position pre, post;
        /// Path to this volume
        std::string path;
        /// Reference to the logical volue
        const G4LogicalVolume* volume;

        /// Initializing constructor
        StepInfo(const Position& pre,
                 const Position& post,
                 const G4LogicalVolume* volume,
                 const std::string& path);
        /// Copy constructor
        StepInfo(const StepInfo& c);
        /// Default destructor
        ~StepInfo() {}
        /// Assignment operator
        StepInfo& operator=(const StepInfo& c);
      };
      typedef std::vector<StepInfo*> Steps;
      Steps  m_steps;
      double m_sumPath = 0;
    public:
      /// Standard constructor
      Geant4GeometryScanner(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeometryScanner();
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
      /// Begin-of-tracking callback
      virtual void begin(const G4Track* track);
      /// End-of-tracking callback
      virtual void end(const G4Track* track);
      /// Registered callback on Begin-event
      void beginEvent(const G4Event* event);
    };
  }
}

//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

using namespace std;
using namespace dd4hep::sim;

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4GeometryScanner)

/// Initializing constructor
Geant4GeometryScanner::StepInfo::StepInfo(const Position& prePos,
                                          const Position& postPos,
                                          const G4LogicalVolume* vol,
                                          const string& p)
: pre(prePos), post(postPos), path(p), volume(vol)
{
}

/// Copy constructor
Geant4GeometryScanner::StepInfo::StepInfo(const StepInfo& c)
  : pre(c.pre), post(c.post), path(c.path), volume(c.volume)
{
}

/// Assignment operator
Geant4GeometryScanner::StepInfo& Geant4GeometryScanner::StepInfo::operator=(const StepInfo& c)  {
  pre = c.pre;
  post = c.post;
  volume = c.volume;
  return *this;
}

/// Standard constructor
Geant4GeometryScanner::Geant4GeometryScanner(Geant4Context* ctxt, const string& nam)
  : Geant4SteppingAction(ctxt,nam)
{
  m_needsControl = true;
  eventAction().callAtBegin(this,&Geant4GeometryScanner::beginEvent);
  trackingAction().callAtEnd(this,&Geant4GeometryScanner::end);
  trackingAction().callAtBegin(this,&Geant4GeometryScanner::begin);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4GeometryScanner::~Geant4GeometryScanner() {
  InstanceCount::decrement(this);
}

/// User stepping callback
void Geant4GeometryScanner::operator()(const G4Step* step, G4SteppingManager*) {
  Geant4StepHandler      h(step);
  Geant4TouchableHandler handler(step);
  m_steps.emplace_back(new StepInfo(h.prePos(), h.postPos(), h.logvol(h.pre), handler.path()));
}

/// Registered callback on Begin-event
void Geant4GeometryScanner::beginEvent(const G4Event* /* event */)   {
  for_each(m_steps.begin(),m_steps.end(),detail::DestroyObject<StepInfo*>());
  m_steps.clear();
  m_sumPath = 0;
}

/// Begin-of-tracking callback
void Geant4GeometryScanner::begin(const G4Track* track) {
  printP2("Starting tracking action for track ID=%d",track->GetTrackID());
  for_each(m_steps.begin(),m_steps.end(),detail::DestroyObject<StepInfo*>());
  m_steps.clear();
  m_sumPath = 0;
}

/// End-of-tracking callback
void Geant4GeometryScanner::end(const G4Track* track)  {
  if ( !m_steps.empty() )  {
    constexpr const char* line = " +--------------------------------------------------------------------------------------------------------------------------------------------------\n";
    constexpr const char* fmt = " | %5d %11.4f %9.3f   (%7.2f,%7.2f,%7.2f)  Path:\"/world%s\" Shape:%s  Mat:%s\n";
    const Position& start = m_steps[0]->pre;
    const Position& stop = m_steps[m_steps.size()-1]->post;

    ::printf("%s + Geometry scan between: x_0 = (%7.2f,%7.2f,%7.2f) [cm] and x_1 = (%7.2f,%7.2f,%7.2f) [cm]  TrackID:%d: \n%s",
             line,start.X()/CLHEP::cm,start.Y()/CLHEP::cm,start.Z()/CLHEP::cm,
             stop.X()/CLHEP::cm,stop.Y()/CLHEP::cm,stop.Z()/CLHEP::cm,
             track->GetTrackID(),line);
    ::printf(" |     \\                Path                                        \n");
    ::printf(" | Num. \\  Thickness    Length   Endpoint                   Volume , Shape , Material\n");
    ::printf(" | Layer \\   [cm]        [cm]    (     cm,     cm,     cm)         \n");
    ::printf("%s",line);
    int count = 1;
    for(Steps::const_iterator i=m_steps.begin(); i!=m_steps.end(); ++i, ++count)  {
      const G4LogicalVolume* logVol = (*i)->volume;
      G4Material*     material  = logVol->GetMaterial();
      G4VSolid*       solid     = logVol->GetSolid();
      const Position& prePos    = (*i)->pre;
      const Position& postPos   = (*i)->post;
      Position        direction = postPos - prePos;
      double          length    = direction.R()/CLHEP::cm;
      m_sumPath += length;
      ::printf(fmt,count,
               length, m_sumPath,
               postPos.X()/CLHEP::cm,postPos.Y()/CLHEP::cm,postPos.Z()/CLHEP::cm,
               (*i)->path.c_str(), typeName(typeid(*solid)).c_str(), material->GetName().c_str());
    }
    for_each(m_steps.begin(),m_steps.end(),detail::DestroyObject<StepInfo*>());
    m_steps.clear();
  }
}
