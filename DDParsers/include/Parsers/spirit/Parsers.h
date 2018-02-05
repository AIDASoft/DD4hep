//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DD4HEP_PARSERS_H
#define DD4HEP_PARSERS_H 1
// ============================================================================
// Include files
#include "Parsers/config.h"
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

// ============================================================================
#define PARSERS_DECL_FOR_SINGLE(Type)                                   \
  namespace dd4hep { namespace Parsers {                                \
      int parse(Type& result, const std::string& input);                \
    }}

#define DD4HEP_DEFINE_PARSER_DUMMY(Type)                                \
  PARSERS_DECL_FOR_SINGLE(Type)                                         \
  namespace dd4hep   {   namespace Parsers   {                          \
      int parse(Type&, const std::string&)     {  return 1;  }          \
    }}

#define PARSERS_DECL_FOR_PAIR(FirstType, SecondType)                    \
  namespace dd4hep { namespace Parsers {                                \
      int parse(std::pair<FirstType, SecondType >& result,const std::string& input); }}

#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define PARSERS_DECL_FOR_LIST(InnerType)                                \
  namespace dd4hep { namespace Parsers {                                \
      int parse(std::vector<InnerType >& result,const std::string& input); \
      int parse(std::list<InnerType >& result,const std::string& input); \
      int parse(std::set<InnerType >& result,const std::string& input); \
      int parse(std::deque<InnerType >& result,const std::string& input); \
      int parse(std::map<std::string,InnerType >& result,const std::string& input); \
      int parse(std::map<int,InnerType >& result,const std::string& input); \
      int parse(std::map<unsigned long,InnerType >& result,const std::string& input); \
      int parse(std::pair<std::string,InnerType >& result,const std::string& input); \
      int parse(std::pair<int,InnerType >& result,const std::string& input); \
      int parse(std::pair<unsigned long,InnerType >& result,const std::string& input); \
    }}
#else

#define PARSERS_DECL_FOR_LIST(InnerType)                                \
  namespace dd4hep { namespace Parsers {                                \
      int parse(std::vector<InnerType >& result,const std::string& input); \
      int parse(std::list<InnerType >& result,const std::string& input); \
      int parse(std::set<InnerType >& result,const std::string& input); \
      int parse(std::map<std::string,InnerType >& result,const std::string& input); \
      int parse(std::map<int,InnerType >& result,const std::string& input); \
      int parse(std::pair<std::string,InnerType >& result,const std::string& input); \
      int parse(std::pair<int,InnerType >& result,const std::string& input); \
    }}

#endif   //  DD4HEP_HAVE_ALL_PARSERS

// ============================================================================
/** @file
 *  The declaration of major parsing functions used e.g
 *  for (re)implementation of new extended properties see class Property
 *  These function also could be used in a different, much wider contex.
 *  all of them have the semantic:
 *  <c>int parse ( TYPE& result , const std::string& input )</c>
 *  where <c>input</c> is the input string to be parsed,
 *  and <c>result</c> is the the result of parsing
 *
 *  @code
 *
 *  const std::string        input = ... ;
 *  std::vector<std::string> result ;
 *
 *  // parse the input
 *  int sc = parse ( result , input ) ;
 *  if ( sc.isFailure() )
 *  {
 *    // error here ...
 *  }
 *  std::cout << "vector size " << result.size() << std::endl ;
 *
 *  @endcode
 *
 *  @see dd4hep::Parsers::parse
 *  @see Property
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-12
 */
// ============================================================================

// ========================================================================
/// parse the <c>bool</c> value
/**  @see dd4hep::Parsers::BoolGrammar
 *  @param result (output) boolean result
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-12
 */
PARSERS_DECL_FOR_SINGLE(bool)
// ========================================================================
/// parse the <c>char</c> value
/**
 *  @see dd4hep::Parsers::CharGrammar
 *  @param result (output) boolean result
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-12
 */
#if defined(DD4HEP_HAVE_ALL_PARSERS)
PARSERS_DECL_FOR_SINGLE(char)
/// @see dd4hep::Parsers::parser(char&,std::string&)
PARSERS_DECL_FOR_SINGLE(unsigned char)
/// @see dd4hep::Parsers::parser(char&,std::string&)
PARSERS_DECL_FOR_SINGLE(signed char)
#endif
// ========================================================================
/// parse the <c>int</c> value
/**
 *  @see dd4hep::Parsers::IntGrammar
 *  @param result (output) integer result
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-14
 */
