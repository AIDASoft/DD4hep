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
#ifndef DD4HEP_SPECPARREGISTRY_H
#define DD4HEP_SPECPARREGISTRY_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace dd4hep {
  using Paths = std::vector<std::string>;
  using PartSelectionMap = std::unordered_map<std::string, std::vector<std::string>>;
  using VectorsMap = std::unordered_map<std::string, std::vector<double>>;

  struct SpecPar {
    std::string_view strValue(const std::string&) const;
    bool hasValue(const std::string&) const;
    bool hasPath(const std::string&) const;
    double dblValue(const std::string&) const;

    template <typename T>
    T value(const std::string&) const;

    Paths paths;
    PartSelectionMap spars;
    VectorsMap numpars;
  };

  using SpecParMap = std::unordered_map<std::string, SpecPar>;
  using SpecParRefs = std::vector<std::pair<std::string, const SpecPar*>>;

  struct SpecParRegistry {
    void filter(SpecParRefs&, const std::string&, const std::string&) const;
    void filter(SpecParRefs&, const std::string&) const;
    std::vector<std::string_view> names() const;
    std::vector<std::string_view> names(const std::string& path) const;
    bool hasSpecPar(std::string_view) const;
    const SpecPar* specPar(std::string_view) const;

    SpecParMap specpars;
  };
}  // namespace dd4hep

#endif
