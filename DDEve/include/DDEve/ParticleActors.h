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
#ifndef DDEVE_PARTICLEACTORS_H
#define DDEVE_PARTICLEACTORS_H

// Framework include files
#include "DDEve/EventHandler.h"
#include "DDEve/DisplayConfiguration.h"

// C/C++ include files
#include <map>

// Forward declarations
class TEveTrackPropagator;
class TEvePointSet;
class TEveCompound;
class TEveElement;
class TEveLine;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Fill eve particles from a MC particle collection
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct MCParticleCreator : public DDEveParticleActor  {
    typedef std::map<std::string,TEveCompound*> Compounds;
    TEveTrackPropagator* propagator {0};
    TEveCompound* particles {0};
    Compounds types;
    double threshold {10e0}; // 10 MeV
    int count {0};
    int lineWidth {4};
    
    /// Standard initializing constructor
    MCParticleCreator(TEveTrackPropagator* p, TEveCompound* ps, const DisplayConfiguration::Config* cfg);
    /// Access sub-compound by name
    void addCompound(const std::string& name, TEveLine* e);
    /// Access sub-compound by name
    void addCompoundLight(const std::string& name, TEveLine* e);
    /// Action callback of this functor: 
    virtual void operator()(const DDEveParticle& particle);
    /// Close compounds
    void close();
  };

  /// Fill a 3D pointset with particle start vertices
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct StartVertexCreator : public  DDEveParticleActor {
    TEvePointSet* pointset;
    float deposit;
    int count;
    /// Standard initializing constructor
    StartVertexCreator(const std::string& collection, size_t length);
    /// Standard initializing constructor
    StartVertexCreator(const std::string& collection, size_t length, const DisplayConfiguration::Config& cfg);
    /// Standard destructor
    virtual ~StartVertexCreator();
    /// Return eve element
    TEveElement* element() const;
    /// Action callback of this functor: 
    virtual void operator()(const DDEveParticle& particle);
  };

} /* End namespace dd4hep   */


#endif // DDEVE_PARTICLEACTORS_H

