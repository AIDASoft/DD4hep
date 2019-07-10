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
#ifndef DD4HEP_DD4HEP_CONTAINERHELPERS_H
#define DD4HEP_DD4HEP_CONTAINERHELPERS_H

// Framework include files
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>

/// Namespace for the AIDA detector description toolkit
/** Functions useful to make projections into various containers
 *  when using Conditions/Alignment/DetElement processors.
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_CONDITIONS
 */
namespace dd4hep {

  template <typename Q, typename T>
  void insert_item(std::vector<T>& c, Q, const T& d)  {
    c.emplace_back(d);
  }
  template <typename Q, typename T>
  void insert_item(std::vector<Q>& c, Q de, const T&)  {
    c.emplace_back(de);
  }

  template <typename Q, typename T>
  void insert_item(std::list<T>&   c, Q, const T& d)  {
    c.emplace_back(d);
  }
  template <typename Q, typename T>
  void insert_item(std::list<Q>&   c, Q de, const T&)  {
    c.emplace_back(de);
  }
  
  template <typename Q, typename T>
  void insert_item(std::set<T>&   c, Q, const T& d)  {
    c.insert(d);
  }
  template <typename Q, typename T>
  void insert_item(std::set<Q>&   c, Q de, const T&)  {
    c.insert(de);
  }

  template <typename Q, typename T>
  void insert_item(std::map<Q,T>&  c, Q de, const T& d)  {
    c.emplace(de,d);
  }
  template <typename Q, typename T>
  void insert_item(std::map<T,Q>&  c, Q de, const T& d)  {
    c.emplace(d,de);
  }
    
  template <typename Q, typename T>
  void insert_item(std::vector<std::pair<Q,T> >& c, Q de, const T& d)  {
    c.emplace_back(de,d);
  }
  template <typename Q, typename T>
  void insert_item(std::vector<std::pair<std::string,T> >& c, Q de, const T& d)  {
    c.emplace_back(de.path(),d);
  }

  template <typename Q, typename T>
  void insert_item(std::list<std::pair<Q,T> >& c, Q de, const T& d)  {
    c.emplace_back(de,d);
  }
  template <typename Q, typename T>
  void insert_item(std::list<std::pair<std::string,T> >& c, Q de, const T& d)  {
    c.emplace_back(de.path(),d);
  }

  template <typename Q, typename T>
  void insert_item(std::set<std::pair<Q,T> >& c, Q de, const T& d)  {
    c.emplace(de,d);
  }
  template <typename Q, typename T>
  void insert_item(std::set<std::pair<std::string,T> >& c, Q de, const T& d)  {
    c.emplace(de.path(),d);
  }

  template <typename Q, typename T>
  void insert_item(std::multimap<Q,T>& c, Q de, const T& d)  {
    c.emplace(de,d);
  }
  template <typename Q, typename T>
  void insert_item(std::map<std::string,T>& c, Q de, const T& d)  {
    c.emplace(de.path(),d);
  }
  template <typename Q, typename T>
  void insert_item(std::multimap<std::string,T>& c, Q de, const T& d)  {
    c.emplace(de.path(),d);
  }
}      // End namespace dd4hep
#endif // DD4HEP_DD4HEP_CONTAINERHELPERS_H
