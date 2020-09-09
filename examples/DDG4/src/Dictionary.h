#ifndef EXAMPLES_DDG4_SRC_DICTIONARY_H
#define EXAMPLES_DDG4_SRC_DICTIONARY_H

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
// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#ifdef DD4HEP_DICTIONARY_MODE

#include <map>
#include <vector>

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


using namespace std;

// Define here the classes we want to write to the output file:
#pragma link C++ class std::vector<double>+;
#pragma link C++ class std::pair<double, std::vector<double> >+;

/// This is to make ROOT happy....
struct Disctionary {};


#endif   // DD4HEP_DICTIONARY_MODE

#endif
