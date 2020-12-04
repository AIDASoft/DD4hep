//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#ifndef EVALUATOR_EVALUATOR_H
#define EVALUATOR_EVALUATOR_H

/// C/C++ include files
#include <ostream>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace containing XML tools.
  namespace tools {

    /// Evaluator of arithmetic expressions with an extendable dictionary.
    /**
     * Taken from CLHEP 1.9.2.1
     *
     * Example:
     * @code
     *   #include "XmlTools/Evaluator.h"
     *   XmlTools::Evaluator eval;
     *   eval.setStdMath();
     *   double res = eval.evaluate("sin(30*degree)");
     *   if (eval.status() != XmlTools::Evaluator::OK) eval.print_error();
     * @endcode
     *
     * @author Evgeni Chernyaev <Evgueni.Tcherniaev@cern.ch>
     * @ingroup evaluator
     */
    class Evaluator {
    public:

      /**
       * List of possible statuses.
       * Status of the last operation can be obtained with status().
       * In case if status() is an ERROR the corresponding error message
       * can be printed with print_error().
       *
       * @see status
       * @see error_position
       * @see print_error
       */
      enum {
        OK,                         /**< Everything OK */
        WARNING_EXISTING_VARIABLE,  /**< Redefinition of existing variable */
        WARNING_EXISTING_FUNCTION,  /**< Redefinition of existing function */
        WARNING_BLANK_STRING,       /**< Empty input string */
        ERROR_NOT_A_NAME,           /**< Not allowed sysmbol in the name of variable or function */
        ERROR_SYNTAX_ERROR,         /**< Systax error */
        ERROR_UNPAIRED_PARENTHESIS, /**< Unpaired parenthesis */
        ERROR_UNEXPECTED_SYMBOL,    /**< Unexpected sysbol */
        ERROR_UNKNOWN_VARIABLE,     /**< Non-existing variable */
        ERROR_UNKNOWN_FUNCTION,     /**< Non-existing function */
        ERROR_EMPTY_PARAMETER,      /**< Function call has empty parameter */
        ERROR_CALCULATION_ERROR     /**< Error during calculation */
      };

      /**
       * Constructor.
       */
      Evaluator(double meter = 1.0, double kilogram = 1.0, double second = 1.0, double ampere = 1.0, double kelvin = 1.0
                , double mole = 1.0, double candela = 1.0, double radians = 1.0);

      /**
       * MoveConstructor.
       */
      Evaluator(Evaluator&&);

      /**
       * Destructor.
       */
      ~Evaluator();

      /**
       * Evaluates the arithmetic expression given as character string.
       * The expression may consist of numbers, variables and functions
       * separated by arithmetic (+, - , /, *, ^, **) and logical
       * operators (==, !=, >, >=, <, <=, &&, ||).
       *
       * @param  expression input expression.
       * @return pair(status,result) of the evaluation.
       */
      std::pair<int,double> evaluate(const std::string& expression)  const;

      /**
       * Evaluates the arithmetic expression given as character string.
       * The expression may consist of numbers, variables and functions
       * separated by arithmetic (+, - , /, *, ^, **) and logical
       * operators (==, !=, >, >=, <, <=, &&, ||).
       *
       * @param  expression input expression.
       * @param  Possible stream identifier for error message
       * @return pair(status,result) of the evaluation.
       */
      std::pair<int,double> evaluate(const std::string& expression, std::ostream& os)  const;

      /**
       * Adds to the dictionary a function without parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param  name function name.
       * @param  fun pointer to the real function in the user code.
       * @return result of the evaluation.
       */
      int setFunction(const std::string& name, double (*fun)())  const;

      /**
       * Adds to the dictionary a function with one parameter.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param  name function name.
       * @param  fun pointer to the real function in the user code.
       * @return result of the evaluation.
       */
      int setFunction(const std::string& name, double (*fun)(double))  const;

      /**
       * Adds to the dictionary a function with two parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param  name function name.
       * @param  fun pointer to the real function in the user code.
       * @return result of the evaluation.
       */
      int setFunction(const std::string& name, double (*fun)(double, double))  const;

      /**
       * Adds to the dictionary a function with three parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param  name function name.
       * @param  fun pointer to the real function in the user code.
       * @return result of the evaluation.
       */
      int setFunction(const std::string& name, double (*fun)(double, double, double))  const;

      /**
       * Adds to the dictionary a function with four parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param  name function name.
       * @param  fun pointer to the real function in the user code.
       * @return result of the evaluation.
       */
      int setFunction(const std::string& name, double (*fun)(double, double, double, double))  const;

      /**
       * Adds to the dictionary a function with five parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param  name function name.
       * @param  fun pointer to the real function in the user code.
       * @return result of the evaluation.
       */
      int setFunction(const std::string& name, double (*fun)(double, double, double, double, double))  const;

      /**
       * Adds to the dictionary a string constant
       *
       * @param  name name of the variable.
       * @param  value value assigned to the variable.
       * @return result of the evaluation.
       */
      int setEnviron(const std::string& name, const std::string& value)  const;

      /**
       * Lookup the dictionary for a string constant
       *
       * @param  name name of the variable.
       * @return pair(status,result) of the evaluation.
       */
      std::pair<int,std::string> getEnviron(const std::string& name)  const;

      /**
       * Lookup the dictionary for a string constant
       *
       * @param  name name of the variable.
       * @param  stream identifier for error message
       * @return pair(status,result) of the evaluation.
       */
      std::pair<int,std::string> getEnviron(const std::string& name, std::ostream& os)  const;

      /**
       * Adds to the dictionary a variable with given value.
       * If a variable with such a name already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_VARIABLE.
       *
       * @param  name name of the variable.
       * @param  value value assigned to the variable.
       * @return result of the evaluation.
       */
      int setVariable(const std::string& name, double value)  const;

      /**
       * Adds to the dictionary a variable with given value.
       * If a variable with such a name already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_VARIABLE.
       *
       * @param  name name of the variable.
       * @param  value value assigned to the variable.
       * @return result of the evaluation.
       */
      int setVariable(const std::string& name, double value, std::ostream& os)  const;

      /**
       * Adds to the dictionary a variable with an arithmetic expression
       * assigned to it.
       * If a variable with such a name already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_VARIABLE.
       *
       * @param  name name of the variable.
       * @param  expression arithmetic expression.
       * @return result of the evaluation.
       */
      int setVariable(const std::string& name, const std::string& expression)  const;

      /**
       * Adds to the dictionary a variable with an arithmetic expression
       * assigned to it.
       * If a variable with such a name already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_VARIABLE.
       *
       * @param  name name of the variable.
       * @param  expression arithmetic expression.
       * @param  stream identifier for error message
       * @return result of the evaluation.
       */
      int setVariable(const std::string& name, const std::string& expression, std::ostream& os)  const;

      /**
       * Finds the variable in the dictionary.
       *
       * @param  name name of the variable.
       * @return true if such a variable exists, false otherwise.
       */
      bool findVariable(const std::string& name)  const;

      /**
       * Finds the function in the dictionary.
       *
       * @param  name name of the function to be unset.
       * @param  npar number of parameters of the function.
       * @return true if such a function exists, false otherwise.
       */
      bool findFunction(const std::string& name, int npar)   const;

      class Object;

    private:
      Object* object = 0;                                  // internal data
      Evaluator(const Evaluator &) = delete;               // copy constructor is not allowed
      Evaluator & operator=(const Evaluator &) = delete;   // assignment is not allowed
    };

  }   // namespace tools
}  // namespace dd4hep

#endif // EVALUATOR_EVALUATOR_H
