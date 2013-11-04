#include "DDSegmentation/BitField64.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace DD4hep{

namespace DDSegmentation {
  
    BitFieldValue::BitFieldValue( long64& bitfield, const std::string& name, 
				  unsigned offset, int signedWidth ) :
    _b(bitfield),
    _mask(0), 
    _name( name ), 
    _offset( offset ),
    _width( abs( signedWidth ) ),
    _minVal(0),
    _maxVal(0),
    _isSigned( signedWidth < 0 ) {
    
    // sanity check
    if( offset > 63 || offset+_width > 64 ) {
      
      std::stringstream s ;
      s << " BitFieldValue '" << _name << "': out of range -  offset : " 
	<< offset  << " width " << _width  ; 
      
      throw( std::runtime_error( s.str() ) ) ;
    }
    
    _mask = ( ( 0x0001LL << _width ) - 1 ) << offset ;
    
    
    // compute extreme values for later checks
    if( _isSigned ){
      
      _minVal =  ( 1LL << ( _width - 1 ) ) - ( 1LL << _width )  ;
      _maxVal =  ( 1LL << ( _width - 1 ) ) - 1 ;
      
    } else {
      
      _maxVal = 0x0001<<_width  ;
    }
    
    //       std::cout << " _mask :" << std::hex << _mask  
    //                 <<  std::dec <<  std::endl ; 
    //       std::cout << " min " << _minVal 
    // 		<< " max " << _maxVal
    // 		<< " width " << _width 
    // 		<< std::endl ; 
 
  }
  

  long64 BitFieldValue::value() const { 
      
    if(  _isSigned   ) {

      long64 val = ( _b & _mask ) >> _offset ;
      
      if( ( val  & ( 1LL << ( _width - 1 ) ) ) != 0 ) { // negative value
	  
	val -= ( 1LL << _width );
      }
	
      return val ;

    } else { 
      
      return  ( _b & _mask ) >> _offset ;
    }
  }

  long64 BitFieldValue::value(long64 id) const {       
    if(  _isSigned   ) {
      long64 val = (id & _mask) >> _offset ;
      if( (val  & (1LL << (_width - 1))) != 0 ) { // negative value
	val -= (1LL << _width);
      }
      return val ;
    } else {
      return  (id & _mask) >> _offset ;
    }
  }

   BitFieldValue& BitFieldValue::operator=(long64 in) {
    
    // check range 
    if( in < _minVal || in > _maxVal  ) {
      
      std::stringstream s ;
      s << " BitFieldValue '" << _name << "': out of range : " << in 
	<< " for width " << _width  ; 
      
      throw( std::runtime_error( s.str() ) );
    }
    
    _b &= ~_mask ;  // zero out the field's range
    
    _b |=  ( (  in  << _offset )  & _mask  ) ; 
    
    return *this ;
  }
  



  size_t BitField64::index( const std::string& name) const {
    
    IndexMap::const_iterator it = _map.find( name ) ;
    
    if( it != _map.end() ) 
      
      return it->second  ;
    
    else
      throw std::runtime_error(" BitFieldValue: unknown name: " + name ) ;
  }
  
  unsigned BitField64::highestBit() const {

    unsigned hb(0) ;

    for(unsigned i=0;i<_fields.size();i++){

      if( hb < ( _fields[i]->offset() + _fields[i]->width() ) )
	hb = _fields[i]->offset() + _fields[i]->width()  ;
    }    
    return hb ;
  }


  std::string BitField64::valueString() const {

    std::stringstream  os ;

    for(unsigned i=0;i<_fields.size();i++){
      
      if( i != 0 )   os << "," ;

      os << _fields[i]->name() <<  ":" << _fields[i]->value() ;

    }
    return os.str() ;
  }
  
  std::string BitField64::fieldDescription() const {
    
    std::stringstream  os ;
    
    for(unsigned i=0;i<_fields.size();i++){
      
      if( i != 0 )   os << "," ;
      
      os << _fields[i]->name() <<  ":"
	 << _fields[i]->offset() << ":" ;
      
      if(  _fields[i]->isSigned()  )  
	os << "-" ;
      
      os  << _fields[i]->width() ;
      
    }
//     for( IndexMap::const_iterator it = _map.begin()  ;
// 	 it !=  _map.end() ; ++it ){

//       if( it !=  _map.begin() )
// 	os << "," ;
      
//       os << it->first <<  ":"
// 	 << _fields[ it->second ]->offset() << ":" ;
      
//       if(  _fields[ it->second ]->isSigned()  )  
// 	os << "-" ;

//       os  << _fields[ it->second ]->width() ;

//     }
    
    return os.str() ;
  }

  void BitField64::addField( const std::string& name,  unsigned offset, int width ){

      
    BitFieldValue* bfv =  new  BitFieldValue( _value, name, offset, width ) ;

    _fields.push_back(  bfv ) ;
    
    _map[ name ] = _fields.size()-1 ;

    if( _joined & bfv->mask()  ) {
      
      std::stringstream s ;
      s << " BitFieldValue::addField(" << name << "): bits already used " << std::hex << _joined
	<< " for mask " <<  bfv->mask()   ; 

      throw( std::runtime_error( s.str() ) ) ;
      
    }

    _joined |= _fields.back()->mask() ;

  }

  void BitField64::init( const std::string& initString) {

    unsigned offset = 0  ;
    
    // need to compute bit field masks and offsets ...
    std::vector<std::string> fieldDescriptors ;
    StringTokenizer t( fieldDescriptors ,',') ;

    std::for_each( initString.begin(), initString.end(), t ) ; 

    for(unsigned i=0; i< fieldDescriptors.size() ; i++ ){
      
      std::vector<std::string> subfields ;
      StringTokenizer ts( subfields ,':') ;
      
      std::for_each( fieldDescriptors[i].begin(), fieldDescriptors[i].end(), ts );

      std::string name ; 
      int  width ; 
      unsigned thisOffset ;

      switch( subfields.size() ){
	
      case 2: 

	name = subfields[0] ; 
	width = atol( subfields[1].c_str()  ) ;
	thisOffset = offset ;

	offset += abs( width ) ;
	
	break ;
	
      case 3: 
	name = subfields[0] ;
	thisOffset = atol( subfields[1].c_str()  ) ;
	width = atol( subfields[2].c_str()  ) ;

	offset = thisOffset + abs( width ) ;

	break ;
	
      default:

	std::stringstream s ;
	s << " BitField64: invalid number of subfields " 
	  <<  fieldDescriptors[i] ;

	throw( std::runtime_error( s.str() ) ) ;
      }

      addField( name , thisOffset, width ) ;
    }
  }



  std::ostream& operator<<(std::ostream& os, const BitField64& b){

    os << " bitfield:  0x" << std::hex // << std::ios::width(16) << std::ios::fill('0') <<
       << b._value << std::dec << std::endl ;

    for( BitField64::IndexMap::const_iterator it = b._map.begin()  ;
	 it !=  b._map.end() ; ++it ){
      
      os << "  " << it->first << " [" <<  b[ it->second ].offset()  << ":"  ;
      
      if(  b[ it->second ].isSigned()  )  os << "-" ;
      
      os <<  b[ it->second ].width() << "]  : "  ;
      
      
      os <<    b[ it->second ].value() 
	 << std::endl ;
      
    }
  
    return os ;
  } 

} // namespace

} // namespace
