//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-10-18
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_DDCORE_MULTISEGMENTATION_H 
#define DD4HEP_DDCORE_MULTISEGMENTATION_H 1

// Framework include files
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/MultiSegmentation.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class MultiSegmentation;  }
  
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class Segmentation;
    template <typename T> class SegmentationWrapper;
    
    /// We need some abbreviation to make the code more readable.
    typedef Handle<SegmentationWrapper<DDSegmentation::MultiSegmentation> > MultiSegmentationHandle;

    /// Implementation class for the grid XZ segmentation.
    /**
     *  Concrete user handle to serve specific needs of client code
     *  which requires access to the base functionality not served
     *  by the super-class Segmentation.
     *
     *  Note:
     *  We only check the validity of the underlying handle.
     *  If for whatever reason the implementation object is not valid
     *  This is not checked.
     *  In principle this CANNOT happen unless some brain-dead has
     *  fiddled with the handled object directly.....
     *
     *  Note:
     *  The handle base corrsponding to this object in for
     *  conveniance reasons instantiated in DD4hep/src/Segmentations.cpp.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class MultiSegmentation : public MultiSegmentationHandle  {
    public:
      typedef MultiSegmentationHandle::Implementation Object;
      typedef DDSegmentation::MultiSegmentation::Segmentations Segmentations;

    public:
      /// Default constructor
      MultiSegmentation() = default;
      /// Copy constructor
      MultiSegmentation(const MultiSegmentation& e) = default;
      /// Copy Constructor from segmentation base object
      MultiSegmentation(const Segmentation& e) : Handle<Object>(e) {}
      /// Copy constructor from handle
      MultiSegmentation(const Handle<Object>& e) : Handle<Object>(e) {}
      /// Copy constructor from other polymorph/equivalent handle
      template <typename Q> MultiSegmentation(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Assignment operator
      MultiSegmentation& operator=(const MultiSegmentation& seg) = default;
      /// Equality operator
      bool operator==(const MultiSegmentation& seg) const
      {  return m_element == seg.m_element;      }
      /// access the field name used to discriminate sub-segmentations
      const std::string& discriminatorName() const;
      /// Discriminating bitfield entry
      BitFieldValue* discriminator() const;
      /// The underlying sub-segementations
      const Segmentations& subSegmentations()  const;
      /// determine the position based on the cell ID
      Position position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
      const std::string& fieldNameY() const;
      /** \brief Returns a vector<double> of the cellDimensions of the given cell ID
          in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

          Returns a vector of the cellDimensions of the given cell ID
          \param cellID is ignored as all cells have the same dimension
          \return std::vector<double> size 2:
          -# size in x
          -# size in z
      */
      std::vector<double> cellDimensions(const CellID& cellID) const;
    };

  } /* End namespace Geometry              */
} /* End namespace DD4hep                */
#endif // DD4HEP_DDCORE_MULTISEGMENTATION_H
