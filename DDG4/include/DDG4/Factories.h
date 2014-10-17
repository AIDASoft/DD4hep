// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DDG4_FACTORIES_H
#define DDG4_FACTORIES_H

#include "DD4hep/Plugins.h"
#include "RVersion.h"

// Framework include files
#include "DDG4/Defs.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <string>
#include <map>

// Forward declarations
class G4ParticleDefinition;
class G4VSensitiveDetector;
class G4MagIntegratorStepper;
class G4EquationOfMotion;
class G4MagneticField;
class G4Mag_EqRhs;
class G4VPhysicsConstructor;
class G4VUserPhysicsList;
class G4VProcess;

namespace DD4hep {
  namespace Geometry {
    class DetElement;
    class LCDD;
  }
  namespace Simulation {
    class Geant4Context;
    class Geant4Action;
    class Geant4Converter;
    class Geant4Sensitive;
    class Geant4UserPhysics;
    class Geant4EventReader;
    class Geant4PhysicsListActionSequence;

    /// Templated factory method to invoke setup action
    template <typename T> class Geant4SetupAction {
    public:
      static long create(Geometry::LCDD& lcdd, const Geant4Converter& cnv, const std::map<std::string, std::string>& attrs);
    };
    /// Deprecated: Templated factory method to create sensitive detector
    template <typename T> class Geant4SensitiveDetectorFactory {
    public:
      static G4VSensitiveDetector* create(const std::string& name, DD4hep::Geometry::LCDD& lcdd);
    };
  }
}

namespace {
  namespace DS = DD4hep::Simulation;

  typedef DD4hep::Geometry::LCDD LCDD;
  typedef DD4hep::Geometry::DetElement DE;
  typedef DS::Geant4Action GA;
  typedef DS::Geant4Context CT;
  typedef std::string STR;
  typedef const std::vector<void*>& ARGS;

  template <typename P> class Factory<P, long(LCDD*, const DS::Geant4Converter*, const std::map<STR, STR>*)> {
  public:
    typedef DS::Geant4Converter cnv_t;
    typedef std::map<STR, STR> attrs_t;
    static void Func(void *ret, void*, ARGS arg, void*) {
      long r = DS::Geant4SetupAction<P>::create(*(LCDD*) arg[0], *(cnv_t*) arg[1], *(attrs_t*) arg[2]);
      new (ret) (long)(r);
    }
  };

  /// Factory to create Geant4 sensitive detectors
  template <typename P> class Factory<P, G4VSensitiveDetector*(STR, LCDD*)> {
  public:
    typedef G4VSensitiveDetector SD;
    static void Func(void *ret, void*, ARGS arg, void*) {
      *(void**) ret = DS::Geant4SensitiveDetectorFactory<P>::create(*(STR*) arg[0], *(LCDD*) arg[1]);
    }
    //{  *(SD**)ret = (SD*)new P(*(STR*)arg[0], *(LCDD*)arg[1]);           }
  };

  /// Factory to create Geant4 sensitive detectors
  template <typename P> class Factory<P, DS::Geant4Sensitive*(CT*, STR, DE*, LCDD*)> {
  public:
    typedef DS::Geant4Sensitive _S;
    static void Func(void *ret, void*, ARGS arg, void*) {
      *(_S**) ret = (_S*) new P((CT*) arg[0], *(STR*) arg[1], *(DE*) arg[2], *(LCDD*) arg[3]);
    }
  };

  /// Factory to create Geant4 sensitive detectors
  template <typename P> class Factory<P, DS::Geant4Action*(CT*, STR)> {
  public:
    static void Func(void *ret, void*, ARGS arg, void*) {
      *(GA**) ret = (GA*) new P((CT*) arg[0], *(STR*) arg[1]);
    }
  };

  /// Templated Factory for constructors without argument
  template <typename P, typename R> struct FF0 {
    static void Func(void *ret, void*, ARGS, void*) {
      *(R*) ret = (R) (new P());
    }
  };
  /// Templated Factory for constructors with exactly 1 argument
  template <typename P, typename R, typename A0> struct FF1 {
    static void Func(void *ret, void*, ARGS arg, void*) {
      *(R*) ret = (R) (new P((A0) arg[0]));
    }
  };