/// @see dd4hep::Parsers::parser( int&, const std::string& )
PARSERS_DECL_FOR_SINGLE(int)
PARSERS_DECL_FOR_SINGLE(long)
#if defined(DD4HEP_HAVE_ALL_PARSERS)
/// @see dd4hep::Parsers::parser( int&, const std::string& )
PARSERS_DECL_FOR_SINGLE(short)
/// @see dd4hep::Parsers::parser( int&, const std::string& )
PARSERS_DECL_FOR_SINGLE(unsigned short)
/// @see dd4hep::Parsers::parser( int&, const std::string& )
PARSERS_DECL_FOR_SINGLE(unsigned int)
/// @see dd4hep::Parsers::parser( int&, const std::string& )
PARSERS_DECL_FOR_SINGLE(unsigned long)
/// @see dd4hep::Parsers::parser( int&, const std::string& )
PARSERS_DECL_FOR_SINGLE(long long)
/// @see dd4hep::Parsers::parser( int&, const std::string& )
  PARSERS_DECL_FOR_SINGLE(unsigned long long)
#endif
// ========================================================================
/// parse the <c>double</c> value
/**
 *  @see dd4hep::Parsers::RealGrammar
 *  @param result (output) double result
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-14
 */
PARSERS_DECL_FOR_SINGLE(double)
/// @see dd4hep::Parsers::parser( double&, const std::string& )
PARSERS_DECL_FOR_SINGLE(float)
#if defined(DD4HEP_HAVE_ALL_PARSERS)
/// @see dd4hep::Parsers::parser( double&, const std::string& )
PARSERS_DECL_FOR_SINGLE(long double)
#endif
// ========================================================================
/** parse the <c>std::string</c> value
 *
 *  @see dd4hep::Parsers::StringGrammar
 *  @param result (output) string result
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-14
 */
PARSERS_DECL_FOR_SINGLE(std::string)
// ========================================================================

PARSERS_DECL_FOR_LIST(int)
PARSERS_DECL_FOR_LIST(long)
PARSERS_DECL_FOR_LIST(bool)
PARSERS_DECL_FOR_LIST(double)
PARSERS_DECL_FOR_LIST(float)
PARSERS_DECL_FOR_LIST(std::string)

#if defined(DD4HEP_HAVE_ALL_PARSERS)
PARSERS_DECL_FOR_LIST(char)
PARSERS_DECL_FOR_LIST(unsigned char)
PARSERS_DECL_FOR_LIST(signed char)
PARSERS_DECL_FOR_LIST(short)
PARSERS_DECL_FOR_LIST(unsigned short)
PARSERS_DECL_FOR_LIST(unsigned int)
PARSERS_DECL_FOR_LIST(unsigned long)
PARSERS_DECL_FOR_LIST(long long)
PARSERS_DECL_FOR_LIST(unsigned long long)
PARSERS_DECL_FOR_LIST(long double)
#endif

// ========================================================================
// Advanced parses
// ========================================================================
/// parse the <c>std::pair\<double,double\></c> value
/**
 *  @see dd4hep::Parsers::PairGrammar
 *  @see dd4hep::Parsers::RealGrammar
 *  @param result (output) pair of doubles
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-14
 */
PARSERS_DECL_FOR_PAIR(double, double)
// ========================================================================
/** parse the <c>std::pair\<int,int\></c> value
 *
 *  @see dd4hep::Parsers::PairGrammar
 *  @see dd4hep::Parsers::IntGrammar
 *  @param result (output) pair of integers
 *  @param input  (input) the string to be parsed
 *  @return status code
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-14
 */
