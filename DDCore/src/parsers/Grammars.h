// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#ifndef DD4HEPKERNEL_GRAMMARS_H
#define DD4HEPKERNEL_GRAMMARS_H 1
#ifdef __GNUC__
#warning                                                                \
  The headers Grammars.h and Parsers.icpp are deprecated                \
  and will be removed from the next release of Gaudi. You should migrate your \
  code the new pasers based on Boost.Spirit 2.
#endif
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <cctype>
// ============================================================================
// Boost.Spirit
// ============================================================================
#include <boost/version.hpp>
#if BOOST_VERSION >= 103800
// FIXME: Move to the new boost::spirit::classic namespace
#if !defined(BOOST_SPIRIT_USE_OLD_NAMESPACE)
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#endif
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/phoenix1.hpp>
#else
#include <boost/spirit.hpp>
#include <boost/spirit/phoenix.hpp>
#endif
#include <boost/bind.hpp>

// ============================================================================
/** @file
 *  Collection of grammars for property types
 *
 *  @see Dd4hep::Parsers::parse
 *  @see Property
 *
 *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-05-12
 */
// ============================================================================
namespace DD4hep
{
  namespace Parsers
  {
    // ========================================================================
    using namespace boost::spirit ;
    // ========================================================================
    using namespace phoenix ;
    // ========================================================================
    /** @struct ClosureGrammar
     *  Grammar or grammar rule which derive from this struct will have
     *  attribute of type <c>T</c> and name <c>val</c>
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template <typename T>
    struct ClosureGrammar : public boost::spirit::closure < ClosureGrammar<T>,T >
    {
      typedef  boost::spirit::closure<ClosureGrammar, T> closure;
      typename closure::member1 val;
    };
    // ========================================================================
    /** @struct AttributesClosureGrammar
     *
     *  Grammar or grammar rule which derive from this struct will have
     *  two attributes: type <c>T1</c> and name <c>val</c>, type <c>T2</c>
     *  and name <c>attrs</c>
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template <typename T1,typename T2>
    struct AttributesClosureGrammar
      : public boost::spirit::closure<AttributesClosureGrammar<T1,T2>,T1,T2>
    {
      typedef boost::spirit::closure<AttributesClosureGrammar, T1,T2> closure;
      typename closure::member1 val;
      typename closure::member2 attrs;
    };
    // ========================================================================
    /** @class BoolGrammar
     *
     *  The valid represenation of boolean values are:
     *
     *   - true  , True  , TRUE  or 1
     *   - false , False , FALSE or 0
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    class BoolGrammar : public grammar
    <
      BoolGrammar,
      ClosureGrammar<bool>::context_t
      >
    {
    public:
      typedef bool ResultT;
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( BoolGrammar const &self)
        {
          boolean_literal
            = true_literal[self.val = true] | false_literal[self.val = false];
          true_literal
            = str_p("true" ) | str_p("True" ) | str_p("TRUE" ) | str_p("1");
          false_literal
            = str_p("false") | str_p("False") | str_p("FALSE") | str_p("0");
        }
        rule<ScannerT> const& start() const
        { return boolean_literal;}
        rule<ScannerT> boolean_literal,true_literal,false_literal;
      };
    };
    // ========================================================================
    /** @class CharGrammar
     *
     *  The valid represenation of char values are:
     *
     *   - 'a', 'b','\''
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template<typename RT=char>
    class CharGrammar : public grammar
    <
      CharGrammar<RT> , typename ClosureGrammar<RT>::context_t
      >
    {
    public:
      typedef RT ResultT;
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( CharGrammar<RT> const &self)
        {
          char_literal
            = int_parser<RT>()[self.val=arg1]
            | ('\''
               >> ( str_p("\\'")[self.val='\'']
                    | (anychar_p[self.val=arg1]-'\'') )>>'\'');
        }
        rule<ScannerT> const& start() const
        { return char_literal; }
        rule<ScannerT> char_literal;
      };
    };
    // ========================================================================
    /** @class IntGrammar
     *
     *  The valid representation of integers values are:
     *
     *   - 1, 100, 123
     *
     *  @todo implement suffixes u U l L
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template<typename RT=int>
    class IntGrammar : public grammar
    <
      IntGrammar<RT>,
      typename ClosureGrammar<RT>::context_t
      >
    {
    public:
      typedef RT ResultT;
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( IntGrammar<RT> const &self)
        {
          int_literal = lexeme_d[int_parser<RT>()[self.val=arg1]
                                 >> !(ch_p('u') | ch_p('U') | ch_p('l') | ch_p('L'))];
        }
        rule<ScannerT> const& start() const { return int_literal; }
        rule<ScannerT> int_literal;
      };
    };
    // ========================================================================
    /** @class RealGrammar
     *
     *  The valid represenation of real values are:
     *
     *   - 1, 1.0 ,1.123, 1E+2, 0.5e-2
     *
     *  @todo implement suffixes f l F L
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template<typename RT=double>
    class RealGrammar : public grammar
    <
      RealGrammar<RT>,typename ClosureGrammar<RT>::context_t
      >
    {
    public:
      typedef RT ResultT;
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( RealGrammar const &self)
        {
          real_literal
            = lexeme_d[real_parser<RT,
                       real_parser_policies<RT> >()[self.val = arg1]
                       >> !(ch_p('f') | ch_p('F') | ch_p('l') | ch_p('L'))];
        }
        rule<ScannerT> const& start() const
        { return real_literal; }
        rule<ScannerT> real_literal;
      };
    };
    // ========================================================================
    /** @class StringGrammar
     *
     *  The valid represenation of string values are:
     *
     *   - "abc" , "\"abc\""
     *   - 'abs' , '\'abc\''
     *
     *  @todo implement not ASCII chars in strings
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */

