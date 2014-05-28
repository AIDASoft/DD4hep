#ifndef DDRec_MaterialManager_H_
#define DDRec_MaterialManager_H_

#include "DD4hep/Objects.h"
#include "DDSurfaces/Vector3D.h"
#include "DDRec/Material.h"
#include "DD4hep/TGeoUnits.h"

#include <vector>


class TGeoManager ;

namespace DD4hep {
  namespace DDRec {

    //  export Geometry::Material to this namespace ;
    using  Geometry::Material ;

    typedef std::vector< std::pair< Material, double > > MaterialVec ;
    
    /** Material manager provides access to the material properties of the detector.
     *  Material can be accessed either for a given point or as a list of materials along a straight
     *  line between two points.
     *
     * @author F.Gaede, DESY
     * @date May, 19 2014
     * @version $Id:$
     */
    class MaterialManager {

    public:

      MaterialManager();
      
      ~MaterialManager();
      
      /** Get a vector with all the materials between the two points p0 and p1 with the corresponding thicknesses -
       *  element type is  std::pair< Material, double >. Materials with a thickness smaller than epsilon (default 1e-4=1mu)
       *  are ignored. Avoid calling this method in inner loops as the computation is not cheap. Ideally the result should be cached,
       *  for example as an averaged material @see createAveragedMaterial().
       */
      const MaterialVec& materialsBetween(const DDSurfaces::Vector3D& p0, const DDSurfaces::Vector3D& p1 , double epsilon=1e-4 ) ;

      /** Get the material at the given position.
       */
      const Material& materialAt(const DDSurfaces::Vector3D& pos );


      /** Create a material with averaged properties from all materials in the list. 
       *  A and Z are averaged by relative number of atoms(molecules), rho is averaged by relative volume
       *  and the inverse radiation and interaction lengths are averaged by relative weight. 
       */
      MaterialData createAveragedMaterial( const MaterialVec& materials ) ;

    protected :

      //cached materials
      MaterialVec  _mV ;
      Material _m ;

      // cached last points
      DDSurfaces::Vector3D _p0 , _p1, _pos ;

      TGeoManager* _tgeoMgr ;
    };

   
    /// dump Material operator 
    inline std::ostream& operator<<( std::ostream& os , const Material& m ) {
      os << "  " << m.name() << " Z: " << m.Z() << " A: " << m.A() << " densitiy: " << m.density() 
	 << " radiationLength: "  <<  m.radLength() 
	 << " interactionLength: " << m.intLength() ;
      return os ;
    }
    

    /// dump MaterialVec operator 
    inline std::ostream& operator<<( std::ostream& os , const MaterialVec& m ) {

      for( unsigned i=0,n=m.size() ; i<n ; ++i ) {
	os << "  material: " << m[i].first << " thickness: " << m[i].second << std::endl ; 
      }
      return os ;
    }

  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_MaterialManager_H_
