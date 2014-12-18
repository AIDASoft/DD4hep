#include "ParsersStandardMiscCommon.h"

int DD4hep::Parsers::parse(std::pair<double,double>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::pair<int, int>& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::vector<std::pair<double, double> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::vector<std::pair<int, int> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::vector<std::vector<std::string> >& result, const std::string& input) {
  return DD4hep::Parsers::parse_(result, input);
}
