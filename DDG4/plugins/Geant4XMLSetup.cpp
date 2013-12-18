// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Printout.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DDG4/Factories.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Config.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
typedef DD4hep::Geometry::LCDD lcdd_t;

namespace DD4hep  {

  struct XMLSetup;
  struct ActionSequence;
  using namespace DD4hep::Simulation;
  using namespace DD4hep::Simulation::Setup;

  template <typename TYPE, typename PTR> TYPE* _action(PTR* in)  {
    return dynamic_cast<TYPE*>(in);
  }
  template <typename T> static void _setAttributes(const T& handle, xml_h& e)  {
    XML::Handle_t props(e);
    // Now we set the object properties
    vector<XML::Attribute> attrs = props.attributes();
    for(vector<XML::Attribute>::iterator i=attrs.begin(); i!=attrs.end(); ++i)   {
      XML::Attribute a = *i;
      handle[XML::_toString(props.attr_name(a))].str(props.attr<string>(a));
    }
  }
  template <typename T> static void _setProperties(const T& handle, xml_h& e)  {
    xml_comp_t action(e);
    // Now we set the object properties
    XML::Handle_t  props = action.child(_Unicode(properties),false);
    if ( props )  {
      _setAttributes(handle, props);
    }
  }

  static Action _convertSensitive(LCDD& lcdd, xml_h e, const string& detector)  {
    xml_comp_t action(e);
    Kernel& kernel = Kernel::access(lcdd);
    TypeName tn = TypeName::split(action.attr<string>(_U(name)));
    // Create the object using the factory method
    Sensitive handle(kernel,action.attr<string>(_U(name)),detector);
    _setProperties(Action(handle.get()),e);
    for(xml_coll_t f(e,_Unicode(filter)); f; ++f)  {
      string nam = f.attr<string>(_U(name));
      Filter filter(kernel.globalFilter(nam,false));
      filter->installMessengers();
      handle->adopt(filter);
    }
    handle->installMessengers();
    printout(INFO,"Geant4Setup","+++ Added sensitive element %s of type %s",
	     tn.second.c_str(),tn.first.c_str());
    return Action(handle);
  }

  static Action _convertAction(LCDD& lcdd, xml_h e)  {
    xml_comp_t action(e);
    Kernel& kernel = Kernel::access(lcdd);
    TypeName tn = TypeName::split(action.attr<string>(_U(name)));
    // Create the object using the factory method
    Action handle(kernel,action.attr<string>(_U(name)));
    _setProperties(handle,e);
    printout(INFO,"Geant4Setup","+++ Added action %s of type %s",tn.second.c_str(),tn.first.c_str());
    handle->installMessengers();
    return handle;
  }
  enum { SENSITIVE, ACTION, FILTER };
  Action _createAction(LCDD& lcdd, xml_h a, const std::string& seqType, int what)  {
    string   nam = a.attr<string>(_U(name));
    TypeName typ    = TypeName::split(nam);
    Kernel&  kernel = Kernel::access(lcdd);
    Action action((what==FILTER) ? (Geant4Action*)kernel.globalFilter(typ.second,false)
		  : (what==ACTION) ? kernel.globalAction(typ.second,false)
		  ///  : (what==FILTER) ? kernel.globalAction(typ.second,false)
		  : 0);
    // Create the object using the factory method
    if ( !action )  {
      action = (what == SENSITIVE) ? Action(_convertSensitive(lcdd, a, seqType))
	: (what==ACTION) ? _convertAction(lcdd, a)
	: (what==FILTER) ? _convertAction(lcdd, a)
	: Action();
      if ( !action )  {
	throw runtime_error(format("Geant4ActionSequence","DDG4: The action '%s'"
				   " cannot be created. [Action-Missing]",nam.c_str()));
      }
    }
    return action;
  }

  /** Convert Geant4Action objects
   *  <actions>
   *    <action name="Geant4PostTrackingAction/PostTrackAction"
   *      <properties 
   *         NAME1="Value1"
   *         NAME2="Value2" />
   *    </action>
   *  </actions>
   */
  template <> void Converter<Action>::operator()(xml_h e)  const  {
    Action a = _convertAction(lcdd, e);
    Kernel::access(lcdd).registerGlobalAction(a);
  }