PARSERS_DECL_FOR_PAIR(int, int)
// ========================================================================

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {

    /// parse the <c>std::vector\<std::pair\<double,double\> \></c> value
    /**
     *  @see dd4hep::Parsers::VectorGrammar
     *  @see dd4hep::Parsers::PairGrammar
     *  @see dd4hep::Parsers::RealGrammar
     *  @param result (output) vector with pairs of doubles
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::vector<std::pair<double, double> >& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::vector\<std::pair\<int,int\> \></c> value
    /**
     *  @see dd4hep::Parsers::VectorGrammar
     *  @see dd4hep::Parsers::PairGrammar
     *  @see dd4hep::Parsers::IntGrammar
     *  @param result (output) vector with pairs of int
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::vector<std::pair<int, int> >& result, const std::string& input);
    // ========================================================================
    // vector< vector< TYPE > >
    // ========================================================================
    /// parse the <c>std::vector\<std::vector\<std::string\> \></c> value
    /**
     *  @see dd4hep::Parsers::VectorGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @param result (output) vector with vectors of strings
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::vector<std::vector<std::string> >& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::vector\<std::vector\<double\> \></c> value
    /**
     *  @see dd4hep::Parsers::VectorGrammar
     *  @see dd4hep::Parsers::RealGrammar
     *  @param result (output) vector with vectors of doubles
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::vector<std::vector<double> >& result, const std::string& input);
    // ========================================================================
    // map< TYPE, TYPE >
    // ========================================================================
    /// parse the <c>std::map\<int , int\></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::IntGrammar
     *  @param result (output) map with integer key and double value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<int, int>& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<int , double\></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::IntGrammar
     *  @see dd4hep::Parsers::RealGrammar
     *  @param result (output) map with integer key and double value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<int, double>& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<std::string , std::string\></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @param result (output) map with string key and value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<std::string, std::string>& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<std::string , int\></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @see dd4hep::Parsers::IntGrammar
     *  @param result (output) map with string key and integer value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<std::string, int>& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<std::string , double\></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @see dd4hep::Parsers::RealGrammar
     *  @param result (output) map with string key and integer value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<std::string, double>& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<std::string , std::vector\<std::string\> \></c>  value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @see dd4hep::Parsers::VectorGrammar
     *  @param result (output) map with string value and
     *                 vector of strings as value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<std::string, std::vector<std::string> >& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<std::string , std::vector\<int\> \></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @see dd4hep::Parsers::VectorGrammar
     *  @see dd4hep::Parsers::IntGrammar
     *  @param result (output) map with string value and
     *                 vector of integers as value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<std::string, std::vector<int> >& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<std::string , std::vector\<double\> \></c> value
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @see dd4hep::Parsers::StringGrammar
     *  @see dd4hep::Parsers::VectorGrammar
     *  @see dd4hep::Parsers::RealGrammar
     *  @param result (output) map with string value and
     *                 vector of doubles as value
     *  @param input  (input) the string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    int parse(std::map<std::string, std::vector<double> >& result, const std::string& input);
    // ========================================================================
    /// parse the <c>std::map\<int,std::string\> \></c> objects
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @date 2007-12-06
     */
    int parse(std::map<int, std::string>& result, const std::string& input);

    // ========================================================================
    /// parse the <c>std::map\<unsigned int,std::string\> \></c> objects
    /**
     *  @see dd4hep::Parsers::MapGrammar
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @date 2007-12-06
     */
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    int parse(std::map<unsigned int, std::string>& result, const std::string& input);

    // ========================================================================
    /// parse the <c>std::map\<unsigned int,std::string\> \></c> objects
    /**
     *  @see dd4hep::Parsers::MapGrammar
     */
    int parse(std::map<std::string, unsigned int>& result, const std::string& input);
#endif

    // ========================================================================
    /// parse the pair expression (map-component)  " 'name' :value"
    /**
     *  @code
     *
     *  const std::string input = "'PackageName':dd4hepKernel" ;
     *  std::string name  ;
     *  std::string value ;
     *  int sc = dd4hep::Parsers::parse ( name , value , input ) ;
     *  if ( sc.isFailure() ) { ... }
     *  std::cout <<  "\tParsed name  is " << name
     *            <<  "\tParsed value is " << value << std::endl
     *  @endcode
     *
     *  @param  name  (output) the parsed name of the component, defined
     *                as 'name' or "name" before the column symbol ":",
     *                the leading and trailing blans are omitted
     *  @param  value (output) the parsed value of the component,
     *                defined as everything after the column symbol ":"
     *                till the end of the string
     *  @param  input (input) string to be parsed
     *  @return status code
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    int parse(std::string& name, std::string& value, const std::string& input);

    // ========================================================================
    /// helper function, needed for implementation of map of pairs
    /** It is very useful construction for monitoring to
     *  represent the value and error or the allowed range for
     *  some parameter
     *  @param params the map of pair
     *  @param input the string to be parsed
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @date 2009-05-19
     */
    int parse(std::map<std::string, std::pair<double, double> >& params, const std::string& input);

    // ========================================================================
    /// parser function for C-arrays
    /**  @param result C-array
     *   @param input the string to be parsed
     *   @return status code
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date 2009-09-15
     */
    template <class T, unsigned int N>
    int parse(T (&result)[N], const std::string& input) {
      typedef std::vector<T> _Vct;
      // create the temporary vector
      _Vct tmp;
      int sc = parse(tmp, input);
      if (!sc) {
        return sc;
      }   //  RETURN
      if (N != tmp.size()) {
        return 0;
      }   //  RETURN
      //
      std::copy(tmp.begin(), tmp.end(), result);
      //
      return 1;                            //  RETURN
    }
    // ========================================================================
    /// parser function for C-strings
    /**  @param result C-string
     *   @param input the string to be parsed
     *   @return status code
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date 2009-09-15
     */
    template <unsigned int N>
    int parse(char (&result)[N], const std::string& input) {
      // clear the string
      std::fill_n(result, N, ' ');
      // create the temporary string
      std::string tmp;
      int sc = parse(tmp, input);
      if (!sc) {
        return sc;
      }   //  RETURN
      if (N == tmp.size()) {
        std::copy(tmp.begin(), tmp.end(), result);
      }
      else if (N + 2 == tmp.size() && ('\'' == tmp[0] || '\"' == tmp[0]) && (tmp[0] == tmp[tmp.size() - 1])) {
        std::copy(tmp.begin() + 1, tmp.end() - 1, result);
      }
      else {
        return 0;
      }
      //
      return 1;                            //  RETURN
    }
  }                                        //  end of namespace Parsers
}                                          //  end of namespace dd4hep

