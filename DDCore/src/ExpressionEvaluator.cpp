#include "Evaluator.h"

namespace DetDesc  {
  XmlTools::Evaluator& evaluator()   {
    static XmlTools::Evaluator e;
    return e;
  }
}

namespace {
  struct _Init { 
    _Init()    {
      // Initialize numerical expressions parser with the standard math functions
      // and the system of units used by Gaudi (Geant4)
      DetDesc::evaluator().setStdMath();
      DetDesc::evaluator().setSystemOfUnits (1.e+3,
                            1./1.60217733e-25,
                            1.e+9,
                            1./1.60217733e-10,
                            1.0,
                            1.0,
                            1.0);
    }
  };
  _Init s_init;
}
