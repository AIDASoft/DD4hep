// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"

#include "DDG4/Geant4Setup.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4ActionPhase.h"


// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4SetupFactory<CINT>::Geant4SetupFactory(Geometry::LCDD& lcdd) : m_lcdd(lcdd)  {
}

/// Default destructor
Geant4SetupFactory<CINT>::~Geant4SetupFactory()   {
}

/// Create the Geant4 Kernel object
Geant4Kernel& Geant4SetupFactory<CINT>::kernel()  {
  static Geant4Kernel* k = new Geant4Kernel(m_lcdd);
  return *k;
}

typedef pair<string,string> TypeName;
static TypeName splitName(const string& type_name)  {
  size_t idx = type_name.find("/");
  string typ=type_name, nam=type_name;
  if ( idx != string::npos )  {
    typ = type_name.substr(0,idx);
    nam = type_name.substr(idx+1);
  }
  return make_pair(typ,nam);
}

template <typename TYPE> static inline TYPE* checked_value(TYPE* p)    {
  if ( p )  {
    return p;
  }
  throw runtime_error(format("Geant4Handle","Attempt to access an invalid object of type:%s!",
			     typeinfoName(typeid(TYPE)).c_str()));
}

template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle() : value(0) 
{
}

template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle(TYPE* typ) : value(typ) 
{
}

template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle(Geant4Handle<TYPE>& handle)
: value(0) 
{
  value = handle.release();
}

template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle(const Geant4Kernel& kernel,const string& type_name)
: value(0) 
{
  TypeName typ = splitName(type_name);
  Geant4Context* ctxt = kernel.context();
  Geant4Action* object = PluginService::Create<Geant4Action*>(typ.first,ctxt,typ.second);
  if ( !object && typ.first == typ.second )  {
    typ.first = typeinfoName(typeid(TYPE));
    printout(DEBUG,"Geant4Handle<Geant4Sensitive>",
	     "Object factory for %s not found. Try out %s",
	     typ.second.c_str(),typ.first.c_str());
    object = PluginService::Create<Geant4Action*>(typ.first,ctxt,typ.second);
    if ( !object )  {
      size_t idx = typ.first.rfind(':');
      if ( idx != string::npos ) typ.first = string(typ.first.substr(idx+1));
      printout(DEBUG,"Geant4Handle<Geant4Sensitive>",
	       "Try out object factory for %s",typ.first.c_str());
      object = PluginService::Create<Geant4Action*>(typ.first,ctxt,typ.second);
    }
  }
  if ( object )  {
    TYPE* ptr = dynamic_cast<TYPE*>(object);
    if ( ptr )  {
      value = ptr;
      return;
    }
    throw runtime_error(format("Geant4Handle",
			       "Failed to convert object of type %s to handle of type %s!",
			       type_name.c_str(),typeinfoName(typeid(TYPE)).c_str()));
  }
  throw runtime_error(format("Geant4Handle","Failed to create object of type %s!",type_name.c_str()));
}

template <typename TYPE> Geant4Handle<TYPE>::~Geant4Handle()  {
  value = 0;
}

template <typename TYPE> Property& Geant4Handle<TYPE>::operator[](const string& property_name) const  {
  PropertyManager& pm = checked_value(value)->properties();
  return pm[property_name];
}

template <typename TYPE> Geant4Handle<TYPE>::operator TYPE*() const   {
  return checked_value(value);
}

template <typename TYPE> TYPE* Geant4Handle<TYPE>::release()   {
  TYPE* ptr = checked_value(value);
  value = 0;
  return ptr;
}

template <typename TYPE> TYPE* Geant4Handle<TYPE>::get() const   {
  return checked_value(value);
}
template <typename TYPE> TYPE* Geant4Handle<TYPE>::operator->() const   {
  return checked_value(value);
}


template <typename TYPE> Geant4Handle<TYPE>& Geant4Handle<TYPE>::operator=(Geant4Handle& handle)  {
  value = handle.release();
  return *this;
}

template <typename TYPE> Geant4Handle<TYPE>& Geant4Handle<TYPE>::operator=(TYPE* typ)   {
  value = typ;
  return *this;
}

namespace DD4hep {  namespace Simulation   {

    template <> Geant4Handle<Geant4Sensitive>::Geant4Handle(const Geant4Kernel& kernel, const string& type_name, const string& detector)  {
      try  {
	Geant4Context*       ctxt = kernel.context();
	TypeName             typ  = splitName(type_name);
	Geometry::LCDD&      lcdd = kernel.lcdd();
	Geometry::DetElement det  = lcdd.detector(detector);
	Geant4Sensitive* object = PluginService::Create<Geant4Sensitive*>(typ.first,ctxt,typ.second,&det,&lcdd);
	if ( object )  {
	  value = object;
	  return;
	}
      }
      catch(const std::exception& e)   {
	printout(ERROR,"Geant4Handle<Geant4Sensitive>","Exception: %s",e.what());
      }
      catch(...)   {
	printout(ERROR,"Geant4Handle<Geant4Sensitive>","Exception: Unknown exception");
      }
      throw runtime_error(format("Geant4Handle<Geant4Sensitive>",
				 "Failed to create sensitive object of type %s for detector %s!",
				 type_name.c_str(),detector.c_str()));
    }
  }}

template class Geant4Handle<Geant4Action>;
template class Geant4Handle<Geant4Sensitive>;
template class Geant4Handle<Geant4ActionPhase>;
template class Geant4Handle<Geant4GeneratorAction>;
template class Geant4Handle<Geant4RunAction>;
template class Geant4Handle<Geant4EventAction>;
template class Geant4Handle<Geant4TrackingAction>;
template class Geant4Handle<Geant4SteppingAction>;
template class Geant4Handle<Geant4StackingAction>;

template class Geant4Handle<Geant4GeneratorActionSequence>;
template class Geant4Handle<Geant4RunActionSequence>;
template class Geant4Handle<Geant4EventActionSequence>;
template class Geant4Handle<Geant4TrackingActionSequence>;
template class Geant4Handle<Geant4SteppingActionSequence>;
template class Geant4Handle<Geant4StackingActionSequence>;
template class Geant4Handle<Geant4SensDetActionSequence>;