    class StringGrammar : public grammar
    <
      StringGrammar, ClosureGrammar<std::string>::context_t
      >
    {
    public:
      typedef std::string ResultT;
      /** remove CR/LF symbols form the parsed strings
       *  @attention it is a bit dangerous operation
       *  The operation allows to write "very long" input strings
       *  for opts-files (it is actual e.g. for DataOnDemandSvc configuration)
       *  by splitting the strings into few lines
       *  All new-line symbols (as well as '\n', '\t', CR/LF etc
       *  are substituted by ordinary blanks.
       */
      void matchString() const
      {
        for ( std::string::iterator cur=this->val().begin();
              cur!=this->val().end();cur++)
        { if(std::isspace(*cur) ) { *cur = ' '; } }
      }
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( StringGrammar const &self )
        {
          string_literal = (lexeme_d
                            [
                             ('"' >> (*( str_p("\\\"")
                                         |
                                         (anychar_p-'"') ))
                              [self.val = construct_<std::string>
                               (arg1,arg2)] >>
                              '"')
                             |
                             ('\'' >> (*( str_p("\\'")
                                          |
                                          (anychar_p-'\'') ))
                              [self.val = construct_<std::string>
                               (arg1,arg2)]>>
                              '\'')])[boost::bind(&StringGrammar::matchString,&self)];
        }
        rule<ScannerT> const& start() const { return string_literal; }
        rule<ScannerT> string_literal;
      };
    };
    // ========================================================================
    /** @class SkipperGrammar
     *
     *  Skipping spaces and comments. Comments can be
     *
     *   - // ... - one line
     *   - \/\* ... \*\/ - multiline
     *
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    class SkipperGrammar : public grammar<SkipperGrammar>
    {
    public:
      /** Constructor
       *  @param skipnewline Skip new line symbols or not
       */
      SkipperGrammar ( const bool skipnewline = true )
        : m_skipnewline(skipnewline){}
    public:
      /// @return true - skip new line symbols, false - not skip
      bool skipnewline() const{return m_skipnewline;}
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( SkipperGrammar const& self)
        {
          if ( self.skipnewline() )
          {
            skip
              =   space_p
              |   comment_p("//")     // C++ comment
              |   comment_p("/*", "*/")     // C comment
              ;
          }
          else
          {
            skip
              =   (space_p-eol_p)
              |   comment_p("//")     // C++ comment
              |   comment_p("/*", "*/")     // C comment
              ;
          }
        }
        rule<ScannerT>  skip;
        rule<ScannerT> const& start() const { return skip; }
      };
    private:
      bool m_skipnewline;
    };
    // ========================================================================
    /** @class PairGrammar
     *
     *  The valid represenation of pairs are:
     *  ("abc",123) or ("abc","def")
     *  Inner types of pair depends on KeyGrammarT and ValueGrammarT grammars
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template <typename KeyGrammarT, typename ValueGrammarT>
    class PairGrammar : public grammar
    <
      PairGrammar<KeyGrammarT,ValueGrammarT>,
      typename ClosureGrammar<
        std::pair<typename KeyGrammarT::ResultT,
                  typename ValueGrammarT::ResultT> >::context_t
      >
    {
    public:
      typedef typename KeyGrammarT::ResultT KeyT;
      typedef typename ValueGrammarT::ResultT ValueT;
      typedef std::pair<KeyT,ValueT> ResultT;
    public:
      /** Constructor
       *  @param delim Delimiter for pair values
       */
      PairGrammar ( const std::string&  delim = "," )
        : m_delim(delim) {}
    public:
      /// callback. Action when we match first value
      void matchFirst  ( const KeyT&   first  ) const { this->val().first = first; }
      /// callback. Action when we match second value
      void matchSecond ( const ValueT& second ) const { this->val().second = second; }
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( PairGrammar const &self)
        {
          para
            = (
               str_p("(")
               >> (grkey[boost::bind(&PairGrammar::matchFirst,&self,_1)])
               >> self.delim().c_str()
               >> (grvalue[boost::bind(&PairGrammar::matchSecond,&self,_1)])
               >> str_p(")")
               ) ;
        }
        rule<ScannerT> const& start() const { return para; }
        rule<ScannerT> para;
        KeyGrammarT grkey;
        ValueGrammarT grvalue;
      };
    public:
      /// @return Delimiter for pair values
      const std::string& delim() const { return m_delim ; }
      /** Set delimiters for pair values
       *  @param delim Delimiter
       */
      void setDelim ( const std::string& delim ) { m_delim = delim;}
    private:
      std::string m_delim;
    };
    // ========================================================================
    /** @class VectorGrammar
     *
     *  The valid represenation of vector are:
     *   - {"abc","defj","i"} or {1,2,3,4,5}
     *   - ["abc","defj","i"] or [1,2,3,4,5]
     *  Inner type depends on GrammarT grammar
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template <typename GrammarT>
    class VectorGrammar : public grammar
    <
      VectorGrammar<GrammarT> ,
      typename ClosureGrammar<std::vector<typename GrammarT::ResultT> >::context_t
      >
    {
    public:
      typedef typename GrammarT::ResultT ValueT;
      typedef std::vector<ValueT> ResultT;
      typedef VectorGrammar<GrammarT> SelfT;
    public:
      /// callback. Action when we match inner value
      void matchItem(const ValueT& value) const { this->val().push_back(value); }
    public:
      template <typename ScannerT>
      struct definition
      {
        definition(SelfT const &self)
        {
          inner =
            !(gr[boost::bind(&VectorGrammar::matchItem,&self,_1)]
              >> *(','>>gr[boost::bind(&VectorGrammar::matchItem,&self,_1)]));
          vec =
            '[' >> inner >> ']' |  // a'la python list
            '(' >> inner >> ')' |  // a'la python tuple
            '{' >> inner >> '}' ;  // like obsolete list from opts-grammar
        }
        rule<ScannerT> const& start() const { return vec; }
        rule<ScannerT> vec,inner;
        GrammarT gr;
      };
    };
    // ========================================================================
    /** @class MapGrammar
     *
     *  The valid represenation of map are:
     *   - {"file1":"path1","something":"nothing"}
     *   - {"file1"="path1","something"="nothing"}
     *   - ["file1":10,"something":20]
     *   - ["file1"=30,"something"=40]
     *  Inner key type depends on KeyGrammarT grammar
     *  Inner value type depends on ValueGrammarT grammar
     *
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-14
     */
    template <typename KeyGrammarT, typename ValueGrammarT>
    class MapGrammar : public grammar
    <
      MapGrammar<KeyGrammarT,ValueGrammarT>,
      typename AttributesClosureGrammar
      < std::map<typename KeyGrammarT::ResultT,
                 typename ValueGrammarT::ResultT>,
        std::pair<typename KeyGrammarT::ResultT,
                  typename ValueGrammarT::ResultT> >::context_t
      >
    {
    public:
      typedef typename KeyGrammarT::ResultT KeyT;
      typedef typename ValueGrammarT::ResultT ValueT;
      typedef std::map<KeyT,ValueT> ResultT;
    public:
      /// call backs. Action when we match pair in map
      void matchItem  () const
      {
        //this->val().insert(this->attrs());
        this->val()[this->attrs().first] = this->attrs().second ;
      }
      /// call backs. Action when we match key of pair
      void matchFirst ( const KeyT&   value ) const {  this->attrs().first = value ; }
      /// call backs. Action when we match value pf pair
      void matchSecond( const ValueT& value ) const { this->attrs().second = value ; }
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( MapGrammar const &self)
        {
          vec
            = ('{'>> inner_list >> '}') | ('['>>inner_list>>']');
          inner_list
            =
            !( inner[boost::bind(&MapGrammar::matchItem,&self)]
               >> *( ch_p(',') >>
                     inner[boost::bind(&MapGrammar::matchItem,&self)] )
               );
          inner
            =
            grKey[boost ::bind(&MapGrammar::matchFirst,&self,_1)]
            >> ( ch_p('=') | ch_p(':'))
            >> grValue[boost::bind(&MapGrammar::matchSecond,&self,_1)] ;
        }
        KeyGrammarT grKey;
        ValueGrammarT grValue;
        rule<ScannerT> const& start() const { return vec; }
        rule<ScannerT> vec,inner, inner_list ;
      };
    };
    // ========================================================================
  } // end of namespace Dd4hep::Parsers
} // end of namespace Dd4hep
// ============================================================================
// The END
// ============================================================================
#endif // DD4HEPKERNEL_GRAMMARS_H
// ============================================================================
