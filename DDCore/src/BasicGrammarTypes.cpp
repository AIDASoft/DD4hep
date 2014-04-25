// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/objects/BasicGrammar_inl.h"

DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(char)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(short)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(int)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(long)
DD4HEP_DEFINE_PARSER_GRAMMAR_U_CONT(long long)

DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(bool,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(float,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(double,eval_item)

DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(std::string)
DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(std::vector<std::string>)
DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(std::list<std::string>)
DD4HEP_DEFINE_PARSER_GRAMMAR_TYPE(std::set<std::string>)

// ROOT::Math Object instances
typedef std::map<std::string, int> map_string_int;
DD4HEP_DEFINE_PARSER_GRAMMAR(map_string_int,eval_obj)

namespace ROOT { 
  namespace Math {
    static bool operator<(const XYZPoint& a, const XYZPoint& b)  {
      if ( a.X() < b.X() ) return true;
      if ( a.Y() < b.Y() ) return true;
      if ( a.Z() < b.Z() ) return true;
      return false;
    }
    static bool operator<(const XYZVector& a, const XYZVector& b)  {
      if ( a.X() < b.X() ) return true;
      if ( a.Y() < b.Y() ) return true;
      if ( a.Z() < b.Z() ) return true;
      return false;
    }
    static bool operator<(const PxPyPzEVector& a, const PxPyPzEVector& b)  {
      if ( a.X() < b.X() ) return true;
      if ( a.Y() < b.Y() ) return true;
      if ( a.Z() < b.Z() ) return true;
      if ( a.T() < b.T() ) return true;
      return false;
    }
  }
}

DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(ROOT::Math::XYZPoint,eval_obj)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(ROOT::Math::XYZVector,eval_obj)
DD4HEP_DEFINE_PARSER_GRAMMAR_CONT(ROOT::Math::PxPyPzEVector,eval_obj)
