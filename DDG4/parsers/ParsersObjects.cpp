// ============================================================================
// Include files
// ============================================================================
#include "ParsersFactory.h"
#include "DDG4/ToStream.h"

using namespace std;

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

  }
}
