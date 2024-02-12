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
#ifndef DD4HEP_NONE

// Framework include files
#include <DD4hep/Primitives.h>
#include <DD4hep/Printout.h>
#include <XML/Layering.h>
#include <XML/XML.h>

void dd4hep::Layer::compute() {
  _thickness = 0.;
  for (std::vector<LayerSlice>::const_iterator i = _slices.begin(); i != _slices.end(); ++i)
    _thickness += (*i)._thickness;
}

double dd4hep::LayerStack::sectionThickness(std::size_t is, std::size_t ie) const {
  double thick = 0.;
  if (is > ie)
    except("LayerStack",
           "sectionThickness: First index (%ld) must be <= second index (%ld)!", is, ie);
  else if (is > _layers.size())
    except("LayerStack",
           "sectionThickness: First index (%ld) out of range. #layers=%ld).", is, _layers.size());
  else if (ie > _layers.size())
    except("LayerStack",
           "sectionThickness: Second index (%ld) out of range. #layers=%ld).", is, _layers.size());
  for (std::size_t i = is; i <= ie; ++i)
    thick += _layers[i]->thicknessWithPreOffset();
  return thick;
}

void dd4hep::xml::LayeringCnv::fromCompact(Layering& layering) const {
  std::vector<Layer*>& layers = layering.layers();
  int count = 0;
  for_each(layers.begin(), layers.end(), detail::deletePtr<Layer>);
  for( xml_coll_t c(m_element, _U (layer)); c; ++c ) {
    Layer lay;
    xml_comp_t layer = c;
    int repeat = layer.hasAttr(_U(repeat)) ? layer.repeat() : 1;
    ++count;
    for (xml_coll_t s(c, _U(slice)); s; ++s) {
      xml_comp_t  slice = s;
      std::string material = slice.materialStr();
      LayerSlice  lslice(slice.isSensitive(), slice.thickness(), material);
      lay.add(lslice);
    }
    lay.compute();
    // Add layer to stack once for each repetition
    for( int k = 0; k < repeat; ++k )
      layers.emplace_back(new Layer(lay));
  }
  if (0 == count) {
    throw std::runtime_error("LayeringCnv::fromCompact: No layer children to be build!");
  }
}

dd4hep::Layering::Layering(xml_elt_t e)  {
  xml::LayeringCnv(e).fromCompact(*this);
}

const dd4hep::Layer* dd4hep::Layering::layer(std::size_t which) const {
  return _stack.layers()[which];
}

double dd4hep::Layering::singleLayerThickness(xml_elt_t e) const {
  xml_comp_t lay = e;
  double thickness = 0e0;
  for (xml_coll_t s(lay, _U(slice)); s; ++s) {
    xml_comp_t slice = s;
    thickness += slice.thickness();
  }
  return thickness;
}

double dd4hep::Layering::absorberThicknessInLayer(xml_elt_t e) const {
  xml_comp_t lay = e;
  double thickness = 0e0;
  for (xml_coll_t s(lay, _U(slice)); s; ++s) {
    xml_comp_t slice = s;
    if (slice.isRadiator())
        thickness += slice.thickness();
  }
  return thickness;
}

void dd4hep::Layering::sensitivePositionsInLayer(xml_elt_t e, std::vector<double>& sens_pos) const {
  xml_comp_t lay = e;
  double pos =- singleLayerThickness(e)/2.0;
  for (xml_coll_t s(lay, _U(slice)); s; ++s) {
    xml_comp_t slice = s;
    pos += slice.thickness();
    if (slice.isSensitive()){
        //store the position at the center of the slice
        sens_pos.emplace_back(pos - slice.thickness()/2.);
    }
  }
}


dd4hep::Layering::~Layering(){
  std::vector<Layer*>& layers = this->layers();
  for_each(layers.begin(), layers.end(), detail::deletePtr<Layer>);
  layers.clear();
}
#endif
