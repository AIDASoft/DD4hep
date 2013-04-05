// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_LAYERING_H
#define DD4hep_LAYERING_H
#include "XML/XMLElements.h"

#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  struct LayerSlice  {
    bool   _sensitive;
    double _thickness;
    std::string _material;
    LayerSlice(bool s, double t, const std::string& m)
      : _sensitive(s), _thickness(t), _material(m) {}
    LayerSlice(const LayerSlice& c)
      : _sensitive(c._sensitive), _thickness(c._thickness), _material(c._material) {}
    LayerSlice& operator=(const LayerSlice& c)
    { _sensitive=c._sensitive; _thickness=c._thickness; _material=c._material; return *this; }
  };

  struct Layer  {
    double _thickness;
    double _preOffset;
    std::vector<LayerSlice> _slices;
    Layer() : _thickness(0.), _preOffset(0.)  {}
    Layer(const Layer& c);
    Layer& operator=(const Layer& c);
    void compute();
    double thickness() const              {  return _thickness;              }
    double thicknessWithPreOffset() const {  return _thickness+_preOffset;   }
    void add(const LayerSlice& s)         {  _slices.push_back(s);           }
  };

  struct LayerStack  {
    std::vector<Layer*> _layers;
    LayerStack()  {}
    LayerStack(const LayerStack& c) : _layers(c._layers)  {}
    ~LayerStack()  {}
    LayerStack& operator=(const LayerStack& c) { _layers = c._layers;  return *this; }
    std::vector<Layer*>& layers()   { return _layers; }
    const std::vector<Layer*>& layers() const   { return _layers; }
    double sectionThickness(size_t is, size_t ie) const;
    double totalThickness()  const  { return sectionThickness(0,_layers.size()-1); }
  };

  struct Layering   {
    LayerStack _stack;
    Layering();
    Layering(XML::Element e);
    virtual ~Layering();
    std::vector<Layer*>& layers()   { return _stack.layers(); }
    const Layer* layer(size_t which) const;
    double totalThickness()  const  { return _stack.totalThickness(); }
    double singleLayerThickness(XML::Element e)  const;
  };

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    // Tag and attribute objects. See XMLTags.cpp for the instantiation
    struct LayeringCnv : public Element  {
      LayeringCnv(Element e) : Element(e ) {}
      void fromCompact(Layering& layering)  const;
    };
  }

}         /* End namespace DD4hep   */
#endif    /* DD4hep_LAYERING_H      */
