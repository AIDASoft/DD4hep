#include "ParsersStandardMiscCommon.h"

int DD4hep::Parsers::parse(std::map<std::string, std::vector<int> >& result, const std::string& input) {
    return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<std::string, std::vector<double> >& result, const std::string& input) {
    return DD4hep::Parsers::parse_(result, input);
}

int DD4hep::Parsers::parse(std::map<int, std::string>& result, const std::string& input) {
    return DD4hep::Parsers::parse_(result, input);
}
