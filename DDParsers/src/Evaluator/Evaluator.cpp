
// -*- C++ -*-
// ---------------------------------------------------------------------------

#include "Evaluator/Evaluator.h"
#include "Evaluator/detail/Evaluator.h"

#include <iostream>
#include <cmath>        // for pow()
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <cstdlib>     // for strtod()
#include <stack>
#include <string>
#include <unordered_map>

// Disable some diagnostics, which we know, but need to ignore
#if defined(__GNUC__) && !defined(__APPLE__) && !defined(__llvm__)
/*  This is OK:
    ../DDCore/src/Evaluator/Evaluator.cpp: In execute_function 'int engine(pchar, pchar, double&, char*&, const dic_type&)':
    ../DDCore/src/Evaluator/Evaluator.cpp:164:23: warning: 'pp[3]' may be used uninitialized in this execute_function [-Wmaybe-uninitialized]
    result = (*fcn.f4)(pp[3],pp[2],pp[1],pp[0]);
    ....
*/
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

// fallthrough only exists from c++17
#if defined __has_cpp_attribute
    #if __has_cpp_attribute(fallthrough)
        #define ATTR_FALLTHROUGH [[fallthrough]]
    #else
        #define ATTR_FALLTHROUGH
    #endif
#else
    #define ATTR_FALLTHROUGH
#endif

//---------------------------------------------------------------------------
#define EVAL dd4hep::tools::Evaluator

namespace  {
  
  /// Internal expression evaluator helper class
  struct Item {
    enum { UNKNOWN, VARIABLE, EXPRESSION, FUNCTION, STRING } what;
    double variable;
    std::string expression;
    void   *function;

    explicit Item()              : what(UNKNOWN),   variable(0),expression(), function(0) {}
    explicit Item(double x)      : what(VARIABLE),  variable(x),expression(), function(0) {}
    explicit Item(std::string x) : what(EXPRESSION),variable(0),expression(x),function(0) {}
    explicit Item(void  *x)      : what(FUNCTION),  variable(0),expression(), function(x) {}
  };

  /// Internal expression evaluator helper union
  union FCN {
    void* ptr;
    double (*f0)();
    double (*f1)(double);
    double (*f2)(double,double);
    double (*f3)(double,double,double);
    double (*f4)(double,double,double,double);
    double (*f5)(double,double,double,double,double);
    FCN(void* p) { ptr = p; }
    FCN(double (*f)()) { f0 = f; }
    FCN(double (*f)(double)) { f1 = f; }
    FCN(double (*f)(double,double)) { f2 = f; }
    FCN(double (*f)(double,double,double)) { f3 = f; }
    FCN(double (*f)(double,double,double,double)) { f4 = f; }
    FCN(double (*f)(double,double,double,double,double)) { f5 = f; }
  };
}

//typedef char * pchar;
typedef std::unordered_map<std::string,Item> dic_type;

/// Internal expression evaluator helper class
struct EVAL::Object::Struct {
  // based on https://stackoverflow.com/a/58018604
  struct ReadLock {
    ReadLock(Struct* s): theStruct(s), theLg(s->theLock) { 
      while(theStruct->theWriterWaiting)
        theStruct->theCond.wait(theLg);
      ++theStruct->theReadersWaiting;
    }

    ReadLock(const ReadLock&) = delete;
    ~ReadLock() {
      --theStruct->theReadersWaiting;
      while(theStruct->theReadersWaiting > 0)
        theStruct->theCond.wait(theLg);
      theStruct->theCond.notify_one();
    }
    Struct* theStruct;
    std::unique_lock<std::mutex> theLg;

  };
  struct WriteLock {
    WriteLock(Struct* s): theStruct(s), theLg(s->theLock) {
      while(theStruct->theWriterWaiting)
        theStruct->theCond.wait(theLg);
    }
    WriteLock(const WriteLock&) = delete;
    ~WriteLock() {
      theStruct->theWriterWaiting = true;
      while(theStruct->theReadersWaiting > 0)
        theStruct->theCond.wait(theLg);
      theStruct->theWriterWaiting = false;
      theStruct->theCond.notify_all();
    }
    Struct* theStruct;
    std::unique_lock<std::mutex> theLg;
  };

  dic_type    theDictionary;
  int theReadersWaiting = 0;
  bool theWriterWaiting = false;
  std::condition_variable theCond;
  std::mutex  theLock;
};

