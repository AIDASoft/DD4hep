// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception> 

namespace DD4hep {
  

  //define some exception to throw
  class OutsideGeometryException : public std::exception{
    
  protected:
    std::string message;
    OutsideGeometryException() {  /*no_op*/ ; } 
    
  public: 
    OutsideGeometryException( std::string text ){
     message = "OutsideGeometryException: " + text ;
    }
    virtual const char* what() const  throw() { return  message.c_str() ; } 
    virtual ~OutsideGeometryException() throw() { /*no_op*/; } 
  }; 
}
#endif
