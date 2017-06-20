//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#ifndef DD4HEP_DDCORE_LAYERING_H
#define DD4HEP_DDCORE_LAYERING_H

// Framework include files
#include "XML/XMLElements.h"

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Class to describe the slice of one layer in a layering stack
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class LayerSlice {
  public:
    bool _sensitive;
    double _thickness;
    std::string _material;
    /// Initializing constructor
    LayerSlice(bool sens, double thick, const std::string& mat);
    /// Copy constructor
    LayerSlice(const LayerSlice& copy) = default;
    /// Assignment operator
    LayerSlice& operator=(const LayerSlice& copy) = default;
  };

  /// Initializing constructor
  inline LayerSlice::LayerSlice(bool sens, double thick, const std::string& mat)
    : _sensitive(sens), _thickness(thick), _material(mat) {
  }

  /// Class to describe one layer in a layering stack
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class Layer {
  public:
    double _thickness = 0.0;
    double _preOffset = 0.0;
    std::vector<LayerSlice> _slices;

    /// Default constructor
    Layer() = default;
    /// Copy constructor
    Layer(const Layer& copy) = default;
    /// Assignment operator
    Layer& operator=(const Layer& copy) = default;

    void compute();
    double thickness() const {
      return _thickness;
    }
    double thicknessWithPreOffset() const {
      return _thickness + _preOffset;
    }
    void add(const LayerSlice& slice) {
      _slices.push_back(slice);
    }
  };

  /// Class to describe a layering stack
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class LayerStack {
  public:
    std::vector<Layer*> _layers;
    /// Default constructor
    LayerStack() = default;
    /// Copy constructor
    LayerStack(const LayerStack& copy) = default;
    /// Default destructor
    ~LayerStack()  = default;
    /// Assignment operator
    LayerStack& operator=(const LayerStack& copy)  = default;
    std::vector<Layer*>& layers() {
      return _layers;
    }
    const std::vector<Layer*>& layers() const {
      return _layers;
    }
    double sectionThickness(size_t is, size_t ie) const;
    double totalThickness() const {
      return sectionThickness(0, _layers.size() - 1);
    }
  };

  /// Class to convert a layering object from the compact notation
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class Layering {
  public:
    LayerStack _stack;
    /// Default constructor
    Layering() = default;
    /// Initializing constructor
    Layering(xml::Element element);
    /// Default destructor
    ~Layering() = default;

    std::vector<Layer*>& layers() {
      return _stack.layers();
    }
    const Layer* layer(size_t which) const;

    double totalThickness() const {
      return _stack.totalThickness();
    }
    double singleLayerThickness(xml::Element e) const;
    double absorberThicknessInLayer(xml::Element e) const;
    void sensitivePositionsInLayer(xml::Element e,std::vector<double>& sens_pos) const;

  };

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace xml {

    /// XML converter for layering objects
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_XML
     */
    class LayeringCnv: public Element {
    public:
      /// Initializing constructor
      LayeringCnv(Element e);
      /// Invoke converter
      void fromCompact(Layering& layering) const;
    };

    /// Initializing constructor
    inline LayeringCnv::LayeringCnv(Element e)
      : Element(e) {
    }
  }
}         /* End namespace dd4hep          */
#endif    /* DD4HEP_DDCORE_LAYERING_H      */
