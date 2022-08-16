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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the BasicGrammar include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_GRAMMARPARSED_H
#define DD4HEP_GRAMMARPARSED_H

#if defined(DD4HEP_GRAMMARUNPARSED_H)
#error "The header files GrammarParsed.h and GrammarUnparsed.h may not be included in the same compilation unit!"
#endif

/// Framework include files
#include <DD4hep/Grammar.h>
#include <DD4hep/Printout.h>
#include <Parsers/Parsers.h>
#include <Evaluator/Evaluator.h>

/// C/C++ include files
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

// Forward declarations

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Alignments internal namespace declaration
  namespace detail  {

    std::string grammar_pre_parse_map(const std::string& in);
    std::string grammar_pre_parse_cont(const std::string& in);
    std::string grammar_pre_parse_obj(const std::string& in);
    std::pair<int,double> grammar_evaluate_item(std::string val);

    /// PropertyGrammar overload: Retrieve value from string
    template <typename TYPE> bool grammar_fromString(const BasicGrammar& gr, void* ptr, const std::string& string_val)   {
      int sc = 0;
      TYPE temp;
      try   {
	sc = ::dd4hep::Parsers::parse(temp,string_val);
      }
      catch (...)  {
      }
      if ( !sc ) sc = gr.evaluate(&temp,string_val);
#if 0
      std::cout << "Sc=" << sc << "  Converting value: " << string_val 
		<< " to type " << typeid(TYPE).name() 
		<< std::endl;
#endif
      if ( sc )   {
	*(TYPE*)ptr = temp;
	return true;
      }
      BasicGrammar::invalidConversion(string_val, typeid(TYPE));
      return false;
    }

    /// Serialize a property to a string
    template <typename TYPE> std::string grammar_str(const BasicGrammar&, const void* ptr)  {
      std::stringstream string_rep;
      Parsers::toStream(*(TYPE*)ptr,string_rep);
      return string_rep.str();
    }

    /// Item evaluator
    template <typename T> inline int eval_item(T* ptr, std::string val)  {
      auto result = grammar_evaluate_item(val);
      if (result.first != tools::Evaluator::OK) {
	return 0;
      }
      *ptr = (T)result.second;
      return 1;
    }

    /// String evaluator
    template <> inline int eval_item<std::string>(std::string* ptr, std::string val)  {
      *ptr = val;
      return 1;
    }

    /// Insertion function for std vectors
    template <typename TYPE> static int fill_data(std::vector<TYPE>* p,const std::vector<std::string>& temp)  {
      TYPE val;
      const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
      for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
	if ( !grammar.fromString(&val,*i) )
	  return 0;
	p->emplace_back(val);
      }
      return 1;
    }

    /// Insertion function for std lists
    template <typename TYPE> static int fill_data(std::list<TYPE>* p,const std::vector<std::string>& temp)  {
      TYPE val;
      const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
      for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
	if ( !grammar.fromString(&val,*i) )
	  return 0;
	p->emplace_back(val);
      }
      return 1;
    }

    /// Insertion function for std sets
    template <typename TYPE> static int fill_data(std::set<TYPE>* p,const std::vector<std::string>& temp)  {
      TYPE val;
      const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
      for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
	if ( !grammar.fromString(&val,*i) )
	  return 0;
	p->emplace(val);
      }
      return 1;
    }

    /// Insertion function for std deque
    template <typename TYPE> static int fill_data(std::deque<TYPE>* p,const std::vector<std::string>& temp)  {
      TYPE val;
      const BasicGrammar& grammar = BasicGrammar::instance<TYPE>();
      for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
	if ( !grammar.fromString(&val,*i) )
	  return 0;
	p->emplace_back(val);
      }
      return 1;
    }

    /// Insertion function for std maps
    template <typename KEY, typename TYPE> static int fill_data(std::map<KEY,TYPE>* p,const std::vector<std::string>& temp)  {
      std::pair<KEY,TYPE> val;
      const BasicGrammar& grammar = BasicGrammar::instance<std::pair<KEY,TYPE> >();
      for(auto i=std::begin(temp); i != std::end(temp); ++i)  {
	if ( !grammar.fromString(&val,*i) )
	  return 0;
	p->emplace(val);
      }
      return 1;
    }

    /// Container evaluator
    template <typename TYPE> static int eval_map(TYPE* p, const std::string& str)  {
      std::vector<std::string> buff;
      p->clear();
      int sc = Parsers::parse(buff,str);
      if ( sc )  {
	return fill_data(p,buff);
      }
      else   {
	TYPE temp;
	std::map<std::string,std::string> map_buff;
	// If we get called from python, the values and keys are already in string form
	sc = 0;
	try  {
	  sc = ::dd4hep::Parsers::parse(map_buff,str);
	}
	catch(...)    {
	}
	if ( !sc )  {
	  // Otherwise stringyfy the values
	  std::string temp_str = grammar_pre_parse_map(str);
	  try  {
	    sc = ::dd4hep::Parsers::parse(map_buff,temp_str);
	  }
	  catch(...)    {
	  }
	}
	if ( sc )   {
	  for(const auto& _o : map_buff )    {
	    typename TYPE::key_type _k;
	    typename TYPE::mapped_type _v;
	    eval_item(&_k, _o.first);
	    eval_item(&_v, _o.second);
	    p->emplace(_k,_v);
	  }
	  return 1;
	}
      }
      return 0;
    }

    /// Container evaluator
    template <typename TYPE> static int eval_container(TYPE* p, const std::string& str)  {
      std::vector<std::string> buff;
      int sc = Parsers::parse(buff,str);
      if ( sc )  {
	return fill_data(p,buff);
      }
      else   {
	TYPE temp;
	/// First try with the simple object string transformation
	std::string temp_str = grammar_pre_parse_obj(str);
	sc = ::dd4hep::Parsers::parse(temp,temp_str);
	if ( sc )   {
	  *p = temp;
	  return 1;
	}
	/// Now the more complicated one:
	temp_str = grammar_pre_parse_cont(str);
	sc = ::dd4hep::Parsers::parse(temp,temp_str);
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
      return 0;
    }

    /// Item evaluator
    template <typename T,typename Q> inline int eval_pair(std::pair<T,Q>* ptr, std::string str)  {
      const BasicGrammar& grammar = BasicGrammar::instance<std::pair<T,Q> >();
      if ( !grammar.fromString(ptr,str) ) return 0;
      return 1;
    }

    /// Object evaluator
    template<typename T> inline int eval_obj(T* ptr, const std::string& str)  {
      return BasicGrammar::instance<T>().fromString(ptr,grammar_pre_parse_obj(str));
    }

    template<typename T> inline int grammar_eval(const BasicGrammar&, void*, const std::string&) { return 0; }
  }
  
  /// Standarsd constructor
  template <typename TYPE> const BasicGrammar& BasicGrammar::instance()  {
    static Grammar<TYPE> *s_gr = 0;
    if ( 0 != s_gr )  {
      return *s_gr;
    }
    static Grammar<TYPE> gr;
    if ( 0 == gr.specialization.bind       ) gr.specialization.bind       = detail::constructObject<TYPE>;
    if ( 0 == gr.specialization.copy       ) gr.specialization.copy       = detail::copyObject<TYPE>;
    if ( 0 == gr.specialization.fromString )  {
      gr.specialization.fromString = detail::grammar_fromString<TYPE>;
      gr.specialization.eval       = detail::grammar_eval<TYPE>;
      gr.specialization.str        = detail::grammar_str<TYPE>;
    }
    s_gr = &gr;
    return *s_gr;
  }
}      // End namespace dd4hep

