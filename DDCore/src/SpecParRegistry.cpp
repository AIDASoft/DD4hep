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

// Framework includes
#include <DD4hep/SpecParRegistry.h>
#include <DD4hep/Detector.h>

// C/C++ include files
#include <algorithm>

using namespace dd4hep;

std::string_view SpecPar::strValue(const std::string& key) const {
  auto const& item = spars.find(key);
  if (item == end(spars))
    return std::string_view();
  return *begin(item->second);
}

bool SpecPar::hasValue(const std::string& key) const {
  if (numpars.find(key) != end(numpars))
    return true;
  else
    return false;
}

bool SpecPar::hasPath(const std::string& path) const {
  auto result = std::find(std::begin(paths), std::end(paths), path);
  if (result != end(paths))
    return true;
  else
    return false;
}

template <>
std::vector<double> SpecPar::value<std::vector<double>>(const std::string& key) const {
  std::vector<double> result;

  auto const& nitem = numpars.find(key);
  if (nitem != end(numpars)) {
    return std::vector<double>(begin(nitem->second), end(nitem->second));
  }

  auto const& sitem = spars.find(key);
  if (sitem != end(spars)) {
    std::transform(begin(sitem->second), end(sitem->second), std::back_inserter(result), [](auto& i) -> double {
      return dd4hep::_toDouble(i);
    });
  }

  return result;
}

template <>
std::vector<int> SpecPar::value<std::vector<int>>(const std::string& key) const {
  std::vector<int> result;

  auto const& nitem = numpars.find(key);
  if (nitem != end(numpars)) {
    return std::vector<int>(begin(nitem->second), end(nitem->second));
  }

  auto const& sitem = spars.find(key);
  if (sitem != end(spars)) {
    std::transform(begin(sitem->second), end(sitem->second), std::back_inserter(result), [](auto& i) -> int {
      return dd4hep::_toInt(i);
    });
  }

  return result;
}

template <>
std::vector<std::string> SpecPar::value<std::vector<std::string>>(const std::string& key) const {
  std::vector<std::string> result;

  auto const& nitem = numpars.find(key);
  if (nitem != end(numpars)) {
    std::transform(begin(nitem->second), end(nitem->second), std::back_inserter(result), [](auto& i) -> std::string {
      return std::to_string(i);
    });

    return result;
  }

  auto const& sitem = spars.find(key);
  if (sitem != end(spars)) {
    return std::vector<std::string>(begin(sitem->second), end(sitem->second));
  }

  return result;
}

double SpecPar::dblValue(const std::string& key) const {
  auto const& item = numpars.find(key);
  if (item == end(numpars))
    return 0;
  return *begin(item->second);
}

void SpecParRegistry::filter(SpecParRefs& refs, const std::string& attribute, const std::string& value) const {
  bool found(false);
  for_each(begin(specpars), end(specpars), [&refs, &attribute, &value, &found](auto& k) {
    found = false;
    for_each(begin(k.second.spars), end(k.second.spars), [&](const auto& l) {
      if (l.first == attribute) {
        if (value.empty()) {
          found = true;
        } else {
          for_each(begin(l.second), end(l.second), [&](const auto& m) {
            if (m == value)
              found = true;
          });
        }
      }
    });
    if (found) {
      refs.emplace_back(std::string(k.first.data(), k.first.size()), &k.second);
    }
  });
}

void SpecParRegistry::filter(SpecParRefs& refs, const std::string& key) const {
  for (auto const& it : specpars) {
    if (it.second.hasValue(key) || (it.second.spars.find(key) != end(it.second.spars))) {
      refs.emplace_back(it.first, &it.second);
    }
  }
}


std::vector<std::string_view> SpecParRegistry::names(const std::string& path) const {
  std::vector<std::string_view> result;
  for_each(begin(specpars), end(specpars), [&](const auto& i) {
    if (i.second.hasPath(path))
      result.emplace_back(i.first);
  });
  return result;
}

std::vector<std::string_view> SpecParRegistry::names() const {
  std::vector<std::string_view> result;
  for_each(begin(specpars), end(specpars), [&result](const auto& i) { result.emplace_back(i.first); });
  return result;
}

bool SpecParRegistry::hasSpecPar(std::string_view name) const {
  auto const& result =
      find_if(begin(specpars), end(specpars), [&name](const auto& i) { return (i.first.compare(name) == 0); });
  if (result != end(specpars))
    return true;
  else
    return false;
}

const SpecPar* SpecParRegistry::specPar(std::string_view name) const {
  auto const& result =
      find_if(begin(specpars), end(specpars), [&name](const auto& i) { return (i.first.compare(name) == 0); });
  if (result != end(specpars)) {
    return &result->second;
  } else {
    return nullptr;
  }
}
