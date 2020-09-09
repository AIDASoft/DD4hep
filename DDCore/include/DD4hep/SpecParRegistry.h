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
#include "tbb/concurrent_unordered_map.h"
#include "tbb/concurrent_vector.h"

namespace dd4hep {
  using Paths = tbb::concurrent_vector<std::string>;
  using PartSelectionMap = tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<std::string>>;
  using VectorsMap = tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>>;

  struct SpecPar {
    std::string_view strValue(const std::string&) const;
    bool hasValue(const std::string& key) const;
    bool hasPath(const std::string& path) const;
    double dblValue(const std::string&) const;

    template <typename T>
    T value(const std::string&) const;

    Paths paths;
    PartSelectionMap spars;
    VectorsMap numpars;
    std::string_view name;
  };

  using SpecParMap = tbb::concurrent_unordered_map<std::string, SpecPar>;
  using SpecParRefs = std::vector<const SpecPar*>;

  struct SpecParRegistry {
    void filter(SpecParRefs&, const std::string&, const std::string& = "") const;
    std::vector<std::string_view> names() const;
    std::vector<std::string_view> names(const std::string& path) const;
    bool hasSpecPar(std::string_view) const;
    const SpecPar* specPar(std::string_view) const;

    SpecParMap specpars;
  };
}  // namespace dd4hep

#endif
