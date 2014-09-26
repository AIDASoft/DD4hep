// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GRAMMAR_INL_H
#define DD4HEP_DDG4_GRAMMAR_INL_H

// Framework include files
#include "DD4hep/BasicGrammar.h"

#ifdef DD4HEP_USE_BOOST
#include "DD4hep/Parsers.h"
#include "DD4hep/ToStream.h"
#endif
#include "XML/Evaluator.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

// C/C++ include files
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

namespace DD4hep { XmlTools::Evaluator& g4Evaluator();  }
namespace {  static XmlTools::Evaluator& s__eval(DD4hep::g4Evaluator());  }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Concrete type dependent grammar definition
  /**
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  template <typename TYPE> class Grammar : public BasicGrammar {
  public:
    /// Standarsd constructor
    Grammar();
    /// Default destructor
    virtual ~Grammar();
    /// PropertyGrammar overload: Access to the type information
    virtual const std::type_info& type() const;
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const;
    /// PropertyGrammar overload: Serialize a property to a string
    virtual std::string str(const void* ptr) const;
    /// PropertyGrammar overload: Retrieve value from string
    virtual bool fromString(void* ptr, const std::string& value) const;
    /// Evaluate string value if possible before calling boost::spirit
    virtual int evaluate(void* ptr, const std::string& value) const;
  };

  /// Standarsd constructor
  template <typename TYPE> Grammar<TYPE>::Grammar() {
  }

  /// Default destructor
  template <typename TYPE> Grammar<TYPE>::~Grammar() {
  }

  /// PropertyGrammar overload: Access to the type information
  template <typename TYPE> const std::type_info& Grammar<TYPE>::type() const {
    return typeid(TYPE);
  }

  /// Access the object size (sizeof operator)
  template <typename TYPE> size_t Grammar<TYPE>::sizeOf() const   {
    return sizeof(TYPE);
  }

  /// Evaluate string value if possible before calling boost::spirit
  template <typename TYPE> int Grammar<TYPE>::evaluate(void*, const std::string&) const {
    return 0;
  }

  /// PropertyGrammar overload: Retrieve value from string
  template <typename TYPE> bool Grammar<TYPE>::fromString(void* ptr, const std::string& str) const {
    int sc = 0;
    TYPE temp;
#ifdef DD4HEP_USE_BOOST
    sc = Parsers::parse(temp,str);
#endif
    if ( !sc ) sc = evaluate(&temp,str);
#if 0
    std::cout << "Sc=" << sc << "  Converting value: " << str 
	      << " to type " << typeid(TYPE).name() 
	      << std::endl;
#endif
    if ( sc )   {
      *(TYPE*)ptr = temp;
      return true;
    }
#ifndef DD4HEP_USE_BOOST
    throw std::runtime_error("This version of DD4HEP is not compiled to use boost::spirit.\n"
			     "To enable elaborated property handling set DD4HEP_USE_BOOST=ON\n"
			     "and BOOST_INCLUDE_DIR=<boost include path>");
#else
    BasicGrammar::invalidConversion(str, typeid(TYPE));
    return false;
#endif
  }

  /// Serialize a property to a string
  template <typename TYPE> std::string Grammar<TYPE>::str(const void* ptr) const {
#ifdef DD4HEP_USE_BOOST
    std::stringstream string_rep;
    Utils::toStream(*(TYPE*)ptr,string_rep);
    return string_rep.str();
#else
    if (ptr) {
    }
    throw std::runtime_error("This version of DD4HEP is not compiled to use boost::spirit.\n"
			     "To enable elaborated property handling set DD4HEP_USE_BOOST=ON\n"
			     "and BOOST_INCLUDE_DIR=<boost include path>");
#endif
  }

  /// Helper function to parse data type
  static std::string pre_parse_obj(const std::string& in)   {
    std::string res = "";
    res.reserve(1024);
    for(const char* c = in.c_str(); *c; ++c)   {
      switch(*c)  {
      case '\'':
	return "Bad object representation";
      case ',':
	res += "','";
	break;
      case '(':
      case '[':
	res += "['";
	break;
      case ')':
      case ']':
	res += "']";
	break;
      default:
	res += *c;
	break;
      }
    }
    //cout << "Pre-parsed:" << res << endl;
    return res;
  }

  /// Insertion function for std vectors
  template <typename TYPE> static int fill_data(std::vector<TYPE>* p,const std::vector<std::string>& temp)  {
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    TYPE val;
    for(std::vector<std::string>::const_iterator i=temp.begin(); i != temp.end(); ++i)  {
      if ( !g.fromString(&val,*i) )
	return 0;
      p->push_back(val);
    }
    return 1;
  }

  /// Insertion function for std lists
  template <typename TYPE> static int fill_data(std::list<TYPE>* p,const std::vector<std::string>& temp)  {
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    TYPE val;
    for(std::vector<std::string>::const_iterator i=temp.begin(); i != temp.end(); ++i)  {
      if ( !g.fromString(&val,*i) )
	return 0;
      p->push_back(val);
    }
    return 1;
  }

  /// Insertion function for std sets
  template <typename TYPE> static int fill_data(std::set<TYPE>* p,const std::vector<std::string>& temp)  {
    const BasicGrammar& g = BasicGrammar::instance<TYPE>();
    TYPE val;
    for(std::vector<std::string>::const_iterator i=temp.begin(); i != temp.end(); ++i)  {
      if ( !g.fromString(&val,*i) )
	return 0;
      p->insert(val);
    }
    return 1;
  }

  /// Container evaluator
  template <typename TYPE> static int eval_container(TYPE* p, const std::string& str)  {
#ifdef DD4HEP_USE_BOOST
    std::vector<std::string> buff;
    int sc = Parsers::parse(buff,str);
    if ( sc )  {
      return fill_data(p,buff);
    }
    else   {
      TYPE temp;
      std::string temp_str = pre_parse_obj(str);
      sc = Parsers::parse(temp,temp_str);
      if ( sc )   {
	*p = temp;
	return 1;
      }
      buff.clear();
      sc = Parsers::parse(buff,temp_str);
      if ( sc )  {
	return fill_data(p,buff);
      }
    }
#else
    if ( p && str.empty() ) return 0;
#endif
    return 0;
  }

  /// Item evaluator
  template <typename T> static int eval_item(T* p, std::string s)  {
    size_t idx = s.find("(int)");
    if (idx != std::string::npos)
      s.erase(idx, 5);
    while (s[0] == ' ')
      s.erase(0, 1);
    double result = s__eval.evaluate(s.c_str());
    if (s__eval.status() != XmlTools::Evaluator::OK) {
      return 0;
    }
    *p = (T)result;
    return 1;
  }
  
  /// Object evaluator
  template<typename T> static int eval_obj(T* p, const std::string& str)  {
    return BasicGrammar::instance<T>().fromString(p,pre_parse_obj(str));
  }
  
  // Containers of objects are not handled!
  
}      // End namespace DD4hep

#define DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)  namespace DD4hep { \
  template<> const BasicGrammar& BasicGrammar::instance<x>()  { static Grammar<x> s; return s;}}

#define DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)  namespace DD4hep {  \
  template<> int Grammar<x >::evaluate(void* p, const std::string& v) const { return func ((x*)p,v); }}

#define DD4HEP_DEFINE_PARSER_GRAMMAR(x,func)                   \
  DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(x)                         \
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(x,func)

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_func)         \
  DD4HEP_DEFINE_PARSER_GRAMMAR(x,eval_func)                    \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::vector<x>,eval_container)  \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::list<x>,eval_container)    \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::set<x>,eval_container)

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(x,eval_func)      \
  DD4HEP_DEFINE_PARSER_GRAMMAR(x,eval_func)                    \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::vector<x>,eval_container)  \
  DD4HEP_DEFINE_PARSER_GRAMMAR(std::list<x>,eval_container)

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(x)                 \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(x,eval_item)               \
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(unsigned x,eval_item)

#endif  /* DD4HEP_DDG4_GRAMMAR_INL_H */
