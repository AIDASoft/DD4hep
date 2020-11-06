//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2017-06-10
//  \version 1.0
//
//==========================================================================
#define __DDG4_STANDALONE_DICTIONARIES__

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Woverlength-strings"

#elif defined(__llvm__) || defined(__clang__) || defined(__APPLE__)

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wunused"
#pragma clang diagnostic ignored "-Woverlength-strings"
#endif

// C/C++ include files
#include "DDG4/DDG4Dict.h"
#include "DDEve/DDEveEventData.h"

namespace { class DDG4IO {}; }

// CINT configuration
#if defined(__MAKECINT__) || defined(__CINT__) || defined(__CLING__) || defined(__ROOTCLING__)
namespace dd4hep { namespace sim {
    typedef Geant4Tracker     SimpleTracker;
    typedef Geant4Calorimeter SimpleCalorimeter;
    typedef Geant4HitData     SimpleHit;
  }}

#else

// C/C++ include files
#include <typeinfo>

// ROOT include files
#include "TROOT.h"
#include "TClass.h"

namespace {
  template <typename T> T* _fill(dd4hep::sim::Geant4HitData* ptr, dd4hep::DDEveHit* target)   {
    T* s = dynamic_cast<T*>(ptr);
    if ( s )   {
      dd4hep::Position* p = &s->position;
      target->x = p->X();
      target->y = p->Y();
      target->z = p->Z();
      target->deposit = s->energyDeposit;
      return s;
    }
    return 0;
  }

  void* _convertHitFunc(void* source, dd4hep::DDEveHit* target)  {
    if (source )  {
      static TClass* cl_calo = gROOT->GetClass(typeid(dd4hep::sim::Geant4Calorimeter::Hit));
      static TClass* cl_tracker = gROOT->GetClass(typeid(dd4hep::sim::Geant4Tracker::Hit));
      //static TClass* cl_particles = gROOT->GetClass(typeid(dd4hep::sim::Geant4Particle));
      void* result = 0;
      dd4hep::sim::Geant4HitData* hit = (dd4hep::sim::Geant4HitData*)source;
      const std::type_info& type = typeid(*hit);
      TClass* cl = gROOT->GetClass(type);
      if ( cl == cl_tracker && (result=_fill<dd4hep::sim::Geant4Tracker::Hit>(hit,target)) ) return result;
      if ( cl == cl_calo && (result=_fill<dd4hep::sim::Geant4Calorimeter::Hit>(hit,target)) ) return result;
    }
    return 0;
  }

  void* _convertParticleFunc(void* source, dd4hep::DDEveParticle* p)  {
    if (source )  {
      dd4hep::sim::Geant4Particle* s = (dd4hep::sim::Geant4Particle*)source;
      p->id        = s->id;
      p->vsx       = s->vsx;
      p->vsy       = s->vsy;
      p->vsz       = s->vsz;
      p->vex       = s->vex;
      p->vey       = s->vey;
      p->vez       = s->vez;
      p->psx       = s->psx;
      p->psy       = s->psy;
      p->psz       = s->psz;
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
    void* (*f)(void*, dd4hep::DDEveHit*);
    void* (*q)(void*, dd4hep::DDEveParticle*);
    FCN(void* (*ff)(void*, dd4hep::DDEveHit*)) { f=ff; }
    FCN(void* (*ff)(void*, dd4hep::DDEveParticle*)) { q=ff; }
  };
  void* _convertHit(const char*)  {
    return FCN(_convertHitFunc).v;
  }
  void* _convertParticle(const char*)  {
    return FCN(_convertParticleFunc).v;
  }

}

#include "DD4hep/Factories.h"
DECLARE_CONSTRUCTOR(DDEve_DDG4HitAccess,_convertHit)
DECLARE_CONSTRUCTOR(DDEve_DDG4ParticleAccess,_convertParticle)
#endif

