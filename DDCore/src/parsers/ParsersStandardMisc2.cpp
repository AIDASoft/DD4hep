#include "ParsersStandardMiscCommon.h"

int DD4hep::Parsers::parse(std::vector<std::vector<double> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<int, int>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<int, double>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, std::string>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}