#define DD4HEP_PARSER_GRAMMAR_CNAME(serial,name)  namespace_dd4hep__grammar_##serial##_##name

#define DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(xx,func)			\
  namespace dd4hep { namespace detail {					\
      template<> int grammar_eval<xx>(const BasicGrammar&, void* _p, const std::string& _v) { return func ((xx*)_p,_v); }}}

#define DD4HEP_DEFINE_PARSER_GRAMMAR_INSTANCE(serial,xx)   namespace dd4hep { template class Grammar< xx >; } \
  namespace DD4HEP_PARSER_GRAMMAR_CNAME(serial,0) { static auto s_reg = ::dd4hep::GrammarRegistry::pre_note< xx >(); }

#define DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,ctxt,xx,func)	\
  DD4HEP_DEFINE_PARSER_GRAMMAR_EVAL(xx,func)				\
  DD4HEP_DEFINE_PARSER_GRAMMAR_INSTANCE(DD4HEP_PARSER_GRAMMAR_CNAME(serial,ctxt),xx)

#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,xx,eval_func)	\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,1,x,eval_func)             \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,2,std::vector<xx>, eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,3,std::list<xx>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,4,std::set<xx>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,5,std::deque<xx>,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,6,dd4hep::detail::Primitive<xx>::int_map_t,      eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,7,dd4hep::detail::Primitive<xx>::ulong_map_t,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,8,dd4hep::detail::Primitive<xx>::string_map_t,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,9,dd4hep::detail::Primitive<xx>::int_pair_t,     eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,10,dd4hep::detail::Primitive<xx>::ulong_pair_t,  eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,11,dd4hep::detail::Primitive<xx>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(serial,xx,eval_func) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,12,xx,eval_func)		\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,13,std::vector<xx>,eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,14,std::list<xx>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(serial,xx)		\
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,xx,eval_item)		\
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,unsigned xx,eval_item)

