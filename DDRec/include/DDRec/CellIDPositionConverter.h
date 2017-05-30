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

    /** CellIDPositionConverter - utility class that
     *  combines functionality of the VolumeManager and Segmentation classes to provide a
     *  high level interface for position to cell ID and cell ID to position conversions
     *  and related information.
     *
     *
     * @author F.Gaede, DESY ( based on IDDecoder by C.Grefe, CERN)
     * @date May 2017
     */

    class CellIDPositionConverter {
      
      CellIDPositionConverter() = delete ;
      CellIDPositionConverter(const CellIDPositionConverter&) = delete ;
      void operator=(const CellIDPositionConverter&) = delete ;
      
    public:
      
      /// Default constructor
      CellIDPositionConverter( Geometry::LCDD& lcdd ) {
	_volumeManager = Geometry::VolumeManager::getVolumeManager(lcdd);

	std::cout << " VolumeManager: \n"  << _volumeManager << std::endl ;
      }

      /// Destructor
      virtual ~CellIDPositionConverter(){} ;
      
      /// Returns the cell ID from the local position in the given volume ID.
      CellID cellIDFromLocal(const Geometry::Position& local, const VolumeID volumeID) const;
      
      /// Returns the global cell ID from a given global position
      CellID cellID(const Geometry::Position& global) const;
      
      /// Returns the global position from a given cell ID
      Geometry::Position position(const CellID& cellID) const;
      
      /// Returns the local position from a given cell ID
      Geometry::Position localPosition(const CellID& cellID) const;
      
      /// Returns the volume ID of a given cell ID
      VolumeID volumeID(const CellID& cellID) const;
      
      /// Returns the volume ID of a given global position
      VolumeID volumeID(const Geometry::Position& global) const;
      
      /// Returns the placement for a given cell ID
      Geometry::PlacedVolume placement(const CellID& cellID) const;

      /// Returns the placement for a given global position
      Geometry::PlacedVolume placement(const Geometry::Position& global) const;

      /// Returns the subdetector for a given cell ID
      Geometry::DetElement subDetector(const CellID& cellID) const;

      /// Returns the subdetector for a given global position
      Geometry::DetElement subDetector(const Geometry::Position& global) const;

      /// Returns the closest detector element in the hierarchy for a given cell ID
      Geometry::DetElement detectorElement(const CellID& cellID) const;

      /// Returns the closest detector element in the hierarchy for a given global position
      Geometry::DetElement detectorElement(const Geometry::Position& global) const;

      /// Access to the Readout object for a given cell ID
      Geometry::Readout readout(const CellID& cellID) const;

      /// Access to the Readout object for a given global position
      Geometry::Readout readout(const Geometry::Position& global) const;

      /// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
      void neighbours(const CellID& cellID, std::set<CellID>& neighbours) const;

      /// Checks if the given cell IDs are neighbours
      bool areNeighbours(const CellID& cellID, const CellID& otherCellID) const;

      /// find the context with DetElement, placements etc for a given cellID of a sensitive volume.
      DD4hep::Geometry::VolumeManagerContext* findContext(const CellID& cellID) const;

      
    protected:
      Geometry::VolumeManager _volumeManager{} ;

      Geometry::Readout findReadout(const Geometry::DetElement& det) const ;
      Geometry::DetElement getClosestDaughter(const Geometry:: DetElement& det, const Geometry::Position& position) const ;
      Geometry::Readout findReadout(const Geometry::PlacedVolume& pv) const ;


    };

  } /* namespace DDRec */
} /* namespace DD4hep */
#endif /* CellIDPositionConverter_H_ */
