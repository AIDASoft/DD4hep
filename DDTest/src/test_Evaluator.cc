#include "DD4hep/DDTest.h"
#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>
#include <array>

#include <atomic>
#include <thread>

#include "Evaluator/Evaluator.h"


using namespace std ;
using namespace dd4hep ;

// this should be the first line in your test
static DDTest test( "Evaluator" ) ; 

namespace {
  double foo() { return 8.0;}
}

//=============================================================================

int main(int /* argc */, char** /* argv */ ){
    
  try{
    
    // ----- write your tests in here -------------------------------------

    test.log( "test Evaluator" );
    using namespace dd4hep::tools;
    Evaluator e;
    {
      auto r = e.evaluate("5.2");
      test( r.second , 5.2, " double value");
      test( r.first, Evaluator::OK, " status OK");
    }

    {
      auto r = e.evaluate("4*m");
      test( r.second , 4, " double value in meters");
      test( r.first, Evaluator::OK, " status OK");
    }

    {
      auto r = e.evaluate("4*cm");
      test( r.second , 0.04, " double value in cm");
      test( r.first, Evaluator::OK, " status OK");
    }

    {
      auto r = e.evaluate("abs(-4)");
      test( r.second , 4., " call abs");
      test( r.first, Evaluator::OK, " status OK");
    }

    {
      auto r = e.evaluate("min(7, 2)");
      test( r.second , 2., " call min");
      test( r.first, Evaluator::OK, " status OK");
    }

    {
      auto r = e.evaluate("7_");
      test( r.first, Evaluator::ERROR_UNEXPECTED_SYMBOL, " status UNEXPECTED SYMBOL");
    }

    {
      e.setVariable("myVar", 12);
      auto r = e.evaluate("myVar");
      test( r.second , 12., " use myVar variable");
      test( r.first, Evaluator::OK, " status OK");
    }

    {
      e.setFunction("foo", foo);
      auto r = e.evaluate("foo()");
      test( r.second , foo(), " call new function foo");
      test( r.first, Evaluator::OK, " status OK");
    }
    
    {
      //use cm as length
      Evaluator e_cm(100.);

      {
        auto r = e_cm.evaluate("4*m");
        test( r.second , 400, " double value in meters with cm as unit");
        test( r.first, Evaluator::OK, " status OK");
      }
      
      {
        auto r = e_cm.evaluate("4*cm");
        test( r.second , 4, " double value in cm with cm as unit");
        test( r.first, Evaluator::OK, " status OK");
      }
    }

    {
      //do modifications while running multple threads
      std::atomic<int> countDownToStart{2};

      std::atomic<bool> success{true};

      Evaluator e_threads;

      std::array<std::string,100> nameExtensions;
      char extension[3];
      extension[2] = 0;
      for(char i = 0; i< 4;++i) {
        extension[1] = 97+i;
        for(char j = 0; j<25;++j) {
          extension[0]=97+j;
          nameExtensions[i*25+j] = std::string(extension,2);
        }
      }
      
      
        std::thread t1([&countDownToStart, &success, &e_threads, &nameExtensions]() { 
          const std::string name("ta");
          --countDownToStart;
          while(countDownToStart != 0);
      
          for(int i=0; i< 100; ++i) {
            std::string newName = name+nameExtensions[i];
             auto status = e_threads.setVariable(newName, i);
            if(status != Evaluator::OK) {
              success = false;
              std::cout <<"Failed to add variable "<<newName<<" "<<status<<std::endl;
              break;
            };
            auto r = e_threads.evaluate( newName+"*m");
            if(r.first != Evaluator::OK) {
              success = false;
              std::cout <<"Parser failure "<<r.first<<" for variable "<<newName<<std::endl;
              break;
            }
            if(r.second != i) {
              success = false;
              std::cout <<"Failed evaluation "<<i <<" != "<<r.second<<" for variable "<<newName<<std::endl;
              break;
            }
          }
          
        }
        ); 
#if 0
      std::thread t2([&countDownToStart, &success, &e_threads,&nameExtensions]()
#endif	
      std::thread t2([&countDownToStart]()
      {
          const std::string name("tb");
          --countDownToStart;
          while(countDownToStart != 0);
          return;
#if 0
	    Commented out to please Coverity:
	    CID 1501200 (#1 of 1): Structurally dead code (UNREACHABLE)unreachable: This code cannot be reached:

          for(int i=0; i< 100; ++i) {
            std::string newName = name+nameExtensions[i];
            e_threads.setVariable(newName, i);
            auto r = e_threads.evaluate( newName+"*m");
            if(r.first != Evaluator::OK) {
              success = false;
              std::cout <<"Parser failure "<<r.first<<" for variable "<<newName<<std::endl;
              break;
            }
            if(r.second != i) {
              success = false;
              std::cout <<"Failed evaluation "<<i <<" != "<<r.second<<" for variable "<<newName<<std::endl;
              break;
            }
          }
#endif
        }
      );
      
      t1.join();
      t2.join();

      test(success.load(), " multi-thread test");

    }

    // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
