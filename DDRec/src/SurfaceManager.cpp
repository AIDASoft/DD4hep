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
#include "DDRec/SurfaceManager.h"

#include "DDRec/SurfaceHelper.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/Detector.h"

#include <sstream>

namespace dd4hep {

  using namespace detail ;

  namespace rec {


    SurfaceManager::SurfaceManager(const Detector& theDetector) :
      m_detector(theDetector)
    {

      // have to make sure the volume manager is populated once in order to have
      // the volumeIDs attached to the DetElements

      VolumeManager::getVolumeManager(theDetector);

      std::set<std::string> worldSet{};
      // collect all detector names that belong to a type e.g.: tracker, calorimeter, passive
      for (const std::string& typeName : theDetector.detectorTypes()) {
        m_detectorTypes.insert(typeName);
        std::set<std::string> nameSet{};
        for (const DetElement& det : theDetector.detectors(typeName)) {
          m_detectorNames.insert(det.name());
          nameSet.insert(det.name());
        }
        worldSet.insert(nameSet.begin(), nameSet.end());
        m_missingDetectors.emplace(typeName, std::move(nameSet));
      }
      m_missingDetectors.emplace("world", std::move(worldSet));
    }

    SurfaceManager::~SurfaceManager(){
      // nothing to do
    }


    const SurfaceMap* SurfaceManager::map( const std::string name ) {

      // return map if it already exists and is complete
      SurfaceMapsMap::const_iterator it = _map.find( name ) ;
      if(it != _map.end() && it->second.first) {
          return &it->second.second ;
      }

      // surface map does not exist or is not completed yet, build it

      bool isDetector = m_detectorNames.find(name) != m_detectorNames.end();
      bool isDetectorType = m_detectorTypes.find(name) != m_detectorTypes.end();
      bool isWorld = name == "world";

      if (isDetector) {
        // name is a detector: easiest case
        return &getOrConstruct(name);
      } else if (!isDetectorType && !isWorld) {
        // neither a type name nor world -> invalid
        // TODO: maybe do an additional debug printout (not done in original implementation!)
        return 0;
      }

      // if we arrived here we were asked for a type map like tracker or world
      // get missing detectors, construct them, look up map again and return it
      auto& missingDets = m_missingDetectors[name];
      for (const auto& detName : missingDets) {
        getOrConstruct(detName);
      }

      return &_map[name].second;
    }

    const SurfaceMap& SurfaceManager::getOrConstruct(const std::string name) {
      // check if we already have this map
      {
        auto it = _map.find(name);
        // second condition is always true because we only call this method with name in m_detectorNames
        // i.e. no compounds like tracker or world!
        if(it != _map.end() /* && it->second.first */){
          return it->second.second ;
        }
      } // scope to get rid of it variable that is useless after this

      // map does not exist yet, we need to construct it ourselves
      // if our detector is also in detectorTypes we construct that
      // part of the corresponding surface map also

      std::vector<std::pair<std::string, bool>> types;
      // we also want to construct a map for the detector itself
      // so we just add it first and set it to be valid as we
      // will construct it completely
      types.push_back(std::make_pair(name, true));

      // first, check if it is part of any compounds to also add our surfaces to them
      // remove the name at the same time as we will construct the map for it now
      for (auto& [typeName, nameSet] : m_missingDetectors) {
        bool found = nameSet.erase(name);
        if (found) {
          // we store the typename and if it was the last missing detector of this type
          types.push_back(std::make_pair(typeName, nameSet.empty()));
        }
      }

      // make a vector of references to all the maps that we need to add surfaces to
      std::vector<std::reference_wrapper<std::pair<bool, SurfaceMap>>> maps{};
      for (const auto& [typeName, valid] : types) {
        auto it = _map.find(typeName);
        if (it == _map.end()) {
          // need to create a new empty map in the global _map
          const auto& res = _map.emplace(typeName, std::make_pair(valid, SurfaceMap()));
          maps.push_back(res.first->second);
        } else {
          maps.push_back(it->second);
        }
      }

      // now get the surfaces and put them into the maps
      const DetElement& det = m_detector.detector(name);
      SurfaceHelper surfHelper(det);
      const SurfaceList& surfList = surfHelper.surfaceList();

      for (ISurface* surf : surfList) {
        for (auto& map : maps) {
          map.get().second.emplace(surf->id(), surf);
        }
      }

      // return the map of the detector
      return maps[0].get().second;
    }

    void SurfaceManager::initialize(const Detector& description) {

      const std::vector<std::string>& types = description.detectorTypes() ;

      for(unsigned i=0,N=types.size();i<N;++i){

        const std::vector<DetElement>& dets = description.detectors( types[i] ) ;

        for(unsigned j=0,M=dets.size();j<M;++j){

          std::string name = dets[j].name() ;

          SurfaceHelper surfH( dets[j] ) ;

          const SurfaceList& detSL = surfH.surfaceList() ;

          // add an empty map for this detector in case there are no surfaces attached
          _map.emplace(name , SurfaceMap());

          for( SurfaceList::const_iterator it = detSL.begin() ; it != detSL.end() ; ++it ){
            ISurface* surf =  *it ;

            // enter surface into map for this detector
            _map[ name ].emplace(surf->id(), surf );

            // enter surface into map for detector type
            _map[ types[i] ].emplace(surf->id(), surf );

            // enter surface into world map
            _map[ "world" ].emplace(surf->id(), surf );

          }
        }
      }

    }

    std::string SurfaceManager::toString() const {

      std::stringstream sstr ;

      sstr << "--------  SurfaceManager contains the following maps : --------- " << std::endl ;

      for( SurfaceMapsMap::const_iterator mi = _map.begin() ; mi != _map.end() ; ++mi ) {

        sstr << "  key: " <<  mi->first << " \t number of surfaces : " << mi->second.second.size() << std::endl ;
      }
      sstr << "---------------------------------------------------------------- " << std::endl ;

      return sstr.str() ;
    }


  } // namespace rec
} // namespace dd4hep