  /** Convert Sensitive detector filters
   *
   *  Note: Filters are Geant4Actions and - if global - may also receive properties!
   *
   *  <filters>
   *    <filter name="GeantinoRejector"/>
   *    <filter name="EnergyDepositMinimumCut">
   *      <properties cut="10*MeV"/>
   *    </filter>
   *  </filters>
   */
  template <> void Converter<Filter>::operator()(xml_h e)  const  {
    Action a = _convertAction(lcdd, e);
    Kernel::access(lcdd).registerGlobalFilter(a);
  }

  /** Convert Geant4Phase objects
   *  <phases>
   *    <phase name="Geant4PostTrackingPhase/PostTrackPhase"
   *      <properties 
   *         NAME1="Value1"
   *         NAME2="Value2" />
   *    </phase>
   *  </phases>
   */
  template <> void Converter<Phase>::operator()(xml_h e)  const  {
    xml_comp_t x_phase(e);
    Kernel& kernel = Kernel::access(lcdd);
    string nam = x_phase.attr<string>(_U(type));
    typedef Geant4ActionPhase PH;
    Phase p;

    if ( nam == "RunAction/begin" )  {
      void (Geant4ActionPhase::*func)(const G4Run*) = &Geant4ActionPhase::call;
      kernel.runAction().callAtBegin((p=kernel.addPhase<const G4Run*>(nam)).get(),func);
      //&Geant4ActionPhase::call<const G4Run*>);
    }
    else if ( nam == "RunAction/end" )  {
      void (Geant4ActionPhase::*func)(const G4Run*) = &Geant4ActionPhase::call;
      kernel.runAction().callAtEnd((p=kernel.addPhase<const G4Run*>(nam)).get(),func);
      //&PH::call<const G4Run*>);
    }
    else if ( nam == "EventAction/begin" )  {
      void (Geant4ActionPhase::*func)(const G4Event*) = &Geant4ActionPhase::call;
      kernel.eventAction().callAtBegin((p=kernel.addPhase<const G4Event*>(nam)).get(),func);
      //&PH::call<const G4Event*>);
    }
    else if ( nam == "EventAction/end" )  {
      void (Geant4ActionPhase::*func)(const G4Event*) = &Geant4ActionPhase::call;
      kernel.eventAction().callAtEnd((p=kernel.addPhase<const G4Event*>(nam)).get(),func);
      //&PH::call<const G4Event*>);
    }
    else if ( nam == "TrackingAction/begin" )  {
      void (Geant4ActionPhase::*func)(const G4Track*) = &Geant4ActionPhase::call;
      kernel.trackingAction().callAtBegin((p=kernel.addPhase<const G4Track*>(nam)).get(),func);
      //&PH::call<const G4Track*>);
    }
    else if ( nam == "TrackingAction/end" )  {
      void (Geant4ActionPhase::*func)(const G4Track*) = &Geant4ActionPhase::call;
      kernel.trackingAction().callAtEnd((p=kernel.addPhase<const G4Track*>(nam,false)).get(),func);
      //&PH::call<const G4Track*>);
    }
    else if ( nam == "StackingAction/newStage" )  {
      kernel.stackingAction().callAtNewStage((p=kernel.addPhase<void>(nam,false)).get(),&PH::call);
    }
    else if ( nam == "StackingAction/prepare" )  {
      kernel.stackingAction().callAtPrepare((p=kernel.addPhase<void>(nam,false)).get(),&PH::call);
    }
    else if ( nam == "SteppingAction" )  {
      void (Geant4ActionPhase::*func)(const G4Step*,G4SteppingManager*) = &Geant4ActionPhase::call;
      kernel.steppingAction().call((p=kernel.addPhase<const G4Step*>(nam)).get(),func);
      //&PH::call<const G4Step*,G4SteppingManager*>);
    }
    else if ( nam == "GeneratorAction/primaries" )  {
      void (Geant4ActionPhase::*func)(G4Event*) = &Geant4ActionPhase::call;
      kernel.generatorAction().call((p=kernel.addPhase<G4Event*>(nam)).get(),func);
      //&PH::call<G4Event*>);
    }
    else   {
      TypeName tn = TypeName::split(nam);
      DetElement det = lcdd.detector(tn.first);
      if ( !det.isValid() )   {
	throw runtime_error(format("Phase","DDG4: The phase '%s' of type SensitiveSeq"
				   " cannot be attached to a non-existing detector"
				   " [Detector-Missing]",nam.c_str()));
      }

      Geometry::SensitiveDetector sd = lcdd.sensitiveDetector(tn.first);
      if ( !sd.isValid() )  {
	throw runtime_error(format("Phase","DDG4: The phase '%s' of type SensitiveSeq"
				   " cannot be attached to a non-existing sensitive detector"
				   " [Sensitive-Missing]",nam.c_str()));
      }
      SensitiveSeq sdSeq = SensitiveSeq(kernel,tn.first);
      if ( tn.second == "begin" )
	sdSeq->callAtBegin((p=kernel.addPhase<G4HCofThisEvent*>(tn.second)).get(),
			   &PH::call<G4HCofThisEvent*>);
      else if ( tn.second == "end" )
	sdSeq->callAtEnd((p=kernel.addPhase<G4HCofThisEvent*>(tn.second)).get(),
			 &PH::call<G4HCofThisEvent*>);
      else if ( tn.second == "clear" )
	sdSeq->callAtClear((p=kernel.addPhase<G4HCofThisEvent*>(tn.second)).get(),
			   &PH::call<G4HCofThisEvent*>);
      else if ( tn.second == "process" )
	sdSeq->callAtProcess((p=kernel.addPhase<G4Step*>(tn.second)).get(),
			     &PH::call<G4Step*,G4TouchableHistory*>);
      else 
	throw runtime_error(format("Phase","DDG4: The phase '%s' of type SensitiveSeq"
				   " cannot be attached to the call '%s'."
				   " [Callback-Missing]",tn.first.c_str(), tn.second.c_str()));
    }
  }

