//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-02-02
//  \version  1.0
//
//==========================================================================
// $Id$
#ifndef DD4HEP_ERRORS_H 
#define DD4HEP_ERRORS_H 1

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Utility namespace: Errors
  namespace Errors  {

    /// System error string for EPERM. Sets errno accordingly
    std::string noPermission();

    /// System error string for ENOENT. Sets errno accordingly
    std::string noEntry();

    /// System error string for EIO. Sets errno accordingly
    std::string ioError();

    /// System error string for EINVAL. Sets errno accordingly
    std::string invalidArg();

    /// System error string for ENOKEY. Sets errno accordingly
    std::string noKey();

    /// System error string for ENOSYS. Sets errno accordingly
    std::string noSys();

    /// System error string for ECANCELED. Sets errno accordingly
    std::string cancelled();

    /// System error string for ELNRNG. Sets errno accordingly
    std::string linkRange();

    
  }

} /* End namespace DD4hep             */
#endif  // DD4HEP_ERRORS_H
