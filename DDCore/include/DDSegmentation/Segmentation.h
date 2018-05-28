//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

/*
 * Segmentation.h
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_SEGMENTATION_H_
#define DDSegmentation_SEGMENTATION_H_

#include "DDSegmentation/BitFieldCoder.h"
#include "DDSegmentation/SegmentationFactory.h"
#include "DDSegmentation/SegmentationParameter.h"

#include <map>
#include <utility>
#include <set>
#include <string>
#include <vector>

namespace dd4hep {
  namespace DDSegmentation {

    typedef SegmentationParameter* Parameter;
    typedef std::vector<Parameter> Parameters;
    typedef TypedSegmentationParameter<int>* IntParameter;
    typedef TypedSegmentationParameter<float>* FloatParameter;
    typedef TypedSegmentationParameter<double>* DoubleParameter;
    typedef TypedSegmentationParameter<std::string>* StringParameter;
    typedef TypedSegmentationParameter<std::vector<int> >* IntVecParameter;
    typedef TypedSegmentationParameter<std::vector<float> >* FloatVecParameter;
    typedef TypedSegmentationParameter<std::vector<double> >* DoubleVecParameter;
    typedef TypedSegmentationParameter<std::vector<std::string> >* StringVecParameter;
    typedef SegmentationParameter::UnitType UnitType;

    /// Useful typedefs to differentiate cell IDs and volume IDs
    typedef long long int CellID;
    typedef long long int VolumeID;

    /// Simple container for a physics vector
    struct Vector3D {
      /// Default constructor
      Vector3D(double x_val = 0., double y_val = 0., double z_val = 0.) :
        X(x_val), Y(y_val), Z(z_val) {
      }
      /// Constructor using a foreign vector class. Requires methods x(), y() and z()
      template<typename T> Vector3D(const T& v) {
        X = v.x();
        Y = v.y();
        Z = v.Z();
      }
      /// Access to x value (required for use with ROOT GenVector)
      double x() const {
        return X;
      }
      /// Access to y value (required for use with ROOT GenVector)
      double y() const {
        return Y;
      }
      /// Access to z value (required for use with ROOT GenVector)
      double z() const {
        return Z;
      }
      double X, Y, Z;
    };

    /// Base class for all segmentations
    class Segmentation {
    public:
      /// Destructor
      virtual ~Segmentation();

      /// Add subsegmentation. Call only valid for Multi-segmentations. Default implementation throws an exception
      virtual void addSubsegmentation(long key_min, long key_max, Segmentation* entry);
      /// Determine the local position based on the cell ID
      virtual Vector3D position(const CellID& cellID) const = 0;
      /// Determine the cell ID based on the position
      virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition,
                            const VolumeID& volumeID) const = 0;
      /// Determine the volume ID from the full cell ID by removing all local fields
      virtual VolumeID volumeID(const CellID& cellID) const;
      /// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
      virtual void neighbours(const CellID& cellID, std::set<CellID>& neighbours) const;
      /// Access the encoding string
      virtual std::string fieldDescription() const {
        return _decoder->fieldDescription();
      }
      /// Access the segmentation name
      virtual const std::string& name() const {
        return _name;
      }
      /// Set the segmentation name
      virtual void setName(const std::string& value) {
        _name = value;
      }
      /// Access the segmentation type
      virtual const std::string& type() const {
        return _type;
      }
      /// Access the description of the segmentation
      virtual const std::string& description() const {
        return _description;
      }
      /// Access the underlying decoder
      virtual const BitFieldCoder* decoder()  const {
        return _decoder;
      }
      /// Set the underlying decoder
      virtual void setDecoder(const BitFieldCoder* decoder);
      /// Access to parameter by name
      virtual Parameter parameter(const std::string& parameterName) const;
      /// Access to all parameters
      virtual Parameters parameters() const;
      /// Set all parameters from an existing set of parameters
      virtual void setParameters(const Parameters& parameters);
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

          \param cellID cellID of the cell for which parameters are returned
          \return vector<double> in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi
      */
      virtual std::vector<double> cellDimensions(const CellID& cellID) const;

    protected:
      /// Default constructor used by derived classes passing the encoding string
      Segmentation(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      Segmentation(const BitFieldCoder* decoder);

      /// Add a parameter to this segmentation. Used by derived classes to define their parameters
      template<typename TYPE> void registerParameter(const std::string& nam, const std::string& desc,
                                                     TYPE& param, const TYPE& defaultVal, UnitType unitTyp = SegmentationParameter::NoUnit,
                                                     bool isOpt = false) {
        _parameters[nam] = new TypedSegmentationParameter<TYPE>(nam,desc,param,defaultVal,unitTyp,isOpt);
      }
      /// Add a cell identifier to this segmentation. Used by derived classes to define their required identifiers
      void registerIdentifier(const std::string& nam, const std::string& desc, std::string& ident,
                              const std::string& defaultVal);

      /// Helper method to convert a bin number to a 1D position
      static double binToPosition(CellID bin, double cellSize, double offset = 0.);
      /// Helper method to convert a 1D position to a cell ID
      static int positionToBin(double position, double cellSize, double offset = 0.);

      /// Helper method to convert a bin number to a 1D position given a vector of binBoundaries
      static double binToPosition(CellID bin, std::vector<double> const& cellBoundaries, double offset = 0.);
      /// Helper method to convert a 1D position to a cell ID given a vector of binBoundaries
      static int positionToBin(double position, std::vector<double> const& cellBoundaries, double offset = 0.);

      /// The segmentation name
      std::string _name;
      /// The segmentation type
      std::string _type;
      /// The description of the segmentation
      std::string _description;
      /// The parameters for this segmentation
      std::map<std::string, Parameter> _parameters;   //! No ROOT persistency
      /// The indices used for the encoding
      std::map<std::string, StringParameter> _indexIdentifiers;   //! No ROOT persistency
      /// The cell ID encoder and decoder
      const BitFieldCoder* _decoder = 0;
      /// Keeps track of the decoder ownership
      bool _ownsDecoder = false;
    private:
      /// No copy constructor allowed
      Segmentation(const Segmentation&);
    };

    /// Macro to instantiate a new SegmentationCreator by its type name
#define REGISTER_SEGMENTATION(classname)                                \
    static const SegmentationCreator<classname> classname##_creator(#classname);

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_SEGMENTATION_H_ */