  /// Factory to create Geant4 steppers
  template <typename P> class Factory<P, G4MagIntegratorStepper*(G4EquationOfMotion*)> : public FF1<P, G4MagIntegratorStepper*,
      G4EquationOfMotion*> {
  };
  /// Factory to create Geant4 steppers
  template <typename P> class Factory<P, G4MagIntegratorStepper*(G4Mag_EqRhs*)> : public FF1<P, G4MagIntegratorStepper*,
      G4Mag_EqRhs*> {
  };
  /// Factory to create Geant4 equations of motion for magnetic fields
  template <typename P> class Factory<P, G4Mag_EqRhs*(G4MagneticField*)> : public FF1<P, G4Mag_EqRhs*, G4MagneticField*> {
  };

  /// Factory to create Geant4 Processes
  template <typename P> class Factory<P, G4VProcess*()> : public FF0<P, G4VProcess*> {
  };
  /// Factory to create Geant4 Processes
  template <typename P> class Factory<P, G4VPhysicsConstructor*()> : public FF0<P, G4VPhysicsConstructor*> {
  };

  template <typename P> class Factory<P, G4ParticleDefinition*()> {
  public:
    static void Func(void *ret, void*, ARGS, void*) {
      *(G4ParticleDefinition**) ret = P::Definition();
    }
  };
  template <typename P> class Factory<P, long()> {
  public:
    static void Func(void *ret, void*, ARGS, void*) {
      P::ConstructParticle();
      *(long*) ret = 1L;
    }
  };
  /// Factory to create Geant4 physics constructions
  template <typename P> class Factory<P, G4VUserPhysicsList*(DS::Geant4PhysicsListActionSequence*, int)> {
  public:
    static void Func(void *ret, void*, ARGS a, void*) {
      DD4hep::printout(DD4hep::INFO,"PhysicsList","+++ Create physics list of type:%s",
		       DD4hep::typeName(typeid(P)).c_str());
      *(G4VUserPhysicsList**) ret = (G4VUserPhysicsList*) new P((DS::Geant4PhysicsListActionSequence*) a[0], *(int*) a[1]);
    }
  };

  /// Factory template to create Geant4 event reader objects
  template <typename P> class Factory<P, DD4hep::Simulation::Geant4EventReader*(std::string)> {  public:
    static void Func(void *ret, void*, const std::vector<void*>& a, void*) 
    { *(DD4hep::Simulation::Geant4EventReader**)ret = (DD4hep::Simulation::Geant4EventReader*)new P(*(std::string*)a[0]);}
  };

}

