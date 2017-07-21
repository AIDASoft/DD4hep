//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// PropertyDictionary.h
//
//
// M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDCORE_PROPERTYDICTIONARY_H
#define DD4HEP_DDCORE_PROPERTYDICTIONARY_H

// Framework include files
#include "DD4hep/ComponentProperties.h"

// C/C++ include files
#include <vector>
#include <map>
#include <string>

#include "TRint.h"
namespace dd4hep {
  namespace detail {}
}

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#ifdef DD4HEP_DICTIONARY_MODE
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

#pragma link C++ class dd4hep::Property;
#if defined(DD4HEP_HAVE_ALL_PARSERS)
#pragma link C++ class dd4hep::PropertyValue<char>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned char>-;
#pragma link C++ class dd4hep::PropertyValue<short>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned short>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned int>-;
#pragma link C++ class dd4hep::PropertyValue<long>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned long>-;

#pragma link C++ class dd4hep::PropertyValue<std::vector<char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<long> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned long> >-;

#pragma link C++ class dd4hep::PropertyValue<std::list<char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<long> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned long> >-;

#pragma link C++ class dd4hep::PropertyValue<std::set<char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<long> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned long> >-;
#endif

#pragma link C++ class dd4hep::PropertyValue<int>-;
#pragma link C++ class dd4hep::PropertyValue<float>-;
#pragma link C++ class dd4hep::PropertyValue<double>-;
#pragma link C++ class dd4hep::PropertyValue<std::string>-;

#pragma link C++ class dd4hep::PropertyValue<std::vector<int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<float> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<double> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<std::string> >-;

#pragma link C++ class dd4hep::PropertyValue<std::list<int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<float> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<double> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<std::string> >-;

#pragma link C++ class dd4hep::PropertyValue<std::set<int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<float> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<double> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<std::string> >-;

#pragma link C++ class std::map<std::string, dd4hep::Property>-;
#pragma link C++ class PropertyManager-;
#pragma link C++ class PropertyConfigurable-;
#pragma link C++ class dd4hep::PropertyConfigurator-;
#pragma link C++ class dd4hep::PropertyGrammar-;

#endif  // __CINT__
#endif  /* DD4HEP_DDCORE_PROPERTYDICTIONARY_H  */