  /** Convert Action sequences into objects
   *  <sequences>
   *    <sequence name="Geant4EventActionSequence/EventAction"
   *      <member name="" type="Geant4TrackerEventMonitor/TrackerEvtAction"/>
   *    </sequence>
   *    <sequence name="Geant4SensdetActionSequence/SiVertexBarrel"
   *      <member type="Geant4TrackerSensitiveMonitor/TrackerHitMonitor">
   *        <properties 
   *           NAME1="Value1"
   *           NAME2="Value2" />
   *      </member>
   *    </sequence>
   *  </sequences>
   */
  template <> void Converter<ActionSequence>::operator()(xml_h e)  const  {
    xml_comp_t seq(e);
    SensitiveSeq sdSeq;
    string       seqNam;
    TypeName     seqType;
    int          what = ACTION;
    Kernel& kernel = Kernel::access(lcdd);

    if ( seq.hasAttr(_U(sd)) )   {
      seqNam  = seq.attr<string>(_U(type))+"/"+seq.attr<string>(_U(sd));
      seqType = TypeName::split(seqNam);
      sdSeq   = SensitiveSeq(kernel,seqNam);
      what    = SENSITIVE;
    }
    else {
      seqNam = seq.attr<string>(_U(name));
      seqType = TypeName::split(seqNam);
    }
    printout(INFO,"Geant4Setup","+++ ActionSequence %s of type %s added.",
	     seqType.second.c_str(),seqType.first.c_str());

    if ( seqType.second == "PhysicsList" )  {
      PhysicsActionSeq pl(&kernel.physicsList());
      PropertyManager& m = kernel.physicsList().properties();
      m.dump();
      _setAttributes(pl,e);
      m.dump();
    }

    for(xml_coll_t a(seq,_Unicode(action)); a; ++a)  {
      string   nam = a.attr<string>(_U(name));
      Action action(_createAction(lcdd,a,seqType.second,what));
      if ( seqType.second == "RunAction" )
	kernel.runAction().adopt(_action<RunAction::handled_type>(action.get()));
      else if ( seqType.second == "EventAction" )
	kernel.eventAction().adopt(_action<EventAction::handled_type>(action.get()));
      else if ( seqType.second == "GeneratorAction" )
	kernel.generatorAction().adopt(_action<GenAction::handled_type>(action.get()));
      else if ( seqType.second == "TrackingAction" )
	kernel.trackingAction().adopt(_action<TrackAction::handled_type>(action.get()));
      else if ( seqType.second == "StackingAction" )
	kernel.stackingAction().adopt(_action<StackAction::handled_type>(action.get()));
      else if ( seqType.second == "SteppingAction" )
	kernel.steppingAction().adopt(_action<StepAction::handled_type>(action.get()));
      else if ( seqType.second == "PhysicsList" )
	kernel.physicsList().adopt(_action<PhysicsList::handled_type>(action.get()));
      else if ( sdSeq.get() )
	sdSeq->adopt(_action<Sensitive::handled_type>(action.get()));
      else   {
	throw runtime_error(format("ActionSequence","DDG4: The action '%s'"
				   " cannot be attached to any sequence '%s'."
				   " [Sequence-Missing]",nam.c_str(), seqNam.c_str()));	
      }
      printout(INFO,"Geant4Setup","+++ ActionSequence %s added filter object:%s",
	       seqType.second.c_str(),action->name().c_str());
    }
    if ( what == SENSITIVE )  {
      for(xml_coll_t a(seq,_Unicode(filter)); a; ++a)  {
	string   nam = a.attr<string>(_U(name));
	Action action(_createAction(lcdd,a,"",FILTER));
	action->installMessengers();
	printout(INFO,"Geant4Setup","+++ ActionSequence %s added filter object:%s",
		 seqType.second.c_str(),action->name().c_str());
	if ( sdSeq.get() )
	  sdSeq->adopt(_action<Filter::handled_type>(action.get()));
	else   {
	  throw runtime_error(format("ActionSequence","DDG4: The action '%s'"
				     " cannot be attached to any sequence '%s'."
				     " [Sequence-Missing]",nam.c_str(), seqNam.c_str()));	
	}
      }
    }
  }

