/*
 * Segmentation.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/Segmentation.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace dd4hep {

  namespace DDSegmentation {

    using std::cerr;
    using std::endl;
    using std::map;
    using std::runtime_error;
    using std::stringstream;
    using std::vector;

    /// Default constructor used by derived classes passing the encoding string
    Segmentation::Segmentation(const std::string& cellEncoding) :
      _name("Segmentation"), _type("Segmentation"), _decoder(new BitFieldCoder(cellEncoding)), _ownsDecoder(true) {

    }

    /// Default constructor used by derived classes passing an existing decoder
    Segmentation::Segmentation(const BitFieldCoder* newDecoder) :
      _name("Segmentation"), _type("Segmentation"), _decoder(newDecoder), _ownsDecoder(false) {
    }

    /// Destructor
    Segmentation::~Segmentation() {
      if (_ownsDecoder and _decoder != 0) {
        delete _decoder;
      }
      for (auto& p : _parameters)  {
        if ( p.second ) delete p.second;
      }
      _parameters.clear();
    }
  
    /// Add subsegmentation. Call only valid for Multi-segmentations. Default implementation throws an exception
    void Segmentation::addSubsegmentation(long /* key_min */, long /* key_max */, Segmentation* /* entry */)   {
      throw std::runtime_error("This segmentation type:"+_type+" does not support sub-segmentations.");
    }

    /// Determine the volume ID from the full cell ID by removing all local fields
    VolumeID Segmentation::volumeID(const CellID& cID) const {
      map<std::string, StringParameter>::const_iterator it;
      VolumeID vID = cID ;
      for (it = _indexIdentifiers.begin(); it != _indexIdentifiers.end(); ++it) {
        std::string identifier = it->second->typedValue();
        _decoder->set(vID,identifier,0);
      }
      return vID;
    }

    /// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
    void Segmentation::neighbours(const CellID& cID, std::set<CellID>& cellNeighbours) const {
      map<std::string, StringParameter>::const_iterator it;
      for (it = _indexIdentifiers.begin(); it != _indexIdentifiers.end(); ++it) {
        const std::string& identifier = it->second->typedValue();
        CellID nID = cID ;
        int currentValue = _decoder->get(cID,identifier);
        // add both neighbouring cell IDs, don't add out of bound indices
        try {
          _decoder->set(nID,identifier,currentValue - 1);
          cellNeighbours.insert(nID);
        } catch (runtime_error& e) {
          // nothing to do
        }
        try {
          _decoder->set(nID,identifier,currentValue + 1);
          cellNeighbours.insert(nID);
        } catch (runtime_error& e) {
          // nothing to do
        }
      }
    }

    /// Set the underlying decoder
    void Segmentation::setDecoder(const BitFieldCoder* newDecoder) {
      if ( _decoder == newDecoder )
        return; //self assignment
      else if (_ownsDecoder)
        delete _decoder;
      _decoder = newDecoder;
      _ownsDecoder = false;
    }

    /// Access to parameter by name
    Parameter Segmentation::parameter(const std::string& parameterName) const {
      map<std::string, Parameter>::const_iterator it = _parameters.find(parameterName);
      if (it != _parameters.end()) {
        return it->second;
      }
      stringstream s;
      s << "Unknown parameter " << parameterName << " for segmentation type " << _type;
      throw std::runtime_error(s.str());
    }

    /// Access to all parameters
    Parameters Segmentation::parameters() const {
      Parameters pars;
      for ( const auto& it : _parameters )
        pars.push_back(it.second);
      return pars;
    }

    /// Set all parameters from an existing set of parameters
    void Segmentation::setParameters(const Parameters& pars) {
      for ( const auto* p : pars )
        parameter(p->name())->value() = p->value();
    }

    /// Add a cell identifier to this segmentation. Used by derived classes to define their required identifiers
    void Segmentation::registerIdentifier(const std::string& idName, const std::string& idDescription, std::string& identifier,
                                          const std::string& defaultValue) {
      StringParameter idParameter =
        new TypedSegmentationParameter<std::string>(idName, idDescription, identifier, defaultValue,
                                                    SegmentationParameter::NoUnit, true);
      _parameters[idName]       = idParameter;
      _indexIdentifiers[idName] = idParameter;
    }

    /// Helper method to convert a bin number to a 1D position
    double Segmentation::binToPosition(long64 bin, double cellSize, double offset) {
      return bin * cellSize + offset;
    }

    /// Helper method to convert a 1D position to a cell ID
    int Segmentation::positionToBin(double position, double cellSize, double offset) {
      if (cellSize <= 1e-10) {
        throw runtime_error("Invalid cell size: 0.0");
      }
      return int(floor((position + 0.5 * cellSize - offset) / cellSize));
    }

    /// Helper method to convert a bin number to a 1D position given a vector of binBoundaries
    double Segmentation::binToPosition(CellID bin, std::vector<double> const& cellBoundaries, double offset) {
      return (cellBoundaries[bin+1] + cellBoundaries[bin])*0.5 + offset;
    }
    /// Helper method to convert a 1D position to a cell ID given a vector of binBoundaries
    int Segmentation::positionToBin(double position, std::vector<double> const& cellBoundaries, double offset) {

      // include the lower edge to the segmentation, deal with numerical issues
      if(fabs(position/cellBoundaries.front()-1.0) < 3e-12) return 0;

      // include the upper edge of the last bin to the segmentation, deal with numerical issues
      if(fabs(position/cellBoundaries.back()-1.0)  < 3e-12) return int(cellBoundaries.size()-2);

      // hits outside cannot be treated
      if(position < cellBoundaries.front()) {
        std::stringstream err;
        err << std::setprecision(20) << std::scientific;
        err << "Hit Position (" << position << ") is below the acceptance"
            << " (" << cellBoundaries.front() << ") "
            << "of the segmentation";
        throw std::runtime_error(err.str());
      }
      if(position > cellBoundaries.back() ) {
        std::stringstream err;
        err << std::setprecision(20) << std::scientific;
        err << "Hit Position (" << position << ") is above the acceptance"
            << " (" << cellBoundaries.back() << ") "
            << "of the segmentation";
        throw std::runtime_error(err.str());
      }
      std::vector<double>::const_iterator bin = std::upper_bound(cellBoundaries.begin(),
                                                                 cellBoundaries.end(),
                                                                 position-offset);
      // need to reduce found bin by one, because upper_bound works that way, lower_bound would not help
      return bin - cellBoundaries.begin() - 1 ;

    }

    std::vector<double> Segmentation::cellDimensions(const CellID&) const {
      std::stringstream errorMessage;
      errorMessage << __func__ << " is not implemented for " << _type;
      throw std::logic_error(errorMessage.str());
    }

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
