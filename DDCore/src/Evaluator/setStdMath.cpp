// -*- C++ -*-
// $Id$
// ----------------------------------------------------------------------

#include "XML/Evaluator.h"

#ifdef DD4HEP_NONE
/// Utility namespace to support TGeo units.
namespace dd4hep {
  static const double radian = 1. ;
  static const double degree = (3.14159265358979323846/180.0)*radian;
}
#else
#include "DD4hep/DD4hepUnits.h"
#endif

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
    setVariable("radian", dd4hep::radian );
    setVariable("rad",    dd4hep::radian );
    setVariable("degree", dd4hep::degree );
    setVariable("deg",    dd4hep::degree );

    //   S E T   S T A N D A R D   F U N C T I O N S

    setFunction("abs",        eval_abs);
    setFunction("fabs",       eval_fabs);
    setFunction("fmod",       eval_fmod);
    setFunction("min",        eval_min);
    setFunction("max",        eval_max);
    setFunction("sqrt",       eval_sqrt);
    setFunction("cbrt",       eval_cbrt);
    setFunction("pow",        eval_pow);
    setFunction("sin",        eval_sin);
    setFunction("cos",        eval_cos);
    setFunction("tan",        eval_tan);
    setFunction("asin",       eval_asin);
    setFunction("acos",       eval_acos);
    setFunction("atan",       eval_atan);
    setFunction("atan2",      eval_atan2);
    setFunction("sinh",       eval_sinh);
    setFunction("cosh",       eval_cosh);
    setFunction("tanh",       eval_tanh);
    setFunction("exp",        eval_exp);
    setFunction("exp2",       eval_exp2);
    setFunction("exp10",      eval_exp10);
    setFunction("expm1",      eval_expm1);
    setFunction("log",        eval_log);
    setFunction("logb",       eval_logb);
    setFunction("log10",      eval_log10);
    setFunction("log1p",      eval_log1p);
    setFunction("hypot",      eval_hypot);
    setFunction("erf",        eval_erf);
    setFunction("erfc",       eval_erfc);
    setFunction("gamma",      eval_gamma);
    setFunction("double",     eval_double);
    setFunction("int",        eval_int);
    setFunction("nint",       eval_nint);
    setFunction("floor",      eval_floor);
    setFunction("round",      eval_round);
    setFunction("ceil",       eval_ceil);
    setFunction("isnan",      eval_isnan);
    setFunction("isinf",      eval_isinf);
    setFunction("nearbyint",  eval_nearbyint);
  }

} // namespace XmlTools
