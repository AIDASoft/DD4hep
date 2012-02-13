#ifndef DD4hep_XMLVOLUMES_H
#define DD4hep_XMLVOLUMES_H
#include "XML/XMLElements.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Solid;
    struct Material;
    struct Volume;
    struct PhysVol;
    struct Position;
    struct Rotation;

    struct Volume : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Volume(Handle_t e = Handle_t(0));
      /// Constructor to be used when reading the already parsed DOM tree
      Volume(const RefElement& e);
      /// Constructor to be used when creating a new DOM tree
      Volume(const Document& document, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree. Also sets materuial and solid attributes
      Volume(const Document& document, const XMLCh* name, const Solid& s, const Material& m);
      Attribute setSolid(const Solid& s)  const;
      Attribute setMaterial(const Material& m)  const;
      void addPhysVol(const PhysVol& vol)  const;
      void setRegion(const RefElement& obj)  const;
      void setLimitSet(const RefElement& obj)  const;
      void setSensitiveDetector(const RefElement& obj) const;
      void setVisAttributes(const RefElement& obj) const;
      const XMLCh* solidRef() const;
    };

    struct PhysVol : Element {
      /// Constructor to be used when reading the already parsed DOM tree
      PhysVol(Handle_t e);
      /// Constructor to be used when creating a new DOM tree
      PhysVol(const Document& document, Volume vol);
      /// Constructor to be used when creating a new DOM tree with additional parameters
      PhysVol(const Document& document, Volume vol, const Position& pos, const Rotation& rot);
      PhysVol& addPhysVolID(const XMLCh* name, int value);
      PhysVol& setRotation(const RefElement& rot);
      PhysVol& setPosition(const RefElement& pos);
      PhysVol& setZ(double value);
    };
  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_XMLVOLUMES_H    */
