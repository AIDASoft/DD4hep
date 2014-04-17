#ifndef DDSurfaces_IMaterial_H_
#define DDSurfaces_IMaterial_H_

#include <string>
#include <ostream>

namespace DDSurfaces {
  
  /** Interface for material description for tracking. 
   *
   * @author F.Gaede, DESY
   * @version $Id$
   * @date Apr 6 2014
   */
  
  class IMaterial {
    
  public:
    
    /// Destructor
    virtual ~IMaterial() {}
    
    /// material name
    virtual std::string name() const =0 ;

    /// averaged proton number
    virtual double Z() const =0 ;
    
    /// averaged atomic number
    virtual double A() const =0 ;
    
    /// density - units ?
    virtual double density() const =0 ;
    
    /// radiation length - units ?
    virtual double radiationLength() const =0 ;
    
    /// interaction length - units ?
    virtual double interactionLength() const =0 ;

  };

 /// dump IMaterial operator 
  inline std::ostream& operator<<( std::ostream& os , const IMaterial& m ) {

    os << "  " << m.name() << ", Z: " << m.Z() << ", A: " << m.A() << ", densitiy: " << m.density() << ", radiationLength: " <<  m.radiationLength() 
       << ", interactionLength: " << m.interactionLength() ;

    return os ;
  }

} /* namespace DDSurfaces */

#endif /* DDSurfaces_MATERIAL_H_ */
