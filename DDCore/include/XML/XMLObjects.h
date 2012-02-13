#ifndef DD4hep_OBJECTS_H
#define DD4hep_OBJECTS_H

// Framework include files
#include "XML/XMLTags.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  class IDDescriptor;

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Author : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Author(Handle_t e) : RefElement(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Author(const Document& doc) : RefElement(doc,Tag_author,Tag_empty) {}
      void setAuthorName(const char* nam)    {  setAttr(Attr_name,nam); }
      void setAuthorEmail(const char* addr)  {  setAttr(Attr_email,addr); }
    };

    struct Header : public Element  {
      /// Constructor to be used when reading the already parsed DOM tree
      Header(Handle_t e=0) : Element(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Header(const Document& doc) : Element(doc,Tag_header) {}
      Header& fromCompact(Document doc, Handle_t element, const XMLCh* fname);
    };

    struct Constant : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Constant(Handle_t e) : RefElement(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Constant(const Document& doc, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree
      Constant(const Document& doc, const XMLCh* name, const XMLCh* val);
      void setValue(const char* val)         {  setAttr(Attr_value,val); }
    };

    struct XYZ_element : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      XYZ_element(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when creating a new DOM tree
      XYZ_element(const Document& doc, const XMLCh* type, const XMLCh* name);
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      XYZ_element(const Document& doc, const XMLCh* type, const XMLCh* name, double x, double y, double z);
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      XYZ_element(const Document& doc, const XMLCh* type, const XMLCh* name, const XMLCh* x, const XMLCh* y, const XMLCh* z);
      const XYZ_element& setX(double value)   const;
      const XYZ_element& setY(double value)   const;
      const XYZ_element& setZ(double value)   const;
      const XYZ_element& fromXML(Handle_t values);
    };

    struct Position : public XYZ_element  {
      /// Constructor to be used when reading the already parsed DOM tree
      Position(Handle_t e) : XYZ_element(e) {}
      /// Constructor to be used when creating a new DOM tree
      Position(const RefElement& e) : XYZ_element(e) {}
      /// Constructor to be used when creating a new DOM tree
      Position(const Document& doc, const XMLCh* name) 
        : XYZ_element(doc,Tag_position,name) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Position(const Document& doc, const XMLCh* name, double x, double y, double z)
        : XYZ_element(doc,Tag_position,name,x,y,z) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Position(const Document& doc, const XMLCh* name, const XMLCh* x, const XMLCh* y, const XMLCh* z)
        : XYZ_element(doc,Tag_position,name,x,y,z) {}
    };

    struct Rotation : public XYZ_element  {
      /// Constructor to be used when reading the already parsed DOM tree
      Rotation(Handle_t e) : XYZ_element(e) {}
      /// Constructor to be used when creating a new DOM tree
      Rotation(const RefElement& e) : XYZ_element(e) {}
      /// Constructor to be used when creating a new DOM tree
      Rotation(const Document& doc, const XMLCh* name) 
        : XYZ_element(doc,Tag_rotation,name) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Rotation(const Document& doc, const XMLCh* name, double x, double y, double z)
        : XYZ_element(doc,Tag_rotation,name,x,y,z) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Rotation(const Document& doc, const XMLCh* name, const XMLCh* x, const XMLCh* y, const XMLCh* z)
        : XYZ_element(doc,Tag_rotation,name,x,y,z) {}
    };

    struct Atom : public RefElement  {
      /// Constructor to be used when creating a new DOM tree
      Atom(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(const Document& doc, const XMLCh* name);
    };

    struct Material : public RefElement  {
      /// Constructor to be used when creating a new DOM tree
      Material(const Handle_t& e) : RefElement(e) {}
      /// Constructor to be used when creating a new DOM tree
      Material(const RefElement& e) : RefElement(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Material(const Document& doc, const XMLCh* name);
    };

    struct VisAttr : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      VisAttr(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      VisAttr(const RefElement& e) : RefElement(e) {}
      /// Constructor to be used when creating a new DOM tree
      VisAttr(const Document& doc, const XMLCh* name);
      void setVisible(bool value);
      /// Convert compact visualization attribute to LCDD visualization attribute
      const VisAttr& fromCompact(Handle_t e)  const;
    };

    struct Limit : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Limit(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      Limit(const Document& doc, const XMLCh* name);
      void setParticles(const XMLCh* particleNames);
      void setValue(double value);
      void setUnit(const XMLCh* unit);
    };

    struct Region : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Region(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      Region(const Document& doc, const XMLCh* name);
      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      Region& setLengthUnit(const XMLCh* unit);
      Region& setEnergyUnit(const XMLCh* unit);
    };

    struct LimitSet : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      LimitSet(const Document& doc, const XMLCh* name);
      void addLimit(const Limit& limit);
    };

    struct IDSpec : public RefElement   {
      /// Constructor to be used when reading the already parsed DOM tree
      IDSpec(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      IDSpec(const Document& doc, const XMLCh* name, const IDDescriptor& dsc);
      void addField(const XMLCh* name, const std::pair<int,int>& field);
      void addField(const std::string& name, const std::pair<int,int>& field);
    };

  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_OBJECTS_H       */
