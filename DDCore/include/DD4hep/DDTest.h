#include <iostream>
#include <sstream>
#include <stdlib.h>

namespace dd4hep{

  /// Simple class for defining unit tests.
  /**  Use in main program that is added as a test to ctest:
   *  
   *    DDTest test = DDTest( "example" ) ; 
   *    test.log( "example test" );
   *    test( "Example", "Example", "example test - string comparison " ); // this test will pass
   *    test( "Example", "BadExample", "example test - string comparison " ); //  this test will fail
   * 
   * @author F.Gaede, DESY, 2014
   * based on original version from J.Engels  
   */
  class DDTest{

  public:
    /// Default constructor
    DDTest() = delete;

    /// Copy constructor
    DDTest(const DDTest& copy) = delete;

    /// Assignment operator
    DDTest& operator=(const DDTest& copy) = delete;
    

    /** Only constructor
     */
    DDTest( const std::string& testname, std::ostream& stream=std::cout ) :
      _testname(testname), 
      _out(stream), 
      _failed(0), 
      _passed(0), 
      _last_test_status(false) {
    
      _out << std::endl << "[" << _testname << "] ";

      _out << "****************************** TEST_BEGIN ******************************" << std::endl << std::endl;
    }



    /** Destructor - print summary of tests passed and failed 
     */
    ~DDTest(){

      std::stringstream sstr ;

      sstr << std::endl;
      sstr << "[" << _testname << "] number of tests PASSED : " << _passed << std::endl ;
      sstr << "[" << _testname << "] number of tests FAILED : " << _failed << std::endl ;
      sstr << std::endl;

      sstr << "[" << _testname << "] " ;
      sstr << "****************************** " ;
      sstr << ( _failed == 0 ? "TEST_PASSED" : "TEST_FAILED" ) ;
      sstr << " ******************************" ;
      sstr << std::endl << std::endl ;

      _out << sstr.str() ;

      if( _failed != 0 ) exit(1) ;

    }


    /** Operator for calling a test - test is passed if v1 == v2
     */
    template <class V1, class V2 >
    void operator()(const V1& v1, const V2& v2, const std::string& name ) {
    
      if ( ! (v1 == v2)  ) {
      
        std::stringstream sstr ;
        sstr << "  " << name<< " : [" << v1 << "] != [" << v2 <<"]" ;

        error( sstr.str() ) ;

      } else {

        std::stringstream sstr ;
        sstr << "  " << name<< " : [" << v1 << "] == [" << v2 <<"]" ;

        pass( sstr.str() ) ;
      }

      return ;
    }

    /** Operator for calling a test - test is passed if (!c)==false 
     */
    template <class Cond >
    void operator()(const Cond& c, const std::string& name ) {
    
      if ( ! (c)  ) {
      
        std::stringstream sstr ;
        sstr << "  " << name<< " : [" << c << "] " ;
      
        error( sstr.str() ) ;
      
      } else {      

        std::stringstream sstr ;
        sstr << "  " << name<< " : [" << c  << "] " ;

        pass( sstr.str() ) ;
      }
      return ;
    }


    /** Simple log message */
    void log( const std::string& msg ){
      _out << "[" << _testname << "] " << msg << std::endl;
    }
  
  
    /** print message when test passed */  
    void pass( const std::string& msg ){
    
      _passed++;
      _last_test_status = true ;

      _out << "[" << _testname << "] test " << last_test_status() << ":  " << msg << std::endl;
    }



    /** print message when test failed */  
    void error( const std::string& msg ){

      _failed++;
      _last_test_status = false ;

      std::stringstream errmsg;
      //    errmsg << std::endl;
      errmsg << "[" << _testname << "] ##################### TEST_FAILED ######################" << std::endl;
      errmsg << "[" << _testname << "] ### ERROR: " << msg << std::endl;
      errmsg << "[" << _testname << "] ########################################################" << std::endl;
      //  errmsg << std::endl;

      _out << errmsg.str();

      // also send error to stderr
      //std::cerr << errmsg.str();
    }

    /** Fatal error ...*/
    void fatal_error( const std::string& msg ){
      error( msg );
      _out << "FATAL ERROR OCCURRED, program will exit now !!" << std::endl ;
      exit(1);
    }

    /** Return the status from the last test - either PASSED or FAILED */
    const char* last_test_status(){
      return ( _last_test_status ? "PASSED" : "FAILED" ) ;
    }

  private:

    std::string _testname ;
    std::ostream& _out = std::cout;

    unsigned int _failed = 0;          // number of failed tests
    unsigned int _passed = 0;          // number of passed tests
    bool _last_test_status = false;    // true if last test succeeded, false otherwise
  };


} // end namespace