//---------------------------------------------------------------------------
#define REMOVE_BLANKS							\
  for(pointer=name;;pointer++) if (!isspace(*pointer)) break;		\
  for(n=strlen(pointer);n>0;n--) if (!isspace(*(pointer+n-1))) break

#define SKIP_BLANKS                             \
  for(;;pointer++) {                            \
    c = (pointer > end) ? '\0' : *pointer;      \
    if (!isspace(c)) break;                     \
  }

#define EVAL_EXIT(STATUS,POSITION) endp = POSITION; return STATUS
#define MAX_N_PAR 5

static constexpr char sss[MAX_N_PAR+2] = "012345";

enum { ENDL, LBRA, OR, AND, EQ, NE, GE, GT, LE, LT,
       PLUS, MINUS, MULT, DIV, POW, RBRA, VALUE };

static int engine(char const*, char const*, double &, char const* &, const dic_type &);

static int variable(const std::string & name, double & result,
                    const dic_type & dictionary)
/***********************************************************************
 *                                                                     *
 * Name: variable                                    Date:    03.10.00 *
 * Author: Evgeni Chernyaev                          Revised:          *
 *                                                                     *
 * Function: Finds value of the variable.                              *
 *           This function is used by operand().                       *
 *                                                                     *
 * Parameters:                                                         *
 *   name   - name of the variable.                                    *
 *   result - value of the variable.                                   *
 *   dictionary - dictionary of available variables and functions.     *
 *                                                                     *
 ***********************************************************************/
{
  dic_type::const_iterator iter = dictionary.find(name);
  if (iter == dictionary.end())
    return EVAL::ERROR_UNKNOWN_VARIABLE;
  //NOTE: copying ::string not thread safe so must use ref
  Item const& item = iter->second;
  switch (item.what) {
  case Item::VARIABLE:
    result = item.variable;
    return EVAL::OK;
  case Item::EXPRESSION: {
    char const* exp_begin = (item.expression.c_str());
    char const* exp_end   = exp_begin + strlen(exp_begin) - 1;
    if (engine(exp_begin, exp_end, result, exp_end, dictionary) == EVAL::OK)
      return EVAL::OK;
    return EVAL::ERROR_CALCULATION_ERROR;
  }
  default:
    return EVAL::ERROR_CALCULATION_ERROR;
  }
}

static int execute_function(const std::string & name, std::stack<double> & par,
                    double & result, const dic_type & dictionary)
/***********************************************************************
 *                                                                     *
 * Name: execute_function                            Date:    03.10.00 *
 * Author: Evgeni Chernyaev                          Revised:          *
 *                                                                     *
 * Function: Finds value of the function.                              *
 *           This function is used by operand().                       *
 *                                                                     *
 * Parameters:                                                         *
 *   name   - name of the function.                                    *
 *   par    - stack of parameters.                                     *
 *   result - value of the function.                                   *
 *   dictionary - dictionary of available variables and functions.     *
 *                                                                     *
 ***********************************************************************/
{
  int npar = par.size();
  if (npar > MAX_N_PAR) return EVAL::ERROR_UNKNOWN_FUNCTION;

  dic_type::const_iterator iter = dictionary.find(sss[npar]+name);
  if (iter == dictionary.end()) return EVAL::ERROR_UNKNOWN_FUNCTION;
  //NOTE: copying ::string not thread safe so must use ref
  Item const& item = iter->second;

  double pp[MAX_N_PAR];
  for(int i=0; i<npar; i++) { pp[i] = par.top(); par.pop(); }
  errno = 0;
  if (item.function == 0)       return EVAL::ERROR_CALCULATION_ERROR;
  FCN fcn(item.function);
  switch (npar) {
  case 0:
    result = (*fcn.f0)();
    break;
  case 1:
    result = (*fcn.f1)(pp[0]);
    break;
  case 2:
    result = (*fcn.f2)(pp[1], pp[0]);
    break;
  case 3:
    result = (*fcn.f3)(pp[2],pp[1],pp[0]);
    break;
  case 4:
    result = (*fcn.f4)(pp[3],pp[2],pp[1],pp[0]);
    break;
  case 5:
    result = (*fcn.f5)(pp[4],pp[3],pp[2],pp[1],pp[0]);
    break;
  }
  return (errno == 0) ? EVAL::OK : EVAL::ERROR_CALCULATION_ERROR;
}

static int operand(char const* begin, char const* end, double & result,
                   char const* & endp, const dic_type & dictionary)
