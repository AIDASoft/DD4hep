//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#ifndef DDSEGMENTATION_BITFIELD64_H
#define DDSEGMENTATION_BITFIELD64_H 1

#include <iostream>

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "DDSegmentation/BitFieldCoder.h"

namespace dd4hep {
  
  typedef long long int long64 ;
  typedef unsigned long long ulong64 ;
  
  namespace DDSegmentation {
    
    /// Lightweight helper class for BitField64 that corresponds to one field value.
    /**  (Not thread safe - only use directly through BitField64).
     *
     *    @author F.Gaede, DESY
     *    @date  2017-09
     */
    class BitFieldValue{

      long64& _value ;
      const BitFieldElement& _bv ;
    
    public :
    
      BitFieldValue() = delete ;
    
      /// only c'tor with reference to bitfield and BitFieldElement
      BitFieldValue( long64& bitfield, const BitFieldElement& bv ) :
        _value( bitfield ), _bv( bv) {}
    
      /** Returns the current field value 
       */
      long64 value() const { return _bv.value( _value ) ; }
  
      /// Calculate Field value given an external 64 bit bitmap value
      long64 value(long64 id) const { return _bv.value( id ) ; }

      //// Assignment operator for user convenience 
      BitFieldValue& operator=(long64 in) {
        _bv.set( _value, in ) ;
        return *this ;
      } 
    
      /** Conversion operator for long64 - allows to write:<br>
       *  long64 index = myBitFieldValue ;
       */
      operator long64() const { return value() ; } 
    
      /** The field's name */
      const std::string& name() const { return _bv.name() ; }

      /** The field's offset */
      unsigned offset() const { return _bv.offset() ; }

      /** The field's width */
      unsigned width() const { return _bv.width() ; }

      /** True if field is interpreted as signed */
      bool isSigned() const { return _bv.isSigned() ; }

      /** The field's mask */
      ulong64 mask() const { return _bv.mask() ; }

      /** Minimal value  */
      int  minValue()  const  { return _bv.minValue();  }

      /** Maximal value  */
      int  maxValue()  const  { return _bv.maxValue();  }

    } ;
    
  
    /// A bit field of 64bits that allows convenient declaration
    /**  and manipulation of sub fields of various widths.<br>
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
     *    unsigned phiIndex = b.index("phi") ;        <br>
     *    int phi = b[  phiIndex ] ;                  <br>
     *
     *    Sep-2017: FG: re-implemented as lightweight wrapper around BitFieldCoder
     *    @author F.Gaede, DESY
     *    @date  2013-06
     */  
    class BitField64{
      
      friend std::ostream& operator<<(std::ostream& os, const BitField64& b) ;

    public :

      virtual ~BitField64() {
        if( _owner)
          delete _coder ;
      } ; 
  
      /** No default c'tor */
      BitField64() = delete ;

      /** The c'tor takes an initialization string of the form:<br>
       *  \<fieldDesc\>[,\<fieldDesc\>...]<br>
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
      BitField64( const std::string& initString ){
      
        _coder = new BitFieldCoder( initString ) ;
      }

      /// Initialize from existing BitFieldCoder
      BitField64( const BitFieldCoder* coder ) : _owner(false), _coder( coder ) {
      }


      /** Returns the current 64bit value 
       */
      long64 getValue() const { return _value ; }
    
      /** Set a new 64bit value  - bits not used in description are set to 0.
       */
      void  setValue(long64 value ) { _value = ( _coder->mask() & value ) ; }

      /** Set a new 64bit value given as high and low 32bit words.
       */
      void  setValue(unsigned low_Word, unsigned high_Word ) {
        setValue( ( low_Word & 0xffffffffULL ) |  ( ( high_Word & 0xffffffffULL ) << 32 ) ) ; 
      }
    
      /** Operator for setting a new value and accessing the BitField directly */
      BitField64& operator()(long64 val) { setValue( val ) ; return *this ; }
 
      /** Reset - same as setValue(0) - useful if the same encoder is used for many objects.
       */
      void  reset() { _value = 0 ; }

      /** Acces to field through index 
       */
      BitFieldValue operator[](size_t idx) {
        return BitFieldValue( _value,  _coder->operator[]( idx  ) ) ; 
      }
    
      // /** Const acces to field through index 
      //  */
      // const BitFieldValue& operator[](size_t idx) const { 
      
      //   return BitFieldValue( _value,  _coder->operator[]( idx  ) ) ; 
      // }

      /** Highest bit used in fields [0-63]
       */
      unsigned highestBit() const { return _coder->highestBit() ; }
    
      /** Number of values */
      size_t size() const { return _coder->size() ; }

      /** Index for field named 'name' 
       */
      size_t index( const std::string& name) const { return _coder->index( name ) ; } 

      /** Access to field through name .
       */
      BitFieldValue operator[](const std::string& name) { 

        return BitFieldValue( _value,  _coder->operator[]( name ) ) ; 
      }
      // /** Const Access to field through name .
      //  */
      // const BitFieldValue& operator[](const std::string& name) const { 

      //   return BitFieldValue( _value,  _coder->operator[]( name )  ); 
      // }


      /** The low  word, bits 0-31
       */
      unsigned lowWord() const { return unsigned( _value &  0xffffFFFFUL )  ; } 

      /** The high  word, bits 32-63
       */
      unsigned highWord() const { return unsigned(_value >> 32) ; } 


      /** Return a valid description string of all fields
       */
      std::string fieldDescription() const { return _coder->fieldDescription() ; }

      /** Return a string with a comma separated list of the current sub field values 
       */
      std::string valueString() const ;

      // const std::vector<BitFieldValue*>& fields()  const  {
      //   return _coder->fields() ;
      // }
    
    protected:

      // -------------- data members:--------------
      bool  _owner{true} ;
      long64    _value{} ;
      const BitFieldCoder* _coder{} ;
    
    };
  
  
    /** Operator for dumping BitField64 to streams 
     */
    std::ostream& operator<<(std::ostream& os, const BitField64& b) ;
  
  
  
  } // end namespace

  using DDSegmentation::BitField64 ;
} // end namespace

#endif




