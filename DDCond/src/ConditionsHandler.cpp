// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DDCond/ConditionsHandler.h"
#include "DDCond/ConditionsOperators.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
typedef ConditionsStack::Entry Entry;

/// Default constructor
ConditionsHandler::ConditionsHandler(LCDD& lcdd)
  : m_lcdd(lcdd), m_refCount(1)
{
}

/// Default destructor
ConditionsHandler::~ConditionsHandler()   {
  printout(INFO,"ConditionsHandler","+++ Destroy cache");
}

/// Add reference count
int ConditionsHandler::addRef()   {
  return ++m_refCount;
}

/// Release object. If reference count goes to NULL, automatic deletion is triggered.
int ConditionsHandler::release()   {
  int value = --m_refCount;
  if ( value == 0 )  {
    delete this;
  }
  return value;
}

/// Open a new transaction stack (Note: only one stack allowed!)
void ConditionsHandler::openTransaction()   {
  /// Check if transaction already present. If not, open, else issue an error
  if ( !ConditionsStack::exists() )  {
    ConditionsStack::create();
    return;
  }
  string msg = "Request to open a second conditions transaction stack -- not allowed!";
  string err = format("Conditions<conditions>",msg);
  printout(FATAL,"ConditionsHandler",msg);
  throw runtime_error(err);
}

/// Close existing transaction stack and apply all conditions
void ConditionsHandler::closeTransaction()   {
  /// Check if transaction is open. If yes, close it and apply conditions stack.
  /// If no transaction is active, ignore the staement, but issue a warning.
  if ( ConditionsStack::exists() )  {
    ConditionsStack& stack = ConditionsStack::get();
    printout(DEBUG,"ConditionsHandler","+++ Closing conditions transaction....");
    apply(stack);
    stack.release();
    printout(INFO,"ConditionsHandler","Conditions were applied....");
    return;
  }
  printout(WARNING,"Conditions<conditions>",
           "Request to close a non-existing conditons transaction.");
}

/// Create and install a new instance tree
void ConditionsHandler::install(LCDD& lcdd)   {
  ConditionsHandler* cache = lcdd.extension<ConditionsHandler>(false);
  if ( !cache )  {
    lcdd.addExtension<ConditionsHandler>(new ConditionsHandler(lcdd));
  }
}

/// Unregister and delete a tree instance
void ConditionsHandler::uninstall(LCDD& lcdd)   {
  if ( lcdd.extension<ConditionsHandler>(false) )  {
    lcdd.removeExtension<ConditionsHandler>(true);
  }
}

void ConditionsHandler::triggerUpdate(DetElement det)  {
  printout(DEBUG,"ConditionsHandler","+++ Trigger conditions update for %s",det.path().c_str());
  det._data().update(DetElement::CONDITIONS_CHANGED,(void*)0x1);
}

/// Apply a complete stack of ordered conditionss to the geometry structure
void ConditionsHandler::apply(ConditionsStack& stack)    {
  typedef set<DetElement> Updates;
  Updates updates;

  printout(DEBUG,"ConditionsHandler","+++ Apply conditions ....");
  while(stack.size() > 0)    {
    Entry* data = stack.pop().release();
    DetElement det = data->detector;
    /// Do something!
    printout(DEBUG,"ConditionsHandler","+++ %s  name:%s type:%s value:%s  Validity:%s",
             det.path().c_str(), data->name.c_str(), data->type.c_str(), 
             data->value.c_str(), data->validity.c_str());
    det.conditions().set(data);
    updates.insert(det);
  }
  // Now trigger update callbacks for all detector elements with changed conditions:
  for_each(updates.begin(), updates.end(), triggerUpdate);
}
