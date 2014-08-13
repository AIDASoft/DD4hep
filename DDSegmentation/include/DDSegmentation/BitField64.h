#ifndef DDSegmentation_BitField64_H
#define DDSegmentation_BitField64_H 1

#include <iostream>

#include <string>
#include <vector>
#include <map>
#include <sstream>


namespace DD4hep {

//fixme: do  need to do this also for 32 bit machines ?
typedef long long int long64 ;
typedef unsigned long long ulong64 ;

namespace DDSegmentation {

  class BitFieldValue ;
  class StringTokenizer ; 


  /** A bit field of 64bits that allows convenient declaration and 
   *  manipulation of sub fields of various widths.<br>
   *  Example:<br>
   *    BitField64 b("layer:7,system:-3,barrel:3,theta:32:11,phi:11" ) ; <br> 
   *    b[ "layer"  ]  = 123 ;         <br> 
   *    b[ "system" ]  = -4 ;          <br> 
   *    b[ "barrel" ]  = 7 ;           <br> 
   *    b[ "theta" ]   = 180 ;         <br> 
   *    b[ "phi" ]     = 270 ;         <br> 
   *    ...                            <br>
   *    int theta = b["theta"] ;                    <br>
   *    ...                                         <br>
   *    unsigned phiIndex = b.index("phi) ;         <br>
   *    int phi = b[  phiIndex ] ;                  <br>
   *
   *    @author F.Gaede, DESY
   *    @version $Id:$
   *    @date  2013-06
   */  
  class BitField64{

    friend std::ostream& operator<<(std::ostream& os, const BitField64& b) ;

  public :

    typedef std::map<std::string, unsigned int> IndexMap ;


    virtual ~BitField64();  // clean up
  
    /** The c'tor takes an initialization string of the form:<br>
     *  <fieldDesc>[,<fieldDesc>...]<br>
     *  fieldDesc = name:[start]:[-]length<br>
     *  where:<br>
     *  name: The name of the field<br>
     *  start: The start bit of the field. If omitted assumed to start 
     *  immediately following previous field, or at the least significant 
     *  bit if the first field.<br>
     *  length: The number of bits in the field. If preceeded by '-' 
     *  the field is signed, otherwise unsigned.<br>
     *  Bit numbering is from the least significant bit (bit 0) to the most 
     *  significant (bit 63). <br>
     *  Example: "layer:7,system:-3,barrel:3,theta:32:11,phi:11"
     */
    BitField64( const std::string& initString ) : _value(0), _joined(0){
    
      init( initString ) ;
    }

    /** Returns the current 64bit value 
     */
    long64 getValue() const { return _value ; }
    
    /** Set a new 64bit value  - bits not used in description are set to 0.
     */
    void  setValue(long64 value ) { _value = ( _joined & value ) ; }

    /** Set a new 64bit value given as high and low 32bit words.
     */
    void  setValue(unsigned lowWord, unsigned highWord ) {

      setValue( ( lowWord & 0xffffffffUL ) |  ( ( highWord & 0xffffffffUL ) << 32 ) ) ; 
    }
    
    /** Operator for setting a new value and accessing the BitField directly */
    BitField64& operator()(long64 val) { setValue( val ) ; return *this ; }
 
    /** Reset - same as setValue(0) - useful if the same encoder is used for many objects.
     */
    void  reset() { _value = 0 ; }

    /** Acces to field through index 
     */
    BitFieldValue& operator[](size_t index) { 
      return *_fields.at( index )  ; 
    }
    
    /** Const acces to field through index 
     */
    const BitFieldValue& operator[](size_t index) const { 
      return *_fields.at( index )  ; 
    }

    /** Highest bit used in fields [0-63]
     */
    unsigned highestBit() const ;
    
    /** Number of values */
    size_t size() const { return _fields.size() ; }

    /** Index for field named 'name' 
     */
    size_t index( const std::string& name) const ;

    /** Access to field through name .
     */
    BitFieldValue& operator[](const std::string& name) { 

      return *_fields[ index( name ) ] ;
    }
    /** Const Access to field through name .
     */
    const BitFieldValue& operator[](const std::string& name) const { 

      return *_fields[ index( name ) ] ;
    }


