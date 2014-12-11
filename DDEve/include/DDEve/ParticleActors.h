// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_PARTICLEHANDLERS_H
#define DD4HEP_DDEVE_PARTICLEHANDLERS_H

// Framework include files
#include "DDEve/EventHandler.h"
#include "DDEve/DisplayConfiguration.h"

// C/C++ include files
#include <map>

// Forward declarations
class TEveTrackPropagator;
class TEveCompound;
class TEveElement;
class TEveLine;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Fill eve particles from a MC particle collection
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct MCParticleCreator : public DDEveParticleActor  {
    typedef std::map<std::string,TEveCompound*> Compounds;
    TEveTrackPropagator* propagator;
    TEveCompound* particles;
    Compounds types;
    int count;
    int lineWidth;
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

} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_PARTICLEHANDLERS_H */

