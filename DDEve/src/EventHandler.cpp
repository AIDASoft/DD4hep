// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/EventHandler.h"

using namespace DD4hep;

ClassImp(EventHandler)
ClassImp(EventConsumer)

/// Standard constructor
EventHandler::EventHandler() : m_hasFile(false), m_hasEvent(false) {
}

/// Default destructor
EventHandler::~EventHandler()   {
}

/// Standard constructor
EventConsumer::EventConsumer()  {
}

/// Default destructor
EventConsumer::~EventConsumer()   {
}


