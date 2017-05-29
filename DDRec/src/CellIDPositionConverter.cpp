
#include "DDRec/CellIDPositionConverter.h"
#include "DDRec/API/Exceptions.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/VolumeManager.h"

namespace DD4hep {
  namespace DDRec {

    using Geometry::DetElement;
    using Geometry::LCDD;
    using Geometry::PlacedVolume;
    using Geometry::Readout;
    using Geometry::Solid;
    using Geometry::VolumeManager;
    using Geometry::Volume;
    using Geometry::SensitiveDetector;
    using Geometry::Position;
    using std::set;



    CellID CellIDPositionConverter::cellIDFromLocal(const Position& local, const VolumeID volID) const {
      double l[3];
      double g[3];
      local.GetCoordinates(l);

      // DetElement det = this->detectorElement(volID);
      // const TGeoMatrix& localToGlobal = det.nominal().worldTransformation();
      // localToGlobal.LocalToMaster(l, g);
      // Position global(g[0], g[1], g[2]);
      // return this->findReadout(det).segmentation().cellID(local, global, volID);
      
      PlacedVolume pv = placement( volID ) ;
      Volume v = pv.volume() ;
      SensitiveDetector sens = v.sensitiveDetector() ;
      Readout r = sens.readout() ;

      return r.segmentation().cellID( local, Position(), volID ) ;
    }

    /**
     * Returns the global cell ID from a given global position
     */
    CellID CellIDPositionConverter::cellID(const Position& global) const {
      VolumeID volID = volumeID(global);
      double l[3];
      double g[3];
      global.GetCoordinates(g);
      DetElement det = this->detectorElement(volID);
      const TGeoMatrix& localToGlobal = det.nominal().worldTransformation();
      localToGlobal.MasterToLocal(g, l);
      Position local(l[0], l[1], l[2]);
      return this->findReadout(det).segmentation().cellID(local, global, volID);
    }

    /**
     * Returns the global position from a given cell ID
     */
    Position CellIDPositionConverter::position(const CellID& cell) const {
      double l[3];
      double g[3];
      DetElement det = this->detectorElement(cell);
      Position local = this->findReadout(det).segmentation().position(cell);
      local.GetCoordinates(l);
      // FIXME: direct lookup of transformations seems to be broken
      //const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(cell);
      const TGeoMatrix& localToGlobal = det.nominal().worldTransformation();
      localToGlobal.LocalToMaster(l, g);
      return Position(g[0], g[1], g[2]);
    }

    /*
     * Returns the local position from a given cell ID
     */
    Position CellIDPositionConverter::localPosition(const CellID& cell) const {
      DetElement det = this->detectorElement(cell);
      return this->findReadout(det).segmentation().position(cell);
    }

    /*
     * Returns the volume ID of a given cell ID
     */
    VolumeID CellIDPositionConverter::volumeID(const CellID& cell) const {
      DetElement det = this->detectorElement(cell);
      return this->findReadout(det).segmentation().volumeID(cell);
    }

    /*
     * Returns the volume ID of a given global position
     */
    VolumeID CellIDPositionConverter::volumeID(const Position& pos) const {
      DetElement det = this->detectorElement(pos);
      return det.volumeID();
    }

    /*
     * Returns the placement for a given cell ID
     */
    PlacedVolume CellIDPositionConverter::placement(const CellID& cell) const {
      return _volumeManager.lookupPlacement(cell);
    }

    /*
     * Returns the placement for a given global position
     */
    PlacedVolume CellIDPositionConverter::placement(const Position& pos) const {
      return placement(volumeID(pos));
    }

    /*
     * Returns the subdetector for a given cell ID
     */
    DetElement CellIDPositionConverter::subDetector(const CellID& cell) const {
      return _volumeManager.lookupDetector(cell);
    }

    /*
     * Returns the subdetector for a given global position
     */
    DetElement CellIDPositionConverter::subDetector(const Position& pos) const {
      return subDetector(volumeID(pos));
    }

    /*
     * Returns the closest detector element in the hierarchy for a given cell ID
     */
    DetElement CellIDPositionConverter::detectorElement(const CellID& cell) const {
      return _volumeManager.lookupDetElement(cell);
    }

    /*
     * Returns the closest detector element in the hierarchy for a given global position
     */
    DetElement CellIDPositionConverter::detectorElement(const Position& pos) const {
      DetElement world = Geometry::LCDD::getInstance().world();
      DetElement det = getClosestDaughter(world, pos);
      if (not det.isValid()) {
	throw invalid_position("DD4hep::DDRec::CellIDPositionConverter::detectorElement", pos);
      }
      std::cout << det.name() << std::endl;
      return det;
    }