#define DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(name,func) \
  namespace DD4hep { namespace Simulation { struct external_geant4_sd_##name {}; \
      template <> G4VSensitiveDetector* Geant4SensitiveDetectorFactory< external_geant4_sd_##name >::create(const std::string& n,DD4hep::Geometry::LCDD& l) { return func(n,l); } \
    }}  using DD4hep::Simulation::external_geant4_sd_##name; \
  PLUGINSVC_FACTORY_WITH_ID(external_geant4_sd_##name,std::string(#name),G4VSensitiveDetector*(std::string,DD4hep::Geometry::LCDD*))

/// Plugin definition to create Geant4 sensitive detectors
#define DECLARE_GEANT4SENSITIVEDETECTOR(name) namespace DD4hep { namespace Simulation { struct geant4_sd_##name {}; \
      template <> G4VSensitiveDetector* Geant4SensitiveDetectorFactory< geant4_sd_##name >::create(const std::string& n,DD4hep::Geometry::LCDD& l) { return new name(n,l); } \
    }}  using DD4hep::Simulation::geant4_sd_##name; \
  PLUGINSVC_FACTORY_WITH_ID(geant4_sd_##name,std::string(#name),G4VSensitiveDetector*(std::string,DD4hep::Geometry::LCDD*))
#if 0     /* Equivalent:  */
#define DECLARE_GEANT4SENSITIVEDETECTOR(name)			\
  namespace DD4hep { namespace Simulation {  static G4VSensitiveDetector* __sd_create__##name(const std::string& n,DD4hep::Geometry::LCDD& p) {  return new name(n,p); } }} \
  DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(name,DD4hep::Simulation::__sd_create__##name)
#endif

#define DECLARE_GEANT4SENSITIVE_NS(ns,name) using ns::name; \
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::Geant4Sensitive*(DD4hep::Simulation::Geant4Context*,std::string,DD4hep::Geometry::DetElement*,DD4hep::Geometry::LCDD*))
#define DECLARE_GEANT4SENSITIVE(name) DECLARE_GEANT4SENSITIVE_NS(DD4hep::Simulation,name)

/// Plugin defintion to create Geant4Action objects
#define DECLARE_GEANT4ACTION_NS(ns,name) using ns::name; \
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::Geant4Action*(DD4hep::Simulation::Geant4Context*,std::string))
/// Plugin defintion to create Geant4Action objects
#define DECLARE_GEANT4ACTION(name)  DECLARE_GEANT4ACTION_NS(DD4hep::Simulation,name)

/// Plugin definition to create Geant4 stepper objects
#define DECLARE_GEANT4_STEPPER(name)    PLUGINSVC_FACTORY_WITH_ID(G4##name,std::string(#name),G4MagIntegratorStepper*(G4EquationOfMotion*))
#define DECLARE_GEANT4_MAGSTEPPER(name) PLUGINSVC_FACTORY_WITH_ID(G4##name,std::string(#name),G4MagIntegratorStepper*(G4Mag_EqRhs*))
/// Plugin definition to create Geant4 equations of motion for magnetic fields
#define DECLARE_GEANT4_MAGMOTION(name) PLUGINSVC_FACTORY_WITH_ID(G4##name,std::string(#name),G4Mag_EqRhs*(G4MagneticField*))
/// Plugin definition to create Geant4 physics processes (G4VProcess)
#define DECLARE_GEANT4_PROCESS(name) PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),G4VProcess*())
/// Plugin definition to create Geant4 physics constructors (G4VPhysicsConstructor)
#define DECLARE_GEANT4_PHYSICS(name) PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),G4VPhysicsConstructor*())
/// Plugin definition to force particle constructors for GEANT4 (G4ParticleDefinition)
#define DECLARE_GEANT4_PARTICLE(name) PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),G4ParticleDefinition*())
/// Plugin definition to force particle constructors for GEANT4 (G4XXXXConstructor)
#define DECLARE_GEANT4_PARTICLEGROUP(name) PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),long())
/// Plugin definition to force geant4 physics constructs such as FTFP_BERT (from source/physics_lists/) etc
#define DECLARE_GEANT4_PHYSICS_LIST(name)  typedef DD4hep::Simulation::Geant4UserPhysicsList< name > G4_physics_list_##name; \
  PLUGINSVC_FACTORY_WITH_ID(G4_physics_list_##name,std::string(#name),G4VUserPhysicsList*(DD4hep::Simulation::Geant4PhysicsListActionSequence*,int))

#define DECLARE_GEANT4_SETUP(name,func) \
  namespace DD4hep { namespace Simulation { struct xml_g4_setup_##name {};  \
  template <> long Geant4SetupAction<DD4hep::Simulation::xml_g4_setup_##name>::create(LCDD& l,const DD4hep::Simulation::Geant4Converter& e, const std::map<std::string,std::string>& a) {return func(l,e,a);} }} \
  PLUGINSVC_FACTORY_WITH_ID(xml_g4_setup_##name,std::string(#name "_Geant4_action"),long(DD4hep::Geometry::LCDD*,const DD4hep::Simulation::Geant4Converter*,const std::map<std::string,std::string>*))

/// Plugin defintion to create event reader objects
#define DECLARE_GEANT4_EVENT_READER(name)  \
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::Geant4EventReader*(std::string))

/// Plugin defintion to create event reader objects
#define DECLARE_GEANT4_EVENT_READER_NS(ns,name) typedef ns::name __##name##__; \
  PLUGINSVC_FACTORY_WITH_ID(__##name##__,std::string(#name),DD4hep::Simulation::Geant4EventReader*(std::string))

#endif // DDG4_FACTORIES_H
