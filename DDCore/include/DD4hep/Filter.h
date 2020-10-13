//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : Ianna Osborne
//
//==========================================================================
#ifndef DD4HEP_FILTER_H
#define DD4HEP_FILTER_H

// -*- C++ -*-
//
// Class:      Filter
//
/**\class Filter

 Description: Filter list

 Implementation:
     Filter criteria is defined in XML
*/
//
// Original Author:  Ianna Osborne
//         Created:  Tue, 12 Mar 2019 09:51:33 CET
//
//
#include <memory>
#include <vector>
#include <regex>
#include <unordered_map>

namespace dd4hep {
  struct SpecPar;

  struct Filter {
    std::vector<bool> isRegex;
    std::vector<bool> hasNamaspace;
    std::vector<int> index;
    std::vector<std::string_view> skeys;
    std::vector<std::regex> keys;
    std::unique_ptr<Filter> next;
    struct Filter* up;
    const SpecPar* spec = nullptr;
  };

  namespace dd {
    bool accepted(std::vector<std::regex> const&, std::string_view);
    bool isRegex(std::string_view);
    bool hasNamespace(std::string_view);
    bool isMatch(std::string_view, std::string_view);
    bool compareEqual(std::string_view, std::string_view);
    bool compareEqual(std::string_view, std::regex);
    std::string_view realTopName(std::string_view);
    std::vector<std::string_view> split(std::string_view, const char*);
    std::string_view noNamespace(std::string_view);
  }  // namespace dd
}  // namespace dd4hep

#endif
