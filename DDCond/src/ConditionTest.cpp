// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace {
  
  /// Dump the conditions of one detectpr element
  void dump_element(DetElement elt) {
    string test = "ConditionsTest1";
    Conditions conds = elt.conditions();
    printout(INFO,test,"DetElement:%s # of conditons:%d",elt.path().c_str(),int(conds.count()));
    const Conditions::Entries& entries = conds.entries();
    for(Conditions::Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
      Condition c((*i).second);
      string type = c.type();
      printout(INFO,test,"           %s Condition[%s]: %s [%s] Validity:%s",
	       c.detector().path().c_str(), type.c_str(), c.name().c_str(), 
	       c.value().c_str(), c.validity().c_str());

      if ( type == "alignment" )  {
	c.bind<string>();
      }
      else if ( type == "temperature" )  {
	c.bind<double>();
	printout(INFO,test,"                %s : double value:%g ",
		 c.name().c_str(), c.get<double>());
      }
      else if ( type == "pressure" )  {
	c.bind<double>();
	printout(INFO,test,"                %s : double value:%g [%g hPa]",
		 c.name().c_str(), c.get<double>(),
		 _multiply(c.get<double>(),"1.0/hPa"));
      }
      else if ( type == "whatever" )  {
	c.bind<vector<double> >();
	const vector<double>& v = c.get<vector<double> >();
	printout(INFO,test,"                %s : vector<double> size:%d = %s",
		 c.name().c_str(), int(v.size()), c.block().str().c_str());
      }
      printout(INFO,test,  "                Type: %s",typeName(c.typeInfo()).c_str());
    }
  }

  /// Dump conditions tree of a detector element
  void dump_conditions(DetElement elt) {
    Conditions conds = elt.conditions();
    const DetElement::Children& children = elt.children();
    if ( !conds.isValid() )
      printout(INFO,"ConditionsTest1","DetElement:%s  NO CONDITIONS present",elt.path().c_str());
    else
      dump_element(elt);
    for(DetElement::Children::const_iterator j=children.begin(); j!=children.end(); ++j)
      dump_conditions((*j).second);
  }

  /// Test execution function
  long conditions_test(LCDD& lcdd, int argc, char** argv) {
    DetElement det = lcdd.world();
    string args = "";
    for(int i=0; i<argc; ++i) { args += argv[i], args += " "; };
    printout(INFO,"ConditionsTest1","Args: %s",args.c_str());
    dump_conditions(det);
    return 1;
  }
}

DECLARE_APPLY(ConditionsTest1,conditions_test)


namespace {

  struct Callee  {
    void call(unsigned int tags, DetElement& det, void* param)    {
      if ( DetElement::CONDITIONS_CHANGED == (tags&DetElement::CONDITIONS_CHANGED) )
	printout(INFO,"Callee","+++ Conditions update %s param:%p",det.path().c_str(),param);
      if ( DetElement::PLACEMENT_CHANGED == (tags&DetElement::PLACEMENT_CHANGED) )  
	printout(INFO,"Callee","+++ Alignment update %s param:%p",det.path().c_str(),param);
    }
  };

  /// Callback conditions tree of a detector element
  void callback_install(DetElement elt, Callee* c) {
    Position local, global;
    const DetElement::Children& children = elt.children();
    elt.localToWorld(local, global);
    elt.callAtUpdate(DetElement::CONDITIONS_CHANGED|DetElement::PLACEMENT_CHANGED,c,&Callee::call);
    for(DetElement::Children::const_iterator j=children.begin(); j!=children.end(); ++j)
      callback_install((*j).second,c);
  }

  /// Test execution function
  long callback_test(LCDD& lcdd, int /* argc */, char** /* argv */) {
    DetElement det = lcdd.world();
    callback_install(det, new Callee());
    return 1;
  }
}

DECLARE_APPLY(CallbackInstallTest,callback_test)
