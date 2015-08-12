// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4hep_LAYERING_H
#define DD4hep_LAYERING_H

// Framework include files
#include "XML/XMLElements.h"

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

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
    LayerSlice(bool s, double t, const std::string& m);
    /// Copy constructor
    LayerSlice(const LayerSlice& c);
    /// Assignment operator
    LayerSlice& operator=(const LayerSlice& c);
  };

  /// Initializing constructor
  inline LayerSlice::LayerSlice(bool s, double t, const std::string& m)
    : _sensitive(s), _thickness(t), _material(m) {
  }
  /// Copy constructor
  inline LayerSlice::LayerSlice(const LayerSlice& c)
    : _sensitive(c._sensitive), _thickness(c._thickness), _material(c._material) {
  }

  /// Class to describe one layer in a layering stack
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class Layer {
  public:
    double _thickness;
    double _preOffset;
    std::vector<LayerSlice> _slices;

    /// Default constructor
    Layer();
    /// Copy constructor
    Layer(const Layer& c);
    /// Assignment operator
    Layer& operator=(const Layer& c);

    void compute();
    double thickness() const {
      return _thickness;
    }
    double thicknessWithPreOffset() const {
      return _thickness + _preOffset;
    }
    void add(const LayerSlice& s) {
      _slices.push_back(s);
    }
  };

  /// Default constructor
  inline Layer::Layer() : _thickness(0.), _preOffset(0.) {
  }


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
    LayerStack();
    /// Copy constructor
    LayerStack(const LayerStack& c);
    /// Default destructor
    ~LayerStack() {
    }
    /// Assignment operator
    LayerStack& operator=(const LayerStack& c) {
      if ( &c != this ) _layers = c._layers;
      return *this;
    }
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

  /// Default constructor
  inline LayerStack::LayerStack() {
  }
  /// Copy constructor
  inline LayerStack::LayerStack(const LayerStack& c)
    : _layers(c._layers) {
  }

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
    Layering();
    /// Initializing constructor
    Layering(XML::Element e);
    /// Default destructor
    virtual ~Layering();

    std::vector<Layer*>& layers() {
      return _stack.layers();
    }
    const Layer* layer(size_t which) const;

    double totalThickness() const {
      return _stack.totalThickness();
    }
    double singleLayerThickness(XML::Element e) const;
    double absorberThicknessInLayer(XML::Element e) const;
    void sensitivePositionsInLayer(XML::Element e,std::vector<double>& sens_pos) const;

  };

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace XML {

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

} /* End namespace DD4hep   */
#endif    /* DD4hep_LAYERING_H      */
