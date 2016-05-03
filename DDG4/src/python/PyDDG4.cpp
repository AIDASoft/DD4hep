// Framework include files
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-03
//
//==========================================================================
// $Id$

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDG4/Python/PyDDG4.h"
#include "DDG4/Python/DDPython.h"

int PyDDG4::run(Kernel& kernel)  {
  int ret;
  DD4hep::DDPython::AllowThreads allow(0);
  if ( 1 != (ret=kernel.configure()) )
    return ret;
  else if ( 1 != (ret=kernel.initialize()) )
    return ret;
  else if ( 1 != (ret=kernel.run()) )
    return ret;
  else if ( 1 != (ret=kernel.terminate()) )
    return ret;
    //DD4hep::DDPython::shutdown();
  return ret;
}

int PyDDG4::execute()  {
  Kernel& k = Kernel::instance(DD4hep::Geometry::LCDD::getInstance());
  return run(k);
}

int PyDDG4::process(const char* fname)  {
  return DD4hep::DDPython::instance().runFile(fname);
}

int PyDDG4::run(const char* fname)  {
  int ret = DD4hep::DDPython::instance().runFile(fname);
  if ( 1 != ret ) return ret;
  return execute();
}
