//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-02-02
//  \version 1.0
//
//==========================================================================
// $Id$

// Framework include files
#include "DD4hep/Errors.h"

// C/C++ includes
#include <cerrno>
#include <cstring>

#define IMPLEMENT(x,y) std::string x () { return ::strerror( errno = y ); }

namespace DD4hep { namespace Errors  {
    IMPLEMENT(noPermission,EPERM)       //  1
    IMPLEMENT(noEntry,ENOENT)           //  2
    IMPLEMENT(ioError,EIO)              //  5
    IMPLEMENT(invalidArg,EINVAL)        // 22
    IMPLEMENT(noSys,ENOSYS)             // 38
    IMPLEMENT(linkRange,ELNRNG)         // 48
    IMPLEMENT(cancelled,ECANCELED)      // 125
    IMPLEMENT(noKey,ENOKEY)             // 126
  }}