/***********************************************************************
 *                                                                     *
 * Name: operand                                     Date:    03.10.00 *
 * Author: Evgeni Chernyaev                          Revised:          *
 *                                                                     *
 * Function: Finds value of the operand. The operand can be either     *
 *           a number or a variable or a function.                     *
 *           This function is used by engine().                        *
 *                                                                     *
 * Parameters:                                                         *
 *   begin  - pointer to the first character of the operand.           *
 *   end    - pointer to the last character of the character string.   *
 *   result - value of the operand.                                    *
 *   endp   - pointer to the character where the evaluation stoped.    *
 *   dictionary - dictionary of available variables and functions.     *
 *                                                                     *
 ***********************************************************************/
{
  char const* pointer = begin;
  int   EVAL_STATUS;
  char  c;

  //   G E T   N U M B E R

  if (!isalpha(*pointer)) {
    errno = 0;
#ifdef _WIN32
    if ( pointer[0] == '0' && pointer < end && (pointer[1] == 'x' || pointer[1] == 'X') )
      result = strtol(pointer, (char **)(&pointer), 0);
    else
#endif
      result = strtod(pointer, (char **)(&pointer));
    if (errno == 0) {
      EVAL_EXIT( EVAL::OK, --pointer );
    }else{
      EVAL_EXIT( EVAL::ERROR_CALCULATION_ERROR, begin );
    }
  }

  //   G E T   N A M E

  while(pointer <= end) {
    c = *pointer;
    if ( !(c == '_' || c == ':') && !isalnum(c)) break;
    pointer++;
  }
  std::string name(begin, pointer-begin);

  //   G E T   V A R I A B L E

  result = 0.0;
  SKIP_BLANKS;
  if (c != '(') {
    EVAL_STATUS = variable(name, result, dictionary);
    EVAL_EXIT( EVAL_STATUS, (EVAL_STATUS == EVAL::OK) ? --pointer : begin);
  }

  //   G E T   F U N C T I O N

  std::stack<char const*>  pos;                // position stack
  std::stack<double> par;                // parameter stack
  double        value;
  char const*   par_begin = pointer+1;
  char const*   par_end;

  for(;;pointer++) {
    c = (pointer > end) ? '\0' : *pointer;
    switch (c) {
    case '\0':
      EVAL_EXIT( EVAL::ERROR_UNPAIRED_PARENTHESIS, pos.top() );
    case '(':
      pos.push(pointer); break;
    case ',':
      if (pos.size() == 1) {
        par_end = pointer-1;
        EVAL_STATUS = engine(par_begin, par_end, value, par_end, dictionary);
        if (EVAL_STATUS == EVAL::WARNING_BLANK_STRING)
	  { EVAL_EXIT( EVAL::ERROR_EMPTY_PARAMETER, --par_end ); }
        if (EVAL_STATUS != EVAL::OK)
	  { EVAL_EXIT( EVAL_STATUS, par_end ); }
        par.push(value);
        par_begin = pointer + 1;
      }
      break;
    case ')':
      if (pos.size() > 1) {
        pos.pop();
        break;
      }else{
        par_end = pointer-1;
        EVAL_STATUS = engine(par_begin, par_end, value, par_end, dictionary);
        switch (EVAL_STATUS) {
        case EVAL::OK:
          par.push(value);
          break;
        case EVAL::WARNING_BLANK_STRING:
          if (par.size() != 0)
	    { EVAL_EXIT( EVAL::ERROR_EMPTY_PARAMETER, --par_end ); }
          break;
        default:
          EVAL_EXIT( EVAL_STATUS, par_end );
        }
        EVAL_STATUS = execute_function(name, par, result, dictionary);
        EVAL_EXIT( EVAL_STATUS, (EVAL_STATUS == EVAL::OK) ? pointer : begin);
      }
    }
  }
}

/***********************************************************************
 *                                                                     *
 * Name: maker                                       Date:    28.09.00 *
 * Author: Evgeni Chernyaev                          Revised:          *
 *                                                                     *
 * Function: Executes basic arithmetic operations on values in the top *
 *           of the stack. Result is placed back into the stack.       *
 *           This function is used by engine().                        *
 *                                                                     *
 * Parameters:                                                         *
 *   op  - code of the operation.                                      *
 *   val - stack of values.                                            *
 *                                                                     *
 ***********************************************************************/
