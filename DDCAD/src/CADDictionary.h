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
#ifndef DD4HEP_DDCAD_CADDICTIONARY_H
#define DD4HEP_DDCAD_CADDICTIONARY_H

// Framework include files
#include "DDCAD/InputReader.h"


namespace { class CADDictionary {};   }

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::cad;

using namespace dd4hep;
using namespace dd4hep::cad;

#pragma link C++ class InputReader+;


#endif

#endif // DD4HEP_DDCAD_CADDICTIONARY_H
