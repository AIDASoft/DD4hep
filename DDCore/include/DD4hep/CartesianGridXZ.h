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
#ifndef DD4HEP_DDCORE_CARTESIANGRIDXZ_H 
#define DD4HEP_DDCORE_CARTESIANGRIDXZ_H 1

// Framework include files
#include "DD4hep/Objects.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for base segmentations
  namespace DDSegmentation  {    class CartesianGridXZ;  }
  
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class Segmentation;
    
    /// Implementation class for the grid XZ segmentation.
    /**
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_GEOMETRY
     */
    class CartesianGridXZ : public Handle<DDSegmentation::CartesianGridXZ>  {
    public:
      /// Defintiion of the basic handled object
      typedef DDSegmentation::CartesianGridXZ Object;

    public:
      /// Default constructor
      CartesianGridXZ() : Handle<Object>() {}
      /// Copy constructor from handle
      CartesianGridXZ(const Handle<Object>& e) : Handle<Object>(e) {}
      /// Copy constructor
      CartesianGridXZ(const CartesianGridXZ& e) = default;
      /// Copy Constructor from segmentation base object
      CartesianGridXZ(const Segmentation& e);
      /// Assignment operator
      CartesianGridXZ& operator=(const CartesianGridXZ& seg) = default;
      /// Equality operator
      bool operator==(const CartesianGridXZ& seg) const {
        return m_element == seg.m_element;
      }

      /// determine the position based on the cell ID
      Position position(const CellID& cellID) const;
      /// determine the cell ID based on the position
      CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const;
      /// access the grid size in X
      double gridSizeX() const;
      /// access the grid size in Z
      double gridSizeZ() const;
      /// access the coordinate offset in X
      double offsetX() const;
      /// access the coordinate offset in Z
      double offsetZ() const;
      /// access the field name used for X
      const std::string& fieldNameX() const;
      /// access the field name used for Z
      const std::string& fieldNameZ() const;
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
#endif // DD4HEP_DDCORE_CARTESIANGRIDXZ_H
