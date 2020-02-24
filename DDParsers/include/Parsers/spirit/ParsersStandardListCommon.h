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
#ifndef PARSERS_STANDARD_LIST_COMMON_H
#define PARSERS_STANDARD_LIST_COMMON_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Parsers/spirit/ParsersFactory.h"

#ifndef DD4HEP_PARSERS_NO_ROOT
// ============================================================================
namespace dd4hep {
  namespace Parsers {
    // ========================================================================
    template<typename T1, typename T2> inline int
    parse_(ROOT::Math::PositionVector3D<T1,T2>& result, const std::string& input){
      Skipper skipper;
      typename Grammar_<IteratorT,ROOT::Math::PositionVector3D<T1,T2>,Skipper>::Grammar g;
      IteratorT iter = input.begin(), end = input.end();
      if (qi::phrase_parse( iter, end, g, skipper, result)){
        return 1;
      }
      return 0;
    }
  }
}
#endif

#define IMPLEMENT_STL_PARSER(x,y)                                       \
  template <> int parse(x < y > & result, const std::string&  input)    \
  { result.clear();  return parse_(result, input);         }            \
  template <> std::ostream& toStream(const x < y >& o,std::ostream& s) \
  { return toStream_(o, s);                                }

#define IMPLEMENT_STL_MAP_PARSER(x,k,v)                                 \
  template <> int parse(x < k, v > & result, const std::string& input)  \
  { result.clear();  return parse_(result, input);         }            \
  template <> std::ostream& toStream(const x <k,v>& o,std::ostream& s)                 \
  { return toStream_(o, s);                                }

#define IMPLEMENT_MAPPED_PARSERS(type,X)                                \
  template <> int parse(std::type <int,X>& result,const std::string& input) \
  { return parse_(result, input);                          }            \
  template <> std::ostream& toStream(const std::type <int,X>& o,std::ostream& s) \
  { return toStream_(o, s);                                }            \
  template <> int parse(std::type <unsigned long,X>& result,const std::string& input) \
  { return parse_(result, input);                          }            \
  template <> std::ostream& toStream(const std::type <unsigned long,X>& o,std::ostream& s) \
  { return toStream_(o, s);                                }            \
  template <> int parse(std::type <std::string,X>& result,const std::string& input) \
  { return parse_(result, input);                          }            \
  template <> std::ostream& toStream(const std::type <std::string,X>& o,std::ostream& s) \
  { return toStream_(o, s);                                }

#if defined(DD4HEP_HAVE_ALL_PARSERS)

#define IMPLEMENT_STL_PARSERS(x)                           \
  IMPLEMENT_STL_PARSER(std::vector,x)                      \
  IMPLEMENT_STL_PARSER(std::list,x)                        \
  IMPLEMENT_STL_PARSER(std::set,x)                         \
  IMPLEMENT_STL_PARSER(std::deque,x)                       \
  IMPLEMENT_STL_MAP_PARSER(std::map,int,x)                 \
  IMPLEMENT_STL_MAP_PARSER(std::map,unsigned long,x)       \
  IMPLEMENT_STL_MAP_PARSER(std::map,std::string,x)

#else   //  DD4HEP_HAVE_ALL_PARSERS

#define IMPLEMENT_STL_PARSERS(x)                           \
  IMPLEMENT_STL_PARSER(std::vector,x)                      \
  IMPLEMENT_STL_PARSER(std::list,x)                        \
  IMPLEMENT_STL_PARSER(std::set,x)                         \
  IMPLEMENT_STL_MAP_PARSER(std::map,int,x)                 \
  IMPLEMENT_STL_MAP_PARSER(std::map,std::string,x)

#endif  //  DD4HEP_HAVE_ALL_PARSERS

// ============================================================================
#define PARSERS_DEF_FOR_LIST(InnerType)                    \
  namespace DD4hep{ namespace Parsers{                     \
      IMPLEMENT_STL_PARSERS(InnerType)                     \
      IMPLEMENT_MAPPED_PARSERS(pair,InnerType)            }}

// ============================================================================
#endif /* PARSERS_STANDARD_LIST_COMMON_H */
