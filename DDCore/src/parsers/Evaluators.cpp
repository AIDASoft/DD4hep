//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#if defined(DD4HEP_PARSER_HEADER)

#define DD4HEP_NEED_EVALUATOR
// This is the case, if the parsers are externalized
// and the DD4hep namespace is renamed!
#include DD4HEP_PARSER_HEADER

#else

// Standard DD4hep parser handling
#include "DD4hep/ToStream.h"

#endif
#include "XML/Evaluator.h"

// C/C++ include files
#include <stdexcept>

namespace DD4hep {
  XmlTools::Evaluator& g4Evaluator();
}
namespace {
  XmlTools::Evaluator& eval(DD4hep::g4Evaluator());
}

//==============================================================================
namespace DD4hep {  namespace Parsers {
    template <typename T> T evaluate_string(const std::string& /* value */)   {
      throw "Bad undefined call";
    }

    template <> double evaluate_string<double>(const std::string& value)   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
        std::cerr << value << ": ";
        eval.print_error();
        throw std::runtime_error("DD4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return result;
    }
    template <> float evaluate_string<float>(const std::string& value)   {
      double result = eval.evaluate(value.c_str());
      if (eval.status() != XmlTools::Evaluator::OK) {
        std::cerr << value << ": ";
        eval.print_error();
        throw std::runtime_error("DD4hep::Properties: Severe error during expression evaluation of " + value);
      }
      return (float) result;
    }
  }
}