static int maker(int op, std::stack<double> & val)
{
  if (val.size() < 2) return EVAL::ERROR_SYNTAX_ERROR;
  double val2 = val.top(); val.pop();
  double val1 = val.top();
  switch (op) {
  case OR:                                // operator ||
    val.top() = (val1 || val2) ? 1. : 0.;
    return EVAL::OK;
  case AND:                               // operator &&
    val.top() = (val1 && val2) ? 1. : 0.;
    return EVAL::OK;
  case EQ:                                // operator ==
    val.top() = (val1 == val2) ? 1. : 0.;
    return EVAL::OK;
  case NE:                                // operator !=
    val.top() = (val1 != val2) ? 1. : 0.;
    return EVAL::OK;
  case GE:                                // operator >=
    val.top() = (val1 >= val2) ? 1. : 0.;
    return EVAL::OK;
  case GT:                                // operator >
    val.top() = (val1 >  val2) ? 1. : 0.;
    return EVAL::OK;
  case LE:                                // operator <=
    val.top() = (val1 <= val2) ? 1. : 0.;
    return EVAL::OK;
  case LT:                                // operator <
    val.top() = (val1 <  val2) ? 1. : 0.;
    return EVAL::OK;
  case PLUS:                              // operator '+'
    val.top() = val1 + val2;
    return EVAL::OK;
  case MINUS:                             // operator '-'
    val.top() = val1 - val2;
    return EVAL::OK;
  case MULT:                              // operator '*'
    val.top() = val1 * val2;
    return EVAL::OK;
  case DIV:                               // operator '/'
    if (val2 == 0.0) return EVAL::ERROR_CALCULATION_ERROR;
    val.top() = val1 / val2;
    return EVAL::OK;
  case POW:                               // operator '^' (or '**')
    errno = 0;
    val.top() = pow(val1,val2);
    if (errno == 0) return EVAL::OK;
    ATTR_FALLTHROUGH;
  default:
    return EVAL::ERROR_CALCULATION_ERROR;
  }
}

/***********************************************************************
 *                                                                     *
 * Name: engine                                      Date:    28.09.00 *
 * Author: Evgeni Chernyaev                          Revised:          *
 *                                                                     *
 * Function: Evaluates arithmetic expression.                          *
 *                                                                     *
 * Parameters:                                                         *
 *   begin  - pointer to the character string with expression.         *
 *   end    - pointer to the end of the character string (it is needed *
 *            for recursive call of engine(), when there is no '\0').  *
 *   result - result of the evaluation.                                *
 *   endp   - pointer to the character where the evaluation stoped.    *
 *   dictionary - dictionary of available variables and functions.     *
 *                                                                     *
 ***********************************************************************/
