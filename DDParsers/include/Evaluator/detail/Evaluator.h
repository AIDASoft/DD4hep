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
// -*- C++ -*-
// ---------------------------------------------------------------------------

#ifndef EVALUATOR_DETAIL_EVALUATOR_H
#define EVALUATOR_DETAIL_EVALUATOR_H

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
    class Evaluator::Object {
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
      struct EvalStatus {
        /**
         * Returns status of the last operation with the evaluator.
         */
        int status() const;

        /**
         * Returns result of the last operation with the evaluator.
         */
        double result() const;
        
        /**
         * Returns position in the input string where the problem occured.
         */
        int error_position(const char* expression) const;
        
        /**
         * Prints error message if status() is an ERROR.
         */
        void print_error(std::ostream& os) const;
        
        /**
         * Prints error message if status() is an ERROR using std::cerr.
         */
        void print_error() const;

        char const* thePosition = 0;
        int         theStatus = OK;
        double      theResult = 0.0;
      };

      /**
       * Constructor.
       */

      /**
       * Sets system of units. Default is the SI system of units.
       * To set the CGS (Centimeter-Gram-Second) system of units
       * one should call:
       *   Object o(100., 1000., 1.0, 1.0, 1.0, 1.0, 1.0);
       *
       * To set system of units accepted in the GEANT4 simulation toolkit
       * one should call:
       * @code
       *   setSystemOfUnits(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10,
       *                    1.0, 1.0, 1.0);
       * @endcode
       *
       * The basic units in GEANT4 are:
       * @code
       *   millimeter              (millimeter = 1.)
       *   nanosecond              (nanosecond = 1.)
       *   Mega electron Volt      (MeV        = 1.)
       *   positron charge         (eplus      = 1.)
       *   degree Kelvin           (kelvin     = 1.)
       *   the amount of substance (mole       = 1.)
       *   luminous intensity      (candela    = 1.)
       *   radian                  (radian     = 1.)
       *   steradian               (steradian  = 1.)
       * @endcode
       */

      Object(double meter = 1.0, double kilogram = 1.0, double second = 1.0, double ampere = 1.0, double kelvin =
                            1.0, double mole = 1.0, double candela = 1.0, double radians = 1.0 );

      /**
       * Destructor.
       */
      ~Object();

      /**
       * Evaluates the arithmetic expression given as character string.
       * The expression may consist of numbers, variables and functions
       * separated by arithmetic (+, - , /, *, ^, **) and logical
       * operators (==, !=, >, >=, <, <=, &&, ||).
       *
       * @param  expression input expression.
       * @return result of the evaluation.
       * @see status
       * @see error_position
       * @see print_error
       */
      EvalStatus evaluate(const char* expression) const;

      /**
       * Adds to the dictionary a string constant
       *
       * @param name name of the variable.
       * @param value value assigned to the variable.
       * @return returns status
       */
      int setEnviron(const char* name, const char* value);

      /**
       * Lookup the dictionary for a string constant
       *
       * @param name name of the variable.
       */
      std::pair<const char*, int> getEnviron(const char* name) const;

      /**
       * Adds to the dictionary a variable with given value.
       * If a variable with such a name already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_VARIABLE.
       *
       * @param name name of the variable.
       * @param value value assigned to the variable.
       */
      int setVariable(const char* name, double value);

      /**
       * Adds to the dictionary a variable with an arithmetic expression
       * assigned to it.
       * If a variable with such a name already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_VARIABLE.
       *
       * @param name name of the variable.
       * @param expression arithmetic expression.
       */
      int setVariable(const char* name, const char* expression);

      /**
       * Adds to the dictionary a function without parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param name function name.
       * @param fun pointer to the real function in the user code.
       */
      int setFunction(const char* name, double (*fun)());

      /**
       * Adds to the dictionary a function with one parameter.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param name function name.
       * @param fun pointer to the real function in the user code.
       */
      int setFunction(const char* name, double (*fun)(double));

      /**
       * Adds to the dictionary a function with two parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param name function name.
       * @param fun pointer to the real function in the user code.
       */
      int setFunction(const char* name, double (*fun)(double, double));

      /**
       * Adds to the dictionary a function with three parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param name function name.
       * @param fun pointer to the real function in the user code.
       */
      int setFunction(const char* name, double (*fun)(double, double, double));

      /**
       * Adds to the dictionary a function with four parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param name function name.
       * @param fun pointer to the real function in the user code.
       */
      int setFunction(const char* name, double (*fun)(double, double, double, double));

      /**
       * Adds to the dictionary a function with five parameters.
       * If such a function already exist in the dictionary,
       * then status will be set to WARNING_EXISTING_FUNCTION.
       *
       * @param name function name.
       * @param fun pointer to the real function in the user code.
       */
      int setFunction(const char* name, double (*fun)(double, double, double, double, double));

      /**
       * Finds the variable in the dictionary.
       *
       * @param  name name of the variable.
       * @return true if such a variable exists, false otherwise.
       */
      bool findVariable(const char* name) const;

      /**
       * Finds the function in the dictionary.
       *
       * @param  name name of the function to be unset.
       * @param  npar number of parameters of the function.
       * @return true if such a function exists, false otherwise.
       */
      bool findFunction(const char* name, int npar) const;

      /**
       * Removes the variable from the dictionary.
       *
       * @param name name of the variable.
       */
      void removeVariable(const char* name);

      /**
       * Removes the function from the dictionary.
       *
       * @param name name of the function to be unset.
       * @param npar number of parameters of the function.
       */
      void removeFunction(const char* name, int npar);

      /**
       * Clear all settings.
       */
      void clear();

      struct Struct;
      
    private:
      /**
       * Sets system of units. Default is the SI system of units.
       * To set the CGS (Centimeter-Gram-Second) system of units
       * one should call:
       *   setSystemOfUnits(100., 1000., 1.0, 1.0, 1.0, 1.0, 1.0);
       *
       * To set system of units accepted in the GEANT4 simulation toolkit
       * one should call:
       * @code
       *   setSystemOfUnits(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10,
       *                    1.0, 1.0, 1.0);
       * @endcode
       *
       * The basic units in GEANT4 are:
       * @code
       *   millimeter              (millimeter = 1.)
       *   nanosecond              (nanosecond = 1.)
       *   Mega electron Volt      (MeV        = 1.)
       *   positron charge         (eplus      = 1.)
       *   degree Kelvin           (kelvin     = 1.)
       *   the amount of substance (mole       = 1.)
       *   luminous intensity      (candela    = 1.)
       *   radian                  (radian     = 1.)
       *   steradian               (steradian  = 1.)
       * @endcode
       */
      void setSystemOfUnits(double meter, double kilogram, double second, double ampere, double kelvin
                          , double mole, double candela, double radians );

      /**
       * Sets standard mathematical functions and constants.
       */
      void setStdMath();

      /**
       * Used only during construction
       */
      void setVariableNoLock(const char* name, double value);
      void setFunctionNoLock(const char* name, double (*fun)(double));
      void setFunctionNoLock(const char* name, double (*fun)(double, double));


      Struct* imp {0};                               // private data
      Object(const Object &) = delete;               // copy constructor is not allowed
      Object & operator=(const Object &) = delete;   // assignment is not allowed
    };

  }   // namespace tools
}  // namespace dd4hep

#endif // EVALUATOR_DETAIL_EVALUATOR_H
