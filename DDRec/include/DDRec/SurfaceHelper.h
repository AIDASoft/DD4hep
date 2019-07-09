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
#ifndef rec_SurfaceHelper_H_
#define rec_SurfaceHelper_H_

#include "DDRec/Surface.h"

namespace dd4hep {
  namespace rec {

    /** Surface helper class that allows to access all surfaces
     *  assigned to a DetElement and all its daughters.
     *  (originally this was called SurfaceManager).
     *
     * @author F.Gaede, DESY
     * @date Apr, 11 2014
     * @version $Id: $
     */
    class SurfaceHelper {

    public:

      SurfaceHelper(const DetElement& e);
      
      ~SurfaceHelper();
      
      /** Get the list of all surfaces added to this DetElement and all its daughters -
       *  instantiate SurfaceHelper with description.world() to get all surfaces.
       */
      const SurfaceList& surfaceList() { return _sL ; }

    protected :
      SurfaceList  _sL ;
      const DetElement& _det ;

      /// initializes surfaces from VolSurfaces assigned to this DetElement in detector construction
      void initialize() ;

    };

  } /* namespace rec */
} /* namespace dd4hep */



#endif // rec_SurfaceHelper_H_
