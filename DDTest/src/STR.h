//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   F.Gaede
//  \date     2015-12-11
//  \version  1.0
//
//==========================================================================
// $Id$
#ifndef STR_H 
#define STR_H 1

#include <sstream>
#include <string>

namespace  {

  template <typename T> std::string _to_string(const T& _val)  {
    std::stringstream res; 
    res << _val ;
    return res.str();
  }

  /// Helper class for floating point comparisons using string representations
  /**
   *  \author  F.Gaede
   *  \date    2015-12-11
   *  \version 1.0
   *
   */
  class STR {
    STR() {} 
    float _val ;
    std::string _str ;
  public:
    STR ( float val ) : _val(val), _str(_to_string(val))  {   }
    std::string str() const { return _str ; }
    float value() const     { return _val;  }
    bool operator==( const STR& s2) const {
      return this->str() == s2.str() ;
    }
  };


  inline std::ostream& operator<<(std::ostream& os , const STR& s) {
    os << s.str() ;
    return os ;
  } 
}

#endif // STR_H
