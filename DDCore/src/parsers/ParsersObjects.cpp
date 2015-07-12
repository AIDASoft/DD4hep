// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//==========================================================================
// Include files
//==========================================================================
#include "ParsersFactory.h"
#include "DD4hep/ToStream.h"

using namespace std;
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
// ============================================================================
namespace DD4hep {
  namespace Parsers {
    // ==========================================================================
    template<typename T1, typename T2> inline int
    parse_(ROOT::Math::PositionVector3D<T1,T2>& result, const string& input){
      Skipper skipper;
      typename Grammar_<IteratorT,ROOT::Math::PositionVector3D<T1,T2>,Skipper>::Grammar g;
      IteratorT iter = input.begin(), end = input.end();
      if (qi::phrase_parse( iter, end, g, skipper, result)){
        return 1;
      }
      return 0;
    }
    // ==========================================================================
    int parse(ROOT::Math::XYZPoint& result,const string& input) {
      return parse_(result, input);
    }
    // ==========================================================================
    /*  parse 3D-vector
     *  @param result (output) the parsed vector
     *  @param input  (input)  the input string
     *  @return status code
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    int parse(ROOT::Math::XYZVector& result,const string& input) {
      ROOT::Math::XYZPoint point;
      int sc = parse(point,input);
      if ( 0 == sc ){ return sc; }                               // RETURN
      result = point;
      return 1;
    }
    int parse(ROOT::Math::PxPyPzEVector& result, const string& input) {
      return parse_(result, input);
    }

    // ==========================================================================
    /*  parse the vector of points
     *  @param resut (OUTPUT) the parser vector
     *  @param input (INPIUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    int parse(vector<ROOT::Math::XYZPoint>& result, const string&  input) {
      result.clear();
      return parse_(result, input);
    }
    int parse(list<ROOT::Math::XYZPoint>& result, const string&  input) {
      result.clear();
      return parse_(result, input);
    }
    int parse(set<ROOT::Math::XYZPoint>& result, const string&  input) {
      result.clear();
      return parse_(result, input);
    }
    // ==========================================================================
    /*  parse the vector of vectors
     *  @param resut (OUTPUT) the parser vector
     *  @param input (INPIUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    int parse(vector<ROOT::Math::XYZVector>& result, const string&  input) {
      result.clear();
      return parse_(result, input);
    }
    int parse(list<ROOT::Math::XYZVector>& result, const string&  input) {
      result.clear();
      return parse_(result, input);
    }
    int parse(set<ROOT::Math::XYZVector>& result, const string&  input) {
      result.clear();
      return parse_(result, input);
    }
    // ==========================================================================
    /*  parse the vector of vectors
     *  @param resut (OUTPUT) the parser vector
     *  @param input (INPIUT) the string to be parsed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-05
     */
    // ==========================================================================
    int parse(vector<ROOT::Math::PxPyPzEVector>& result,const string& input)  {
      result.clear();
      return parse_(result, input);
    }
    int parse(list<ROOT::Math::PxPyPzEVector>& result,const string& input)  {
      result.clear();
      return parse_(result, input);
    }
    int parse(set<ROOT::Math::PxPyPzEVector>& result,const string& input)  {
      result.clear();
      return parse_(result, input);
    }

  }
}