  /** Convert PhysicsList objects
   *  <physicslist>
   *    <processes>
   *      <particle name="'e-'">
   *  	    <process name="G4eMultipleScattering" ordAtRestDoIt="-1" ordAlongSteptDoIt="1" ordPostStepDoIt="1"/>
   *        <process name="G4eIonisation"         ordAtRestDoIt="-1" ordAlongSteptDoIt="2" ordPostStepDoIt="2"/>
   *      </particle>
   *    </processes>
   *  </physicslist>
   */
  template <> void Converter<Geant4PhysicsList::ParticleProcesses>::operator()(xml_h e) const {
    xml_comp_t part(e);
    string part_name = part.nameStr();
    Geant4PhysicsList::ParticleProcesses& procs = 
      _object<Geant4PhysicsList>().processes(part_name);
    for(xml_coll_t q(part,_Unicode(process)); q; ++q)  {
      xml_comp_t proc(q);
      Geant4PhysicsList::Process p;
      p.name = proc.nameStr();
      p.ordAtRestDoIt     = proc.attr<int>(_Unicode(ordAtRestDoIt));
      p.ordAlongSteptDoIt = proc.attr<int>(_Unicode(ordAlongSteptDoIt));
      p.ordPostStepDoIt   = proc.attr<int>(_Unicode(ordPostStepDoIt));
      procs.push_back(p);
      printout(INFO,"Geant4Setup","+++ Converter<ParticleProcesses: Particle:%s add process %s %d %d %d",
	       part_name.c_str(),p.name.c_str(),p.ordAtRestDoIt,p.ordAlongSteptDoIt,p.ordPostStepDoIt);
    }
  }

