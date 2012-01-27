#ifndef DETDESC_GEOMETRY_OBJECTS_H
#define DETDESC_GEOMETRY_OBJECTS_H

// Framework include files
#include "Elements.h"
class TMap;

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  // Forward declarations
  class IDDescriptor;

  /*
  *   XML namespace declaration
  */
  namespace Geometry  {

    // Forward declarations
    struct Document;

    struct Author : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Author(Handle_t e) : RefElement(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Author(const Document& doc) : RefElement(doc,"author","") {}
      //void setAuthorName(const char* nam)    {  setAttr("name",nam); }
      //void setAuthorEmail(const char* addr)  {  setAttr("email",addr); }
    };

    struct Header : public Element  {
      /// Constructor to be used when reading the already parsed DOM tree
      Header(Handle_t e=0) : Element(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Header(const Document& doc) : Element(doc,"header") {}
      //Header& fromCompact(Document doc, Handle_t element, const std::string& fname);
    };

    struct Constant : public RefElement  {
      /// Constructor to be used when reading the already parsed DOM tree
      Constant(Handle_t e) : RefElement(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Constant(const Document& doc, const std::string& name);
      /// Constructor to be used when creating a new DOM tree
      Constant(const Document& doc, const std::string& name, const std::string& val);
    };

    struct Matrix : public RefElement  {
      /// Constructor to be used RefElement reading the already parsed DOM tree
      Matrix(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when creating a new DOM tree
      Matrix(const Document& doc, const std::string& type, const std::string& name) 
        : RefElement(doc,type,name) {}
    };

    struct Position : public Matrix  {
      /// Constructor to be used when reading the already parsed DOM tree
      Position(Handle_t e) : Matrix(e) {}
      /// Constructor to be used when creating a new DOM tree
      Position(const Document& doc, const std::string& name) 
        : Matrix(doc,"position",name) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Position(const Document& doc, const std::string& name, double x, double y, double z);
    };

    struct Rotation : public Matrix  {
      /// Constructor to be used RefElement reading the already parsed DOM tree
      Rotation(Handle_t e) : Matrix(e) {}
      /// Constructor to be used when creating a new DOM tree
      Rotation(const Document& doc, const std::string& name) 
        : Matrix(doc,"rotation",name) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Rotation(const Document& doc, const std::string& name, double x, double y, double z);
    };

    struct Atom : public RefElement  {
      /// Constructor to be used when creating a new DOM tree
      Atom(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(const Document& doc, const std::string& name);
    };

    struct Material : public RefElement  {
      /// Constructor to be used when creating a new DOM tree
      Material(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Material(const Document& doc, const std::string& name);
    };

    struct VisAttr : public RefElement  {
      struct Object  {
        int           color;
        unsigned char drawingStyle, lineStyle, showDaughters, visible;
        Object() : color(0), drawingStyle(true), showDaughters(true), visible(true)  {}
      };
      enum DrawingStyle { 
        WIREFRAME=0x1,
        LAST_DRAWING_STYLE
      };
      enum LineStyle  {
        SOLID=0x1,
        DASHED=0x2,
        LAST_LINE_STYLE
      };
      /// Constructor to be used when reading the already parsed DOM tree
      VisAttr(Handle_t e) : RefElement(e) {}
      /// Constructor to be used when creating a new DOM tree
      VisAttr(const Document& doc, const std::string& name);
      /// Set Flag to show/hide daughter elements
      void setShowDaughters(bool value);
      /// Set line style
      void setLineStyle(LineStyle style);
      /// Set drawing style
      void setDrawingStyle(DrawingStyle style);
      /// Set visibility flag
      void setVisible(bool value);
      /// Set alpha value
      void setAlpha(float value);
      /// Set object color
      void setColor(float red, float green, float blue);
    };

    struct Limit : public RefElement  {
      typedef std::pair<std::string,double> Object;

      /// Constructor to be used when creating a new DOM tree
      Limit(const Document& doc, const std::string& name);
      void setParticles(const std::string& particleNames);
      void setValue(double value);
      void setUnit(const std::string& unit);
    };

    struct LimitSet : public RefElement  {
      typedef TMap Object;
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      LimitSet(const Document& doc, const std::string& name);
      void addLimit(const RefElement& limit);
    };

    struct Region : public RefElement  {
      struct Object  {
        double      Attr_threshold;
        double      Attr_cut;
        bool        Attr_store_secondaries;
        std::string Attr_lunit, Attr_eunit;
      };
      /// Constructor to be used when reading the already parsed DOM tree
      Region(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      Region(const Document& doc, const std::string& name);
      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      Region& setLengthUnit(const std::string& unit);
      Region& setEnergyUnit(const std::string& unit);
    };

    struct IDSpec : public RefElement   {
      /// Constructor to be used when reading the already parsed DOM tree
      IDSpec(Handle_t h) : RefElement(h) {}
      /// Constructor to be used when creating a new DOM tree
      IDSpec(const Document& doc, const std::string& name, const IDDescriptor& dsc);
      void addField(const std::string& name, const std::pair<int,int>& field);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_OBJECTS_H       */
