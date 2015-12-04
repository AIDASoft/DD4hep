// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#define __DDG4_STANDALONE_DICTIONARIES__

// C/C++ include files
#include "DDG4/DDG4Dict.h"
#include "DDEve/DDEveEventData.h"

// CINT configuration
#if defined(__MAKECINT__) || defined(__CINT__)
namespace DD4hep { namespace Simulation {
    typedef Geant4Tracker SimpleTracker;
    typedef Geant4Calorimeter SimpleCalorimeter;
    typedef Geant4HitData SimpleHit;
  }}
#else

// C/C++ include files
#include <typeinfo>

// ROOT include files
#include "TROOT.h"
#include "TClass.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace {
  template <typename T> T* _fill(Geant4HitData* ptr, DDEveHit* target)   {
    T* s = dynamic_cast<T*>(ptr);
    if ( s )   {
      Simulation::Position* p = &s->position;
      target->x = p->X();
      target->y = p->Y();
      target->z = p->Z();
      target->deposit = s->energyDeposit;
      return s;
    }
    return 0;
  }

  void* _convertHitFunc(void* source, DDEveHit* target)  {
    if (source )  {
      static TClass* cl_calo = gROOT->GetClass(typeid(Geant4Calorimeter::Hit));
      static TClass* cl_tracker = gROOT->GetClass(typeid(Geant4Tracker::Hit));
      //static TClass* cl_particles = gROOT->GetClass(typeid(Geant4Particle));
      void* result = 0;
      Geant4HitData* hit = (Geant4HitData*)source;
      const std::type_info& type = typeid(*hit);
      TClass* cl = gROOT->GetClass(type);
      if ( cl == cl_tracker && (result=_fill<Geant4Tracker::Hit>(hit,target)) ) return result;
      if ( cl == cl_calo && (result=_fill<Geant4Calorimeter::Hit>(hit,target)) ) return result;
    }
    return 0;
  }

  void* _convertParticleFunc(void* source, DDEveParticle* p)  {
    if (source )  {
      Geant4Particle* s = (Geant4Particle*)source;
      p->id = s->id;
      p->vsx = s->vsx;
      p->vsy = s->vsy;
      p->vsz = s->vsz;
      p->vex = s->vex;
      p->vey = s->vey;
      p->vez = s->vez;
      p->psx = s->psx;
      p->psy = s->psy;
      p->psz = s->psz;
      p->pdgID     = s->pdgID;
      p->parent    = s->parents.empty() ? -1 : *(s->parents.begin());
      p->energy    = std::sqrt(s->vsx*s->vsx + s->vsy*s->vsy + s->vsz*s->vsz + s->mass*s->mass);
      p->time      = s->time;
      p->daughters = s->daughters;
      return p;
    }
    return 0;
  }

  union FCN  {
    void* v;
    void* (*f)(void*, DDEveHit*);
    void* (*q)(void*, DDEveParticle*);
    FCN(void* (*ff)(void*, DDEveHit*)) { f=ff; }
    FCN(void* (*ff)(void*, DDEveParticle*)) { q=ff; }
  };
  void* _convertHit(const char*)  {
    return FCN(_convertHitFunc).v;
  }
  void* _convertParticle(const char*)  {
    return FCN(_convertParticleFunc).v;
  }

}

#include "DD4hep/Factories.h"
using namespace DD4hep::Geometry;
DECLARE_CONSTRUCTOR(DDEve_DDG4HitAccess,_convertHit)
DECLARE_CONSTRUCTOR(DDEve_DDG4ParticleAccess,_convertParticle)
#endif

