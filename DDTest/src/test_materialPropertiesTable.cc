#include "DD4hep/DDTest.h"
#include <exception>
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <cmath>
#include "DD4hep/Objects.h"
#include "DD4hep/detail/ObjectsInterna.h"

using namespace std ;
using namespace dd4hep ;

// this should be the first line in your test
static DDTest test( "materialPropertiesTable" ) ; 

//=============================================================================

int main(int /* argc */, char** /* argv */ ){
    
  try{
    
    // ----- write your tests in here -------------------------------------

    test.log( "materialPropertiesTable test" );

    // ----- materialPropertiesTable test for testing two expressions for equality:
    //test( "materialPropertiesTable", "materialPropertiesTable", "materialPropertiesTable test - string comparison " ); // this test will pass
    //test( "materialPropertiesTable", "BadmaterialPropertiesTable", "materialPropertiesTable test - string comparison " ); //  this test will fail

    // --------------------------------------------------------------------
    dd4hep::MaterialPropertiesTable mpt;
    std::string fname = std::string(std::getenv("DD4hepINSTALL"))+"/share/DD4hep/data/scintillation_yields.json";
    mpt.loadFile(fname.c_str());
    //std::cout << mpt.nArrayData() << std::endl;
    //std::cout << mpt.nConstData() << std::endl;
    //std::cout << mpt.nStringData() << std::endl;
    int nArray  = mpt.nArrayData() ;
    int nConst  = mpt.nConstData() ;
    int nString = mpt.nStringData();
    test(  nArray == 7 , " materialPropertiesTable check:  nArray == 7 " ) ;  

    //double pi_d = M_PI ;
    //float  pi_f = M_PI ;
    //test(  1.1 * pi_f > pi_d   , " materialPropertiesTable comparison:  1.1* float(M_PI)  > double( M_PI) " ) ;  

    // --------------------------------------------------------------------
    //
    fname = std::string(std::getenv("DD4hepINSTALL"))+"/share/DD4hep/data/N2_index_of_refraction.json";
    dd4hep::MaterialPropertiesTable mpt2;
    mpt2.loadFile(fname.c_str());
    //std::cout << mpt2.nArrayData() << std::endl;
    //std::cout << mpt2.nConstData() << std::endl;
    //std::cout << mpt2.nStringData() << std::endl;
    test(  mpt2.nArrayData() == 3 , " materialPropertiesTable check:  nArray == 3 " ) ;  

    //int nArray  = mpt.nArrayData() ;
    //int nConst  = mpt.nConstData() ;
    //int nString = mpt.nStringData();

    // ... guess which of these will pass ...
    // test(  pi_f == pi_d  , " materialPropertiesTable comparison:  float(M_PI) == double( M_PI) " ) ;  
    // test(  pi_f <  pi_d  , " materialPropertiesTable comparison:  float(M_PI)  < double( M_PI) " ) ;  
    // test(  pi_f >  pi_d  , " materialPropertiesTable comparison:  float(M_PI)  > double( M_PI) " ) ;  

    // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
