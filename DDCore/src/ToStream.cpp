// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/ToStream.h"
#include "XML/Evaluator.h"

// C/C++ include files
#include <stdexcept>

namespace DD4hep {
  XmlTools::Evaluator& g4Evaluator();
}
namespace {
  XmlTools::Evaluator& eval(DD4hep::g4Evaluator());
}

using namespace std;
//==============================================================================
namespace DD4hep {  namespace Parsers {
    template <typename T> T evaluate_string(const string& value)   {
      throw "Bad undefined call";
    }

    template <> double evaluate_string<double>(const string& value)   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
	cerr << value << ": ";
	eval.print_error();
	throw runtime_error("DD4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return result;
    }
    template <> float evaluate_string<float>(const string& value)   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
	cerr << value << ": ";
	eval.print_error();
	throw runtime_error("DD4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return (float) result;
    }
  }
}

// ============================================================================
// print XYZ-point
ostream& DD4hep::Utils::toStream(const ROOT::Math::XYZPoint&  obj, ostream& s)  {
  s << "( ";
  toStream(obj.X () , s );
  s << " , ";
  toStream(obj.Y () , s );
  s << " , ";
  toStream(obj.Z () , s );
  s << " )";
  return s;
}
// ============================================================================
// print XYZ-vector
ostream& DD4hep::Utils::toStream(const ROOT::Math::XYZVector& obj, ostream& s)  {
  s << "( ";
  toStream(obj.X () , s );
  s << " , ";
  toStream(obj.Y () , s );
  s << " , ";
  toStream(obj.Z () , s );
  s << " )";
  return s;
}
// ============================================================================
// print LorentzVector
ostream& DD4hep::Utils::toStream(const ROOT::Math::PxPyPzEVector& obj, ostream& s){
  s << "( ";
  toStream(obj.Px () , s , 12 );
  s << " , ";
  toStream(obj.Py () , s , 12 );
  s << " , ";
  toStream(obj.Pz () , s , 13 );
  s << " , ";
  toStream(obj.E  () , s , 14 );
  s << " )";
  return s;
}
