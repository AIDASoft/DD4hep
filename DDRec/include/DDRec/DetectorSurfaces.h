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
#ifndef DDREC_DETECTORSURFACES_H
#define DDREC_DETECTORSURFACES_H

#include "DDRec/Surface.h"

namespace dd4hep {
  namespace rec {

    /** View on DetElements for accessing the surfaces assigned to this DetElement
     *  in detector construction.
     *
     * @author F.Gaede, DESY
     * @date Apr, 10 2014
     * @version $Id$
     */
    class DetectorSurfaces: virtual public DetElement {

    public:
      typedef DetElement DetElement;

      DetectorSurfaces(const DetElement& e);

      virtual ~DetectorSurfaces();

      /// get the list of surfaces added to this DetElement
      const SurfaceList& surfaceList() { return *_sL ; }

    protected :
      SurfaceList* _sL ;

      /// initializes surfaces from VolSurfaces assigned to this DetElement in detector construction
      void initialize() ;

    };

  } /* namespace rec */
} /* namespace dd4hep */



#endif // DDREC_DETECTORSURFACES_H
