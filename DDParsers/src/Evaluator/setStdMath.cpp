// -*- C++ -*-
// ----------------------------------------------------------------------

#include "Evaluator/Evaluator.h"
#include "Evaluator/detail/Evaluator.h"
#include <limits>

#ifdef DD4HEP_NONE
/// Utility namespace to support TGeo units.
namespace dd4hep {
  //namespace units  {
  static const double radian = 1. ;
  static const double degree = (3.14159265358979323846/180.0)*radian;
  //}
}
#else
#include "Evaluator/DD4hepUnits.h"
#endif
namespace units = dd4hep;

#include <cmath>        // for sqrt and pow
#include <math.h>

using namespace std;

static double eval_abs  (double a)           { return (a < 0) ? -a : a;        }
static double eval_fabs (double a)           { return std::fabs(a);            }
static double eval_fmod (double a, double b) { return std::fmod(a,b);          }
static double eval_min  (double a, double b) { return (a < b) ?  a : b;        }
static double eval_max  (double a, double b) { return (a > b) ?  a : b;        }
static double eval_sqrt (double a)           { return std::sqrt(a);            }
static double eval_cbrt (double a)           { return ::cbrt(a);               }
static double eval_pow  (double a, double b) { return std::pow(a,b);           }
static double eval_sin  (double a)           { return std::sin(a);             }
static double eval_cos  (double a)           { return std::cos(a);             }
static double eval_tan  (double a)           { return std::tan(a);             }
static double eval_asin (double a)           { return std::asin(a);            }
static double eval_acos (double a)           { return std::acos(a);            }
static double eval_atan (double a)           { return std::atan(a);            }
static double eval_atan2(double a, double b) { return std::atan2(a,b);         }
static double eval_sinh (double a)           { return std::sinh(a);            }
static double eval_cosh (double a)           { return std::cosh(a);            }
static double eval_tanh (double a)           { return std::tanh(a);            }
static double eval_exp  (double a)           { return std::exp(a);             }
static double eval_exp2 (double a)           { return ::exp2(a);               }
//static double eval_exp10(double a)           { return ::exp10(a);              }
static double eval_exp10(double a)           { return ::pow(10.,a) ;           }
static double eval_expm1(double a)           { return ::expm1(a);              }
static double eval_log  (double a)           { return std::log(a);             }
static double eval_logb (double a)           { return ::logb(a);               }
static double eval_log10(double a)           { return std::log10(a);           }
static double eval_log1p(double a)           { return ::log1p(a);              }
static double eval_hypot(double a, double b) { return ::hypot(a,b);            }
static double eval_erf  (double a)           { return ::erf(a);                }
static double eval_erfc (double a)           { return ::erfc(a);               }
static double eval_gamma(double a)           { return ::tgamma(a);             }
static double eval_double(double a)          { return a;                       }
static double eval_int  (double a)           { return (double)int(a);          }
static double eval_nint (double a)           { return std::floor(a);           }
static double eval_floor(double a)           { return std::floor(a);           }
static double eval_round(double a)           { return ::round(a);              }
static double eval_ceil (double a)           { return std::ceil(a);            }
static double eval_isnan(double a)           { return std::isnan(a);           }
static double eval_isinf(double a)           { return std::isinf(a);           }
static double eval_nearbyint(double a)       { return ::nearbyint(a);          }

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  namespace tools {

    void Evaluator::Object::setStdMath() {

      //   S E T   S T A N D A R D   C O N S T A N T S

      setVariableNoLock("pi",     3.14159265358979323846);
      setVariableNoLock("e",      2.7182818284590452354);
      setVariableNoLock("gamma",  0.577215664901532861);


      // setVariableNoLock("radian", 1.0);
      // setVariableNoLock("rad",    1.0);
      // setVariableNoLock("degree", 3.14159265358979323846/180.);
      // setVariableNoLock("deg",    3.14159265358979323846/180.);
      setVariableNoLock("radian", units::radian );
      setVariableNoLock("rad",    units::radian );
      setVariableNoLock("degree", units::degree );
      setVariableNoLock("deg",    units::degree );
      setVariableNoLock("int:epsilon",    std::numeric_limits<int>::epsilon());
      setVariableNoLock("long:epsilon",   std::numeric_limits<long>::epsilon());
      setVariableNoLock("float:epsilon",  std::numeric_limits<float>::epsilon());
      setVariableNoLock("double:epsilon", std::numeric_limits<double>::epsilon());

      //   S E T   S T A N D A R D   F U N C T I O N S

      setFunctionNoLock("abs",        eval_abs);
      setFunctionNoLock("fabs",       eval_fabs);
      setFunctionNoLock("fmod",       eval_fmod);
      setFunctionNoLock("min",        eval_min);
      setFunctionNoLock("max",        eval_max);
      setFunctionNoLock("sqrt",       eval_sqrt);
      setFunctionNoLock("cbrt",       eval_cbrt);
      setFunctionNoLock("pow",        eval_pow);
      setFunctionNoLock("sin",        eval_sin);
      setFunctionNoLock("cos",        eval_cos);
      setFunctionNoLock("tan",        eval_tan);
      setFunctionNoLock("asin",       eval_asin);
      setFunctionNoLock("acos",       eval_acos);
      setFunctionNoLock("atan",       eval_atan);
      setFunctionNoLock("atan2",      eval_atan2);
      setFunctionNoLock("sinh",       eval_sinh);
      setFunctionNoLock("cosh",       eval_cosh);
      setFunctionNoLock("tanh",       eval_tanh);
      setFunctionNoLock("exp",        eval_exp);
      setFunctionNoLock("exp2",       eval_exp2);
      setFunctionNoLock("exp10",      eval_exp10);
      setFunctionNoLock("expm1",      eval_expm1);
      setFunctionNoLock("log",        eval_log);
      setFunctionNoLock("logb",       eval_logb);
      setFunctionNoLock("log10",      eval_log10);
      setFunctionNoLock("log1p",      eval_log1p);
      setFunctionNoLock("hypot",      eval_hypot);
      setFunctionNoLock("erf",        eval_erf);
      setFunctionNoLock("erfc",       eval_erfc);
      setFunctionNoLock("gamma",      eval_gamma);
      setFunctionNoLock("double",     eval_double);
      setFunctionNoLock("int",        eval_int);
      setFunctionNoLock("nint",       eval_nint);
      setFunctionNoLock("floor",      eval_floor);
      setFunctionNoLock("round",      eval_round);
      setFunctionNoLock("ceil",       eval_ceil);
      setFunctionNoLock("isnan",      eval_isnan);
      setFunctionNoLock("isinf",      eval_isinf);
      setFunctionNoLock("nearbyint",  eval_nearbyint);
    }

  } // namespace tools
}  // namespace dd4hep