static int engine(char const* begin, char const* end, double & result,
                  char const*& endp, const dic_type & dictionary)
{
  static constexpr int SyntaxTable[17][17] = {
    //E  (  || && == != >= >  <= <  +  -  *  /  ^  )  V - current token
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 1 },   // E - previous
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 1 },   // (   token
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // ||
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // &&
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // ==
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // !=
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // >=
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // >
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // <=
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // <
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // +
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // -
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // *
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // /
    { 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // ^
    { 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 },   // )
    { 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 }    // V = {.,N,C}
  };
  static constexpr int ActionTable[15][16] = {
    //E  (  || && == != >= >  <= <  +  -  *  /  ^  ) - current operator
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1 }, // E - top operator
    {-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3 }, // (   in stack
    { 4, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4 }, // ||
    { 4, 1, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4 }, // &&
    { 4, 1, 4, 4, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4 }, // ==
    { 4, 1, 4, 4, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4 }, // !=
    { 4, 1, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1, 1, 1, 1, 4 }, // >=
    { 4, 1, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1, 1, 1, 1, 4 }, // >
    { 4, 1, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1, 1, 1, 1, 4 }, // <=
    { 4, 1, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1, 1, 1, 1, 4 }, // <
    { 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1, 1, 1, 4 }, // +
    { 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1, 1, 1, 4 }, // -
    { 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1, 4 }, // *
    { 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1, 4 }, // /
    { 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 }  // ^
  };

  std::stack<int>    op;                      // operator stack
  std::stack<char const*>  pos;                     // position stack
  std::stack<double> val;                     // value stack
  double        value;
  char const*   pointer = begin;
  int           iWhat, iCur, iPrev = 0, iTop, EVAL_STATUS;
  char          c;

  op.push(0); pos.push(pointer);         // push EOL to the stack
  SKIP_BLANKS;
  if (c == '\0') { EVAL_EXIT( EVAL::WARNING_BLANK_STRING, begin ); }
  for(;;pointer++) {

    //   N E X T   T O K E N

    c = (pointer > end) ? '\0' : *pointer;
    if (isspace(c)) continue;            // skip space, tab etc.
    switch (c) {
    case '\0': iCur = ENDL; break;
    case '(':  iCur = LBRA; break;
    case '|':
      if (*(pointer+1) == '|') {
        pointer++; iCur = OR; break;
      }else{
        EVAL_EXIT( EVAL::ERROR_UNEXPECTED_SYMBOL, pointer );
      }
    case '&':
      if (*(pointer+1) == '&') {
        pointer++; iCur = AND; break;
      }else{
        EVAL_EXIT( EVAL::ERROR_UNEXPECTED_SYMBOL, pointer );
      }
    case '=':
      if (*(pointer+1) == '=') {
        pointer++; iCur = EQ; break;
      }else{
        EVAL_EXIT( EVAL::ERROR_UNEXPECTED_SYMBOL, pointer );
      }
    case '!':
      if (*(pointer+1) == '=') {
        pointer++; iCur = NE; break;
      }else{
        EVAL_EXIT( EVAL::ERROR_UNEXPECTED_SYMBOL, pointer );
      }
    case '>':
      if (*(pointer+1) == '=') { pointer++; iCur = GE; } else { iCur = GT; }
      break;
    case '<':
      if (*(pointer+1) == '=') { pointer++; iCur = LE; } else { iCur = LT; }
      break;
    case '+':  iCur = PLUS;  break;
    case '-':  iCur = MINUS; break;
    case '*':
      if (*(pointer+1) == '*') { pointer++; iCur = POW; }else{ iCur = MULT; }
      break;
    case '/':  iCur = DIV;  break;
    case '^':  iCur = POW;  break;
    case ')':  iCur = RBRA; break;
    default:
      if (c == '.' || isalnum(c)) {
        iCur = VALUE; break;
      }else{
        EVAL_EXIT( EVAL::ERROR_UNEXPECTED_SYMBOL, pointer );
      }
    }

    //   S Y N T A X   A N A L I S Y S

    iWhat = SyntaxTable[iPrev][iCur];
    iPrev = iCur;
    switch (iWhat) {
    case 0:                             // syntax error
      EVAL_EXIT( EVAL::ERROR_SYNTAX_ERROR, pointer );
    case 1:                             // operand: number, variable, function
      EVAL_STATUS = operand(pointer, end, value, pointer, dictionary);
      if (EVAL_STATUS != EVAL::OK) { EVAL_EXIT( EVAL_STATUS, pointer ); }
      val.push(value);
      continue;
    case 2:                             // unary + or unary -
      val.push(0.0);
    case 3: default:                    // next operator
      break;
    }

    //   N E X T   O P E R A T O R

    for(;;) {
      if (op.size() == 0) { EVAL_EXIT( EVAL::ERROR_SYNTAX_ERROR, pointer ); }
      iTop = op.top();
      switch (ActionTable[iTop][iCur]) {
      case -1:                           // syntax error
        if (op.size() > 1) pointer = pos.top();
        EVAL_EXIT( EVAL::ERROR_UNPAIRED_PARENTHESIS, pointer );
      case 0:                            // last operation (assignment)
        if (val.size() == 1) {
          result = val.top();
          EVAL_EXIT( EVAL::OK, pointer );
        }else{
          EVAL_EXIT( EVAL::ERROR_SYNTAX_ERROR, pointer );
        }
      case 1:                           // push current operator in stack
        op.push(iCur); pos.push(pointer);
        break;
      case 2:                           // execute top operator
        EVAL_STATUS = maker(iTop, val); // put current operator in stack
        if (EVAL_STATUS != EVAL::OK) {
          EVAL_EXIT( EVAL_STATUS, pos.top() );
        }
        op.top() = iCur; pos.top() = pointer;
        break;
      case 3:                           // delete '(' from stack
        op.pop(); pos.pop();
        break;
      case 4: default:                  // execute top operator and
        EVAL_STATUS = maker(iTop, val); // delete it from stack
        if (EVAL_STATUS != EVAL::OK) {  // repete with the same iCur
          EVAL_EXIT( EVAL_STATUS, pos.top() );
        }
        op.pop(); pos.pop();
        continue;
      }
      break;
    }
  }
}