#ifndef DD4HEP_PARSERS_NO_ROOT

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

namespace ROOT {
  namespace Math {
    /// Allow point insertion of a point in maps
    bool operator<(const XYZPoint& a, const XYZPoint& b);
    /// Allow 3-vector insertion of a  in maps
    bool operator<(const XYZVector& a, const XYZVector& b);
    /// Allow 4-vector insertion of a  in maps
    bool operator<(const PxPyPzEVector& a, const PxPyPzEVector& b);
  }
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {

    // ========================================================================
    /// parse 3D-point
    /**
     *  Valid representations of 3D-point:
     *
     *      - a'la python tuple with 3 elements ("canonical")
     *      - a'la python list with 3 elements
     *      - tuple or list with named ordered fields
     *
     *  @code
     *
     *    " (1,2,3) "
     *    " [1,2,3] "
     *    " [ x : 1, 2, Z:3 ] "
     *    " [ pX : 1 , PY : 2, 3] "
     *
     *  @endcode
     *
     *  Valid keys for names fields:
     *
     *  @code
     *
     *    "x", "X" , "pX" , "Px" , "PX "
     *    "y", "Y" , "pY" , "Py" , "PY "
     *    "z", "Z" , "pZ" , "Pz" , "PZ "
     *
     *  @endcode
     *
     *  @attention Named fields must be ordered <code>(x,y,z)</code>
     *
     *  @param result (output) the parsed point
     *  @param input  (input)  the input string
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    int parse(ROOT::Math::XYZPoint& result, const std::string& input);

    // ========================================================================
    /// parse 3D-vector
    /**
     *  Valid representations of 3D-vector:
     *
     *      - a'la python tuple with 3 elements ("canonical")
     *      - a'la python list with 3 elements
     *      - tuple or list with named ordered fields
     *
     *  @code
     *
     *    " (1,2,3) "
     *    " [1,2,3] "
     *    " [ x : 1, 2, Z:3 ] "
     *    " [ pX : 1 , PY : 2, 3] "
     *
     *  @endcode
     *
     *  Valid keys for names fields:
     *
     *  @code
     *
     *    "x", "X" , "pX" , "Px" , "PX "
     *    "y", "Y" , "pY" , "Py" , "PY "
     *    "z", "Z" , "pZ" , "Pz" , "PZ "
     *
     *  @endcode
     *
     *  @attention Named fields must be ordered <code>(x,y,z)</code>
     *
     *  @param result (output) the parsed vector
     *  @param input  (input)  the input string
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    int parse( ROOT::Math::XYZVector& result, const std::string& input);

    // ========================================================================
    /// parse PxPyPzEVector
    /**
     *  Valid representations of Lorenzt vector
     *
     *      - a'la python tuple with 4 elements ("canonical")
     *      - a'la python list with 4 elements
     *      - python/list with inner representation of 3D-point/vector
     *      - tuple or list with named ordered fields
     *
     *  @code
     *
     *    " (1,2,3,4) "
     *    " (1,2,3;4) "
     *
     *    " [1,2,3,4] "
     *    " [1,2,3;4] "
     *
     *    " [ x:1 ,2,3; e= 4] "
     *    " [ pX : 1 , PY : 2, 3 , T= 4] "
     *
     *    " [ ( pX : 1 , PY : 2, 3 ) , 4] "
     *    " [ ( pX : 1 , PY : 2, 3 ) ; 4] "
     *
     *    " [ 4 , ( pX : 1 , PY : 2, 3 ) ] "
     *    " [ 4 ; ( pX : 1 , PY : 2, 3 ) ] "
     *
     *    " [ [ pX : 1 , PY : 2, 3 ] , 4] "
     *    " [ [ pX : 1 , PY : 2, 3 ] ; 4] "
     *
     *    " [ 4 , [ pX : 1 , PY : 2, 3 ] ] "
     *    " [ 4 ; [ pX : 1 , PY : 2, 3 ] ] "
     *
     *    " ( ( pX : 1 , PY : 2, 3 ) , 4 )"
     *    " ( ( pX : 1 , PY : 2, 3 ) ; 4 )"
     *
     *    " ( 4 , ( pX : 1 , PY : 2, 3 ) )"
     *    " ( 4 ; ( pX : 1 , PY : 2, 3 ) )"
     *
     *    " ( [ pX : 1 , PY : 2, 3 ] , 4 )"
     *    " ( [ pX : 1 , PY : 2, 3 ] ; 4 )"
     *
     *    " ( 4 , [ pX : 1 , PY : 2, 3 ] )"
     *    " ( 4 ; [ pX : 1 , PY : 2, 3 ] )"
     *
     *
     *  @endcode
     *
     *  Note that "energy" element can be separated with semicolon.
     *
     *  Valid keys for names fields:
     *
     *  @code
     *
     *    "x", "X" , "pX" , "Px" , "PX "
     *    "y", "Y" , "pY" , "Py" , "PY "
     *    "z", "Z" , "pZ" , "Pz" , "PZ "
     *    "t", "T" , "e"  , "E"
     *
     *  @endcode
     *
     *  @attention Named fields must be ordered <code>(x,y,z)</code>
     *
     *  @param result (output) the parsed lorentz vector
     *  @param input  (input)  the input string
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    int parse(ROOT::Math::PxPyPzEVector& result, const std::string& input);
  }                                        //  end of namespace Parsers
}                                          //  end of namespace dd4hep


// ========================================================================
/// parse the vector of points
/**  @param result (OUTPUT) the parser vector
 *   @param input (INPUT) the string to be parsed
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-09-05
 */
PARSERS_DECL_FOR_LIST(ROOT::Math::XYZPoint)

// ========================================================================
/// parse the vector of vectors
/**  @param result (OUTPUT) the parser vector
 *   @param input (INPIUT) the string to be parsed
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-09-05
 */
PARSERS_DECL_FOR_LIST(ROOT::Math::XYZVector)

// ========================================================================
/// parse the vector of vectors
/**  @param result (OUTPUT) the parser vector
 *   @param input (INPUT) the string to be parsed
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-09-05
 */
PARSERS_DECL_FOR_LIST(ROOT::Math::PxPyPzEVector)

#endif
// ============================================================================
// The END
// ============================================================================
#endif // DD4HEP_PARSERS_H
// ============================================================================
