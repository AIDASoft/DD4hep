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
#ifndef DDG4_FACTORIES_H
#define DDG4_FACTORIES_H

// Framework include files
#include "DDG4/Defs.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
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
class G4VPrimaryGenerator;
class G4VProcess;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class DetElement;
  class Detector;

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {
    class GeoHandler;
  }
  
  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    class Geant4Context;
    class Geant4Action;
    class Geant4Converter;
    class Geant4Sensitive;
    class Geant4UserPhysics;
    class Geant4EventReader;
    class Geant4PhysicsListActionSequence;
  }

  /// Templated factory method to invoke setup action
  template <typename T> class Geant4SetupAction : public PluginFactoryBase {
  public:
    static long create(dd4hep::Detector& description, const dd4hep::detail::GeoHandler& cnv, const std::map<str_t,str_t>& attrs);
  };
  /// Deprecated: Templated factory method to create sensitive detector
  template <typename T> class Geant4SensitiveDetectorFactory : public PluginFactoryBase {
  public:
    static G4VSensitiveDetector* create(const str_t& name, dd4hep::Detector& description);
  };

}

namespace {

  namespace DS = dd4hep::sim;
  struct _ns {
    typedef dd4hep::detail::GeoHandler GH;
    typedef dd4hep::sim::Geant4Action GA;
    typedef dd4hep::sim::Geant4Context CT;
    typedef dd4hep::sim::Geant4EventReader RDR;
    typedef std::map<std::string,std::string> STRM;
    typedef G4MagIntegratorStepper Stepper;
  };

  DD4HEP_PLUGIN_FACTORY_ARGS_3(long, dd4hep::Detector*, const _ns::GH*, const _ns::STRM*)   {
    static long ret;
    ret = dd4hep::Geant4SetupAction<P>::create(*a0, *a1, *a2);
    return long(&ret);
  }
  /// Factory to create Geant4 sensitive detectors
  DD4HEP_PLUGIN_FACTORY_ARGS_2(G4VSensitiveDetector*,std::string,dd4hep::Detector*)
  {    return dd4hep::Geant4SensitiveDetectorFactory<P>::create(a0,*a1);  }

  /// Factory to create Geant4 sensitive detectors
  DD4HEP_PLUGIN_FACTORY_ARGS_4(DS::Geant4Sensitive*,_ns::CT*,std::string,dd4hep::DetElement*,dd4hep::Detector*)
  {    return new P(a0, a1, *a2, *a3);  }

  /// Factory to create Geant4 action objects
  DD4HEP_PLUGIN_FACTORY_ARGS_2(DS::Geant4Action*,_ns::CT*, std::string)
  {    return new P(a0,a1);  }

  /// Factory to create Geant4 equations of motion for magnetic fields
  DD4HEP_PLUGIN_FACTORY_ARGS_1(G4Mag_EqRhs*,G4MagneticField*)
  {    return new P(a0);  }

  /// Factory to create Geant4 steppers
  DD4HEP_PLUGIN_FACTORY_ARGS_1(_ns::Stepper*,G4EquationOfMotion*)
  {    return new P(a0);  }

  /// Factory to create Geant4 steppers
  DD4HEP_PLUGIN_FACTORY_ARGS_1(_ns::Stepper*,G4Mag_EqRhs*)
  {    return new P(a0);  }

  /// Factory to create Geant4 Processes
  DD4HEP_PLUGIN_FACTORY_ARGS_0(G4VProcess*)
  {    return (G4VProcess*)new P();  }

  /// Factory to create Geant4 Physics objects
  DD4HEP_PLUGIN_FACTORY_ARGS_0(G4VPhysicsConstructor*)
  {    return new P();  }

  /// Factory to access Geant4 Particle definitions
  DD4HEP_PLUGIN_FACTORY_ARGS_0(G4ParticleDefinition*)
  {    return P::Definition();  }

  /// Factory to create Geant4 primary generator objects
  DD4HEP_PLUGIN_FACTORY_ARGS_0(G4VPrimaryGenerator*)
  {    return new P();  }

  /// Generic particle constructor
  DD4HEP_PLUGIN_FACTORY_ARGS_0(long)  {
    P::ConstructParticle();
    static long ret;
    ret = 1L;
    return long(&ret);
  }

  /// Factory to create Geant4 physics constructions
  DD4HEP_PLUGIN_FACTORY_ARGS_2(G4VUserPhysicsList*,DS::Geant4PhysicsListActionSequence*,int) {
    dd4hep::printout(dd4hep::INFO,"PhysicsList","+++ Create physics list of type:%s",
                     dd4hep::typeName(typeid(P)).c_str());
    return new P(a0,a1);
  }

  /// Factory template to create Geant4 event reader objects
  DD4HEP_PLUGIN_FACTORY_ARGS_1(_ns::RDR*,std::string)
  {    return new P(a0);   }
}