//---------------------------------------------------------------------------
static int setItem(const char * prefix, const char * name,
                   const Item & item, EVAL::Object::Struct* imp) {

  if (name == 0 || *name == '\0') {
    return EVAL::ERROR_NOT_A_NAME;
  }

  //   R E M O V E   L E A D I N G   A N D   T R A I L I N G   S P A C E S

  const char * pointer; int n; REMOVE_BLANKS;

  //   C H E C K   N A M E

  if (n == 0) {
    return EVAL::ERROR_NOT_A_NAME;
  }
  for(int i=0; i<n; i++) {
    char c = *(pointer+i);
    if ( !(c == '_' || c== ':') && !isalnum(c)) {
      return EVAL::ERROR_NOT_A_NAME;
    }
  }

  //   A D D   I T E M   T O   T H E   D I C T I O N A R Y

  std::string item_name = prefix + std::string(pointer,n);
  EVAL::Object::Struct::WriteLock guard(imp);
  dic_type::iterator iter = imp->theDictionary.find(item_name);
  if (iter != imp->theDictionary.end()) {
    iter->second = item;
    if (item_name == name) {
      return EVAL::WARNING_EXISTING_VARIABLE;
    }else{
      return EVAL::WARNING_EXISTING_FUNCTION;
    }
  }
  imp->theDictionary[item_name] = item;
  return EVAL::OK;
}

//---------------------------------------------------------------------------
  static void print_error_status(std::ostream& os, int status, char const* extra) {
  static char prefix[] = "Evaluator::Object : ";
  const char* opt = (extra ? extra : "");
  switch (status) {
  case EVAL::WARNING_EXISTING_VARIABLE:
    os << prefix << "existing variable";
    return;
  case EVAL::WARNING_EXISTING_FUNCTION:
    os << prefix << "existing function";
    return;
  case EVAL::WARNING_BLANK_STRING:
    os << prefix << "blank string detected";
    return;
  case EVAL::ERROR_NOT_A_NAME:
    os << prefix << "invalid name : " << opt;
    return;
  case EVAL::ERROR_SYNTAX_ERROR:
    os << prefix << "systax error"        ;
    return;
  case EVAL::ERROR_UNPAIRED_PARENTHESIS:
    os << prefix << "unpaired parenthesis";
    return;
  case EVAL::ERROR_UNEXPECTED_SYMBOL:
    os << prefix << "unexpected symbol : " << opt;
    return;
  case EVAL::ERROR_UNKNOWN_VARIABLE:
    os << prefix << "unknown variable : " << opt;
    return;
  case EVAL::ERROR_UNKNOWN_FUNCTION:
    os << prefix << "unknown function : " << opt;
    return;
  case EVAL::ERROR_EMPTY_PARAMETER:
    os << prefix << "empty parameter in function call: " << opt;
    return;
  case EVAL::ERROR_CALCULATION_ERROR:
    os << prefix << "calculation error";
    return;
  default:
    return;
  }
}

//---------------------------------------------------------------------------
using namespace dd4hep::tools;

//---------------------------------------------------------------------------
Evaluator::Object::Object(double meter, double kilogram, double second, double ampere, double kelvin
                          , double mole, double candela, double radians) : imp( new Struct()) {
  setStdMath();
  setSystemOfUnits(meter, kilogram, second, ampere, kelvin
                   , mole, candela, radians );
}

//---------------------------------------------------------------------------
Evaluator::Object::~Object() {
  delete imp;
}

//---------------------------------------------------------------------------
Evaluator::Object::EvalStatus Evaluator::Object::evaluate(const char * expression) const {
  EvalStatus s;
  if (expression != 0) {
    Struct::ReadLock guard(imp);
    s.theStatus = engine(expression,
                         expression+strlen(expression)-1,
                         s.theResult,
                         s.thePosition,
                         imp->theDictionary);
  }
  return s;
}

//---------------------------------------------------------------------------
int Evaluator::Object::EvalStatus::status() const {
  return theStatus;
}

//---------------------------------------------------------------------------
double Evaluator::Object::EvalStatus::result() const {
  return theResult;
}

//---------------------------------------------------------------------------
int Evaluator::Object::EvalStatus::error_position(const char* expression) const {
  return thePosition - expression;
}

//---------------------------------------------------------------------------
void Evaluator::Object::EvalStatus::print_error() const {
  std::stringstream str;
  print_error(str);
  if ( str.str().empty() )  return;
  std::cerr << str.str() << std::endl;
}

//---------------------------------------------------------------------------
void Evaluator::Object::EvalStatus::print_error(std::ostream& os) const {
  print_error_status(os, theStatus, thePosition);
}

