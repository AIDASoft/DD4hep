#include "DDSegmentation/BitField64.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace dd4hep{

namespace DDSegmentation {
  

  std::string BitField64::valueString() const {

    std::stringstream  os ;

    for(unsigned i=0;i<size();i++){
      
      if( i != 0 )   os << "," ;

      os << fields()[i]->name() <<  ":" << _coder->get( _value , i ) ;

    }
    return os.str() ;
  }
  

  std::ostream& operator<<(std::ostream& os, const BitField64& b){

    os << " bitfield:  0x" << std::hex // << std::ios::width(16) << std::ios::fill('0') <<
       << b._value << std::dec << std::endl ;

    for(unsigned i=0;i<_coder->size();i++){
      
      const BitFieldCoder::BitFieldValue* bv = b.fields()[i] ;
      
      os << "  " <<  bv->name()
	 << " [" <<  bv->offset()  << ":"  ;
      
      if(  bv->isSigned()  )  os << "-" ;

      os <<  bv->width() << "]  : "  ;
      
      os <<   _coder->get( _value , i) 
	 << std::endl ;
      
    }
  
    return os ;
  } 

} // namespace

} // namespace
