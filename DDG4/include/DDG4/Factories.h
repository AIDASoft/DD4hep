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
#include <map>

// Forward declarations
class G4MagIntegratorStepper;
class G4EquationOfMotion;
class G4MagneticField;
class G4Mag_EqRhs;

namespace DD4hep { 
  namespace Geometry   {  
    class LCDD; 
  }
  namespace Simulation {  
    class Geant4Converter;
    class Geant4SensitiveDetector; 
    template <typename T> class Geant4SetupAction  {
    public:
      static long create(Geometry::LCDD& lcdd, const Geant4Converter& cnv, const std::map<std::string,std::string>& attrs);
    };
  }
}

namespace {

  template < typename P > class Factory<P, long(DD4hep::Geometry::LCDD*,const DD4hep::Simulation::Geant4Converter*,const std::map<std::string,std::string>*)> {
  public:
    typedef DD4hep::Geometry::LCDD              LCDD;
    typedef DD4hep::Simulation::Geant4Converter cnv_t;
    typedef std::map<std::string,std::string>   attrs_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD*    lcdd = (LCDD* )arg[0];
      cnv_t*   cnv  = (cnv_t*)arg[1];
      attrs_t* attr = (attrs_t*)arg[2];
      long ret = DD4hep::Simulation::Geant4SetupAction<P>::create(*lcdd,*cnv,*attr);
      new(retaddr) (long)(ret);
    }
  };

  /// Factory to create Geant4 sensitive detectors
  template <typename P> class Factory<P, DD4hep::Simulation::Geant4SensitiveDetector*(std::string,DD4hep::Geometry::LCDD*)> {
  public:  typedef DD4hep::Simulation::Geant4SensitiveDetector SD;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) 
      {  *(SD**)retaddr = (SD*)new P(*(std::string*)arg[0], *(DD4hep::Geometry::LCDD*)arg[1]);           }
  };

  /// Factory to create Geant4 steppers
  template <typename P> class Factory<P, G4MagIntegratorStepper*(G4EquationOfMotion*)> {
  public: static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      P* stepper = new P((G4EquationOfMotion*)arg[0]);
      *(G4MagIntegratorStepper**)retaddr = (G4MagIntegratorStepper*)stepper;
    }
  };

  /// Factory to create Geant4 steppers
  template <typename P> class Factory<P, G4MagIntegratorStepper*(G4Mag_EqRhs*)> {
  public: static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      P* stepper = new P((G4Mag_EqRhs*)arg[0]);
      *(G4MagIntegratorStepper**)retaddr = (G4MagIntegratorStepper*)stepper;
    }
  };

  /// Factory to create Geant4 equations of motion for magnetic fields
  template <typename P> class Factory<P, G4Mag_EqRhs*(G4MagneticField*)> {
  public: static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      P* o = new P((G4MagneticField*)arg[0]);
      *(G4Mag_EqRhs**)retaddr = (G4Mag_EqRhs*)o;
    }
  };
}

/// Plugin definition to create Geant4 sensitive detectors
#define DECLARE_GEANT4SENSITIVEDETECTOR(name) namespace DD4hep { namespace Simulation { }}  using DD4hep::Simulation::name; \
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::Geant4SensitiveDetector*(std::string,DD4hep::Geometry::LCDD*))

/// Plugin definition to create Geant4 stpper objects
#define DECLARE_GEANT4_STEPPER(name)    PLUGINSVC_FACTORY_WITH_ID(G4##name,std::string(#name),G4MagIntegratorStepper*(G4EquationOfMotion*))
#define DECLARE_GEANT4_MAGSTEPPER(name) PLUGINSVC_FACTORY_WITH_ID(G4##name,std::string(#name),G4MagIntegratorStepper*(G4Mag_EqRhs*))

/// Plugin definition to create Geant4 equations of motion for magnetic fields
#define DECLARE_GEANT4_MAGMOTION(name) PLUGINSVC_FACTORY_WITH_ID(G4##name,std::string(#name),G4Mag_EqRhs*(G4MagneticField*))

#define DECLARE_GEANT4_SETUP(name,func) \
  namespace DD4hep { namespace Simulation { namespace { struct xml_g4_setup_##name {}; }               \
  template <> long Geant4SetupAction<DD4hep::Simulation::xml_g4_setup_##name>::create(LCDD& l,const DD4hep::Simulation::Geant4Converter& e, const std::map<std::string,std::string>& a) {return func(l,e,a);} }} \
  PLUGINSVC_FACTORY_WITH_ID(xml_g4_setup_##name,std::string(#name "_Geant4_action"),long(DD4hep::Geometry::LCDD*,const DD4hep::Simulation::Geant4Converter*,const std::map<std::string,std::string>*))
#endif // DDG4_FACTORIES_H
