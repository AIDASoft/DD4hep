// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/ViewConfiguration.h"
#include "DDEve/View.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep;

ClassImp(ViewConfiguration)

static inline View* checkedView(View* view)   {
  if ( !view )  {
    throw std::runtime_error("Invalid View!");
  }
  return view;
}

/// Standard constructor
ViewConfiguration::ViewConfiguration() : config(), m_display(0)  {
}

/// Initializing constructor
ViewConfiguration::ViewConfiguration(Display* disp, const DisplayConfiguration::ViewConfig& cfg)   
  : config(cfg), m_display(disp)
{
}

/// Default destructor
ViewConfiguration::~ViewConfiguration()   {
}

/// Configure a single view
void ViewConfiguration::ConfigureGeometry(View* view) const   {
  checkedView(view)->ConfigureGeometry(config);
}

/// Configure a single view
void ViewConfiguration::ConfigureEvent(View* view) const   {
  checkedView(view)->ConfigureEvent(config);
}

/// Prepare the view for adding event data 
void ViewConfiguration::PrepareEvent(View* view)  const   {
  checkedView(view)->PrepareEvent();
}
