//  AIDA Detector description implementation 
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

// Framework include files
#include "DD4hep/Errors.h"

// C/C++ includes
#include <cerrno>
#include <cstring>

#define IMPLEMENT(x,y) std::string x () { return ::strerror( errno = y ); }

namespace dd4hep { namespace Errors  {
    IMPLEMENT(noPermission,EPERM)       //  1
    IMPLEMENT(noEntry,ENOENT)           //  2
    IMPLEMENT(ioError,EIO)              //  5
    IMPLEMENT(invalidArg,EINVAL)        // 22
    IMPLEMENT(noSys,ENOSYS)             // 38
    IMPLEMENT(cancelled,ECANCELED)      // 125
#ifdef __APPLE__
    IMPLEMENT(linkRange,EINVAL)         // 48  does not exist on apple
    IMPLEMENT(noKey,EINVAL)             // 126 does not exist on apple
#else
    IMPLEMENT(linkRange,ELNRNG)         // 48
    IMPLEMENT(noKey,ENOKEY)             // 126
#endif
  }}
