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
#ifndef rec_MaterialManager_H_
#define rec_MaterialManager_H_

#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"
#include "DDRec/Vector3D.h"
#include "DDRec/Material.h"
#include "DD4hep/DD4hepUnits.h"

#include <vector>


class TGeoManager ;

namespace dd4hep {
  namespace rec {

    typedef std::vector< std::pair< Material, double > >     MaterialVec;
    typedef std::vector< std::pair< PlacedVolume, double > > PlacementVec;
    
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

      /// Instantiate the MaterialManager for this (world) volume
      MaterialManager(Volume world);

#if defined(G__ROOT)
      MaterialManager() = default ;
#else
      MaterialManager() = delete ;
#endif

      ~MaterialManager();
      
      /** Get a vector with all the materials between the two points p0 and p1 with the corresponding thicknesses -
       *  element type is  std::pair< Material, double >. Materials with a thickness smaller than epsilon (default 1e-4=1mu)
       *  are ignored. Avoid calling this method in inner loops as the computation is not cheap. Ideally the result should be cached,
       *  for example as an averaged material @see createAveragedMaterial().
       */
      const MaterialVec& materialsBetween(const Vector3D& p0, const Vector3D& p1 , double epsilon=1e-4 );

      /** Get a vector with all the placements between the two points p0 and p1
       */
      const PlacementVec& placementsBetween(const Vector3D& p0, const Vector3D& p1 , double epsilon=1e-4 );
      
      /** Get the material at the given position.
       */
      const Material& materialAt(const Vector3D& pos );

      /** Get the placed volume at the given position.
       */
      PlacedVolume placementAt(const Vector3D& pos );

      /** Create a material with averaged properties from all materials in the list. 
       *  A and Z are averaged by relative number of atoms(molecules), rho is averaged by relative volume
       *  and the inverse radiation and interaction lengths are averaged by relative weight. 
       */
      MaterialData createAveragedMaterial( const MaterialVec& materials ) ;

    protected :
      /// Cached materials
      MaterialVec  _mV ;
      Material     _m ;
      /// Cached nodes
      PlacedVolume _pv;
      PlacementVec _placeV;
      /// cached last points
      Vector3D     _p0 , _p1, _pos ;
      /// Reference to the TGeoManager
      TGeoManager* _tgeoMgr ;
    };

    /// dump Material operator 
    inline std::ostream& operator<<( std::ostream& os , const Material& m ) {
      os << "  " << m.name() << " Z: " << m.Z() << " A: " << m.A() << " density: " << m.density() 
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

  } /* namespace rec */
} /* namespace dd4hep */



#endif // rec_MaterialManager_H_
