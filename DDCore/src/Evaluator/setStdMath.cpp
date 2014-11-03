// -*- C++ -*-
// $Id$
// ----------------------------------------------------------------------

#include "XML/Evaluator.h"
#include "DD4hep/DD4hepUnits.h"

#include <cmath>	// for sqrt and pow

using namespace std;

static double eval_abs  (double a)           { return (a < 0) ? -a : a; } 
static double eval_min  (double a, double b) { return (a < b) ?  a : b; } 
static double eval_max  (double a, double b) { return (a > b) ?  a : b; } 
static double eval_sqrt (double a)           { return std::sqrt(a); } 
static double eval_pow  (double a, double b) { return std::pow(a,b); } 
static double eval_sin  (double a)           { return std::sin(a); } 
static double eval_cos  (double a)           { return std::cos(a); } 
static double eval_tan  (double a)           { return std::tan(a); } 
static double eval_asin (double a)           { return std::asin(a); } 
static double eval_acos (double a)           { return std::acos(a); } 
static double eval_atan (double a)           { return std::atan(a); } 
static double eval_atan2(double a, double b) { return std::atan2(a,b); } 
static double eval_sinh (double a)           { return std::sinh(a); } 
static double eval_cosh (double a)           { return std::cosh(a); } 
static double eval_tanh (double a)           { return std::tanh(a); } 
static double eval_exp  (double a)           { return std::exp(a); } 
static double eval_log  (double a)           { return std::log(a); } 
static double eval_log10(double a)           { return std::log10(a); } 
static double eval_double(double a)          { return a;              } 
static double eval_int  (double a)           { return (double)int(a); } 
static double eval_nint (double a)           { return std::floor(a); } 
static double eval_floor(double a)           { return std::floor(a); } 

namespace XmlTools {

void Evaluator::setStdMath() {

  //   S E T   S T A N D A R D   C O N S T A N T S

  setVariable("pi",     3.14159265358979323846);
  setVariable("e",      2.7182818284590452354);
  setVariable("gamma",  0.577215664901532861);


  // setVariable("radian", 1.0);
  // setVariable("rad",    1.0);
  // setVariable("degree", 3.14159265358979323846/180.);
  // setVariable("deg",    3.14159265358979323846/180.);
  setVariable("radian", dd4hep::rad );
  setVariable("rad",    dd4hep::rad );
  setVariable("degree", dd4hep::deg );
  setVariable("deg",    dd4hep::deg );

  //   S E T   S T A N D A R D   F U N C T I O N S

  setFunction("floor", eval_floor);
  setFunction("double",eval_double);
  setFunction("int",   eval_int);
  setFunction("nint",  eval_nint);
  setFunction("abs",   eval_abs);
  setFunction("min",   eval_min);
  setFunction("max",   eval_max);
  setFunction("sqrt",  eval_sqrt);
  setFunction("pow",   eval_pow);
  setFunction("sin",   eval_sin);
  setFunction("cos",   eval_cos);
  setFunction("tan",   eval_tan);
  setFunction("asin",  eval_asin);
  setFunction("acos",  eval_acos);
  setFunction("atan",  eval_atan);
  setFunction("atan2", eval_atan2);
  setFunction("sinh",  eval_sinh);
  setFunction("cosh",  eval_cosh);
  setFunction("tanh",  eval_tanh);
  setFunction("exp",   eval_exp);
  setFunction("log",   eval_log);
  setFunction("log10", eval_log10);
}

} // namespace XmlTools
