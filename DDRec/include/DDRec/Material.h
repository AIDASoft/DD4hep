#ifndef DDRec_Material_H
#define DDRec_Material_H

#include "DD4hep/Objects.h"
#include "DDSurfaces/IMaterial.h"

#include <list>

namespace DD4hep {
  namespace DDRec {
    
    
    /** Simple data class that implements the DDSurfaces::IMaterial interface.
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

      /** Instantiate from Geometry::Material handle */
      MaterialData( Geometry::Material m )  {
	// handle data can only be accessed if the handle is valid 
	if( m.isValid() ) {
	  _name= m.name() ;
	  _Z= m.Z() ;
	  _A= m.A() ;
	  _rho= m.density() ;
	  _x0= m.radLength() ;
	  _lambda= m.intLength() ;
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
      MaterialData( const std::string& name, double Z, double A, double density, double radLength, double intLength )  : _name( name ),
															 _Z( Z ),
															 _A( A ),
															 _rho( density ),
															 _x0( radLength ),
															 _lambda(  intLength ) {}
      
      /** Copy c'tor .*/
      MaterialData( const MaterialData& m )  : _name( m.name() ),
					       _Z( m.Z() ),
					       _A( m.A() ),
					       _rho( m.density() ),
					       _x0( m.radiationLength() ),
					       _lambda( m.interactionLength() ) {}
      

      /// true if initialized
      bool isValid() const { return true ; } //( _Z > 0. ) ; }

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
