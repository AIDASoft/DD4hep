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

class G4VSensitiveDetector;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {
#if 0
    /** @class XMLElementFactory Factories.h DDCore/Factories.h
     *  Create an arbitrary object from it's XML representation.
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    2012/07/31
     */
    template <typename T> class Geant4SensitiveDetectorFactory  {
    public:
      static G4VSensitiveDetector* create(const std::string& name,LCDD& lcdd) {  return new T(name,lcdd); }
    }; 
#endif
  }    // End namespace Simulation
}      // End namespace DD4hep

namespace {
  template < typename P > class Factory<P, G4VSensitiveDetector*(const std::string& name,DD4hep::Geometry::LCDD*)> {
  public:
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      typedef G4VSensitiveDetector SD;
      std::string nam = *(std::string*)arg[0];
      DD4hep::Geometry::LCDD* lcdd = (DD4hep::Geometry::LCDD* )arg[1];
      *(SD**)retaddr = new P(nam,*lcdd);
    }
  };
}

#define DECLARE_GEANT4SENSITIVEDETECTOR(name) \
  using DD4hep::Simulation::name;			     \
  PLUGINSVC_FACTORY(name,G4VSensitiveDetector*(const std::string&,DD4hep::Geometry::LCDD*))


#endif // DDG4_FACTORIES_H
