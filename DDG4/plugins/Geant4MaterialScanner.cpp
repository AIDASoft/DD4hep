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
    class Geant4MaterialScanner : public Geant4SteppingAction  {
    protected:
      /// Structure to hold the information of one simulation step.
      class StepInfo {
      public:
        /// Pre-step and Post-step position
        Position pre, post;
        /// Reference to the logical volue
        const G4LogicalVolume* volume;

        /// Initializing constructor
        StepInfo(const Position& pre, const Position& post, const G4LogicalVolume* volume);
        /// Copy constructor
        StepInfo(const StepInfo& c);
        /// Default destructor
        ~StepInfo() {}
        /// Assignment operator
        StepInfo& operator=(const StepInfo& c);
      };
      typedef std::vector<StepInfo*> Steps;

      double m_sumX0     = 0E0;
      double m_sumLambda = 0E0;
      double m_sumPath   = 0E0;
      Steps  m_steps;

    public:
      /// Standard constructor
      Geant4MaterialScanner(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4MaterialScanner();
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

// $Id: $
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

using namespace std;
using namespace dd4hep::sim;

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4MaterialScanner)

/// Initializing constructor
Geant4MaterialScanner::StepInfo::StepInfo(const Position& prePos, const Position& postPos, const G4LogicalVolume* vol)
: pre(prePos), post(postPos), volume(vol)
{
}

/// Copy constructor
Geant4MaterialScanner::StepInfo::StepInfo(const StepInfo& c)
: pre(c.pre), post(c.post), volume(c.volume)
{
}

/// Assignment operator
Geant4MaterialScanner::StepInfo& Geant4MaterialScanner::StepInfo::operator=(const StepInfo& c)  {
  pre = c.pre;
  post = c.post;
  volume = c.volume;
  return *this;
}

/// Standard constructor
Geant4MaterialScanner::Geant4MaterialScanner(Geant4Context* ctxt, const string& nam)
  : Geant4SteppingAction(ctxt,nam)
{
  m_needsControl = true;
  eventAction().callAtBegin(this,&Geant4MaterialScanner::beginEvent);
  trackingAction().callAtEnd(this,&Geant4MaterialScanner::end);
  trackingAction().callAtBegin(this,&Geant4MaterialScanner::begin);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4MaterialScanner::~Geant4MaterialScanner() {
  InstanceCount::decrement(this);
}

/// User stepping callback
void Geant4MaterialScanner::operator()(const G4Step* step, G4SteppingManager*) {
  Geant4StepHandler h(step);
#if 0
  Geant4TouchableHandler pre_handler(step);
  string prePath = pre_handler.path();
  Geant4TouchableHandler post_handler(step);
  string postPath = post_handler.path();
#endif
  G4LogicalVolume* logVol = h.logvol(h.pre);
  m_steps.push_back(new StepInfo(h.prePos(), h.postPos(), logVol));
}

/// Registered callback on Begin-event
void Geant4MaterialScanner::beginEvent(const G4Event* /* event */)   {
  for_each(m_steps.begin(),m_steps.end(),detail::DestroyObject<StepInfo*>());
  m_steps.clear();
  m_sumX0 = 0;
  m_sumLambda = 0;
  m_sumPath = 0;
}

/// Begin-of-tracking callback
void Geant4MaterialScanner::begin(const G4Track* track) {
  printP2("Starting tracking action for track ID=%d",track->GetTrackID());
  for_each(m_steps.begin(),m_steps.end(),detail::DestroyObject<StepInfo*>());
  m_steps.clear();
  m_sumX0 = 0;
  m_sumLambda = 0;
  m_sumPath = 0;
}

/// End-of-tracking callback
void Geant4MaterialScanner::end(const G4Track* track) {
  using namespace CLHEP;
  if ( !m_steps.empty() )  {
    const char* line = " +--------------------------------------------------------------------------------------------------------------------------------------------------\n";
    const char* fmt1 = " | %5d %-20s %3.0f %8.3f %8.4f %11.4f  %11.4f %10.3f %8.2f %11.6f %11.6f  (%7.2f,%7.2f,%7.2f)\n";
    const char* fmt2 = " | %5d %-20s %3.0f %8.3f %8.4f %11.6g  %11.6g %10.3f %8.2f %11.6f %11.6f  (%7.2f,%7.2f,%7.2f)\n";
    const Position& pre = m_steps[0]->pre;
    const Position& post = m_steps[m_steps.size()-1]->post;

    ::printf("%s + Material scan between: x_0 = (%7.2f,%7.2f,%7.2f) [cm] and x_1 = (%7.2f,%7.2f,%7.2f) [cm]  TrackID:%d: \n%s",
             line,pre.X()/cm,pre.Y()/cm,pre.Z()/cm,post.X()/cm,post.Y()/cm,post.Z()/cm,track->GetTrackID(),line);
    ::printf(" |     \\   %-11s        Atomic                 Radiation   Interaction               Path   Integrated  Integrated    Material\n","Material");
    ::printf(" | Num. \\  %-11s   Number/Z   Mass/A  Density    Length       Length    Thickness   Length      X0        Lambda      Endpoint  \n","Name");
    ::printf(" | Layer \\ %-11s            [g/mole]  [g/cm3]     [cm]        [cm]          [cm]      [cm]     [cm]        [cm]     (     cm,     cm,     cm)\n","");
    ::printf("%s",line);
    int count = 1;
    for(Steps::const_iterator i=m_steps.begin(); i!=m_steps.end(); ++i, ++count)  {
      const G4LogicalVolume* logVol = (*i)->volume;
      G4Material* material = logVol->GetMaterial();
      const Position& prePos  = (*i)->pre;
      const Position& postPos = (*i)->post;
      Position direction = postPos - prePos;
      double length  = direction.R()/cm;
      double intLen  = material->GetNuclearInterLength()/cm;
      double radLen  = material->GetRadlen()/cm;
      double density = material->GetDensity()/(gram/cm3);
      double nLambda = length / intLen;
      double nx0     = length / radLen;
      double Aeff    = 0.0;
      double Zeff    = 0.0;
      const char* fmt = radLen >= 1e5 ? fmt2 : fmt1;
      const double* fractions = material->GetFractionVector();
      for(size_t j=0; j<material->GetNumberOfElements(); ++j)  {
        Zeff += fractions[j]*(material->GetElement(j)->GetZ());
        Aeff += fractions[j]*(material->GetElement(j)->GetA())/gram;
      }
      m_sumX0     += nx0;
      m_sumLambda += nLambda;
      m_sumPath   += length;
      ::printf(fmt,count,material->GetName().c_str(),
               Zeff, Aeff, density, radLen, intLen, length,
               m_sumPath,m_sumX0,m_sumLambda,
               postPos.X()/cm,postPos.Y()/cm,postPos.Z()/cm);
      //cout << *m << endl;
    }
    for_each(m_steps.begin(),m_steps.end(),detail::DestroyObject<StepInfo*>());
    m_steps.clear();
  }
}
