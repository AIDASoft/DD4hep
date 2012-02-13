#ifndef DD4hep_XMLLCDD_H
#define DD4hep_XMLLCDD_H
#include "XML/XMLElements.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Subdetector;
    struct Readout;
    struct Document;
    struct Solid;
    struct Constant;
    struct Volume;
    struct Region;
    struct Material;
    struct Materials;
    struct Position;
    struct Rotation;
    struct LimitSet;
    struct VisAttr;
    struct Compact;
    struct LCDD;

    template <class T> struct XMLConverter  {
      LCDD&  lcdd;
      XMLConverter(LCDD& l) : lcdd(l) {}
      void operator()(const Handle_t& c) const;
    };

    struct LCDD {

      virtual ~LCDD() {}

      virtual Document document() const = 0;
      virtual Document create() = 0;
      virtual void endDocument() = 0;
      virtual void addStdMaterials() = 0;

      virtual Handle_t header()  const = 0;
      virtual Handle_t structure()  const = 0;
      virtual Handle_t solids()  const = 0;
      virtual Handle_t materials()  const = 0;
      virtual RefElement worldVolume() const = 0;
      virtual RefElement trackingVolume() const = 0;
      
      virtual RefElement region(const XMLCh* name)  const = 0;
      virtual RefElement visAttributes(const XMLCh* name) const = 0;
      virtual RefElement limitSet(const XMLCh* name)  const = 0;
      virtual RefElement material(const XMLCh* name)  const = 0;
      virtual RefElement readout(const XMLCh* name)  const = 0;
      virtual RefElement idSpec(const XMLCh* name)  const = 0;
      virtual RefElement pickMotherVolume(const Subdetector& sd) const = 0;
      virtual RefElement constant(const XMLCh* name) const = 0;
      virtual RefElement position(const XMLCh* name) const = 0;
      virtual RefElement rotation(const XMLCh* name) const = 0;
      virtual RefElement solid(const XMLCh* name) const = 0;
      virtual RefElement define(const XMLCh* tag, const XMLCh* name) const = 0;

      virtual LCDD& add(const Constant& constant) = 0;
      virtual LCDD& add(const Solid& solid) = 0;
      virtual LCDD& add(const Region& region) = 0;
      virtual LCDD& add(const Volume& vol) = 0;
      virtual LCDD& add(const Material& mat) = 0;
      virtual LCDD& add(const VisAttr& attr) = 0;
      virtual LCDD& add(const Position& pos) = 0;
      virtual LCDD& add(const Rotation& rot) = 0;
      virtual LCDD& add(const LimitSet& limset) = 0;

      virtual LCDD& addIDSpec(const RefElement& element) = 0;
      virtual LCDD& addConstant(const RefElement& element) = 0;
      virtual LCDD& addMaterial(const RefElement& element) = 0;
      virtual LCDD& addVisAttribute(const RefElement& element) = 0;
      virtual LCDD& addSensitiveDetector(const RefElement& e) = 0;
      virtual LCDD& addLimitSet(const RefElement& limset) = 0;
      virtual LCDD& addRegion(const RefElement& region) = 0;
      virtual LCDD& addReadout(const RefElement& readout) = 0;
    };
  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_XMLLCDD_H       */
