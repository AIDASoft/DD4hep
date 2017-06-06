//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DD4HEP_CONTAINERHELPERS_H
#define DD4HEP_DD4HEP_CONTAINERHELPERS_H

// Framework include files
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <map>
#include <list>
#include <vector>
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  template <typename Q, typename T>
  void insert_item(std::vector<T>& c, Q, const T& d)  {
    c.push_back(d);
  }
  template <typename Q, typename T>
  void insert_item(std::list<T>&   c, Q, const T& d)  {
    c.push_back(d);
  }
  template <typename Q, typename T>
  void insert_item(std::map<Q,T>& c, Q de, const T& d)  {
    c.insert(std::make_pair(de,d));
  }
  template <typename Q, typename T>
  void insert_item(std::vector<std::pair<Q,T> >& c, Q de, const T& d)  {
    c.push_back(std::make_pair(de,d));
  }
  template <typename Q, typename T>
  void insert_item(std::vector<std::pair<std::string,T> >& c, Q de, const T& d)  {
    c.push_back(std::make_pair(de.path(),d));
  }
  
  template <typename Q, typename T>
  void insert_item(std::multimap<Q,T>& c, Q de, const T& d)  {
    c.insert(std::make_pair(de,d));
  }
  template <typename Q, typename T>
  void insert_item(std::map<std::string,T>& c, Q de, const T& d)  {
    c.insert(std::make_pair(de.path(),d));
  }
  template <typename Q, typename T>
  void insert_item(std::multimap<std::string,T>& c, Q de, const T& d)  {
    c.insert(std::make_pair(de.path(),d));
  }
}      // End namespace DD4hep
#endif // DD4HEP_DD4HEP_CONTAINERHELPERS_H
