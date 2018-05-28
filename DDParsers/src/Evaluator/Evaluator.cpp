
// -*- C++ -*-
// ---------------------------------------------------------------------------

#include "Evaluator/Evaluator.h"

#include <iostream>
#include <cmath>        // for pow()
#include "stack.src"
#include "string.src"
#include "hash_map.src"
#include <sstream>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <cstdlib>     // for strtod()

// Disable some diagnostics, which we know, but need to ignore
#if defined(__GNUC__) && !defined(__APPLE__) && !defined(__llvm__)
/*  This is OK:
    ../DDCore/src/Evaluator/Evaluator.cpp: In function 'int engine(pchar, pchar, double&, char*&, const dic_type&)':
    ../DDCore/src/Evaluator/Evaluator.cpp:164:23: warning: 'pp[3]' may be used uninitialized in this function [-Wmaybe-uninitialized]
    result = (*fcn.f4)(pp[3],pp[2],pp[1],pp[0]);
    ....
*/
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

//---------------------------------------------------------------------------

/// Internal expression evaluator helper class
struct Item {
  enum { UNKNOWN, VARIABLE, EXPRESSION, FUNCTION, STRING } what;
  double variable;
  string expression;
  void   *function;

  explicit Item()         : what(UNKNOWN),   variable(0),expression(), function(0) {}
  explicit Item(double x) : what(VARIABLE),  variable(x),expression(), function(0) {}
  explicit Item(string x) : what(EXPRESSION),variable(0),expression(x),function(0) {}
  explicit Item(void  *x) : what(FUNCTION),  variable(0),expression(), function(x) {}
};

typedef char * pchar;
typedef hash_map<string,Item> dic_type;

namespace {

