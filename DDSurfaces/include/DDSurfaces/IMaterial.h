#ifndef DDSurfaces_IMaterial_H_
#define DDSurfaces_IMaterial_H_

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

} /* namespace DDSurfaces */

#endif /* DDSurfaces_MATERIAL_H_ */
