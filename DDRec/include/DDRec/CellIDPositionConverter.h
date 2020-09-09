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
#ifndef DDREC_CELLIDPOSITIONCONVERTER_H
#define DDREC_CELLIDPOSITIONCONVERTER_H

#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"
#include "DD4hep/VolumeManager.h"

#include "DDSegmentation/Segmentation.h"

#include <set>
#include <string>


namespace dd4hep {
  namespace rec {

    typedef DDSegmentation::CellID CellID;
    typedef DDSegmentation::VolumeID VolumeID;

    /** Utility for position to cellID and cellID to position conversions.
     *  (Correctly re-implements some of the functionality of the deprecated IDDecoder).
     *
     * @author F.Gaede, DESY
     * @date May 2017
     */

    class CellIDPositionConverter {
#if defined(G__ROOT)
      CellIDPositionConverter() = default ;
#else
      CellIDPositionConverter() = delete ;
#endif
      CellIDPositionConverter(const CellIDPositionConverter&) = delete ;
      void operator=(const CellIDPositionConverter&) = delete ;
      
    public:
      
      /// The constructor - takes the main description object.
      CellIDPositionConverter( Detector& description ) : _description( &description )  {
        _volumeManager = VolumeManager::getVolumeManager(description);
      }

      /// Destructor
      virtual ~CellIDPositionConverter(){} ;
      
      /** Return the nominal global position for a given cellID of a sensitive volume.
       *  No Alignment corrections are applied.
       *  If no sensitive volume is found, (0,0,0) is returned.
       */
      Position positionNominal(const CellID& cellID) const;
      
      /** Return the global position for a given cellID of a sensitive volume.
       *  Alignment corrections are applied (TO BE DONE).
       *  If no sensitive volume is found, (0,0,0) is returned.
       */
      Position position(const CellID& cellID) const;


      /** Return the global cellID for the given global position.
       *  Note: this call is rather slow - only use it when really needed !
       *  
       */
      CellID cellID(const Position& global) const;



      /** Find the context with DetElement, placements etc for a given cellID of a sensitive volume.
       *  Returns NULL if not found (e.g. if the cellID does not correspond to a sensitive volume).
       */
      const VolumeManagerContext* findContext(const CellID& cellID) const;



      /** Find the DetElement that contains the given point - if no DetElement is found, an
       *  invalid DetElement is returned. Uses the optionally given DetElement as start for the search.
       */ 
      DetElement findDetElement(const Position& global,
                                const DetElement& det=DetElement() ) const; 


      /** Find the lowest daughter Placement in the given Placement that
       *  contains the point (in the coordinate system of the mother placement).
       *  Return the local coordinates in this daughter Placement and collect all volIDs 
       *  on the way.
       */
      PlacedVolume findPlacement(const Position& point, const  PlacedVolume& mother, double locPos[3], PlacedVolume::VolIDs& volIDs) const ; 


      /** Find the readout object for the given DetElement. If the DetElement is sensitive the corresondig 
       *  Readout is returned, else a recursive search in the daughter volumes (nodes) of this DetElement's
       *  volume is performed and the first Readout object is returned. 
       *  
       */
      Readout findReadout(const DetElement& det) const ;


      /** Return this PlacedVolume's Readout or, if the volume is not sensitive, recursively search for 
       * a Readout object in the daughter nodes (volumes).
       */
      Readout findReadout(const PlacedVolume& pv) const ;

      /** Returns the segmentation.cellDimensions. 
       *
       */
    std::vector<double> cellDimensions(const CellID& cell) const ;

    protected:
      VolumeManager _volumeManager{} ;
      const Detector* _description ;

    };

  } /* namespace rec */
} /* namespace dd4hep */







#endif // DDREC_CELLIDPOSITIONCONVERTER_H
