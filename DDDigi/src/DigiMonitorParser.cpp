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

/// Framework include files
#include <Parsers/spirit/ParsersFactory.h>
#include <DDDigi/DigiMonitorOptions.h>

// ============================================================================
namespace dd4hep   {
  // ==========================================================================
  namespace Parsers  {

    // ========================================================================
    // 1D histogram parameters
    // ========================================================================
    std::ostream& toStream(const dd4hep::digi::H1DParams& o, std::ostream& s)   {
      return s << "( "
               << '"' << o.name  << '"' << " , "
               << '"' << o.title << '"' << " , "
               << o.nbin_x << " , "
               << o.min_x  << " , "
               << o.max_x
               << " )" ;
    }

    // ========================================================================
    template< typename Iterator, typename Skipper>
    class H1DParamsGrammar : public qi::grammar<Iterator, dd4hep::digi::H1DParams(), Skipper>
    {
    public:
      // ======================================================================
      typedef dd4hep::digi::H1DParams ResultT;
      // ======================================================================
    public:
      struct tag_name{};
      struct tag_title{};
      struct tag_nbin_x{};
      struct tag_min_x{};
      struct tag_max_x{};
      struct Operations {
        // Some magic:
        template <typename A, typename B = boost::fusion::unused_type,
	          typename C = boost::fusion::unused_type,
	          typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // Actions:
        // --------------------------------------------------------------------
        void operator()(dd4hep::digi::H1DParams& val, const int bin, tag_nbin_x) const {
          val.nbin_x = bin;
        }

        void operator()(dd4hep::digi::H1DParams& val, const float x, tag_min_x) const {
          val.min_x = x;
        }

        void operator()(dd4hep::digi::H1DParams& val, const float x, tag_max_x) const {
          val.max_x = x;
        }

        void operator()(dd4hep::digi::H1DParams& val, const std::string& nam, tag_name) const {
          val.name = nam;
        }

        void operator()(dd4hep::digi::H1DParams& val, const std::string& tit, tag_title) const {
          val.title = tit;
        }

      };
    public:
      H1DParamsGrammar() : H1DParamsGrammar::base_type(para)      {
        para = qi::lit('(')
          >> name[op(qi::_val, qi::_1, tag_name())]
          >> ','
          >> title[op(qi::_val, qi::_1, tag_title())]
          >> ','
          >> qi::int_[op(qi::_val, qi::_1, tag_nbin_x())]
          >> ','
          >> qi::double_[op(qi::_val, qi::_1, tag_min_x())]
          >> ','
          >> qi::double_[op(qi::_val, qi::_1, tag_max_x())]
          >> ')';
      }
      qi::rule<Iterator, dd4hep::digi::H1DParams(), Skipper> para;
      StringGrammar<Iterator, Skipper> name;
      StringGrammar<Iterator, Skipper> title;
      ph::function<Operations> op;
      // ======================================================================
    };
    REGISTER_GRAMMAR(dd4hep::digi::H1DParams, H1DParamsGrammar);
    // ========================================================================
    int parse(dd4hep::digi::H1DParams& result, const std::string& input)   {
      return parse_(result, input);
    }

    // ========================================================================
    // 2D histogram parameters
    // ========================================================================
    std::ostream& toStream(const dd4hep::digi::H2DParams& o, std::ostream& s)   {
      return s << "[ "
               << '"' << o.name << "\" , "
               << '"' << o.title << "\" , "
               << o.nbin_x << " , "
               << o.min_x  << " , "
               << o.max_x  << " , "
               << o.nbin_y << " , "
               << o.min_y  << " , "
               << o.max_y
               << " ]" ;
    }
    // ========================================================================
    template< typename Iterator, typename Skipper>
    class H2DParamsGrammar : public qi::grammar<Iterator, dd4hep::digi::H2DParams(), Skipper>
    {
    public:
      // ======================================================================
      typedef dd4hep::digi::H2DParams ResultT;
      // ======================================================================
    public:
      struct tag_name{};
      struct tag_title{};
      struct tag_nbin_x{};
      struct tag_min_x{};
      struct tag_max_x{};
      struct tag_nbin_y{};
      struct tag_min_y{};
      struct tag_max_y{};
      struct Operations {
        // Some magic:
        template <typename A, typename B = boost::fusion::unused_type,
	          typename C = boost::fusion::unused_type,
	          typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // Actions:
        // --------------------------------------------------------------------
        void operator()(dd4hep::digi::H2DParams& val, const int bin, tag_nbin_x) const {
          val.nbin_x = bin;
        }
        void operator()(dd4hep::digi::H2DParams& val, const float x, tag_min_x) const {
          val.min_x = x;
        }
        void operator()(dd4hep::digi::H2DParams& val, const float x, tag_max_x) const {
          val.max_x = x;
        }
        void operator()(dd4hep::digi::H2DParams& val, const int bin, tag_nbin_y) const {
          val.nbin_y = bin;
        }
        void operator()(dd4hep::digi::H2DParams& val, const float y, tag_min_y) const {
          val.min_y = y;
        }
        void operator()(dd4hep::digi::H2DParams& val, const float y, tag_max_y) const {
          val.max_y = y;
        }
        void operator()(dd4hep::digi::H2DParams& val, const std::string& name, tag_name) const {
          val.name = name;
        }
        void operator()(dd4hep::digi::H2DParams& val, const std::string& title, tag_title) const {
          val.title = title;
        }

      };
    public:
      H2DParamsGrammar() : H2DParamsGrammar::base_type(para)      {
	para = qi::lit('[')
          >> name[op(qi::_val, qi::_1, tag_name())]
          >> ','
          >> title[op(qi::_val, qi::_1, tag_title())]
          >> ','
          >> qi::int_[op(qi::_val, qi::_1, tag_nbin_x())]
          >> ','
          >> qi::float_[op(qi::_val, qi::_1, tag_min_x())]
          >> ','
          >> qi::float_[op(qi::_val, qi::_1, tag_max_x())]
          >> ','
          >> qi::int_[op(qi::_val, qi::_1, tag_nbin_y())]
          >> ','
          >> qi::float_[op(qi::_val, qi::_1, tag_min_y())]
          >> ','
          >> qi::float_[op(qi::_val, qi::_1, tag_max_y())]
          >> ']' ;
        }
        qi::rule<Iterator, dd4hep::digi::H2DParams(), Skipper> para;
        StringGrammar<Iterator, Skipper> name;
        StringGrammar<Iterator, Skipper> title;
        ph::function<Operations> op;
      // ======================================================================
    };
    REGISTER_GRAMMAR(dd4hep::digi::H2DParams, H2DParamsGrammar);
    // ========================================================================
    int parse(dd4hep::digi::H2DParams& result, const std::string& input)   {
      return parse_(result, input);
    }
    // ========================================================================
  } //                                         end of namespace dd4hep::Parsers
  // ==========================================================================
} //                                                    end of namespace dd4hep
// ============================================================================
#include "DD4hep/GrammarParsed.h"
static auto s_registry = dd4hep::GrammarRegistry::pre_note<dd4hep::digi::H1DParams>()
  .pre_note<dd4hep::digi::H2DParams>();