    /// Access to the Readout object for a given cell ID
    Geometry::Readout CellIDPositionConverter::readout(const CellID& cell) const {
      DetElement det = this->detectorElement(cell);
      return this->findReadout(det);
    }

    /// Access to the Readout object for a given global position
    Geometry::Readout CellIDPositionConverter::readout(const Position& global) const {
      DetElement det = this->detectorElement(global);
      return this->findReadout(det);
    }

    /*
     * Calculates the neighbours of the given cell ID and adds them to the list of neighbours
     */
    void CellIDPositionConverter::neighbours(const CellID& cell, set<CellID>& neighbour_cells) const {
      DetElement det = this->detectorElement(cell);
      this->findReadout(det).segmentation().neighbours(cell, neighbour_cells);
    }

    /*
     * Checks if the given cell IDs are neighbours
     */
    bool CellIDPositionConverter::areNeighbours(const CellID& cell, const CellID& otherCellID) const {
      set<CellID> neighbour_cells;
      DetElement det = this->detectorElement(cell);
      this->findReadout(det).segmentation().neighbours(cell, neighbour_cells);
      return neighbour_cells.count(otherCellID) != 0;
    }

    //    long int CellIDPositionConverter::layerIndex(const CellID& cell) const {
    //   Readout r = this->readout(cell);
    //   return r.idSpec().field(this->layerIdentifier())->value(cell);
    // }

    // /// Access to the system index
    // long int CellIDPositionConverter::systemIndex(const CellID& cell) const {
    //   Readout r = this->readout(cell);
    //   return r.idSpec().field(this->systemIdentifier())->value(cell);
    // }

    // helper method to find the corresponding Readout object to a DetElement

    Readout CellIDPositionConverter::findReadout(const Geometry::DetElement& det) const {

      // first check if top level is a sensitive detector
      if (det.volume().isValid() and det.volume().isSensitive()) {
	Geometry::SensitiveDetector sd = det.volume().sensitiveDetector();
	if (sd.isValid() and sd.readout().isValid()) {
	  return sd.readout();
	}
      }
      // then return the first sensitive daughter volume's readout

      Readout r = findReadout( det.placement() ) ;
      if( r.isValid() )
	return r ;

      // nothing found !?
      return Readout();
    }

    // Readout CellIDPositionConverter::findReadout(const Geometry::DetElement& det) const {

    //   // first check if top level is a sensitive detector
    //   if (det.volume().isValid() and det.volume().isSensitive()) {
    // 	Geometry::SensitiveDetector sd = det.volume().sensitiveDetector();
    // 	if (sd.isValid() and sd.readout().isValid()) {
    // 	  return sd.readout();
    // 	}
    //   }

    //   // check all children recursively for the first valid Readout object
    //   const DetElement::Children& children = det.children();
    //   DetElement::Children::const_iterator it = children.begin();
    //   while (it != children.end()) {
    // 	Readout r = findReadout(it->second);
    // 	if (r.isValid()) {
    // 	  return r;
    // 	}
    // 	++it;
    //   }

    //   // neither this or any daughter is sensitive
    //   return Readout();
    // }

    Readout CellIDPositionConverter::findReadout(const Geometry::PlacedVolume& pv) const {
      
      // first check if we are in a sensitive volume
      if( pv.volume().isSensitive() ){
	Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
      	if (sd.isValid() and sd.readout().isValid()) {
	  return sd.readout();
	}
      }

      // traverse the daughter nodes:
      const TGeoNode* node = pv.ptr();
      
      for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
	  
	TGeoNode* daughter = node->GetDaughter(idau);
	PlacedVolume dpv( daughter );
	Readout r = findReadout( dpv ) ;
	if( r.isValid() )
	  return r ;
      }

      return Readout() ;
    }

      // helper method to get the closest daughter DetElement to the position starting from the given DetElement
    DetElement CellIDPositionConverter::getClosestDaughter(const DetElement& det, const Position& position) const {
      DetElement result;

      // check if we have a shape and see if we are inside
      if (det.volume().isValid() and det.volume().solid().isValid()) {
	double globalPosition[3] = { position.x(), position.y(), position.z() };
	double localPosition[3] = { 0., 0., 0. };
	det.nominal().worldTransformation().MasterToLocal(globalPosition, localPosition);
	if (det.volume().solid()->Contains(localPosition)) {
	  result = det;
	} else {
	  // assuming that any daughter shape would be inside this shape
	  return DetElement();
	}
      }

      const DetElement::Children& children = det.children();
      DetElement::Children::const_iterator it = children.begin();
      while (it != children.end()) {
	DetElement daughterDet = getClosestDaughter(it->second, position);
	if (daughterDet.isValid()) {
	  result = daughterDet;
	  break;
	}
	++it;
      }
      return result;
    }

  } /* namespace DDRec */
} /* namespace DD4hep */
