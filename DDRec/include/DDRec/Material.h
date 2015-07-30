#ifndef DDRec_Material_H
#define DDRec_Material_H

#include "DD4hep/Objects.h"
#include "DDSurfaces/IMaterial.h"

#include <list>

namespace DD4hep {
  namespace DDRec {
    
    
    /** Simple data class that implements the DDSurfaces::IMaterial interface
     *  and is used in the Surface implementation.
     *
     * @author F.Gaede, DESY
     * @date May, 20 2014
     * @version $Id$
     */
    class MaterialData : public DDSurfaces::IMaterial{
      
    protected:
      std::string _name ;
      double _Z ;
      double _A ;
      double _rho ;
      double _x0 ;
      double _lambda ;

    public:

      /** Instantiate from Geometry::Material - default initialization if handle is not valid */
      MaterialData( Geometry::Material m ) : 

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
      

      /// assignment from Geometry::Material
      MaterialData& operator=(const Geometry::Material& m){
      
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

#endif /* DDRec_Material_H */
