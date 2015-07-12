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

// Framework include files
#include "XML/XMLDetector.h"
#include "XML/Layering.h"
#include "XML/XMLTags.h"

// C/C++ include files
#include <algorithm>
#include <stdexcept>
#include <cfloat>
#include <cmath>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::XML;

LayerSlice& LayerSlice::operator=(const LayerSlice& c) {
  if (&c != this) {
    _sensitive = c._sensitive;
    _thickness = c._thickness;
    _material = c._material;
  }
  return *this;
}

Layer::Layer(const Layer& c)
  : _thickness(c._thickness), _preOffset(c._preOffset), _slices(c._slices) {
}

Layer& Layer::operator=(const Layer& c) {
  if (this != &c) {
    _thickness = c._thickness;
    _preOffset = c._preOffset;
    _slices = c._slices;
  }
  return *this;
}

void Layer::compute() {
  _thickness = 0.;
  for (vector<LayerSlice>::const_iterator i = _slices.begin(); i != _slices.end(); ++i)
    _thickness += (*i)._thickness;
}

double LayerStack::sectionThickness(size_t is, size_t ie) const {
  double thick = 0.;
  if (is > ie)
    throw runtime_error(
                        "LayerStack::sectionThickness: First index (" + _toString(is) + ") must be <= second index (" + _toString(ie) + ")!");
  else if (is > _layers.size())
    throw runtime_error(
                        "LayerStack::sectionThickness: First index (" + _toString(is) + ") out of range. #layers=" + _toString(_layers.size())
                        + ").");
  else if (ie > _layers.size())
    throw runtime_error(
                        "LayerStack::sectionThickness: Second index (" + _toString(is) + ") out of range. #layers=" + _toString(_layers.size())
                        + ").");
  for (size_t i = is; i <= ie; ++i)
    thick += _layers[i]->thicknessWithPreOffset();
  return thick;
}

Layering::Layering() {
}

Layering::Layering(Element e) {
  LayeringCnv(e).fromCompact(*this);
}

Layering::~Layering() {
}

const Layer* Layering::layer(size_t which) const {
  return _stack.layers()[which];
}

void LayeringCnv::fromCompact(Layering& layering) const {
  vector<Layer*>& layers = layering.layers();
  int count = 0;
  for_each(layers.begin(), layers.end(), deletePtr<Layer>);
  for (Collection_t c(m_element, _U (layer)); c; ++c) {
    Layer lay;
    Component layer = c;
    int repeat = layer.hasAttr(_U(repeat)) ? layer.repeat() : 1;
    ++count;
    for (Collection_t s(c, _U(slice)); s; ++s) {
      Component slice = s;
      string mat = slice.materialStr();
      LayerSlice lslice(slice.isSensitive(), slice.thickness(), mat);
      lay.add(lslice);
    }
    lay.compute();
    // Add layer to stack once for each repetition
    for (int k = 0; k < repeat; ++k)
      layers.push_back(new Layer(lay));
  }
  if (0 == count) {
    throw runtime_error("LayeringCnv::fromCompact: No layer children to be build!");
  }
}

double Layering::singleLayerThickness(XML::Element e) const {
  Component lay = e;
  double thickness = 0e0;
  for (Collection_t s(lay, _U(slice)); s; ++s) {
    Component slice = s;
    thickness += slice.thickness();
  }
  return thickness;
}
