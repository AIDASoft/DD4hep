#include "DD4hep/Filter.h"
#include <functional>
#include <regex>
#include <iostream>

using namespace std;

namespace dd4hep {
  namespace dd {

    bool isMatch(string_view node, string_view name) {
      if (!dd4hep::dd::isRegex(name)) {
        return (name == node);
      } else {
        regex pattern({name.data(), name.size()});
        return regex_match(begin(node), end(node), pattern);
      }
    }

    bool compareEqual(string_view node, string_view name) { return (name == node); }

    bool compareEqual(string_view node, regex pattern) {
      return regex_match(std::string(node.data(), node.size()), pattern);
    }

    bool compareEqualName(const std::string_view selection, const std::string_view name) {
      return (!(dd4hep::dd::isRegex(selection)) ? dd4hep::dd::compareEqual(name, selection)
	      : regex_match(name.begin(), name.end(), regex(std::string(selection))));
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

    bool accepted(vector<std::regex> const& keys, string_view node) {
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

    bool isRegex(string_view input) {
      return (input.find(".") != std::string_view::npos) || (input.find("*") != std::string_view::npos);
    }

    bool hasNamespace(std::string_view input) {
      return (input.find(":") != std::string_view::npos);
    }
    
    string_view realTopName(string_view input) {
      string_view v = input;
      auto first = v.find_first_of("//");
      v.remove_prefix(min(first + 2, v.size()));
      return v;
    }

    vector<string_view> split(string_view str, const char* delims) {
      vector<string_view> ret;

      string_view::size_type start = 0;
      auto pos = str.find_first_of(delims, start);
      while (pos != string_view::npos) {
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