  /** Convert PhysicsList objects: Particle constructors
   *  <physicslist>
   *    <particles>
   *      <construct name="G4Electron"/>
   *      <construct name="G4Gamma"/>
   *      <construct name="G4BosonConstructor"/>
   *      <construct name="G4LeptonConstructor"/>
   *      <construct name="G4BaryonConstructor"/>
   *    </particles>
   *  </physicslist>
   */
  template <> void Converter<Geant4PhysicsList::ParticleConstructor>::operator()(xml_h e) const {
    Geant4PhysicsList::ParticleConstructors& parts = _object<Geant4PhysicsList>().particles();
    xml_comp_t part(e);
    string n = part.nameStr();
    parts.push_back(n);
    printout(INFO,"Geant4Setup","+++ ParticleConstructor: Add Geant4 particle constructor '%s'",n.c_str());
  }

  /** Convert PhysicsList objects: Physics constructors
   *  <physicslist>
   *    <physics>
   *      <construct name="G4EmStandardPhysics"/>
   *      <construct name="HadronPhysicsQGSP"/>
   *    </physics>
   *  </physicslist>
   */
  template <> void Converter<Geant4PhysicsList::PhysicsConstructor>::operator()(xml_h e) const {
    Geant4PhysicsList::PhysicsConstructors& parts = _object<Geant4PhysicsList>().physics();
    xml_comp_t part(e);
    string n = part.nameStr();
    parts.push_back(n);
    printout(INFO,"Geant4Setup","+++ PhysicsConstructor: Add Geant4 physics constructor '%s'",n.c_str());
  }


  /** Convert PhysicsList objects: Predefined Geant4 Physics lists
   *  <physicslist>
   *    <list name="TQGSP_FTFP_BERT_95"/>
   *  </physicslist>
   *  Note: list items are Geant4Actions and - if global - may receive properties!
   */
  struct PhysicsListExtension;
  template <> void Converter<PhysicsListExtension>::operator()(xml_h e) const {
    Kernel& kernel = Kernel::access(lcdd);
    string ext = xml_comp_t(e).nameStr();
    kernel.physicsList().properties()["extends"].str(ext);
    printout(INFO,"Geant4Setup","+++ PhysicsListExtension: Set predefined Geant4 physics list to '%s'",ext.c_str());
  }

  template <> void Converter<PhysicsList>::operator()(xml_h e)  const  {
    string name = e.attr<string>(_U(name));
    Kernel& kernel = Kernel::access(lcdd);
    PhysicsList handle(kernel,name);
    _setAttributes(handle,e);
    xml_coll_t(e,_Unicode(particles)).for_each(_Unicode(construct),Converter<Geant4PhysicsList::ParticleConstructor>(lcdd,handle.get()));
    xml_coll_t(e,_Unicode(processes)).for_each(_Unicode(particle),Converter<Geant4PhysicsList::ParticleProcesses>(lcdd,handle.get()));
    xml_coll_t(e,_Unicode(physics)).for_each(_Unicode(construct),Converter<Geant4PhysicsList::PhysicsConstructor>(lcdd,handle.get()));
    xml_coll_t(e,_Unicode(extends)).for_each(Converter<PhysicsListExtension>(lcdd,handle.get()));
    kernel.physicsList().adopt(handle);
  }

  template <> void Converter<XMLSetup>::operator()(xml_h seq)  const  {
    xml_elt_t compact(seq);
    // First execute the basic setup from the plugins module
    ROOT::Reflex::PluginService::Create<TNamed*>("geant4_xml_setup",&lcdd,&seq);
    // Now deal with the new stuff.....
    xml_coll_t(compact,_Unicode(actions) ).for_each(_Unicode(action),Converter<Action>(lcdd,param));
    xml_coll_t(compact,_Unicode(filters) ).for_each(_Unicode(filter),Converter<Filter>(lcdd,param));
    xml_coll_t(compact,_Unicode(sequences) ).for_each(_Unicode(sequence),Converter<ActionSequence>(lcdd,param));
    xml_coll_t(compact,_Unicode(phases) ).for_each(_Unicode(phase),Converter<Phase>(lcdd,param));
    xml_coll_t(compact,_Unicode(physicslist)).for_each(Converter<PhysicsList>(lcdd,param));
  }
}
static long setup_Geant4(lcdd_t& lcdd, const xml_h& element) {
  (Converter<XMLSetup>(lcdd))(element);
  return 1;
}
DECLARE_XML_DOC_READER(geant4_setup,setup_Geant4)
