#include "ParsersStandardMiscCommon.h"

int DD4hep::Parsers::parse(std::map<std::string, int>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, unsigned int>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, double>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, std::vector<std::string> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}