    /** The low  word, bits 0-31
     */
    unsigned lowWord() const { return unsigned( _value &  0xffffFFFFUL )  ; } 

    /** The high  word, bits 32-63
     */
    unsigned highWord() const { return unsigned(_value >> 32) ; } 


    /** Return a valid description string of all fields
     */
    std::string fieldDescription() const ;

    /** Return a string with a comma separated list of the current sub field values 
     */
    std::string valueString() const ;

  protected:

    /** Add an additional field to the list 
     */
    void addField( const std::string& name,  unsigned offset, int width ); 

    /** Decode the initialization string as described in the constructor.
     *  @see BitField64( const std::string& initString )
     */
    void init( const std::string& initString) ;

    /** No default c'tor */
    BitField64() : _value(0) , _joined(0) { }


    // -------------- data members:--------------

    std::vector<BitFieldValue*>  _fields ;
    long64    _value ;
    IndexMap  _map ;
    long64    _joined ;


  };



  /** Operator for dumping BitField64 to streams 
   */
  std::ostream& operator<<(std::ostream& os, const BitField64& b) ;



 /** Helper class  for string tokenization. Usage:<br>
   *    std::vector<std::string> tokens ; <br>
   *    StringTokenizer t( tokens ,',') ; <br>
   *    std::for_each( aString.begin(), aString.end(), t ) ;  <br>
   *
   *    @author F.Gaede, DESY
   *    @date  2013-06
   */
  class StringTokenizer{
    
    std::vector< std::string >& _tokens ;
    char _del ;
    char _last ;

  public:
    
    /** Only c'tor, give (empty) token vector and delimeter character */
    StringTokenizer( std::vector< std::string >& tokens, char del ) 
      : _tokens(tokens) 
	, _del(del), 
	_last(del) {
    }
    
    /** Operator for use with algorithms, e.g. for_each */
    void operator()(const char& c) { 
      
      if( c != _del  ) {
	
	if( _last == _del  ) {
	  _tokens.push_back("") ; 
	}
	_tokens.back() += c ;
      }
      _last = c ;
    } 
    
  };


  /** Helper class for BitField64 that corresponds to one field value. 
   *    @author F.Gaede, DESY
   *    @date  2013-06
   */

  class BitFieldValue{
  
  public :
    virtual ~BitFieldValue() {}
  
    /** The default c'tor.
     * @param  bitfield      reference to the 64bit bitfield
     * @param  offset        offset of field
     * @param  signedWidth   width of field, negative if field is signed
     */
    BitFieldValue( long64& bitfield, const std::string& name, 
		   unsigned offset, int signedWidth ) ; 


    /** Returns the current field value 
     */
    long64 value() const ;
  
    /// Calculate Field value given an external 64 bit bitmap value
    long64 value(long64 id) const;

    /** Assignment operator for user convenience 
     */
    BitFieldValue& operator=(long64 in) ;

    /** Conversion operator for long64 - allows to write:<br>
     *  long64 index = myBitFieldValue ;
     */
    operator long64() const { return value() ; } 
    
    /** fg: removed because it causes ambiguities with operator long64().
     *  Conversion operator for int - allows to write:<br>
     *  int index = myBitFieldValue ;
     */
    //     operator int() const { return (int) value() ; } 
    
    /** The field's name */
    const std::string& name() const { return _name ; }

    /** The field's offset */
    unsigned offset() const { return _offset ; }

    /** The field's width */
    unsigned width() const { return _width ; }

    /** True if field is interpreted as signed */
    bool isSigned() const { return _isSigned ; }

    /** The field's mask */
    ulong64 mask() const { return _mask ; }


  protected:
  
    long64& _b ;
    ulong64 _mask ;
    std::string _name ;
    unsigned _offset ;
    unsigned _width ;
    int _minVal ;
    int _maxVal ;
    bool _isSigned ;

  };


  inline BitField64::~BitField64() {  // clean up
    for(unsigned i=0;i<_fields.size();i++){
      delete _fields[i] ;
    }
  }


} // end namespace

} // end namespace

#endif




