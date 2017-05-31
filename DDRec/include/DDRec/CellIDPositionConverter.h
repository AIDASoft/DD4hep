#ifndef CellIDPositionConverter_H_
#define CellIDPositionConverter_H_

#include "DD4hep/Readout.h"
#include "DD4hep/VolumeManager.h"

#include "DDSegmentation/Segmentation.h"

#include <set>
#include <string>

class TGeoManager;

namespace DD4hep {
  namespace DDRec {

    typedef DDSegmentation::CellID CellID;
    typedef DDSegmentation::VolumeID VolumeID;

    /** Utility for position to cell ID and cell ID to position conversions.
     *  (Correctly re-implements some of the functionality of the deprecated IDDecoder).
     *
     * @author F.Gaede, DESY
     * @date May 2017
     */

    class CellIDPositionConverter {
      
      CellIDPositionConverter() = delete ;
      CellIDPositionConverter(const CellIDPositionConverter&) = delete ;
      void operator=(const CellIDPositionConverter&) = delete ;
      
    public:
      
      /// The constructor - takes the main lcdd object.
      CellIDPositionConverter( Geometry::LCDD& lcdd ) {
	_volumeManager = Geometry::VolumeManager::getVolumeManager(lcdd);
      }

      /// Destructor
      virtual ~CellIDPositionConverter(){} ;
      
      /// returns the global cell ID from a given global position
      CellID cellID(const Geometry::Position& global) const;
      
      /** Return the nominal global position for a given cellID of a sensitive volume.
       *  No Alignment corrections are applied.
       *  If no sensitive volume is found, (0,0,0) is returned.
       */
      Geometry::Position positionNominal(const CellID& cellID) const;
      
      /** Return the global position for a given cellID of a sensitive volume.
       *  Alignment corrections are applied (TO BE DONE).
       *  If no sensitive volume is found, (0,0,0) is returned.
       */
      Geometry::Position position(const CellID& cellID) const;

      /** Find the context with DetElement, placements etc for a given cellID of a sensitive volume.
       *  Returns NULL if not found (e.g. if the cellID does not correspond to a sensitive volume).
       */
      const Geometry::VolumeManagerContext* findContext(const CellID& cellID) const;


      /** Find the readout object for the given DetElement. If the DetElement is sensitive the corresondig 
       *  Readout is returned, else a recursive search in the daughter volumes (nodes) of this DetElement's
       *  volume is performed and the first Readout object is returned. 
       *  
       */
      Geometry::Readout findReadout(const Geometry::DetElement& det) const ;


      /** Return this PlacedVolume's Readout or, if the volume is not sensitive, recursively search for 
       * a Readout object in the daughter nodes (volumes).
       */
      Geometry::Readout findReadout(const Geometry::PlacedVolume& pv) const ;


    protected:
      Geometry::VolumeManager _volumeManager{} ;

    };

  } /* namespace DDRec */
} /* namespace DD4hep */
#endif /* CellIDPositionConverter_H_ */
