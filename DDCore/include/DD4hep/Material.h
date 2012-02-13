#ifndef DD4hep_MATERIAL_H
#define DD4hep_MATERIAL_H
#include <string>

/*
 *   Gaudi namespace declaration
 */
namespace DD4hep {

  /** @class Material
    *
    *
    */
  struct Atom  {
  protected:
    /// Default constructor
    Atom() : N(0), Z(0), A(0) {}
  public:
    int N;
    int Z;
    double A;
    std::string name;
    /// Initializing constructor: Create a Material entity
    Atom(int n, int z, double a, const std::string& nam) : N(n), Z(z), A(a), name(nam) {}
    /// Copy constructor: Create a Material entity
    Atom(const Atom& c) : N(c.N), Z(c.Z), A(c.A), name(c.name) {}
    /// Destructor,
    ~Atom()   {}
  };

  struct Density  {
    double      value;
    std::string unit;
    Density() : value(0.) {}
    Density(double d, const std::string& u) : value(d), unit(u) {}
    Density(const Density& c) : value(c.value), unit(c.unit) {}
  };

  struct Material  {
  protected:
    /// Default constructor
    Material() {}
  public:
    struct Composite {
      double fraction;
      std::string element;
      Composite(double f, const std::string& e) : fraction(f), element(e) {}
      Composite(const Composite& c) : fraction(c.fraction), element(c.element) {}
    };

    Density D;
    std::string name;
    std::vector<Composite> composition;
    /// Initializing constructor: Create a Material entity
    Material(const Density& d, const std::string& nam) : D(d), name(nam) {}
    /// Copy constructor: Create a Material entity
    Material(const Material& c) : D(c.D), name(c.name), composition(c.composition) {}
    /// Destructor,
    virtual ~Material()   {}
  };

  struct Constant  {
  public:
    std::string name, value;
    /// Default constructor
    Constant() {}
    /// Initializing constructor
    Constant(const std::string& nam, const std::string& val) : name(nam), value(val) {}
    /// Copy constructor: Create a Material entity
    Constant(const Constant& c) : name(c.name), value(c.value) {}
  };

  struct Visualization   {
  public:
    enum DrawingStyle { WIREFRAME };
    std::string name;
    float alpha;
    unsigned int rgb;
    DrawingStyle drawingStyle;
    bool showDaughters, visible;
    /// Default constructor
    Visualization() : alpha(0.), rgb(0), drawingStyle(WIREFRAME), showDaughters(false), visible(true)  {}
  };

}         /* End namespace DD4hep   */
#endif    /* DD4hep_MATERIAL_H      */
