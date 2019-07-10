#include "DDSegmentation/BitFieldCoder.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace dd4hep{

namespace DDSegmentation {
  
  BitFieldElement::BitFieldElement( const std::string& fieldName,
				    unsigned fieldOffset, int signedWidth ) :
    _mask(0), 
    _offset( fieldOffset ),
    _width( abs( signedWidth ) ),
    _minVal(0),
    _maxVal(0),
    _isSigned( signedWidth < 0 ),
    _name( fieldName ) {
    
    // sanity check
    if( _offset > 63 || _offset+_width > 64 ) {
      
      std::stringstream s ;
      s << " BitFieldElement '" << _name << "': out of range -  offset : " 
	<< _offset  << " width " << _width  ;
      
      throw( std::runtime_error( s.str() ) ) ;
    }
    
    _mask = ( ( 0x0001LL << _width ) - 1 ) << _offset ;
    
    
    // compute extreme values for later checks
    if( _isSigned ){
      
      _minVal =  ( 1LL << ( _width - 1 ) ) - ( 1LL << _width )  ;
      _maxVal =  ( 1LL << ( _width - 1 ) ) - 1 ;
      
    } else {
      
      _maxVal = 0x0001<<_width  ;
    }
    
  }
  

  long64 BitFieldElement::value(long64 id) const { 
      
    if(  _isSigned   ) {

      long64 val = ( id & _mask ) >> _offset ;
      
      if( ( val  & ( 1LL << ( _width - 1 ) ) ) != 0 ) { // negative value
	  
	val -= ( 1LL << _width );
      }
	
      return val ;

    } else { 
      
      return  ( id & _mask ) >> _offset ;
    }
  }

  void BitFieldElement::set(long64& field, long64 in) const {
    
    // check range 
    if( in < _minVal || in > _maxVal  ) {
      
      std::stringstream s ;
      s << " BitFieldElement '" << _name << "': out of range : " << in 
	<< " for width " << _width  ; 
      
      throw( std::runtime_error( s.str() ) );
    }
    
    field &= ~_mask ;  // zero out the field's range
    
    field |=  ( (  in  << _offset )  & _mask  ) ; 

  }
  



  size_t BitFieldCoder::index( const std::string& name) const {
    
    IndexMap::const_iterator it = _map.find( name ) ;
    
    if( it != _map.end() ) 
      
      return it->second  ;
    
    else
      throw std::runtime_error(" BitFieldElement: unknown name: " + name ) ;
  }
  
  unsigned BitFieldCoder::highestBit() const {
    
    unsigned hb(0) ;
    
    for(unsigned i=0;i<_fields.size();i++){
      
      if( hb < ( _fields[i].offset() + _fields[i].width() ) )
	hb = _fields[i].offset() + _fields[i].width()  ;
    }    
    return hb ;
  }


  std::string BitFieldCoder::valueString(ulong64 bitfield) const {

    std::stringstream  os ;

    for(unsigned i=0;i<_fields.size();i++){
      
      if( i != 0 )   os << "," ;

      os << _fields[i].name() <<  ":" << _fields[i].value(bitfield) ;

    }
    return os.str() ;
  }
  
  std::string BitFieldCoder::fieldDescription() const {
    
    std::stringstream  os ;
    
    for(unsigned i=0;i<_fields.size();i++){
      
      if( i != 0 )   os << "," ;
      
      os << _fields[i].name() <<  ":"
	 << _fields[i].offset() << ":" ;
      
      if(  _fields[i].isSigned()  )
	os << "-" ;
      
      os  << _fields[i].width() ;
      
    }

    return os.str() ;
  }

  void BitFieldCoder::addField( const std::string& name,  unsigned offset, int width ){

      
    _fields.emplace_back(name, offset, width);
    BitFieldElement& bfv = _fields.back() ;

    _map[ name ] = _fields.size()-1 ;

    if( _joined & bfv.mask()  ) {
      
      std::stringstream s ;
      s << " BitFieldElement::addField(" << name << "): bits already used " << std::hex << _joined
	<< " for mask " <<  bfv.mask() ;

      throw( std::runtime_error( s.str() ) ) ;
      
    }

    _joined |= bfv.mask() ;

  }

  void BitFieldCoder::init( const std::string& initString) {

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
	s << " BitFieldCoder: invalid number of subfields " 
	  <<  fieldDescriptors[i] ;

	throw( std::runtime_error( s.str() ) ) ;
      }

      addField( name , thisOffset, width ) ;
    }
  }




} // namespace

} // namespace
