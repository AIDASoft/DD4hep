//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  RootDictionary.h
//
//
//  M.Frank
//
//==========================================================================
// Framework include files
#include <DD4hep/Filter.h>

// C/C++ include files
#include <functional>
#include <regex>
#include <iostream>

namespace dd4hep {
  
  namespace dd {

    bool isMatch(std::string_view node, std::string_view name) {
      if (!dd4hep::dd::isRegex(name)) {
        return (name == node);
      } else {
        std::regex pattern({name.data(), name.size()});
        return std::regex_match(begin(node), end(node), pattern);
      }
    }

    bool compareEqual(std::string_view node, std::string_view name) { return (name == node); }

    bool compareEqual(std::string_view node, std::regex pattern) {
      return std::regex_match(std::string(node.data(), node.size()), pattern);
    }

    bool compareEqualName(const std::string_view selection, const std::string_view name) {
      return (!(dd4hep::dd::isRegex(selection)) ? dd4hep::dd::compareEqual(name, selection)
              : std::regex_match(name.begin(), name.end(), std::regex(std::string(selection))));
    }

    bool compareEqualCopyNumber(std::string_view name, int copy) {
      auto pos = name.rfind('[');
      if (pos != name.npos) {
        if (std::stoi(std::string(name.substr(pos + 1, name.rfind(']')))) == copy) {
          return true;
        }
      }
      
      return false;
    }

    bool accepted(std::vector<std::regex> const& keys, std::string_view node) {
      return (find_if(begin(keys), end(keys), [&](const auto& n) -> bool { return compareEqual(node, n); }) !=
              end(keys));
    }

    bool accepted(const Filter* filter, std::string_view name) {
      for(unsigned int i = 0; i < filter->isRegex.size(); i++ ) {
        if(!filter->isRegex[i]) {
          if(compareEqual(filter->skeys[i], name)) {
            return true;
          }
        } else {
          if(compareEqual(name, filter->keys[filter->index[i]])) {
            return true;
          }
        }
      }
      return false;
    }

    bool isRegex(std::string_view input) {
      return (input.find(".") != std::string_view::npos) || (input.find("*") != std::string_view::npos);
    }

    bool hasNamespace(std::string_view input) {
      return (input.find(":") != std::string_view::npos);
    }
    
    std::string_view realTopName(std::string_view input) {
      std::string_view v = input;
      auto first = v.find_first_of("//");
      v.remove_prefix(std::min(first + 2, v.size()));
      return v;
    }

    std::vector<std::string_view> split(std::string_view str, const char* delims) {
      std::vector<std::string_view> ret;

      std::string_view::size_type start = 0;
      auto pos = str.find_first_of(delims, start);
      while (pos != std::string_view::npos) {
        if (pos != start) {
          ret.emplace_back(str.substr(start, pos - start));
        }
        start = pos + 1;
        pos = str.find_first_of(delims, start);
      }
      if (start < str.length())
        ret.emplace_back(str.substr(start, str.length() - start));
      return ret;
    }

    std::string_view noNamespace(std::string_view input) {
      std::string_view v = input;
      auto first = v.find_first_of(":");
      v.remove_prefix(std::min(first + 1, v.size()));
      return v;
    }
  }  // namespace dd
}  // namespace dd4hep