//---------------------------------------------------------------------------
int Evaluator::Object::setEnviron(const char* name, const char* value)  {
  std::string prefix = "${";
  std::string item_name = prefix + std::string(name) + std::string("}");

  //Need to take lock before creating Item since since Item to be destroyed
  // before the lock in order avoid ::string ref count thread problem
  Struct::WriteLock guard(imp);
  Item item;
  item.what = Item::STRING;
  item.expression = value;
  item.function = 0;
  item.variable = 0;
  dic_type::iterator iter = imp->theDictionary.find(item_name);
  if (iter != imp->theDictionary.end()) {
    iter->second = item;
    if (item_name == name) {
      return EVAL::WARNING_EXISTING_VARIABLE;
    }else{
      return EVAL::WARNING_EXISTING_FUNCTION;
    }
  }else{
    imp->theDictionary[item_name] = item;
    return EVAL::OK;
  }
}

//---------------------------------------------------------------------------
std::pair<const char*,int> Evaluator::Object::getEnviron(const char* name)  const {
  Struct::ReadLock guard(imp);
  Struct const* cImp = imp;
  dic_type::const_iterator iter = cImp->theDictionary.find(name);
  if (iter != cImp->theDictionary.end()) {
    return std::make_pair(iter->second.expression.c_str(), EVAL::OK);
  }
  if ( ::strlen(name) > 3 )  {
    // Need to remove braces from ${xxxx} for call to getenv()
    std::string env_name(name+2,::strlen(name)-3);
    const char* env_str = ::getenv(env_name.c_str());
    if ( 0 != env_str )    {
      return std::make_pair(env_str, EVAL::OK);
    }
  }
  return std::make_pair(nullptr,EVAL::ERROR_UNKNOWN_VARIABLE);
}

//---------------------------------------------------------------------------
int Evaluator::Object::setVariable(const char * name, double value)  {
  return setItem("", name, Item(value), imp);
}

int Evaluator::Object::setVariable(const char * name, const char * expression)  {
  Item item(expression);
  auto returnValue = setItem("", name, item, imp);
  {
    //We need to decrement the ref count on the item.expression while holding
    // the lock since the ::string was copied into the dictionary
    Struct::WriteLock guard(imp);
    item.expression = "";
  }
  return returnValue;
}

void Evaluator::Object::setVariableNoLock(const char * name, double value)  {
  std::string item_name = name;
  Item item(value);
  imp->theDictionary[item_name] = item;
}

int Evaluator::Object::setFunction(const char * name,double (*fun)())   {
  return setItem("0", name, Item(FCN(fun).ptr), imp);
}

int Evaluator::Object::setFunction(const char * name,double (*fun)(double))   {
  return setItem("1", name, Item(FCN(fun).ptr), imp);
}

int Evaluator::Object::setFunction(const char * name, double (*fun)(double,double))  {
  return setItem("2", name, Item(FCN(fun).ptr), imp);
}

int Evaluator::Object::setFunction(const char * name, double (*fun)(double,double,double))  {
  return setItem("3", name, Item(FCN(fun).ptr), imp);
}

int Evaluator::Object::setFunction(const char * name, double (*fun)(double,double,double,double)) {
  return setItem("4", name, Item(FCN(fun).ptr), imp);
}

int Evaluator::Object::setFunction(const char * name, double (*fun)(double,double,double,double,double))  {
  return setItem("5", name, Item(FCN(fun).ptr), imp);
}

void Evaluator::Object::setFunctionNoLock(const char * name,double (*fun)(double))   {
  std::string item_name = "1"+std::string(name);
  Item item(FCN(fun).ptr);
  imp->theDictionary[item_name] = item;
}

void Evaluator::Object::setFunctionNoLock(const char * name, double (*fun)(double,double))  {
  std::string item_name = "2"+std::string(name);
  Item item(FCN(fun).ptr);
  imp->theDictionary[item_name] = item;
}


//---------------------------------------------------------------------------
bool Evaluator::Object::findVariable(const char * name) const {
  if (name == 0 || *name == '\0') return false;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return false;
  Struct::ReadLock guard(imp);
  return
    (imp->theDictionary.find(std::string(pointer,n)) == imp->theDictionary.end()) ?
    false : true;
}

//---------------------------------------------------------------------------
bool Evaluator::Object::findFunction(const char * name, int npar) const {
  if (name == 0 || *name == '\0')    return false;
  if (npar < 0  || npar > MAX_N_PAR) return false;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return false;
  Struct::ReadLock guard(imp);
  return (imp->theDictionary.find(sss[npar]+std::string(pointer,n)) ==
	  imp->theDictionary.end()) ? false : true;
}

