// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include <algorithm>
#include "DDG4/Geant4Callback.h"
#include "DDG4/ComponentUtils.h"

using namespace DD4hep;

/// Check the compatibility of two typed objects. The test is the result of a dynamic_cast
void CallbackSequence::checkTypes(const std::type_info& typ1, const std::type_info& typ2, void* test)  {
  if ( !test )   {
    throw unrelated_type_error(typ1,typ2,"Cannot install a callback for these 2 types.");
  }
}