#else

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,xx,eval_func)	\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,1,xx,eval_func)		\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,2,std::vector<xx>, eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,3,std::list<xx>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,4,std::set<xx>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,5,dd4hep::detail::Primitive<xx>::int_map_t,     eval_map) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,6,dd4hep::detail::Primitive<xx>::string_map_t,  eval_map) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,7,dd4hep::detail::Primitive<xx>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,8,dd4hep::detail::Primitive<xx>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_ROOTMATH(serial,xx,eval_func)	\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,1,xx,eval_func)		\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,2,std::vector<xx>, eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,3,std::list<xx>,   eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,4,std::set<xx>,    eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,5,dd4hep::detail::Primitive<xx>::int_map_t,     eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,6,dd4hep::detail::Primitive<xx>::string_map_t,  eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,7,dd4hep::detail::Primitive<xx>::int_pair_t,    eval_pair) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,8,dd4hep::detail::Primitive<xx>::string_pair_t, eval_pair) 

#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(serial,xx,eval_func) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,9,xx,eval_func)		\
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,10,std::vector<xx>,eval_container) \
  DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(serial,11,std::list<xx>,eval_container)   

#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(serial,xx)	\
  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(serial,xx,eval_item)

#endif

#define DD4HEP_DEFINE_PARSER_GRAMMAR(xx,func)               DD4HEP_DEFINE_PARSER_GRAMMAR_SERIAL(__LINE__,__LINE__,xx,func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(xx,eval_func)     DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_SERIAL(__LINE__,xx,eval_func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_ROOTMATH(xx,eval_func) DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_ROOTMATH(__LINE__,xx,eval_func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(xx)             DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT_SERIAL(__LINE__,xx)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL(xx,eval_func)  DD4HEP_DEFINE_PARSER_GRAMMAR_CONT_VL_SERIAL(__LINE__,xx,eval_func)
#define DD4HEP_DEFINE_PARSER_GRAMMAR_DUMMY(xx,func)         DD4HEP_DEFINE_PARSER_GRAMMAR_DUMMY_SERIAL(__LINE__,xx,func)

#endif // DD4HEP_GRAMMARPARSED_H
