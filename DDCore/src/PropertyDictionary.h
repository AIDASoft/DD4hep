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
#ifndef DDCORE_SRC_PROPERTYDICTIONARY_H
#define DDCORE_SRC_PROPERTYDICTIONARY_H

// Framework include files
#include "DD4hep/ComponentProperties.h"

// C/C++ include files
#include <vector>
#include <list>
#include <map>
#include <set>
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
template class dd4hep::PropertyValue<char>;
template class dd4hep::PropertyValue<unsigned char>;
template class dd4hep::PropertyValue<short>;
template class dd4hep::PropertyValue<unsigned short>;
template class dd4hep::PropertyValue<unsigned int>;
template class dd4hep::PropertyValue<long>;
template class dd4hep::PropertyValue<unsigned long>;

#pragma link C++ class dd4hep::PropertyValue<char>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned char>-;
#pragma link C++ class dd4hep::PropertyValue<short>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned short>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned int>-;
#pragma link C++ class dd4hep::PropertyValue<long>-;
#pragma link C++ class dd4hep::PropertyValue<unsigned long>-;

template class dd4hep::PropertyValue<std::vector<char> >;
template class dd4hep::PropertyValue<std::vector<unsigned char> >;
template class dd4hep::PropertyValue<std::vector<short> >;
template class dd4hep::PropertyValue<std::vector<unsigned short> >;
template class dd4hep::PropertyValue<std::vector<unsigned int> >;
template class dd4hep::PropertyValue<std::vector<long> >;
template class dd4hep::PropertyValue<std::vector<unsigned long> >;

#pragma link C++ class dd4hep::PropertyValue<std::vector<char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<long> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<unsigned long> >-;

template class dd4hep::PropertyValue<std::list<char> >;
template class dd4hep::PropertyValue<std::list<unsigned char> >;
template class dd4hep::PropertyValue<std::list<short> >;
template class dd4hep::PropertyValue<std::list<unsigned short> >;
template class dd4hep::PropertyValue<std::list<unsigned int> >;
template class dd4hep::PropertyValue<std::list<long> >;
template class dd4hep::PropertyValue<std::list<unsigned long> >;

#pragma link C++ class dd4hep::PropertyValue<std::list<char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<long> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<unsigned long> >-;

template class dd4hep::PropertyValue<std::set<char> >;
template class dd4hep::PropertyValue<std::set<unsigned char> >;
template class dd4hep::PropertyValue<std::set<short> >;
template class dd4hep::PropertyValue<std::set<unsigned short> >;
template class dd4hep::PropertyValue<std::set<unsigned int> >;
template class dd4hep::PropertyValue<std::set<long> >;
template class dd4hep::PropertyValue<std::set<unsigned long> >;

#pragma link C++ class dd4hep::PropertyValue<std::set<char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned char> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned short> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<long> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<unsigned long> >-;
#endif

template class dd4hep::PropertyValue<int>;
template class dd4hep::PropertyValue<float>;
template class dd4hep::PropertyValue<double>;
template class dd4hep::PropertyValue<std::string>;

#pragma link C++ class dd4hep::PropertyValue<int>-;
#pragma link C++ class dd4hep::PropertyValue<float>-;
#pragma link C++ class dd4hep::PropertyValue<double>-;
#pragma link C++ class dd4hep::PropertyValue<std::string>-;

template class dd4hep::PropertyValue<std::vector<int> >;
template class dd4hep::PropertyValue<std::vector<float> >;
template class dd4hep::PropertyValue<std::vector<double> >;
template class dd4hep::PropertyValue<std::vector<std::string> >;
#pragma link C++ class dd4hep::PropertyValue<std::vector<int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<float> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<double> >-;
#pragma link C++ class dd4hep::PropertyValue<std::vector<std::string> >-;

template class dd4hep::PropertyValue<std::list<int> >;
template class dd4hep::PropertyValue<std::list<float> >;
template class dd4hep::PropertyValue<std::list<double> >;
template class dd4hep::PropertyValue<std::list<std::string> >;
#pragma link C++ class dd4hep::PropertyValue<std::list<int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<float> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<double> >-;
#pragma link C++ class dd4hep::PropertyValue<std::list<std::string> >-;

template class dd4hep::PropertyValue<std::set<int> >;
template class dd4hep::PropertyValue<std::set<float> >;
template class dd4hep::PropertyValue<std::set<double> >;
template class dd4hep::PropertyValue<std::set<std::string> >;
#pragma link C++ class dd4hep::PropertyValue<std::set<int> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<float> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<double> >-;
#pragma link C++ class dd4hep::PropertyValue<std::set<std::string> >-;

template class std::map<std::string, dd4hep::Property>;
#pragma link C++ class std::map<std::string, dd4hep::Property>-;
#pragma link C++ class dd4hep::PropertyManager-;
#pragma link C++ class dd4hep::PropertyConfigurable-;
#pragma link C++ class dd4hep::PropertyConfigurator-;
#pragma link C++ class dd4hep::PropertyGrammar-;

#endif  // __CINT__
#endif // DDCORE_SRC_PROPERTYDICTIONARY_H