  /// Internal expression evaluator helper class
  struct Struct {
    dic_type theDictionary;
    pchar    theExpression;
    pchar    thePosition;
    int      theStatus;
    double   theResult;
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

//---------------------------------------------------------------------------
#define EVAL dd4hep::tools::Evaluator

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

static const char sss[MAX_N_PAR+2] = "012345";

enum { ENDL, LBRA, OR, AND, EQ, NE, GE, GT, LE, LT,
       PLUS, MINUS, MULT, DIV, POW, RBRA, VALUE };

static int engine(pchar, pchar, double &, pchar &, const dic_type &);

static int variable(const string & name, double & result,
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
  Item item = iter->second;
  switch (item.what) {
  case Item::VARIABLE:
    result = item.variable;
    return EVAL::OK;
  case Item::EXPRESSION: {
    pchar exp_begin = (char *)(item.expression.c_str());
    pchar exp_end   = exp_begin + strlen(exp_begin) - 1;
    if (engine(exp_begin, exp_end, result, exp_end, dictionary) == EVAL::OK)
      return EVAL::OK;
    return EVAL::ERROR_CALCULATION_ERROR;
  }
  default:
    return EVAL::ERROR_CALCULATION_ERROR;
  }
}

static int function(const string & name, stack<double> & par,
                    double & result, const dic_type & dictionary)
/***********************************************************************
 *                                                                     *
 * Name: function                                    Date:    03.10.00 *
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
  Item item = iter->second;

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

static int operand(pchar begin, pchar end, double & result,
                   pchar & endp, const dic_type & dictionary)
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
  pchar pointer = begin;
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
    if (c != '_' && !isalnum(c)) break;
    pointer++;
  }
  c = *pointer;
  *pointer = '\0';
  string name(begin);
  *pointer = c;

  //   G E T   V A R I A B L E

  result = 0.0;
  SKIP_BLANKS;
  if (c != '(') {
    EVAL_STATUS = variable(name, result, dictionary);
    EVAL_EXIT( EVAL_STATUS, (EVAL_STATUS == EVAL::OK) ? --pointer : begin);
  }

  //   G E T   F U N C T I O N

  stack<pchar>  pos;                // position stack
  stack<double> par;                // parameter stack
  double        value;
  pchar         par_begin = pointer+1, par_end;

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
        EVAL_STATUS = function(name, par, result, dictionary);
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
static int maker(int op, stack<double> & val)
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
    [[fallthrough]];
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
static int engine(pchar begin, pchar end, double & result,
                  pchar & endp, const dic_type & dictionary)
{
  static const int SyntaxTable[17][17] = {
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
  static const int ActionTable[15][16] = {
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

  stack<int>    op;                      // operator stack
  stack<pchar>  pos;                     // position stack
  stack<double> val;                     // value stack
  double        value;
  pchar         pointer = begin;
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
    case 0:                             // systax error
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
static void setItem(const char * prefix, const char * name,
                    const Item & item, Struct * s) {

  if (name == 0 || *name == '\0') {
    s->theStatus = EVAL::ERROR_NOT_A_NAME;
    return;
  }

  //   R E M O V E   L E A D I N G   A N D   T R A I L I N G   S P A C E S

  const char * pointer; int n; REMOVE_BLANKS;

  //   C H E C K   N A M E

  if (n == 0) {
    s->theStatus = EVAL::ERROR_NOT_A_NAME;
    return;
  }
  for(int i=0; i<n; i++) {
    char c = *(pointer+i);
    if (c != '_' && !isalnum(c)) {
      s->theStatus = EVAL::ERROR_NOT_A_NAME;
      return;
    }
  }

  //   A D D   I T E M   T O   T H E   D I C T I O N A R Y

  string item_name = prefix + string(pointer,n);
  dic_type::iterator iter = (s->theDictionary).find(item_name);
  if (iter != (s->theDictionary).end()) {
    iter->second = item;
    if (item_name == name) {
      s->theStatus = EVAL::WARNING_EXISTING_VARIABLE;
    }else{
      s->theStatus = EVAL::WARNING_EXISTING_FUNCTION;
    }
  }else{
    (s->theDictionary)[item_name] = item;
    s->theStatus = EVAL::OK;
  }
}

//---------------------------------------------------------------------------
using namespace dd4hep::tools;

//---------------------------------------------------------------------------
Evaluator::Evaluator() {
  Struct * s = new Struct();
  p = (void *) s;
  s->theExpression = 0;
  s->thePosition   = 0;
  s->theStatus     = OK;
  s->theResult     = 0.0;
}

//---------------------------------------------------------------------------
Evaluator::~Evaluator() {
  Struct * s = reinterpret_cast<Struct*>(p);
  if (s->theExpression != 0) {
    delete[] s->theExpression;
    s->theExpression = 0;
  }
  delete reinterpret_cast<Struct*>(p);
}

//---------------------------------------------------------------------------
double Evaluator::evaluate(const char * expression) {
  Struct * s = reinterpret_cast<Struct*>(p);
  if (s->theExpression != 0) { delete[] s->theExpression; }
  s->theExpression = 0;
  s->thePosition   = 0;
  s->theStatus     = WARNING_BLANK_STRING;
  s->theResult     = 0.0;
  if (expression != 0) {
    s->theExpression = new char[strlen(expression)+1];
    strcpy(s->theExpression, expression);
    s->theStatus = engine(s->theExpression,
			  s->theExpression+strlen(expression)-1,
			  s->theResult,
			  s->thePosition,
			  s->theDictionary);
  }
  return s->theResult;
}

//---------------------------------------------------------------------------
int Evaluator::status() const {
  return (reinterpret_cast<Struct*>(p))->theStatus;
}

//---------------------------------------------------------------------------
int Evaluator::error_position() const {
  return (reinterpret_cast<Struct*>(p))->thePosition - (reinterpret_cast<Struct*>(p))->theExpression;
}

//---------------------------------------------------------------------------
void Evaluator::print_error() const {
  std::stringstream str;
  print_error(str);
  if ( str.str().empty() )  return;
  std::cerr << str.str() << std::endl;
}

//---------------------------------------------------------------------------
void Evaluator::print_error(std::ostream& os) const {
  static char prefix[] = "Evaluator : ";
  Struct * s = reinterpret_cast<Struct*>(p);
  const char* opt = (s->thePosition ? s->thePosition : "");
  switch (s->theStatus) {
  case ERROR_NOT_A_NAME:
    os << prefix << "invalid name : " << opt;
    return;
  case ERROR_SYNTAX_ERROR:
    os << prefix << "systax error"        ;
    return;
  case ERROR_UNPAIRED_PARENTHESIS:
    os << prefix << "unpaired parenthesis";
    return;
  case ERROR_UNEXPECTED_SYMBOL:
    os << prefix << "unexpected symbol : " << opt;
    return;
  case ERROR_UNKNOWN_VARIABLE:
    os << prefix << "unknown variable : " << opt;
    return;
  case ERROR_UNKNOWN_FUNCTION:
    os << prefix << "unknown function : " << opt;
    return;
  case ERROR_EMPTY_PARAMETER:
    os << prefix << "empty parameter in function call: " << opt;
    return;
  case ERROR_CALCULATION_ERROR:
    os << prefix << "calculation error";
    return;
  default:
    return;
  }
}

//---------------------------------------------------------------------------
void Evaluator::setEnviron(const char* name, const char* value)  {
  Struct* s = reinterpret_cast<Struct*>(p);
  string prefix = "${";
  string item_name = prefix + string(name) + string("}");
  dic_type::iterator iter = (s->theDictionary).find(item_name);
  Item item;
  item.what = Item::STRING;
  item.expression = value;
  item.function = 0;
  item.variable = 0;
  //std::cout << " ++++++++++++++++++++++++++++ Saving env:" << name << " = " << value << std::endl;
  if (iter != (s->theDictionary).end()) {
    iter->second = item;
    if (item_name == name) {
      s->theStatus = EVAL::WARNING_EXISTING_VARIABLE;
    }else{
      s->theStatus = EVAL::WARNING_EXISTING_FUNCTION;
    }
  }else{
    (s->theDictionary)[item_name] = item;
    s->theStatus = EVAL::OK;
  }
}
//---------------------------------------------------------------------------
const char* Evaluator::getEnviron(const char* name)  {
  Struct* s = reinterpret_cast<Struct*>(p);
  string item_name = name;
  //std::cout << " ++++++++++++++++++++++++++++ Try to resolve env:" << name << std::endl;
  dic_type::iterator iter = (s->theDictionary).find(item_name);
  if (iter != (s->theDictionary).end()) {
    s->theStatus = EVAL::OK;
    return iter->second.expression.c_str();
  }
  if ( ::strlen(item_name.c_str()) > 3 )  {
    // Need to remove braces from ${xxxx} for call to getenv()
    string env_name(name+2,::strlen(name)-3);
    const char* env_str = ::getenv(env_name.c_str());
    if ( 0 != env_str )    {
      s->theStatus = EVAL::OK;
      return env_str;
    }
  }
  s->theStatus = EVAL::ERROR_UNKNOWN_VARIABLE;
  return 0;
}

//---------------------------------------------------------------------------
void Evaluator::setVariable(const char * name, double value)
{ setItem("", name, Item(value), reinterpret_cast<Struct*>(p)); }

void Evaluator::setVariable(const char * name, const char * expression)
{ setItem("", name, Item(expression), reinterpret_cast<Struct*>(p)); }

//---------------------------------------------------------------------------
void Evaluator::setFunction(const char * name,double (*fun)())   {
  FCN fcn(fun);
  setItem("0", name, Item(fcn.ptr), reinterpret_cast<Struct*>(p));
}

void Evaluator::setFunction(const char * name,double (*fun)(double))   {
  FCN fcn(fun);
  setItem("1", name, Item(fcn.ptr), reinterpret_cast<Struct*>(p));
}

void Evaluator::setFunction(const char * name, double (*fun)(double,double))  {
  FCN fcn(fun);
  setItem("2", name, Item(fcn.ptr), reinterpret_cast<Struct*>(p));
}

void Evaluator::setFunction(const char * name, double (*fun)(double,double,double))  {
  FCN fcn(fun);
  setItem("3", name, Item(fcn.ptr), reinterpret_cast<Struct*>(p));
}

void Evaluator::setFunction(const char * name, double (*fun)(double,double,double,double)) {
  FCN fcn(fun);
  setItem("4", name, Item(fcn.ptr), reinterpret_cast<Struct*>(p));
}

void Evaluator::setFunction(const char * name, double (*fun)(double,double,double,double,double))  {
  FCN fcn(fun);
  setItem("5", name, Item(fcn.ptr), reinterpret_cast<Struct*>(p));
}

//---------------------------------------------------------------------------
bool Evaluator::findVariable(const char * name) const {
  if (name == 0 || *name == '\0') return false;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return false;
  Struct * s = reinterpret_cast<Struct*>(p);
  return
    ((s->theDictionary).find(string(pointer,n)) == (s->theDictionary).end()) ?
    false : true;
}

//---------------------------------------------------------------------------
bool Evaluator::findFunction(const char * name, int npar) const {
  if (name == 0 || *name == '\0')    return false;
  if (npar < 0  || npar > MAX_N_PAR) return false;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return false;
  Struct * s = reinterpret_cast<Struct*>(p);
  return ((s->theDictionary).find(sss[npar]+string(pointer,n)) ==
	  (s->theDictionary).end()) ? false : true;
}

//---------------------------------------------------------------------------
void Evaluator::removeVariable(const char * name) {
  if (name == 0 || *name == '\0') return;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return;
  Struct * s = reinterpret_cast<Struct*>(p);
  (s->theDictionary).erase(string(pointer,n));
}

//---------------------------------------------------------------------------
void Evaluator::removeFunction(const char * name, int npar) {
  if (name == 0 || *name == '\0')    return;
  if (npar < 0  || npar > MAX_N_PAR) return;
  const char * pointer; int n; REMOVE_BLANKS;
  if (n == 0) return;
  Struct * s = reinterpret_cast<Struct*>(p);
  (s->theDictionary).erase(sss[npar]+string(pointer,n));
}

//---------------------------------------------------------------------------
void Evaluator::clear() {
  Struct * s = reinterpret_cast<Struct*>(p);
  s->theDictionary.clear();
  s->theExpression = 0;
  s->thePosition   = 0;
  s->theStatus     = OK;
  s->theResult     = 0.0;
}