#define __IMPLEMENT_GEANT4SENSDET(name,func) DD4HEP_OPEN_PLUGIN(dd4hep,geant4_sd_##name)  { \
    template <> G4VSensitiveDetector* Geant4SensitiveDetectorFactory< geant4_sd_##name >:: \
      create(const str_t& n,dd4hep::Detector& l) { return func (n,l); }        \
      DD4HEP_PLUGINSVC_FACTORY(geant4_sd_##name,name,G4VSensitiveDetector*(std::string,dd4hep::Detector*),__LINE__)  }

#define DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(id,func) __IMPLEMENT_GEANT4SENSDET(id,func)
#define DECLARE_GEANT4SENSITIVEDETECTOR(id)               __IMPLEMENT_GEANT4SENSDET(id,new id)
#define DECLARE_GEANT4SENSITIVEDETECTOR_NS(ns,id)         __IMPLEMENT_GEANT4SENSDET(id,new ns::id)


#define DECLARE_GEANT4SENSITIVE_NS(name_space,name) using name_space::name; \
  DD4HEP_PLUGINSVC_FACTORY(name,name,DS::Geant4Sensitive*(_ns::CT*,std::string,dd4hep::DetElement*,dd4hep::Detector*),__LINE__)
#define DECLARE_GEANT4SENSITIVE(name)      DECLARE_GEANT4SENSITIVE_NS(dd4hep::sim,name)

/// Plugin defintion to create Geant4Action objects
#define DECLARE_GEANT4ACTION_NS(name_space,name) using name_space::name; \
  DD4HEP_PLUGINSVC_FACTORY(name,name,dd4hep::sim::Geant4Action*(_ns::CT*,std::string),__LINE__)
/// Plugin defintion to create Geant4Action objects
#define DECLARE_GEANT4ACTION(name)         DECLARE_GEANT4ACTION_NS(dd4hep::sim,name)

/// Plugin definition to create Geant4 stepper objects
#define DECLARE_GEANT4_STEPPER(name)       DD4HEP_PLUGINSVC_FACTORY(G4##name,name,_ns::Stepper*(G4EquationOfMotion*),__LINE__)
#define DECLARE_GEANT4_MAGSTEPPER(name)    DD4HEP_PLUGINSVC_FACTORY(G4##name,name,_ns::Stepper*(G4Mag_EqRhs*),__LINE__)
/// Plugin definition to create Geant4 equations of motion for magnetic fields
#define DECLARE_GEANT4_MAGMOTION(name)     DD4HEP_PLUGINSVC_FACTORY(G4##name,name,G4Mag_EqRhs*(G4MagneticField*),__LINE__)
/// Plugin definition to create Geant4 physics processes (G4VProcess)
#define DECLARE_GEANT4_PROCESS(name)       DD4HEP_PLUGINSVC_FACTORY(name,name,G4VProcess*(),__LINE__)
/// Plugin definition to create Geant4 physics constructors (G4VPhysicsConstructor)
#define DECLARE_GEANT4_PHYSICS(name)       DD4HEP_PLUGINSVC_FACTORY(name,name,G4VPhysicsConstructor*(),__LINE__)
/// Plugin definition to create Geant4 physics processes (G4VProcess)
#define DECLARE_GEANT4_GENERATOR(name)     DD4HEP_PLUGINSVC_FACTORY(name,name,G4VPrimaryGenerator*(),__LINE__)
/// Plugin definition to force particle constructors for GEANT4 (G4ParticleDefinition)
#define DECLARE_GEANT4_PARTICLE(name)      DD4HEP_PLUGINSVC_FACTORY(name,name,G4ParticleDefinition*(),__LINE__)
/// Plugin definition to force particle constructors for GEANT4 (G4XXXXConstructor)
#define DECLARE_GEANT4_PARTICLEGROUP(name) DD4HEP_PLUGINSVC_FACTORY(name,name,long(),__LINE__)
/// Plugin definition to force geant4 physics constructs such as FTFP_BERT (from source/physics_lists/) etc
#define DECLARE_GEANT4_PHYSICS_LIST(name)  typedef DS::Geant4UserPhysicsList< name > G4_physics_list_##name; \
  DD4HEP_PLUGINSVC_FACTORY(G4_physics_list_##name,name,G4VUserPhysicsList*(DS::Geant4PhysicsListActionSequence*,int),__LINE__)
/// Plugin defintion for setup actions
#define DECLARE_GEANT4_SETUP(name,func) DD4HEP_OPEN_PLUGIN(dd4hep,xml_g4_setup_##name)  { \
    template <> long Geant4SetupAction< xml_g4_setup_##name >::         \
      create(dd4hep::Detector& l,const _ns::GH& e, const _ns::STRM& a) {return func(l,e,a);} \
      DD4HEP_PLUGINSVC_FACTORY(xml_g4_setup_##name,name##_Geant4_action,long(dd4hep::Detector*,const _ns::GH*,const _ns::STRM*),__LINE__) }

/// Plugin defintion to create event reader objects
#define DECLARE_GEANT4_EVENT_READER(name) DD4HEP_PLUGINSVC_FACTORY(name,name,_ns::RDR*(std::string),__LINE__)

/// Plugin defintion to create event reader objects
#define DECLARE_GEANT4_EVENT_READER_NS(name_space,name) typedef name_space::name __##name##__; \
  DD4HEP_PLUGINSVC_FACTORY(__##name##__,name,_ns::RDR*(std::string),__LINE__)

#endif // DDG4_FACTORIES_H
