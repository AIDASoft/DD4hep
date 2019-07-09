//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#ifndef rec_Material_H
#define rec_Material_H

#include "DD4hep/Detector.h"
#include "DDRec/IMaterial.h"
#include "DD4hep/Objects.h"

#include <list>

namespace dd4hep {
  namespace rec {
    
    
    /** Simple data class that implements the IMaterial interface
     *  and is used in the Surface implementation.
     *
     * @author F.Gaede, DESY
     * @date May, 20 2014
     * @version $Id$
     */
    class MaterialData : public IMaterial{
      
    protected:
      std::string _name ;
      double _Z ;
      double _A ;
      double _rho ;
      double _x0 ;
      double _lambda ;

    public:

      /** Instantiate from Material - default initialization if handle is not valid */
      MaterialData( Material m ) : 

        _name("unknown"),
        _Z( -1. ),
        _A( 0. ),
        _rho( 0. ),
        _x0( 0. ),
        _lambda( 0.)  {

        if( m.isValid() ) {

          _name= m.name() ;
          _Z = m.Z() ;
          _A = m.A() ;
          _rho = m.density() ;
          _x0 = m.radLength() ;
          _lambda = m.intLength() ;

        }
      }
      
      /** Default c'tor .*/
      MaterialData()  : _name("unknown"),
                        _Z( -1. ),
                        _A( 0. ),
                        _rho( 0. ),
                        _x0( 0. ),
                        _lambda( 0.) {}

      /** C'tor setting all attributes .*/
      MaterialData( const std::string& nam, double Z_val, double A_val, double density_val, double radLength, double intLength )
        : _name( nam ),
          _Z( Z_val ),
          _A( A_val ),
          _rho( density_val ),
          _x0( radLength ),
          _lambda(  intLength ) {}
      
      /** Copy c'tor .*/
      MaterialData( const MaterialData& m )  : _name( m.name() ),
                                               _Z( m.Z() ),
                                               _A( m.A() ),
                                               _rho( m.density() ),
                                               _x0( m.radiationLength() ),
                                               _lambda( m.interactionLength() ) {}

      /** Copy c'tor .*/
      MaterialData( const IMaterial& m )  : _name( m.name() ),
					    _Z( m.Z() ),
					    _A( m.A() ),
					    _rho( m.density() ),
					    _x0( m.radiationLength() ),
					    _lambda( m.interactionLength() ) {}
      
      /// copy assignement
       MaterialData& operator=(const MaterialData& m){
        if ( this != &m )  {
          _name = m._name ;
          _Z = m._Z ;
          _A = m._A  ;
          _rho = m._rho ;
          _x0 = m._x0 ;
          _lambda = m._lambda ;
        }
        return *this ;
      }

     /// assignment from Material
      MaterialData& operator=(const IMaterial& m){
        if ( this != &m )  {
          _name = m.name() ;
          _Z = m.Z() ;
          _A = m.A() ;
          _rho = m.density() ;
          _x0 = m.radiationLength() ;
          _lambda = m.interactionLength() ;
        }
        return *this ;
      }

      /// assignment from Material
      MaterialData& operator=(const Material& m){
      
        if( m.isValid() ) {

          _name = m.name() ;
          _Z = m.Z() ;
          _A = m.A() ;
          _rho = m.density() ;
          _x0 = m.radLength() ;
          _lambda = m.intLength() ;

        }  else {

          _name= "unknown";
          _Z = -1.  ;
          _A =  0. ;
          _rho = 0. ;
          _x0 = 0. ;
          _lambda = 0. ;
        }

        return *this ;
      }

      /// true if initialized
      bool isValid() const { return ( _Z > 0. ) ; }

      /** D'tor.*/
      virtual ~MaterialData() {}

      /// material name
      virtual std::string name() const { return _name ; }
      
      /// averaged proton number
      virtual double Z() const {  return _Z ; } 
      
      /// averaged atomic number
      virtual double A() const { return _A ; } 
      
      /// density
      virtual double density() const {  return _rho ; }
      
      /// radiation length - tgeo units 
      virtual double radiationLength() const { return _x0 ; } 
      
      /// interaction length - tgeo units 
      virtual double interactionLength() const  { return _lambda ; }

    };


  } /* namespace */
} /* namespace */



#endif /* rec_Material_H */