//---------------------------------------------------------------------------
void Evaluator::Object::removeVariable(const char * name) {
  if (name == 0 || *name == '\0') return;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return;
  Struct::WriteLock guard(imp);
  imp->theDictionary.erase(std::string(pointer,n));
}

//---------------------------------------------------------------------------
void Evaluator::Object::removeFunction(const char * name, int npar) {
  if (name == 0 || *name == '\0')    return;
  if (npar < 0  || npar > MAX_N_PAR) return;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return;
  Struct::WriteLock guard(imp);
  imp->theDictionary.erase(sss[npar]+std::string(pointer,n));
}

//---------------------------------------------------------------------------
Evaluator::Evaluator(double meter, double kilogram, double second, double ampere, double kelvin
                          , double mole, double candela, double radians)   {
  object = new Object(meter, kilogram, second, ampere, kelvin, mole, candela, radians);
}

//---------------------------------------------------------------------------
Evaluator::Evaluator(Evaluator&& other):object(other.object) {
  other.object=nullptr;
}

//---------------------------------------------------------------------------
Evaluator::~Evaluator()   {
  delete object;
}

//---------------------------------------------------------------------------
std::pair<int,double> Evaluator::evaluate(const std::string& expression)  const   {
  auto result = object->evaluate(expression.c_str());
  return std::make_pair(result.status(),result.result());
}

//---------------------------------------------------------------------------
std::pair<int,double> Evaluator::evaluate(const std::string& expression, std::ostream& os)  const   {
  auto result = object->evaluate(expression.c_str());
  int    status = result.status();
  if ( status != OK )   {
    result.print_error(os);
  }
  return std::make_pair(result.status(),result.result());
}

//---------------------------------------------------------------------------
int Evaluator::setEnviron(const std::string& name, const std::string& value)  const    {
  int result = object->setEnviron(name.c_str(), value.c_str());
  return result;
}

//---------------------------------------------------------------------------
std::pair<int,std::string> Evaluator::getEnviron(const std::string& name)  const    {
  std::pair<int,std::string> result;
  auto env_status = object->getEnviron(name.c_str());
  result.first = env_status.second;
  if( env_status.first ) result.second = env_status.first;
  return result;
}

//---------------------------------------------------------------------------
std::pair<int,std::string> Evaluator::getEnviron(const std::string& name, std::ostream& os)  const    {
  std::pair<int,std::string> result;
  auto env_status = object->getEnviron(name.c_str());
  result.first = env_status.second;
  if ( env_status.first )   {
    result.second = env_status.first;
  }
  if ( result.first != OK )   {
    print_error_status(os, result.first, name.c_str());
  }
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setVariable(const std::string& name, double value)  const    {
  int result = object->setVariable(name.c_str(), value);
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setVariable(const std::string& name, double value, std::ostream& os)  const    {
  int result = object->setVariable(name.c_str(), value);
  if ( result != OK )   {
    print_error_status(os, result, name.c_str());
  }
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setVariable(const std::string& name, const std::string& value)  const    {
  int result = object->setVariable(name.c_str(), value.c_str());
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setVariable(const std::string& name, const std::string& value, std::ostream& os)  const    {
  int result = object->setVariable(name.c_str(), value.c_str());
  if ( result != OK )   {
    print_error_status(os, result, name.c_str());
  }
  return result;
}

//---------------------------------------------------------------------------
bool Evaluator::findVariable(const std::string& name)  const    {
  bool ret;
  ret = object->findVariable(name.c_str());
  return ret;
}

//---------------------------------------------------------------------------
int Evaluator::setFunction(const std::string& name, double (*fun)())  const    {
  int result = object->setFunction(name.c_str(), fun);
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setFunction(const std::string& name, double (*fun)(double))  const    {
  int result = object->setFunction(name.c_str(), fun);
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setFunction(const std::string& name, double (*fun)(double, double))  const    {
  int result = object->setFunction(name.c_str(), fun);
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setFunction(const std::string& name, double (*fun)(double, double, double))  const    {
  int result = object->setFunction(name.c_str(), fun);
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setFunction(const std::string& name, double (*fun)(double, double, double, double))  const    {
  int result = object->setFunction(name.c_str(), fun);
  return result;
}

//---------------------------------------------------------------------------
int Evaluator::setFunction(const std::string& name, double (*fun)(double, double, double, double, double))  const    {
  int result =object->setFunction(name.c_str(), fun);
  return result;
}

//---------------------------------------------------------------------------
bool Evaluator::findFunction(const std::string& name, int npar) const    {
  bool ret;
  ret = object->findFunction(name.c_str(), npar);
  return ret;
}
