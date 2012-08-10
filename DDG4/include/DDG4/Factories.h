// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DDG4_FACTORIES_H
#define DDG4_FACTORIES_H

#ifndef __CINT__
#include "Reflex/PluginService.h"
#endif
#include "RVersion.h"

// Framework include files
#include "DDG4/Defs.h"
#include <string>

// Forward declarations
class Geant4SensitiveDetector;

namespace DD4hep { 
  namespace Geometry   {  class LCDD; }
  namespace Simulation {  class Geant4SensitiveDetector; }
}

namespace {
  template < typename P > class Factory<P, DD4hep::Simulation::Geant4SensitiveDetector*(std::string,DD4hep::Geometry::LCDD*)> {
  public:  typedef DD4hep::Simulation::Geant4SensitiveDetector SD;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) 
      {  *(SD**)retaddr = (SD*)new P(*(std::string*)arg[0], *(DD4hep::Geometry::LCDD*)arg[1]);           }
  };
}

#define DECLARE_GEANT4SENSITIVEDETECTOR(name) namespace DD4hep { namespace Simulation { }}   \
  using DD4hep::Simulation::name;					                     \
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::Geant4SensitiveDetector*(std::string,DD4hep::Geometry::LCDD*))

#endif // DDG4_FACTORIES_H
