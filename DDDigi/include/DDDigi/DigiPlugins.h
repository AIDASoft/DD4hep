//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DDDIGI_DIGIPLUGINS_H
#define DDDIGI_DIGIPLUGINS_H

/// Framework include files
#include <DD4hep/Primitives.h>

/// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiKernel;

    void* create_action(const std::string& factory,
			const DigiKernel& kernel,
			const std::string& arg,
			void* (*cast)(void*));

    /// Handler for factories of type: ConstructionFactory with casted return type
    template <typename T> T* createAction(const std::string& factory,
					  const DigiKernel& kernel,
					  const std::string& arg)
      {
	struct __cast{ static void* cast(void* p) { return &dynamic_cast<T&>(*(T*)p); } };
	return (T*)create_action(factory, kernel, arg, __cast::cast);
      }
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIPLUGINS_H
